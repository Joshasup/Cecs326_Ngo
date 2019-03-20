//
// Created by christopher on 2/15/19.
//

#include "define.h"
#include "force_patch.h"
#include <cstring>
#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>

struct queue {
    // Create message queue
    queue() {
        qid = msgget(ftok(".", 'u'), IPC_EXCL | IPC_CREAT | 00666);
        if (qid == -1) {
            std::cout << "Failed to create a new queue, attempting to use a "
                         "preexisting queue.\n";
        }
        qid = msgget(ftok(".", 'u'), 0);
        std::cout << "Using QID of " << qid << std::endl;
    }

    // Delete message queue
    ~queue() {
        int status = msgctl(qid, IPC_RMID, nullptr);
        std::cout << "Program finished with code " << status << "\n";
    }

    int qid;
};

// Repeatedly asks user for PID of ProbeB so Hub can terminate the program.
pid_t getpid() {
    int i;
    while (!(std::cin >> i)) {
        std::cout << "Invalid entry, try again. ";
        std::cin.clear();
        std::cin.ignore();
    }
    return i;
}

// Code used to test route A
void a_route(int qid) {
    message_buffer msg{shared_mtype};
    long probe_a_mtype = 2;

    ssize_t status = msgrcv(qid, &msg, msg_size, shared_mtype, IPC_NOWAIT);
    while (std::string_view{msg.message} != std::string_view{"TERM"}) {
        // TODO: Fix this condition

        // The message was able to be read from Probe A
        if (status != -1) {
            // Send acknowledgement back to ProbeA
            // TODO: Decode that the number is indeed from ProbeA using modulo
            std::cout << "Probe [A]: " << std::string_view{msg.message} << "\n";
            msg.message_type = probe_a_mtype;
            strncpy(msg.message, "Received from Probe A", sizeof(msg.message));
            msgsnd(qid, &msg, msg_size, 0);
        }
        // Attempt to read the next message from Probe A
        msg.message_type = shared_mtype;
        status = msgrcv(qid, &msg, msg_size, shared_mtype, IPC_NOWAIT);
    }
    std::cout << "ProbeA has been terminated.\n";
}

// Code used to test route B
void b_route(int qid, pid_t pid) {
    message_buffer msg{shared_mtype};
    std::cout << "Waiting for Probe B...\n";
    int message_count = 0;
    while (message_count < 10'000) {
        ssize_t status =
            msgrcv(qid, &msg, msg_size, msg.message_type, IPC_NOWAIT);

        // The message was able to be read
        if (status != -1) {
            std::cout << "Probe [B]: " << std::string_view{msg.message} << "\n";
            ++message_count;
        }
    }
    std::cout << "Limit reached, killing ProbeB.\n";
    force_patch(pid);
}

// Code used to test route C
void c_route(int qid) {
    message_buffer c{3};
    message_buffer msg{shared_mtype};
    std::cout << "Waiting for Probe B...\n";

    auto kill_status = msgrcv(qid, &c, msg_size, c.message_type, IPC_NOWAIT);
    while (kill_status == -1) {
        ssize_t status =
            msgrcv(qid, &msg, msg_size, msg.message_type, IPC_NOWAIT);

        // The message was able to be read
        if (status != -1) {
            std::cout << "Probe [C]: " << std::string_view{msg.message} << "\n";
        }
        kill_status = msgrcv(qid, &c, msg_size, c.message_type, IPC_NOWAIT);
        if (kill_status != -1) {
            break;
        }
    }
    std::cout << "Final message from ProbeC: " << std::string_view{c.message}
              << std::endl;
}

enum pids { A, B, C, D };

// Split string into two
auto parse_message(message_buffer &m, int &a_pid, int &b_pid, int &c_pid) {
    std::string a{m.message};
    auto pivot = a.find_first_of(':');
    std::string pid(a.begin(), a.begin() + pivot);
    std::string num(a.begin() + pivot + 1, a.end());

    auto i = std::atoi(num.c_str());
    if (valid_reading(i, 274471)) {
        a_pid = std::atoi(pid.c_str());
        return pids::A;
    } else if (valid_reading(i, 80107)) {
        b_pid = std::atoi(pid.c_str());
        return pids::B;
    } else if (valid_reading(i, 75479)) {
        c_pid = std::atoi(pid.c_str());
        return pids::C;
    }
}

bool a_end(int qid, message_buffer &m) {
    ssize_t kill_status = msgrcv(qid, &m, msg_size, m.message_type, IPC_NOWAIT);
    // Kill message received when A sends final message
    // Message received
    if (kill_status != -1) {
        // Message is valid
        if (std::string_view{m.message} == "TERM") {
            std::cout << "Last ProbeA message: " << std::string_view{m.message}
                      << std::endl;
            return true;
        }
        return false;
    }
    return false;
}

bool c_end(int qid, message_buffer &m) {
    ssize_t kill_status = msgrcv(qid, &m, msg_size, m.message_type, IPC_NOWAIT);
    // Kill message received when C is killed by separate process
    bool ret = kill_status != -1;
    if (ret) {
        std::cout << "Last ProbeC message: " << std::string_view{m.message}
                  << std::endl;
    }
    return ret;
}

void general_route(queue &q) {
    // General setup
    message_buffer shared_msg{shared_mtype};
    int qid = q.qid;
    // A setup
    message_buffer a_snd{2};
    message_buffer a_rcv{4};
    strncpy(a_snd.message, "Acknowledged.", sizeof(a_snd.message));
    bool a_dead = false;
    // B setup
    int message_count = 0;
    bool b_dead = false;
    // C setup
    message_buffer c_msg{3};
    bool c_dead = false;

    while (!(a_dead && b_dead && c_dead)) {
        // Receive a message
        ssize_t status = msgrcv(qid, &shared_msg, msg_size,
                                shared_msg.message_type, IPC_NOWAIT);
        pid_t a_pid;
        pid_t b_pid;
        pid_t c_pid;

        // Message received
        if (status != -1 && std::string_view{shared_msg.message} != "") {
            printf("Message: %s\n", shared_msg.message);

            auto from = parse_message(shared_msg, a_pid, b_pid, c_pid);
            // Send acknowledgement back
            if (from == pids::A) {
                msgsnd(qid, &a_snd, msg_size, 0);
            }
            ++message_count;
        }
        // A termination
        if (!a_dead && a_end(qid, a_rcv)) {
            printf("ProbeA generated a value less than 100.\n");
            a_dead = true;
        }
        // B termination
        if (!b_dead && message_count == 10000) {
            printf("Limit reached, killing ProbeB.\n");
            force_patch(b_pid);
            b_dead = true;
        }
        // C termination
        if (!c_dead && c_end(qid, c_msg)) {
            printf("Probe C has suddenly stopped.\n");
            c_dead = true;
        }
    }
    printf("All Probes have detached from queue. Exiting...\n");
}

int main() {
    // Create message queue shared by all 3 hubs
    queue q{};

    // a_route(q.qid);
    // b_route(q.qid, pid);
    // c_route(q.qid);
    general_route(q);
}
