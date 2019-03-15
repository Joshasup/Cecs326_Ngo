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
    int acknowledge = 0;

    message_buffer msg{shared_mtype};

    while (true) {
        // TODO: Receive acknowledgement from DataHub
        const int randomNum = rand();
        if (randomNum < 100) {
            std::cout << "Generated a number less than 100. Exiting.";
            break;
        } else if (acknowledge == 1 &&
                   valid_reading(randomNum, alpha)) {
            strncpy(msg.message, "ProbeA message", sizeof(msg.message));
            msgsnd(qid, &msg, msg_size, 0);
        }
    }
}
