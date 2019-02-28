#include "define.h"
#include <cstring>
#include <random>
#include <sys/ipc.h>
#include <sys/msg.h>

using namespace std;

int main() {
    srand(random_device{}());
    int qid = msgget(ftok(".", 'u'), 0);
    int acknowledge = 0;
    struct buf {
        long mtype;
        char message[50];
    };

    buf msg{};
    int size = sizeof(msg) - sizeof(long);
    msg.mtype = alpha;

    while (true) {
        int randomNum = rand();
        if (randomNum < 100) {
            break;
        } else if (acknowledge == 1 && valid_reading(randomNum, msg.mtype)) {
            msg.mtype = 997;
            strcpy(msg.message, "ProbeA message");
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
    }
}
