//
// Created by christopher on 2/15/19.
//

#include "define.h"
#include "force_patch.h"
#include <cstring>
#include <iostream>
#include <optional>
#include <sys/ipc.h>
#include <sys/msg.h>

struct queue {
    // Create message queue when program begins
    queue() {
        qid = msgget(ftok(".", 'u'), IPC_EXCL | IPC_CREAT | 00666);
        if (qid == -1) {
            std::cout << "Failed to create a new queue, attempting to use a "
                         "preexisting queue.\n";
        }
        qid = msgget(ftok(".", 'u'), 0);
        std::cout << "Using QID of " << qid << std::endl;
    }

    // Delete message queue when program ends
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

// Split message into two, print the message, and set pid of Probes if they
// haven't been yet
std::optional<pid_t> parse_message(message_buffer &m,
                                   std::optional<pid_t> &a_pid,
                                   std::optional<pid_t> &b_pid,
                                   std::optional<pid_t> &c_pid) {

    // The pivot is set by the first strtol to the address before the next token
    // So getting the next int is a matter of reading one token ahead
    char *pivot;
    auto id = std::strtol(m.message, &pivot, 10);
    auto num = std::strtol(pivot + 1, nullptr, 10);
    printf("[%li]: %li", id, num);
    if (valid_reading(num, 274471)) {
        a_pid = id;
        return a_pid;
    } else if (valid_reading(num, 80107)) {
        b_pid = id;
        return b_pid;
    } else if (valid_reading(num, 75479)) {
        c_pid = id;
        return c_pid;
    }
    return std::nullopt;
}

// Returns true if it detects ProbeA's kill message
bool a_end(int qid, message_buffer &m) {
    ssize_t kill_status = msgrcv(qid, &m, msg_size, m.message_type, IPC_NOWAIT);
    // Kill message received when ProbeA produces number less than 100
    if (kill_status != -1) {
        // Message is valid, condition may not be needed...
        if (std::string_view{m.message} == "TERM") {
            std::cout << "Last ProbeA message: " << std::string_view{m.message}
                      << std::endl;
            return true;
        }
        return false;
    }
    return false;
}

// Returns true if it detects ProbeC's kill_patch message
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
    std::optional<pid_t> a_pid;
    // B setup
    int message_count = 0;
    bool b_dead = false;
    std::optional<pid_t> b_pid;
    // C setup
    message_buffer c_msg{3};
    bool c_dead = false;
    std::optional<pid_t> c_pid;

    // Optionals are used as a form of type-safe lazy initialization

    while (!(a_dead && b_dead && c_dead)) {
        // Receive a message
        ssize_t status = msgrcv(qid, &shared_msg, msg_size,
                                shared_msg.message_type, IPC_NOWAIT);

        // Message received
        if (status != -1 && std::string_view{shared_msg.message} != "") {
            // Send acknowledgement back if the message came from ProbeA
            if (auto from = parse_message(shared_msg, a_pid, b_pid, c_pid);
                a_pid && from == a_pid) {
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
        if (!b_dead && message_count >= 10000) {
            printf("Limit reached, killing ProbeB.\n");

            // If ProbeB hasn't been set, prevent program from killing a random
            // process.
            if (b_pid) {
                force_patch(b_pid.value());
                b_dead = true;
            } else {
                printf("Attempting to kill ProbeB, but ProbeB doesn't exist\n");
            }
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

    // Begin main logic loop
    general_route(q);
}
