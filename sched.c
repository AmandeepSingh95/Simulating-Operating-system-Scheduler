#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
/*
1> every (1) sec pe 4 process leni he how?? 
<1 sec hone pe suspend jo chal rhi he and get new pids ?? >
any better wa

---2> msgget me del remove if existing 

3> find max value /..i.e prior wise ka pid...first occurence
4> check if linked list is empty or not
... add entire node data at the end
procs
termination pe file me add

how will i know ki io se konsa awake
so send me the pid in message queue

>.. time ka funda set karna he

// itialize node values ke liye func
//total run time ko 0
//flag for first ko 0 

*/

// Nomenclature
/// SIGUSR1 :- IO req
//  SIGUSR2 :- Terminate 
#define MAX 1024
#define KEY 130

int flag=0;  // flag=0 => RR
			 // flag=1 => PR 
int termination=0;
typedef struct message
{

	long type;
	char data[MAX];

}message;
double a,b,c;
double a1=0,b1=0,c1=0;
struct node
{
	int pid;
	int priority;
	//int flag;//
	//clock_t intime; //
	//clock_t ftime; //
	//clock_t outtime;//
	double total_runtime;
	struct node *next;
};
typedef struct tim
{
	int flag;//
	clock_t intime; //
	clock_t ftime; //
	clock_t outtime;//
}tim;
tim aa[100000];
struct node tempnode;
struct node running2;
clock_t start,end;
int flg=0;
double cpu_time_used;
//intialize my linked list
struct node* readyq=NULL;
struct node* waitingq=NULL;

message msg;
int tmquanta,msqid,killed=0,io=0;
void iorequest(); //delete that entry<no need> <already deleted kyunki running he>
				 // add in waiting queue
void terminate(); //delete that entry and file may stuff "append" daldo
void append(struct node** head_ref);
struct node running; 
void deletefirstNode(struct node **head_ref);
void deleteNode(struct node **head_ref, int key);
int getmax(struct node *nod);
void check();

