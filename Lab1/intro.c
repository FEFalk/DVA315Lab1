#include <stdio.h>
#include <windows.h>
#include <string.h>

int myFunction(int number);
int helloMoonOnRepeat();
int helloWorldOnRepeat();

int main(void)
{
	LPTSTR SlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");
	HANDLE mailSlot;
	HANDLE hFile;

	
	threadCreate(helloWorldOnRepeat);

	mailSlot = mailslotCreate(SlotName);
	hFile = mailslotConnect(SlotName);
	printf("%d\n", mailslotWrite(hFile, "Hello", 7));


	return 0;
}

int myFunction(int number)
{
	printf("%d \n", number);

	return 0;
}

int helloWorldOnRepeat()
{
	for (int i = 0; i < 10; i++) {
		if (i>0)
			Sleep(1000);
		printf("Hello World!\n");
	}
	return 0;
}

int helloMoonOnRepeat()
{
	while(1==1){
		Sleep(200);
		printf("Hello Moon\n");
	}
	return 0;
}