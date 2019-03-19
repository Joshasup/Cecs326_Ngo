#include "define.h"
#include <cstring>
#include <iostream>
#include <random>
#include <sys/ipc.h>
#include <sys/msg.h>

constexpr auto alpha = 997;

using namespace std;

int main() {
    srand(random_device{}());
    int qid = msgget(ftok(".", 'u'), 0);

    message_buffer msg{shared_mtype};

    while (true) {
        // TODO: Receive acknowledgement from DataHub
        const int randomNum = rand();
        if (randomNum < 100) {
            std::cout << "Generated a number less than 100. Exiting.";
            break;
        } else if (valid_reading(randomNum, 997)) {

            msgsnd(qid, &msg, msg_size, 0);
            msgrcv(qid, &msg, msg_size, 2, 0);
            msg.message_type = 997;
            strncpy(msg.message, "ProbeA message", sizeof(msg.message));
        }
    }

    msg.message_type = 1;
    strcpy(msg.message, "ProbeA Closed");
    msgsnd(qid, &msg, msg_size, 0);

    return 0;
}