void printList(struct node *node)
{
	printf("intial list :- \n");
  while (node != NULL)
  {
     printf(" %d ", node->pid);
     node = node->next;
  }

  printf("\n");
}
int main(int argc,char *argv[])
{
	key_t key=130;
	//int msqid;

	char buff[MAX],buffer[MAX];

	if(argc!=2)
	{
		printf("Invalid number of arguements\n");
		exit(0);	
	}


	if(strcmp(argv[1],"RR")==0)
	{
		flag=0;
		tmquanta=1000;
	}
	else if(strcmp(argv[1],"PR")==0)
	{
		flag=1;
		tmquanta=2000;
	}	
	else
	{
		printf("Invalid Entry please try again..\n");
		exit(0);
	}


	signal(SIGUSR1,iorequest); // IO req
	signal(SIGUSR2,terminate); // terminate
	msqid=msgget(key,IPC_CREAT|0644);
	

	// read the four process inputed

	// do RR implementation first
	//get and schedule first process
	char *token;
	char s[2]=" ";
	int pid;
	int priority;
	int i,ko=0;

	printf("starting sched in 5sec \n");
	sleep(5);
	printf("scheduler start\n");
	if(flag==0)
	{
		for(i=0;i<4;i++)
		{
			//check;
			msgrcv(msqid,&msg,MAX,1,MSG_NOERROR);
			token=strtok(msg.data,s);
			pid=atoi(token);
			token=strtok(NULL,s);
			priority=atoi(token);
			
			printf("In RR sched.c pid %d priority %d \n",pid,priority);
			//set it \m/
			//set the in time everywhere!!! to start counting
			tempnode.pid=pid;
			tempnode.priority=priority;
			tempnode.total_runtime=0;
			aa[pid].flag=0;
			aa[pid].intime=clock();

			append(&readyq);// temp hi hoga
			// send ur pid -_-
			sprintf(msg.data,"%d",getpid());
			msg.type=pid;
			int stat=msgsnd(msqid,&msg,MAX,0);

			if(stat==-1)
				printf("NOT SENTTTTTTTTTTTTTT!!!\n");

			if(stat>=0)
				printf("message sent to pid %d containing type %d and data %s \n",pid,msg.type,msg.data);
		}

		printList(readyq);

		sleep(7);
		printf("all should have received my pid\n");
//int stuff=2000;
		while(termination!=4)  // linked list not empty
		{printf("\n\n\n");
			//if(readyq!=NULL)
			{//stuff--;
				//if(stuff==0)
				//	exit(0);
				printf("new loop\n");
				killed=0;io=0;
				deletefirstNode(&readyq);

				printf("after deletion \n");
				printList(readyq);
				pid=running.pid;
				printf("run pid %d \n",pid );
				if(aa[pid].flag==0)
				{
					aa[pid].ftime=clock();
					aa[pid].flag=1;
				}

				// signal it for running
				kill(pid,SIGUSR2);
				
				start=clock();
				usleep(500);
				for(i=0;i<tmquanta;i++)//busy waiting
				{
					if(killed!=0 || io!=0)
					{
						printf("cut short\n");
						break;
					}
				}
				end=clock();
				cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;
				running.total_runtime=running.total_runtime + cpu_time_used;
				if(killed==0 && io==0)
				{
					//time quanta expires signal to suspend ..add at the end
					printf(" %d your time quata has expired\n",pid );
					kill(pid,SIGUSR1); //suspend
					//addd entire node data at the end 3
					tempnode.pid=running.pid;
					tempnode.priority=running.priority;
					tempnode.total_runtime=running.total_runtime;
					append(&readyq);
				}
			}

			check();
		}


		printf(" list size 0 :P \n");

	}
	

	// doing the PR sched.

	else
	{
		//smart time
	
		//check;
		msgrcv(msqid,&msg,MAX,1,MSG_NOERROR);
		token=strtok(msg.data,s);
		pid=atoi(token);
		token=strtok(NULL,s);
		priority=atoi(token);
		printf("In RR sched.c pid %d priority %d \n",pid,priority);
		tempnode.pid=pid;
		tempnode.priority=priority;
		tempnode.total_runtime=0;
		aa[pid].flag=0;
		aa[pid].intime=clock();
		append(&readyq);
		// send ur pid -_-
		sprintf(msg.data,"%d",getpid());
		msg.type=pid;
		msgsnd(msqid,&msg,MAX,0);
		

		//sched it

		killed=0;io=0;
		deletefirstNode(&readyq);
		pid=running.pid;
		if(aa[pid].flag==0)
		{
			aa[pid].ftime=clock();
			aa[pid].flag=1;
		}
		// signal it for running
		start=clock();
		usleep(500);
		kill(pid,SIGUSR2);
		for(i=0;i<tmquanta;i++)//busy waiting
		{
			if(killed!=0 || io!=0)
			{
				break;
			}
		}
		end=clock();
		cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;
		running.total_runtime=running.total_runtime + cpu_time_used;
		if(killed==0 && io==0)
		{
			//time quanta expires signal to suspend ..add at the end
			kill(pid,SIGUSR1);
			tempnode.pid=running.pid;
			tempnode.priority=running.priority;
			tempnode.total_runtime=running.total_runtime;
			append(&readyq);
			//addd entire node data at the end 
		}
		
		sleep(1);
		for(i=0;i<3;i++)
		{
			//check;
			msgrcv(msqid,&msg,MAX,1,MSG_NOERROR);
			token=strtok(msg.data,s);
			pid=atoi(token);
			token=strtok(NULL,s);
			priority=atoi(token);
			printf("In RR sched.c pid %d priority %d \n",pid,priority);
			tempnode.pid=pid;
			tempnode.priority=priority;
			tempnode.total_runtime=0;
			aa[pid].flag=0;
			aa[pid].intime=clock();
			append(&readyq);
			// send ur pid -_-
			sprintf(msg.data,"%d",getpid());
			msg.type=pid;
			msgsnd(msqid,&msg,MAX,0);
		}

		sleep(5);
int kao=10;
		while(termination!=4)  // linked list not empty
		{
			kao--;
			if(kao==0)
				exit(0);
			printf("\n\n\n");
			if(readyq!=NULL)
			{


				printf("new loop\n");
				killed=0;io=0;
				//deletefirstNode(&readyq); 
				//1get max prior value ka pid in readyq
				pid=getmax(readyq);
				//2.sched that value wala ..meaning usko del
				printf("max priority pid is %d\n",pid );
				deleteNode(&readyq,pid);
				

				printf("after deletion \n");
				printList(readyq);
				pid=running.pid;
				if(aa[pid].flag==0)
				{
					aa[pid].ftime=clock();
					aa[pid].flag=1;
				}
				// signal it for running
				start=clock();
				usleep(500);
				kill(pid,SIGUSR2);

				printf("run pid %d \n",pid );

				for(i=0;i<tmquanta;i++)//busy waiting
				{
					if(killed!=0 || io!=0)
					{
						break;
					}
				}
				end=clock();
				cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;
				running.total_runtime=running.total_runtime + cpu_time_used;
				if(killed==0 && io==0)
				{
					//time quanta expires signal to suspend ..add at the end
					printf(" %d your time quata has expired\n",pid );
					kill(pid,SIGUSR1);
					//addd entire node data at the end 
					tempnode.pid=running.pid;
					tempnode.priority=running.priority;
					tempnode.total_runtime=running.total_runtime;
					append(&readyq);
				}
			}
			


			check();
		}
		
	}


	FILE *fp=fopen("result.txt","a");
	fprintf(fp,"average values of response time , waiting time and turnaround time");
	fprintf(fp,"\t\t%f\t%f\t%f\n",a1/4.0,b1/4.0,c1/4.0);
	fclose(fp); 

	return 0;
}

