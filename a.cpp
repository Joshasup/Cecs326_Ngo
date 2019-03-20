#include "define.h"
#include <cstring>
#include <iostream>
#include <optional>
#include <random>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

constexpr auto magic_seed = 274471;

std::pair<int, bool> promised_random() {
    const int r = rand();
    return {r, (valid_reading(r, magic_seed))};
}

bool get_acknowledgement(int qid, message_buffer &m) {
    m.message_type = 2;
    const auto ret = msgrcv(qid, &m, msg_size, 2, IPC_NOWAIT);
    if (ret != -1) {
        printf("Acknowledged.\n");
    }
    return ret != -1;
}

void route(int qid) {
    message_buffer msg{shared_mtype};
    bool acknowledged = true;
    auto random = promised_random();

    while (true) {
        if (random.second || acknowledged) {
            if (random.second && acknowledged) {
                // Send message
                msg.message_type = shared_mtype;
                snprintf(msg.message, sizeof(msg.message), "%i:%i", getpid(), random.first);
                printf("Sending %s\n", msg.message);
                msgsnd(qid, &msg, msg_size, 0);
            } else if (random.second) {
                // We have the random number but no acknowledgement
                acknowledged = get_acknowledgement(qid, msg);
                continue;
            } else {
                // We have the acknowledgement but not the right random number
                if (random.first < 100) {
                    // Send kill message
                    printf("Generated a number less than 100. Exiting.\n");
                    strncpy(msg.message, "TERM", sizeof(msg.message));
                    msg.message_type = 4;
                    msgsnd(qid, &msg, msg_size, 0);
                    exit(0);
                }
                random = promised_random();
                continue;
            }
        }
        if (random.first < 100) {
            // Send kill message
            printf("Generated a number less than 100. Exiting.\n");
            strncpy(msg.message, "TERM", sizeof(msg.message));
            msg.message_type = 4;
            msgsnd(qid, &msg, msg_size, 0);
            exit(0);
        }
        random = promised_random();
        acknowledged = get_acknowledgement(qid, msg);
    }
}

int main() {
    srand(std::random_device{}());

    // Wait until the queue has started.
    int qid = -1;
    do {
        qid = msgget(ftok(".", 'u'), 0);
    } while (qid == -1);
    printf("Queue %i found\n", qid);

    route(qid);
}
