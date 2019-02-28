//
// Created by christopher on 2/15/19.
//

#include "define.h"
#include <iostream>
#include <memory>
#include <string_view>
#include <sys/ipc.h>
#include <sys/msg.h>

struct queue {
    // Create message queue
    queue() : qid{msgget(ftok(".", 'u'), IPC_EXCL | IPC_CREAT | 00600)} {}
    // Delete message queue
    ~queue() {
        int status = msgctl(qid, IPC_RMID, nullptr);
        std::cerr << "Program finished with code " << status << "\n";
    }
    int qid;
};

void b_route(int qid) {
    message_buffer msg{beta, std::make_unique<char[]>(max_size)};
    std::cout << "Waiting for Probe B...\n";
    int message_count = 0;
    while (message_count < 10'000) {
        ssize_t status = msgrcv(qid, &msg, message_size, msg.message_type, IPC_NOWAIT);
        // The message was able to be read
        if (status != -1) {
            std::cout << "Probe [B]: "
                      << std::string_view{msg.message.get(), msg.size()}
                      << "\n";
            ++message_count;
        }
    }
}

int main() {
    // Create message queue shared by all 3 hubs
    queue q{};
    // a_route(q.qid);
    b_route(q.qid);
    // c_route(q.qid);
}
