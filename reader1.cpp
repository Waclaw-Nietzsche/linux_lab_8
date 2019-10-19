#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <time.h>  
#include <sys/wait.h>
#include <fstream>
#include <string>
#include <signal.h>

using namespace std;

struct req
{
    long type; // Для кого посылка с запросом
    int sender;
    int qid; // Номер очереди отправителя
    struct tm tm;
} request[4];

struct ans
{
    long type; // Кому отвечаем
    int prognum; // Номер ответчика
    struct tm tm;
} answer[4];

/* Параметры: номер программы, время ожидания */
int main(int argc, char const *argv[])
{
    struct msqid_ds buf;
    time_t T = time(NULL);
    struct tm tim;
    bool flag = false, flag1 = false, flag2 = false;
    key_t que = ftok("reader", 1);
    key_t key1 = ftok("reader", 2);
    key_t key2 = ftok("reader", 3);
    key_t key3 = ftok("reader", 4);
    int programm = atoi(argv[1]);

    int qid_r, qid_a;

    if (msgget(que, 0666 | IPC_CREAT | IPC_EXCL) != -1)
    {
        cout << programm << " created the queue q_r!" << endl;
        flag = true;
    }
    if((qid_r = msgget(que, 0666 | IPC_CREAT)) == -1)
    {
        cerr << "Can't access 'qid_r' queue! Exiting..." << endl;
        return(-1);
    } 
    else

    switch (programm)
    {
    case 1:
        {
            if((qid_a = msgget(key1, 0666 | IPC_CREAT)) == -1)
            {
                cerr << "Can't access 'qid_a' queue! Exiting..." << endl;
                return(-1);
            }  
            cout << programm << " created q_a" << endl;

            request[0].qid = qid_a;
            request[0].type = 2;
            request[0].sender = 1;
            request[0].tm = *localtime(&T);
            if(msgsnd(qid_r, &request[0], sizeof(request[0]), 0) == -1) 
            {
                cerr << "Can't send message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " sent request to " << request[0].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;

            request[1].qid = qid_a;
            request[1].type = 3;
            request[1].sender = 1;
            request[1].tm = *localtime(&T);
            
            if(msgsnd(qid_r, &request[1], sizeof(request[1]), 0) == -1) 
            {
                cerr << "Can't send message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " sent request to " << request[1].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            

            if (msgrcv(qid_r, &request[2], sizeof(request[2]), 1, 0) == -1)
            {
                cerr << "Can't get message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " got request from " << request[2].sender << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;

            if ((request[2].tm.tm_hour < request[0].tm.tm_hour) || ((request[2].tm.tm_min < request[0].tm.tm_min)) || ((request[2].tm.tm_sec <= request[0].tm.tm_sec)))
            {
                flag1 = true;
                answer[0].prognum = 1;
                answer[0].type = request[2].sender;
                answer[0].tm = *localtime(&T);
                if(msgsnd(request[2].qid, &answer[0], sizeof(answer[0]), 0) == -1) 
                {
                    cerr << "Can't send message! Exiting..." << endl;
                    return(-1);
                }
                tim = *localtime(&T);
                cout << programm << " sent answer to " << answer[0].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
                
            }
            
            if (msgrcv(qid_r, &request[3], sizeof(request[3]), 1, 0) == -1)
            {
                cerr << "Can't get message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " got request from " << request[3].sender << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;

            if ((request[3].tm.tm_hour < request[1].tm.tm_hour) || ((request[3].tm.tm_min < request[1].tm.tm_min)) || ((request[3].tm.tm_sec <= request[1].tm.tm_sec)))
            {
                flag2 = true;
                answer[1].prognum = 1;
                answer[1].type = request[3].sender;
                answer[1].tm = *localtime(&T);
                if(msgsnd(request[3].qid, &answer[1], sizeof(answer[1]), 0) == -1) 
                {
                    cerr << "Can't send message! Exiting..." << endl;
                    return(-1);
                }
                tim = *localtime(&T);
                cout << programm << " sent answer to " << answer[1].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            }
            

            if (msgrcv(qid_a, &answer[2], sizeof(answer[2]), 1, 0) == -1)
            {
                cerr << "Can't get message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " got answer from " << answer[2].prognum << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;

            if (msgrcv(qid_a, &answer[3], sizeof(answer[3]), 1, 0) == -1)
            {
                cerr << "Can't get message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " got answer from " << answer[3].prognum << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            
            ifstream file("test.txt", ios::in);
            while(!file.eof())
            {
                string str;
                getline(file, str);
                cout << str << endl;
            }
            file.close();
            

            if (!flag2)
            {
                answer[0].prognum = 1;
                answer[0].type = request[3].sender;
                answer[0].tm = *localtime(&T);

                if(msgsnd(request[3].qid, &answer[0], sizeof(answer[0]), 0) == -1) 
                {
                    cerr << "Can't send message! Exiting..." << endl;
                }
                tim = *localtime(&T);
                cout << programm << " sent answer to " << answer[0].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            }
            if(!flag1)
            {        
                answer[1].prognum = 1;
                answer[1].type = request[2].sender;
                answer[1].tm = *localtime(&T);
                    
                if(msgsnd(request[2].qid, &answer[1], sizeof(answer[1]), 0) == -1) 
                {
                    cerr << "Can't send message! Exiting..." << endl;
                }
                tim = *localtime(&T);
                cout << programm << " sent answer to " << answer[1].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            }
            
            cout << "Deleting q_a_1." << endl;
            msgctl(qid_a, IPC_RMID, NULL);            
            break;
        }
    case 2:
        {
            if((qid_a = msgget(key2, 0666 | IPC_CREAT)) == -1)
            {
                cerr << "Can't access 'qid_a' queue! Exiting..." << endl;
                return(-1);
            }  
            cout << programm << " created q_a_2" << endl;

            request[0].qid = qid_a;
            request[0].type = 1;
            request[0].sender = 2;
            request[0].tm = *localtime(&T);
            if(msgsnd(qid_r, &request[0], sizeof(request[0]), 0) == -1) 
            {
                cerr << "Can't send message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " sent request to " << request[0].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;

            request[1].qid = qid_a;
            request[1].type = 3;
            request[1].sender = 2;
            request[1].tm = *localtime(&T);
            if(msgsnd(qid_r, &request[1], sizeof(request[1]), 0) == -1) 
            {
                cerr << "Can't send message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " sent request to " << request[1].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            

            if (msgrcv(qid_r, &request[2], sizeof(request[2]), 2, 0) == -1)
            {
                cerr << "Can't get message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " got request from " << request[2].sender << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;

            if ((request[2].tm.tm_hour < request[0].tm.tm_hour) || ((request[2].tm.tm_min < request[0].tm.tm_min)) || ((request[2].tm.tm_sec <= request[0].tm.tm_sec)))
            {
                flag1 = true;
                answer[0].prognum = 2;
                answer[0].type = request[2].sender;
                answer[0].tm = *localtime(&T);
                if(msgsnd(request[2].qid, &answer[0], sizeof(answer[0]), 0) == -1) 
                {
                    cerr << "Can't send message! Exiting..." << endl;
                    return(-1);
                }
                tim = *localtime(&T);
                cout << programm << " sent answer to " << answer[0].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
                
            }
            
            if (msgrcv(qid_r, &request[3], sizeof(request[3]), 2, 0) == -1)
            {
                cerr << "Can't get message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " got request from " << request[3].sender << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;

            if ((request[3].tm.tm_hour < request[1].tm.tm_hour) || ((request[3].tm.tm_min < request[1].tm.tm_min)) || ((request[3].tm.tm_sec <= request[1].tm.tm_sec)))
            {
                flag2 = true;
                answer[1].prognum = 2;
                answer[1].type = request[3].sender;
                answer[1].tm = *localtime(&T);
                if(msgsnd(request[3].qid, &answer[1], sizeof(answer[1]), 0) == -1) 
                {
                    cerr << "Can't send message! Exiting..." << endl;
                    return(-1);
                }
                tim = *localtime(&T);
                cout << programm << " sent answer to " << answer[1].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            }
            

            if (msgrcv(qid_a, &answer[2], sizeof(answer[2]), 2, 0) == -1)
            {
                cerr << "Can't get message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " got answer from " << answer[2].prognum << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;

            if (msgrcv(qid_a, &answer[3], sizeof(answer[3]), 2, 0) == -1)
            {
                cerr << "Can't get message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " got answer from " << answer[3].prognum << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            
            ifstream file("test.txt", ios::in);
            while(!file.eof())
            {
                string str;
                getline(file, str);
                cout << str << endl;
            }
            file.close();
            
            if (!flag2)
            {
                answer[0].prognum = 2;
                answer[0].type = request[3].sender;
                answer[0].tm = *localtime(&T);
                if(msgsnd(request[3].qid, &answer[0], sizeof(answer[0]), 0) == -1) 
                {
                    cerr << "Can't send message! Exiting..." << endl;
                }
                tim = *localtime(&T);
                cout << programm << " sent answer to " << answer[0].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            }
            if(!flag1)  
            {      
                answer[1].prognum = 2;
                answer[1].type = request[2].sender;
                answer[1].tm = *localtime(&T);
                if(msgsnd(request[2].qid, &answer[1], sizeof(answer[1]), 0) == -1) 
                {
                    cerr << "Can't send message! Exiting..." << endl;
                }
                tim = *localtime(&T);
                cout << programm << " sent answer to " << answer[1].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            }
            
            cout << "Deleting q_a_2." << endl;
            msgctl(qid_a, IPC_RMID, NULL);           
            break;
        }
    case 3:
        {
            if((qid_a = msgget(key3, 0666 | IPC_CREAT)) == -1)
            {
                cerr << "Can't access 'qid_a' queue! Exiting..." << endl;
                return(-1);
            }  
            cout << programm << " created q_a_3" << endl;

            request[0].qid = qid_a;
            request[0].type = 1;
            request[0].sender = 3;
            request[0].tm = *localtime(&T);
            if(msgsnd(qid_r, &request[0], sizeof(request[0]), 0) == -1) 
            {
                cerr << "Can't send message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " sent request to " << request[0].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;

            request[1].qid = qid_a;
            request[1].type = 2;
            request[1].sender = 3;
            request[1].tm = *localtime(&T);
            if(msgsnd(qid_r, &request[1], sizeof(request[1]), 0) == -1) 
            {
                cerr << "Can't send message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " sent request to " << request[1].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            

            if (msgrcv(qid_r, &request[2], sizeof(request[2]), 3, 0) == -1)
            {
                cerr << "Can't get message! Exiting..." << endl;
                return(-1);
            }
            cout << programm << " got request from " << request[2].sender << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;

            if ((request[2].tm.tm_hour < request[0].tm.tm_hour) || ((request[2].tm.tm_min < request[0].tm.tm_min)) || ((request[2].tm.tm_sec <= request[0].tm.tm_sec)))
            {
                flag1 = true;
                answer[0].prognum = 3;
                answer[0].type = request[2].sender;
                answer[0].tm = *localtime(&T);
                if(msgsnd(request[2].qid, &answer[0], sizeof(answer[0]), 0) == -1) 
                {
                    cerr << "Can't send message! Exiting..." << endl;
                    return(-1);
                }
                tim = *localtime(&T);
                cout << programm << " sent answer to " << answer[0].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
                
            }
            
            if (msgrcv(qid_r, &request[3], sizeof(request[3]), 3, 0) == -1)
            {
                cerr << "Can't get message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " got request from " << request[3].sender << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;

            if ((request[3].tm.tm_hour < request[1].tm.tm_hour) || ((request[3].tm.tm_min < request[1].tm.tm_min)) || ((request[3].tm.tm_sec <= request[1].tm.tm_sec)))
            {
                flag2 = true;
                answer[1].prognum = 3;
                answer[1].type = request[3].sender;
                answer[1].tm = *localtime(&T);
                if(msgsnd(request[3].qid, &answer[1], sizeof(answer[1]), 0) == -1) 
                {
                    cerr << "Can't send message! Exiting..." << endl;
                    return(-1);
                }
                tim = *localtime(&T);
                cout << programm << " sent answer to " << answer[1].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            }
            

            if (msgrcv(qid_a, &answer[2], sizeof(answer[2]), 3, 0) == -1)
            {
                cerr << "Can't get message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " got answer from " << answer[2].prognum << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;

            if (msgrcv(qid_a, &answer[3], sizeof(answer[3]), 3, 0) == -1)
            {
                cerr << "Can't get message! Exiting..." << endl;
                return(-1);
            }
            tim = *localtime(&T);
            cout << programm << " got answer from " << answer[3].prognum << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            
            ifstream file("test.txt", ios::in);
            while(!file.eof())
            {
                string str;
                getline(file, str);
                cout << str << endl;
            }
            file.close();
            
            
            if (!flag2)
            {
                answer[0].prognum = 3;
                answer[0].type = request[3].sender;
                answer[0].tm = *localtime(&T);
                if(msgsnd(request[3].qid, &answer[0], sizeof(answer[0]), 0) == -1) 
                {
                    cerr << "Can't send message! Exiting..." << endl;
                }
                tim = *localtime(&T);
                cout << programm << " sent answer to " << answer[0].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            }  
            if(!flag1) 
            {     
                answer[1].prognum = 3;
                answer[1].type = request[2].sender;
                answer[1].tm = *localtime(&T);
                if(msgsnd(request[2].qid, &answer[1], sizeof(answer[1]), 0) == -1) 
                {
                    cerr << "Can't send message! Exiting..." << endl;
                }
                tim = *localtime(&T);
                cout << programm << " sent answer to " << answer[1].type << " at " << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << endl;
            }
            
            cout << "Deleting q_a_3." << endl;
            msgctl(qid_a, IPC_RMID, NULL);           
            break;
        }    
    default:
        {
            cerr << "Wrong input!" << endl;
            return(-1);
            break;
        }
        
    }
    
    if (flag)
    {
        cout << "Deleting q_r." << endl;
        msgctl(qid_r, IPC_RMID, NULL);
    }

    return 0;
}
