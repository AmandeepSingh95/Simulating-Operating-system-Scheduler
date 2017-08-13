#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/msg.h>
/* 
1. root pid dekha ?...got it from the sched after pause ?
coz only for first time i.e change kar sakte he proc


*/

#define MAX 1024
#define KEY 130
// Nomenclature
/// SIGUSR1 :-  SUSPEND
//  SIGUSR2 :- NOTIFY 
typedef struct message
{

	long type;
	char data[MAX];

}message;
int counter=0,iterations=0,flag=0,i=0;       // to keep the count of which loop is running and hence also the number of iterations left
int root_pid;    // this will be the pid of sched.c 
//the data that was passed
int noi,prior,sleeptime,msqid;
int prob;
pid_t proc_pid;


void notify();
void suspend();
void iterate();
message msg;
int flg=0;

int main( int argc,char *argv[]  )
{
	//printf("yolo how are you the value of argc is %d and argv is %s\n",argc,argv[1]);
	srand(time(NULL)); // seeding time
	key_t key=130;
	
	int pid;
	char buff[MAX],buffer[MAX];
	proc_pid=getpid();

	
	// print as in question
	printf("PID %d\n",proc_pid);
	//the signal handlers notify and suspend 

	signal(SIGUSR1,suspend);
	signal(SIGUSR2,notify);

	// get the data that has been passed
	// char *token;
	// char s[2]=' ';
	// token=strtok(s,)

	if(argc!=5)
	{
		printf("error in the number of passed parameters\n");
		exit(0);
	}	

	noi=atoi(argv[1]);
	prior=atoi(argv[2]);
	prob=atoi(argv[3]);
	sleeptime=atoi(argv[4]);
	printf("In process.c noi%d prior=%d prob=%d slptime=%d \n",noi,prior,prob,sleeptime);
	iterations=noi;
	//prob=prob*100;

	//printf("the value of noi is %d\n",noi );

	//signal the scheduler of the process creation via the message queue
    // send the pid and priority <space separated>
	msqid=msgget(key,IPC_CREAT|0644);
	sprintf(buffer,"%d ",proc_pid);
	sprintf(buff,"%d",prior);   
	strcat(buffer,buff);
	strcat(msg.data,buffer);
	printf("sending message %s\n",msg.data);
	msg.type=1;            // type 1 in message queue will be for the NEW process added 

	int stat=msgsnd(msqid,&msg,MAX,0);
	if(stat<0)
		printf("failed\n");


	printf("now waiting for sched pid\n");


	// get the pid of sched.c
	msgrcv(msqid,&msg,MAX,proc_pid,MSG_NOERROR); // WAIT untill recv message
	
	root_pid=atoi(msg.data);
	printf("Sched ki pid recved %d \n",root_pid);



	// sched notifiedn
	// wait for the notify signal
	printf("waiting for notify\n");
	pause();  // flag will be 0 do nothing 
	flag=1;
	

	
	
	iterate();

	/*while(i!=iterations)
	{
		pause();
	}
*/

	return 0;
}


void iterate()
{
	printf("starting iterations\n");
	int pb,stat;
	for(i=counter;i<iterations;i++,counter++)
	{
		
		while(flg==1)
		{
			sleep(1);
		}

		pb=1 + rand()%100;
		printf("the value of pb is %d\n",pb );
		if(pb<=prob) 
		{

			// io request signal to be sent sched.c
			kill(root_pid,SIGUSR1);  // SIGUSR1 -> i/o req
									//since only one toh sched me flag rakhna padega

			printf("PID: %d Going for IO  at i =%d value of iter %d \n",proc_pid,i,iterations );
			sleep(sleeptime);

			// signal tohe wake to sched ?? how ..send it again
			//har ke corres ek flag rakha jayega

			sprintf(msg.data,"%d",proc_pid);
			msg.type=2;
			stat=msgsnd(msqid,&msg,MAX,0);
			if(stat==-1)
				printf("not sent\n");

			// now pause for a notify signal from shed
			//printf("awake pause se pehle");
			pause();
			printf("waking\n");
			
		}

	}

	if(i==iterations)
	{
		//termination of the process
		kill(root_pid,SIGUSR2); // SIGUSSR2-> TERMINATION
	}
}
// notify signal comes 
void notify()
{
	printf("notified received \n");
	flg=0;
	signal(SIGUSR1,suspend);
}

void suspend()
{
	signal(SIGUSR2,notify);
	printf("being suspended by shed\n");
	//pause();
	//
	counter=i;
	flg=1;
}