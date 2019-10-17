#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

struct send
{
    long type;
    char text[256];
};

struct itimerval timer, timerinfo;

int create_queue(key_t key)
{ 
    int qid;

    if((qid = msgget(key, IPC_CREAT)) == -1)
    {
        cerr << "Can't create queue! Exiting..." << endl;
        return(-1);
    }    

    return(qid);
}

void wait(int seconds)
{
  clock_t endwait;
  endwait = clock () + seconds * CLOCKS_PER_SEC;
  while (clock() < endwait) {}
} 

void *thread_activity(void *args)
{
    int t;
    do
    {
        getitimer(ITIMER_REAL, &timerinfo);
        t = timerinfo.it_value.tv_sec;
        cout << "Waiting: " << t << " [s] left." << endl;
        wait(2);
    } 
    while (t != 0);
    pthread_exit(0);
}

/* Параметр: таймаут, ключ очереди */
int main(int argc, char const *argv[])
{
    int timeout = atoi(argv[1]);
    key_t key = atoi(argv[2]);
    int qid = create_queue(key);
    char text[256];
    bool flag = false;
    send msg;
    pthread_t threadID;
    pthread_attr_t threadAttr;
    msg.type = 1;
    

    timer.it_value.tv_sec = timeout;
    timer.it_interval.tv_sec = timeout;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1)
    {
        cerr << "Can't handle with ITIMER_VIRTUAL!" << endl;
        return -1;
    }
    while (true)
    {
        if (!flag)
        {
            if ((pthread_attr_init(&threadAttr)) != 0)
            {
                cerr << "Can't initialize attributes! Exiting..." << endl;
                return -1;
            }
            if ((pthread_create(&threadID,&threadAttr,thread_activity,NULL)) != 0)
            {
                cerr << "Can't create thread! Exiting..." << endl;
                return -1;
            }
            flag = true;
        }
        
        if(msgrcv(qid, &text, sizeof(msg.text), 0, 0) != -1) 
        {
            struct itimerval zero_timer = { 0 };
            if (setitimer(ITIMER_REAL, &zero_timer, &timer))
            {
                cerr << "Can't handle with ITIMER_REAL!" << endl;
                return -1;
            }
            cout << "Received Message! It is: " << msg.text << endl;
            if (setitimer(ITIMER_REAL, &timer, NULL) == -1)
            {
                cerr << "Can't handle with ITIMER_REAL!" << endl;
                return -1;
            }
        }
    }
    pthread_join(threadID,NULL);
    return 0;
}
