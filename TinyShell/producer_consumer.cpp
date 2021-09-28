#include<windows.h>
#include<stdio.h>

int in = 0;
int out = 0;
const int BUFFER_SIZE = 10;
int COUNTER =0;
int Buffer[BUFFER_SIZE] = {};

void Producer(){
	while(1) { 
	int nextProducer = in +1;
	while(COUNTER == BUFFER_SIZE);
	Buffer[in] = nextProducer;
	in = (in +1)%BUFFER_SIZE;
	COUNTER ++;
	printf("Producer: %4d\n",COUNTER);
	}
}
void Consumer(){
	while(1) {
		while(COUNTER == 0);
		
		int nextConsumer = Buffer[out];
		Buffer[out] =0;
		out = (out +1)%BUFFER_SIZE;
		COUNTER --;
		printf("Consumer: %4d\n",COUNTER);
	}
}

int main(){
	HANDLE h1,h2; DWORD Id;
	h1= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Producer,NULL,0,&Id);
	h2= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Consumer,NULL,0,&Id);
	WaitForSingleObject(h1,INFINITE);
	WaitForSingleObject(h2,INFINITE);
	return 0;
}

