//
// Created by christopher on 2/15/19.
//

#include "define.h"
#include "kill_patch.h"
#include <iostream>
#include <unistd.h>
#include <sys/msg.h>
#include <optional>

constexpr auto rho = 251;

std::optional<int> promised_random() {
    const int r = rand();
    if (valid_reading(r, rho))
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
    
}