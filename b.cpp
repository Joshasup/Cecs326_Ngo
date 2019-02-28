//
// Created by christopher on 2/15/19.
//

#include "define.h"
#include <cstring>
#include <memory>
#include <random>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

int promised_random() {
    std::minstd_rand0 rng{std::random_device{}()};
    std::uniform_int_distribution<int> dist{};
    int r;
    do {
        r = dist(rng);
    } while (!valid_reading(r, beta));
    return r;
}

void route(int qid) {
    message_buffer msg{beta, std::make_unique<char[]>(max_size)};
    while (true) {
        // Copy the formatted int into the message queue
        int random = promised_random();
        snprintf(msg.message.get(), max_size, "%i", random);

        msgsnd(qid, &msg, message_size, 0);
    }
}

int main() {
    int qid = msgget(ftok(".", 'u'), 0);
    route(qid);
}
