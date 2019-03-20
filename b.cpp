//
// Created by christopher on 2/15/19.
//

#include "define.h"
#include <cstring>
#include <iostream>
#include <optional>
#include <random>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

constexpr auto magic_seed = 80107;

std::optional<int> promised_random() {
    const int r = rand();
    if (valid_reading(r, magic_seed))
        return r;
    return {};
}

void route(int qid) {
    message_buffer msg{shared_mtype};

    // Copy the formatted int into the message queue
    while (true) {
        const auto random = promised_random();
        if (random) {
            snprintf(msg.message, sizeof(msg.message), "%i:%i", getpid(),
                     random.value());
            msgsnd(qid, &msg, msg_size, 0);
        }
    }
}

int main() {
    std::cout << "Broadcasting from " << getpid() << '\n';

    srand(std::random_device{}());

    // Wait until the queue has started.
    int qid = -1;
    do {
        qid = msgget(ftok(".", 'u'), 0);
    } while (qid == -1);
    printf("Queue %i found\n", qid);

    route(qid);
}
