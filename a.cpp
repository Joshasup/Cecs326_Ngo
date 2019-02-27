#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <random>

using namespace std;

int main(){

    int qid = msgget(ftok(".",'u'),0);
    int acknowledge = 0;
    struct buf {
        long mtype;
        char message[50];

    }

    buf msg;
    int size = sizeof(msg) - sizeof(long);
    msg.mtype = 997;

    while (true){

        int randomNum = rand();
        if (randomNum < 100){
            break;
        }

        else if ((acknowledge = 1) && ((randomNum%msg.mtype)== 0))
        {
            msg.mtype = 997;
            strcpy(msg.greeting, "ProbeA message");
            msgsnd(qid,(struct msgbuf *)&msg,size, 0);

        }
    }
}
