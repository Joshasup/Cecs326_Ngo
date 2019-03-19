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
    long probe_a_mtype = 2;
    bool acknowledged = true;

    while (true) {
        if (acknowledged) {
            std::cout << "Acknowledged.\n";
            while (true) {
                int randomNum = rand();
                if (randomNum < 100) {
                    std::cout << "Generated a number less than 100. Exiting.\n";
                    strncpy(msg.message, "TERM", sizeof(msg.message));
                    msg.message_type = shared_mtype;
                    msgsnd(qid, &msg, msg_size, 0);
                    exit(0);
                } else if (valid_reading(randomNum, alpha)) {
                    // Send message now that the random number is valid.
                    snprintf(msg.message, sizeof(msg.message), "%i", randomNum);
                    msg.message_type = shared_mtype;
                    msgsnd(qid, &msg, msg_size, 0);
                    break;
                }
            }
        }

        // Receive acknowledgements from the DataHub using msgid 2
        msg.message_type = probe_a_mtype;
        auto ret = msgrcv(qid, &msg, msg_size, probe_a_mtype, IPC_NOWAIT);
        acknowledged = !(ret == -1);
    }

    msg.message_type = 1;
    strcpy(msg.message, "ProbeA Closed");
    msgsnd(qid, &msg, msg_size, 0);

    return 0;
}

