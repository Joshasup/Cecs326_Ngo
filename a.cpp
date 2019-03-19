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
        const int randomNum = rand();

        // Send a final acknowledgement to the DataHub that the program has
        // generated a random number less than 100.
        // TODO: OR implement a timeout system in the DataHub when ProbeA takes
        // too long to send a message back to the DataHub
        if (randomNum < 100) {
            std::cout << "Generated a number less than 100. Exiting.";
            msgsnd(qid, &msg, msg_size, 0);
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
