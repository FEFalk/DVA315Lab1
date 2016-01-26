#include <stdio.h>
#include <windows.h>
#include <string.h>

#define MAX_THREADS 2

int myFunction(int number);
int helloMoonOnRepeat();
int helloWorldOnRepeat();
int outputLoop(LPVOID param);
int inputLoop(LPVOID param);


LPTSTR SlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");
HANDLE semaphore;
CRITICAL_SECTION criticalSection;

typedef struct {
	char message[256];
} stringStruct;

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
		threadCreate(inputLoop, NULL),
		threadCreate(outputLoop, NULL)
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

int outputLoop(LPVOID param) {
	char *msg;
	int read;
	BOOL loop = TRUE;
	HANDLE mail;

	mail = mailslotCreate(SlotName);
	ReleaseSemaphore(semaphore, 1, NULL); // Tell other thread the mailslot is created

	while (loop) {
		if (WaitForSingleObject(semaphore, INFINITE) == WAIT_OBJECT_0) { // Wait for messages
			read = mailslotRead(mail, &msg, 0);
			if (read != 0) {
				printf("Read: %s\n", msg);
				if (strcmp(msg, "END") == 0)
					loop = FALSE;
			}
			free(msg);
		}
	}
	mailslotClose(mail);
	return 0;
}

int inputLoop(LPVOID param) {
	WaitForSingleObject(semaphore, INFINITE); // Wait for mailslot to be created
	HANDLE mail = mailslotConnect(SlotName);
	char *read = calloc(256, sizeof(char));
	BOOL loop = TRUE;

	while (loop) {
		fgets(read, 255, stdin);
		if (strlen(read) > 1)
			read[strlen(read) - 1] = '\0';
		stringStruct input;
		strncpy(input.message, read, strlen(read) + 1);
		mailslotWrite(mail, &input, sizeof(input));
		ReleaseSemaphore(semaphore, 1, NULL); // Tell the other thread there are new messages!
		if (strcmp(read, "END") == 0)
			loop = FALSE;
	}
	free(read);
	mailslotClose(mail);
	return 0;
}