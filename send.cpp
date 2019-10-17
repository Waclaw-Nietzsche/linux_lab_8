#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


using namespace std;

struct send
{
    long type;
    char text[256];
};

/* Параметр: ключ очереди */
int main(int argc, char const *argv[])
{
    key_t key = atoi(argv[1]);
    int qid;
    send msg;
    msg.type = 1;
    char message[256];

    if ((qid = msgget(key, IPC_CREAT)) == -1)
    {
        cerr << "The first programm is not opened! Exiting..." << endl;
        return -1;
    }
    else
    {
        cout << "Please enter the message: " << endl;
        cin >> message;
        if(msgsnd(qid, (void *)&msg, sizeof(msg.text), IPC_NOWAIT) == -1) 
        {
            cerr << "Can't send message! Exiting..." << endl;
            return -1;
        }
    }
    
    return 0;
}
