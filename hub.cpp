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
        std::cerr << "Program finished with code " << status << "\n";
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

    ssize_t status = msgrcv(qid, &msg, msg_size, 997, IPC_NOWAIT);
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
void b_route(int qid) {
    std::cout << "Enter the PID of Probe B: ";
    pid_t pid = getpid();

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

bool from_a(message_buffer &m) {
    auto i = std::atoi(m.message);
    return valid_reading(i, 274471);
}

bool from_b(message_buffer &m) {
    auto i = std::atoi(m.message);
    return valid_reading(i, 80107);
}

bool c_end(int qid, message_buffer &m) {
    ssize_t kill_status = msgrcv(qid, &m, msg_size, 3, IPC_NOWAIT);
    std::cout << "Last ProbeC message: " << std::string_view{m.message}
              << std::endl;
    return kill_status == -1;
}

void general_route(int qid, pid_t pid) {
    message_buffer msg{shared_mtype};
    message_buffer a{2};
    message_buffer c{3};
    // A setup
    strncpy(a.message, "Acknowledged.", sizeof(a.message));
    // B setup
    int message_count = 0;
    bool b_dead = false;
    // C setup
    bool c_dead = false;

    while (true) {
        // Receive a message
        ssize_t status =
            msgrcv(qid, &msg, msg_size, msg.message_type, IPC_NOWAIT);
        if (status != 1) {
            if (from_a(msg)) {
                std::cout << "Probe [A]: " << std::string_view{msg.message}
                          << "\n";
                // Send acknowledgement back
                msgsnd(qid, &a, msg_size, 0);
            } else if (from_b(msg)) {
                std::cout << "Probe [B]: " << std::string_view{msg.message}
                          << "\n";
            } else if (c_end(qid, c)) {
                // C termination
                std::cout << "Probe C has been killed.\n";
                c_dead = true;
            } else {
                std::cout << "Probe [C]: " << std::string_view{msg.message}
                          << "\n";
            }
            ++message_count;
        } else if (b_dead && c_dead) {
            // Probe B and C have stopped running and there are no more messages
            // to read from the queue. (A must have stopped too)
            std::cout << "If ProbeA is still running, this is likely a logic "
                         "or timing error.\n";
            break;
        }
        // B termination
        if (message_count >= 10000) {
            std::cout << "Limit reached, killing ProbeB.\n";
            force_patch(pid);
            b_dead = true;
        }
    }
}

int main() {
    // Create message queue shared by all 3 hubs
    queue q{};
    std::cout << "Make sure to initialize the other probes now before entering "
                 "anything in.\n";

    // Get pid so that the program can force quit Probe B
    std::cout << "Enter the PID of Probe B: ";
    pid_t pid = getpid();

    // a_route(q.qid);
    // b_route(q.qid, pid);
    // c_route(q.qid);
    general_route(q.qid, pid);
}
