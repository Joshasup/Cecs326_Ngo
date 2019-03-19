//
// Created by christopher on 2/15/19.
//

#include "define.h"
#include "kill_patch.h"
#include <cstring>
#include <iostream>
#include <optional>
#include <random>
#include <sys/msg.h>
#include <unistd.h>

constexpr auto magic_seed = 75479;

std::optional<int> promised_random() {
    const int r = rand();
    if (valid_reading(r, magic_seed))
        return r;
    return {};
}

void route(int qid) {
    message_buffer msg{shared_mtype};

    std::cout << "Broadcasting from " << getpid() << '\n';

    // Copy the formatted int into the message queue
    while (true) {
        const auto random = promised_random();
        if (random) {
            snprintf(msg.message, sizeof(msg.message), "%i", random.value());
            msgsnd(qid, &msg, msg_size, 0);
        }
    }
}

int main() {
    srand(std::random_device{}());
    int qid = msgget(ftok(".", 'u'), 0);

    message_buffer msg{3};
    strncpy(msg.message, "Mama Mia!", sizeof(msg.message));
    kill_patch(qid, (struct msgbuf *)&msg, msg_size, 3);

    route(qid);
}