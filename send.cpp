#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


using namespace std;

struct send
{
    long type;
    char text[256];
} message;

/* Параметр: ключ очереди */
int main(int argc, char const *argv[])
{
    key_t key = ftok("wait", 5);
    int qid;
    message.type = 1;

    if ((qid = msgget(key, 0666 | IPC_CREAT | IPC_EXCL)) != -1)
    {
        qid = msgget(key, 0666 | IPC_CREAT);
        msgctl(qid, IPC_RMID, NULL);
        cerr << "The first programm is not opened! Exiting..." << endl;
        return -1;
    }
    else
    {
        qid = msgget(key, IPC_CREAT);
        cout << qid << endl;
        cout << "Please enter the message: " << endl;
        cin >> message.text;
        if(msgsnd(qid, &message, sizeof(message), IPC_NOWAIT) == -1) 
        {
            cerr << "Can't send message! Exiting..." << endl;
            return -1;
        }
    }
    
    return 0;
}
