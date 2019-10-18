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
} message;

struct itimerval timer, timerinfo;
pthread_t threadID;
pthread_attr_t threadAttr;
int qid;
int timeout;


int create_queue(key_t key)
{ 
    int qid;

    if((qid = msgget(key, 0666 | IPC_CREAT)) == -1)
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
        wait(1);
    } 
    while (t != 0);
    pthread_exit(0);
}

/*Функция-обработчик для SIGALRM*/
void timer_handler(int signal, siginfo_t *siginfo, void *context)
{
    pthread_join(threadID,NULL);
    msgctl(qid, IPC_RMID, NULL);
    exit(0);
}

/* Параметр: таймаут */
int main(int argc, char const *argv[])
{
    timeout = atoi(argv[1]);
    key_t key = ftok("wait", 5);
    qid = create_queue(key);
    cout << qid << endl;
    bool flag, flag1 = false;
    
    struct sigaction time_signal;
    struct itimerval zero_timer;

    time_signal.sa_sigaction = &timer_handler;
    time_signal.sa_flags = SA_SIGINFO;
    
    zero_timer.it_value.tv_sec = 0;
    zero_timer.it_interval.tv_sec = 0;
    zero_timer.it_value.tv_usec = 0;
    zero_timer.it_interval.tv_usec = 0;

    timer.it_value.tv_sec = timeout;
    timer.it_interval.tv_sec = timeout;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_usec = 0;

    if (sigaction(SIGALRM, &time_signal, NULL) == -1)
    {
        cerr << "Can't handle with SIGVTALRM!" << endl;
    }

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
        if (!flag1)
        {
            if(msgrcv(qid, &message, sizeof(message), 1, 0) != -1) 
            {
                if (setitimer(ITIMER_REAL, &zero_timer, &timer))
                {
                    cerr << "Can't handle with ITIMER_REAL!" << endl;
                    return -1;
                }
                string temp = string(message.text);
                cout << "Received Time! New time is: " << temp << endl;
                timeout = stoi(temp);
                timer.it_value.tv_sec = timeout;
                timer.it_interval.tv_sec = timeout;
                if (setitimer(ITIMER_REAL, &timer, NULL) == -1)
                {
                    cerr << "Can't handle with ITIMER_REAL!" << endl;
                    return -1;
                }
            }
            flag1 = true;
        }
        else
        {
            if(msgrcv(qid, &message, sizeof(message), 1, 0) != -1) 
            {
                if (setitimer(ITIMER_REAL, &zero_timer, &timer))
                {
                    cerr << "Can't handle with ITIMER_REAL!" << endl;
                    return -1;
                }
                cout << "Received Message! It is: " << message.text << endl;
                timer.it_value.tv_sec = timeout;
                timer.it_interval.tv_sec = timeout;
                if (setitimer(ITIMER_REAL, &timer, NULL) == -1)
                {
                    cerr << "Can't handle with ITIMER_REAL!" << endl;
                    return -1;
                }
            }
        }
        
        
    }
    return 0;
}
