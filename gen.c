#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int main()
{
	//0 for cpu io bound
	//1 for io bound 

	int pid;
	int noi[2];
	noi[0]=100;  
	noi[1]=40;
	int prior[2];
	prior[0]=10;
	prior[1]=5;
	int sleeptime[2];
	sleeptime[0]=1;
	sleeptime[1]=3;
	int prob[2];
	prob[0]=30;
	prob[1]=70;

	// 4 processes in 1 sec execute 
	int i,j=0;
	char call[1024],add[1024];	
	// eclosing for loop
	/*
	while(1)
	{
		//put for loop here
		sleep(1);
	}
	*/
	for(i=0,j=0;i<4;i++)
	{ // noi,prior,sleepprob,time
		memset(&call,0,1024);
		sprintf(call,"/home/aman/Desktop/aman/process.out ");
		sprintf(add,"%d ",noi[j]);
		strcat(call,add);
		sprintf(add,"%d ",prior[j]);
		strcat(call,add);
		sprintf(add,"%d ",prob[j]);
		strcat(call,add);
		sprintf(add,"%d ",sleeptime[j]);
		strcat(call,add);
		printf("checking value of call =%s \n",call);


		// forking the xterm for every process

		pid=fork();
		if(pid==0)
		{
			execl("/usr/bin/xterm", "/usr/bin/xterm", "-e", "bash", "-c", call, (void*)NULL);
		}


		if(i>=1)
		{	
			j=1;
		}
	}



	return 0;
}


















