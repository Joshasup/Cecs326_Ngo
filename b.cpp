//
// Created by christopher on 2/15/19.
//

#include "define.h"
#include <cstring>
#include <random>
#include <sys/ipc.h>
#include <sys/msg.h>

int promised_random() {
    int r;
    do {
        r = rand();
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
    srand(std::random_device{}());
    int qid = msgget(ftok(".", 'u'), 0);
    route(qid);
}
