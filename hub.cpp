//
// Created by christopher on 2/15/19.
//

#include "define.h"
#include "force_patch.h"
#include <iostream>
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

pid_t getpid() {
    int i;
    while (!(std::cin >> i)) {
        std::cout << "Invalid entry, try again. ";
        std::cin.clear();
        std::cin.ignore();
    }
    return i;
}
void a_route(int qid){
    message_buffer msg{alpha};
    if (msg.mtype == 997){
        msg.mtype = 2;
        strcpy(msg.message, "Recieved from ProbeA");
		msgsnd(qid, &msg, size,0);
    }
}
void b_route(int qid, pid_t pid) {
    // TODO: Ask professor if message count and printing is shared across probes.
    message_buffer msg{beta};
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

int main() {
    // Create message queue shared by all 3 hubs
    queue q{};
    std::cout << "Make sure to initialize the other probes now before entering "
                 "anything in.\n";

    // Get pid so that the program can force quit Probe B
    std::cout << "Enter the PID of Probe B: ";
    pid_t pid = getpid();

    // a_route(q.qid);
    b_route(q.qid, pid);
    // c_route(q.qid);
}
