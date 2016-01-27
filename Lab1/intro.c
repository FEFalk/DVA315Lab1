#include <stdio.h>
#include <windows.h>
#include <string.h>

#define MAX_THREADS 2

int myFunction(int number);
int helloMoonOnRepeat();
int helloWorldOnRepeat();
int readMessage(LPVOID param);
int writeMessage(LPVOID param);


LPTSTR SlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");
HANDLE semaphore;
CRITICAL_SECTION criticalSection;

struct stringStruct{
	char message[256];
};

int main(void)
{
	//InitializeCriticalSection(&criticalSection);
	//HANDLE threads[MAX_THREADS] = {
	//	threadCreate(helloWorldOnRepeat, NULL),
	//	threadCreate(helloMoonOnRepeat, NULL)
	//};

	//WaitForMultipleObjects(MAX_THREADS, threads, TRUE, INFINITE);
	//DeleteCriticalSection(&criticalSection);

	semaphore = CreateSemaphore(NULL, 0, 10, NULL);
	if (semaphore == NULL)
	{
		printf("CreateSemaphore Error: %d\n", GetLastError());
		return 1;

	}

	HANDLE threads[MAX_THREADS] = {
		threadCreate(readMessage, NULL),
		threadCreate(writeMessage, NULL)
	};


	WaitForMultipleObjects(MAX_THREADS, threads, TRUE, INFINITE);
	
	CloseHandle(threads[0]);
	CloseHandle(threads[1]);

	//Sleep(4000);

	//mailSlot = mailslotCreate(SlotName);
	//hFile = mailslotConnect(SlotName);
	//printf("%d\n", mailslotWrite(hFile, "Hello", 7));

	return 0;
}

int myFunction(int number)
{
	printf("%d \n", number);

	return 0;
}

int helloWorldOnRepeat()
{
	while(1){
		EnterCriticalSection(&criticalSection);
		for (int i = 0; i < 10; i++) {
			Sleep(200);
			printf("Hello World!\n");
		}
		LeaveCriticalSection(&criticalSection);
		Sleep(100);
	}
}

int helloMoonOnRepeat()
{
	while (1) {
		EnterCriticalSection(&criticalSection);
		for (int i = 0; i < 10; i++) {
			Sleep(200);
			printf("Hello Moon!\n");
		}
		LeaveCriticalSection(&criticalSection);
		Sleep(100);
	}
}

int readMessage(LPVOID param) {
	char *msg;
	int read;
	BOOL spin = TRUE;
	HANDLE mail;

	mail = mailslotCreate(SlotName); // Mailslot create
	ReleaseSemaphore(semaphore, 1, NULL); // Opens a spot for the other thread to take

	while (spin) {
		WaitForSingleObject(semaphore, INFINITE);  // Wait for messages
		read = mailslotRead(mail, &msg, 0);
		if (read != 0) {
			printf("Read: %s\n", msg);
			if (strcmp(msg, "END") == 0)
				spin = FALSE;
			
			free(msg);
		}
	}
	mailslotClose(mail);
	return 0;
}

int writeMessage(LPVOID param) {
	WaitForSingleObject(semaphore, INFINITE); // Waiting for a semaphore spot (Until the mailslot is created)
	HANDLE mail = mailslotConnect(SlotName); //Connect to the mailslot
	char *read = calloc(256, sizeof(char));
	BOOL spin = TRUE;

	while (spin) {
		fgets(read, 255, stdin);
		if (strlen(read) > 1)
			read[strlen(read) - 1] = '\0';
		struct stringStruct input;
		strncpy(input.message, read, strlen(read) + 1);
		mailslotWrite(mail, &input, sizeof(input));
		ReleaseSemaphore(semaphore, 1, NULL); // Opens a new semaphore-spot (lets the other thread know there's a message to be read)
		if (strcmp(read, "END") == 0)
			spin = FALSE;
	}
	free(read);
	mailslotClose(mail);
	return 0;
}