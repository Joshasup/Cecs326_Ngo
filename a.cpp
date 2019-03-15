#include "define.h"
#include <cstring>
#include <iostream>
#include <random>
#include <sys/ipc.h>
#include <sys/msg.h>

using namespace std;

int main() {
    srand(random_device{}());
    int qid = msgget(ftok(".", 'u'), 0);


    message_buffer msg{alpha};

    while (true) {
        // TODO: Receive acknowledgement from DataHub
        int randomNum = rand();
        if (randomNum < 100) {
            std::cout << "Generated a number less than 100. Exiting.";
            break;
        } else if (valid_reading(randomNum, 997)) {
            
            msgsnd(qid, &msg, msg_size, 0);
            msgrcv(qid,&msg, msg_size, 2, 0);
            msg.mtype = 997;
            strncpy(msg.message, "ProbeA message", sizeof(msg.message));
        }
    }
    
    	msg.mtype=1;													
	strcpy(msg.message,"ProbeA Closed");
	msgsnd(qid, &msg, size,0);
    
    return 0;
}
