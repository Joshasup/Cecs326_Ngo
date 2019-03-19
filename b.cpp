//
// Created by christopher on 2/15/19.
//

#include "define.h"
#include <cstring>
#include <iostream>
#include <random>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

int promised_random() {
    int r;
    do {
        r = rand();
    } while (!valid_reading(r, beta));
    return r;
}

void route(int qid) {
    message_buffer msg{beta};

    std::cout << "Broadcasting from " << getpid() << '\n';

    // Copy the formatted int into the message queue
    while (true) {
        int random = promised_random();
        snprintf(msg.message, sizeof(msg.message), "%i", random);
        msgsnd(qid, &msg, msg_size, 0);
    }
}

int main() {
    std::cout<<"hi";
    int randomNum = rand();
    std::cout << randomNum;
    srand(std::random_device{}());
    int qid = msgget(ftok(".", 'u'), 0);
    route(qid);
}
