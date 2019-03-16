#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <random>

#include "kill_patch.h"

//generate random value
//if rand val is divisible by magic seed -> message sent to the hub
//does not receive any acknowledgment from the hub
//user uses a kill command in a sepearte terminal to kill this probe

int main() {
    int qid = msgget(ftok(".",'u'), 0);

    struct buf{
        long mtype;
        char message[50];
    };

    buf msg;
    int size = sizeof(msg) - sizeof(long);
    msg.mtype = 257; //magic seed
    kill_patch(qid, &msg, size, 251);

    while(true){
        int randNum = rand();
        int div = randNum % msg.mtype;

        if (randNum == 0){
            msg.mtype = 251;
            strncopy(msg.message, "ProbeC");
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
    }
    return 0;
}