//appends a new node at the end 
void append(struct node** head_ref)
{
    /* 1. allocate node */
    struct node* new_node = (struct node*) malloc(sizeof(struct node));
 
    struct node *last = *head_ref;  
  
    /* 2. put in the data  */
    //new_node=tempnod;
    new_node->pid  = tempnode.pid;
    new_node->priority = tempnode.priority;
    new_node->total_runtime=tempnode.total_runtime;

 // it is the last node
    new_node->next = NULL;
 
    /* 4. If the Linked List is empty, then make the new node as head */
    if (*head_ref == NULL)
    {
       *head_ref = new_node;
       return;
    }  
      
    while (last->next != NULL)
        last = last->next;
  
    /* 6. Change the next of last node */
    last->next = new_node;
    return;    
}


void deletefirstNode(struct node **head_ref)
{
	int position=0;
   // If linked list is empty
   if (*head_ref == NULL)
      return;
 
   struct node* temp = *head_ref;

   //ASSIGN THE RUNNING PROCESS NODE
   if(io==0)
   	running=*temp;
 
    // If head needs to be removed
    if (position == 0)
    {
        *head_ref = temp->next;   // Change head
        free(temp);               // free old head
        return;
    }
 
    // Find previous node of the node to be deleted
    int i;
    for (i=0; temp!=NULL && i<position-1; i++)
         temp = temp->next;
 
    // If position is more than number of ndoes
    if (temp == NULL || temp->next == NULL)
         return;
 
    // Node temp->next is the node to be deleted
    // Store pointer to the next of node to be deleted
    struct node *next = temp->next->next;
 
    // Unlink the node from linked list
    free(temp->next);  // Free memory
 
    temp->next = next;  // Unlink the deleted node from list
}

// key will be the pid
void deleteNode(struct node **head_ref, int key)
{
    // Store head node
    struct node* temp = *head_ref, *prev;
 	
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->pid == key)
    {
    	if(flg==2)
    	{
    		running2=*temp;
    	}
    	else
    		running=*temp;
        *head_ref = temp->next;   // Changed head
        free(temp);               // free old head
        return;
    }
 
    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->pid != key)
    {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (temp == NULL) return;

    if(flg==2)
    	running2=*temp;
    else	
    	running=*temp; // assign temp to running 
 
    // Unlink the node from linked list
    prev->next = temp->next;
 
    free(temp);  // Free memory
}


int getmax(struct node *nod)
{
	int flag=0,max,pid;
	while (nod != NULL)
	{
		if(flag==0)
		{
			max=nod->priority;
			pid=nod->pid;
			flag=1;
		}
		if(nod->priority > max)
		{
			max=nod->priority;
			pid=nod->pid;
		}
		nod = nod->next;

	}
	return pid;
}



void iorequest()
{
	io=1;
	//add in io list after checking ki isne stop ke liye kiya he
	//please send only pid
	printf("iorequest raised..add to waitingq\n");
	
	//printf("adding to waitingq\n");
	// add to waitingq what running wala node
	//and it's data 
	int pid=running.pid;
// not doing it 
	 //  kill(pid,SIGUSR1); //suspend
	end=clock();
	cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;
	running.total_runtime=running.total_runtime + cpu_time_used;

	tempnode.pid=running.pid;
	tempnode.priority=running.priority;
	tempnode.total_runtime=running.total_runtime;

	append(&waitingq);	

}

void terminate()
{
	printf("termination of %d \n",running.pid );
	termination++;
	killed=1;
	cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;
	running.total_runtime=running.total_runtime + cpu_time_used;
	int pid=running.pid;
	//calculate stuff 
	// write stuff 
	// 3 global vars for final average

	aa[pid].outtime=clock();	
	a=((double)(aa[pid].ftime - aa[pid].intime ))/CLOCKS_PER_SEC;
	c=((double)(aa[pid].outtime - aa[pid].intime ))/CLOCKS_PER_SEC;
	b=(c - running.total_runtime );
	a1=a1+a;
	b1=b1+b;
	c1=c1+c;
	FILE *fp=fopen("result.txt","a");
	fprintf(fp,"Process PID\t\tResponseT\tWaitingT\tTurnaroundT\t(in seconds)\n");
	fprintf(fp,"%d\t\t\t%f\t%f\t%f\n",running.pid,a,b,c);
	fclose(fp); 
}
//get any sleep se awake wali process back to readyq from waiting given it's pid
void check()
{
	printf("checking for any awaking processses\n");
	int status=msgrcv(msqid,&msg,MAX,2,IPC_NOWAIT);
	if(status==-1)
	{
	//	printf("none found ...returniing \n");
		return;
	}
	int pi=atoi(msg.data);
	printf("process with pid %d is waking \n",pi );
	//remove from waitingq

	flg=2;
	deleteNode(&waitingq,pi);
	// add to ready q

	tempnode.pid=running2.pid;
	tempnode.priority=running2.priority;
	tempnode.total_runtime=running2.total_runtime;
	flg=0;
	append(&readyq);
	return;

}

