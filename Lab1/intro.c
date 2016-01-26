#include <stdio.h>
#include <windows.h>
#include <string.h>

#define MAX_THREADS 2

int myFunction(int number);
int helloMoonOnRepeat();
int helloWorldOnRepeat();

HANDLE helloSemaphore;
CRITICAL_SECTION criticalSection;
int main(void)
{
	LPTSTR SlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");
	HANDLE mailSlot;
	HANDLE hFile;
	
	helloSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
	if (helloSemaphore == NULL)
	{
		printf("CreateSemaphore Error: %d\n", GetLastError());
		return 1;

	}
	InitializeCriticalSection(&criticalSection);
	HANDLE threads[MAX_THREADS] = {
		threadCreate(helloWorldOnRepeat, NULL),
		threadCreate(helloMoonOnRepeat, NULL)
	};

	WaitForMultipleObjects(MAX_THREADS, threads, TRUE, INFINITE);
	DeleteCriticalSection(&criticalSection);
	
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