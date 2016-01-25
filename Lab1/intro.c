#include <stdio.h>
#include <windows.h>
#include <string.h>

int myFunction(int number);

int main(void)
{
	LPTSTR SlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");
	HANDLE mailSlot;
	HANDLE hFile;

	threadCreate(myFunction, 5);

	mailSlot = mailslotCreate(SlotName);
	hFile = mailslotConnect(SlotName);
	printf("%d\n", mailslotWrite(hFile, "Hello", 7) );

	return 0;
}

int myFunction(int number)
{
	printf("%d \n", number);

	return 0;
}