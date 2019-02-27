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
            //delete
        }

        else if ((acknowledge = 1) && ((randomNum%msg.mtype)== 0))
        {
            //send
            
        }
    }
}
