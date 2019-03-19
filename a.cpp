#include "define.h"
#include <cstring>
#include <iostream>
#include <random>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
//using namespace std;

int main() {
    int qid = msgget(1234, IPC_CREAT | 0666);
    std::cout<<"hi";
    int randomNum = rand();
    //std::cout << randomNum;
    //srand(random_device{}());
    std::cout<< qid << "Hi";
    message_buffer msg{alpha};

    while (true) 
    {
        // TODO: Receive acknowledgement from DataHub
        int randomNum = rand();
        if (randomNum < 100) 
        {
            std::cout << "Generated a number less than 100. Exiting.";
            break;
        } 
        else if (valid_reading(randomNum, 997))
        {
    
                std::cout<<"error";

            //std::cout << "greetings";
            msgsnd(qid, &msg, msg_size, IPC_NOWAIT);
            msgrcv(qid, &msg, msg_size, 2, 0);
            msg.message_type = 997;
            strncpy(msg.message, "ProbeA message", sizeof(msg.message));
        }
    }
    msg.message_type = 1;
    strcpy(msg.message, "ProbeA Closed");
    //msgsnd(qid, &msg, msg_size, 0);
}
