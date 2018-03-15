// Ýsmail Burak Kurhan
//220201055
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <time.h>

#define BUFSIZE 1000 
#pragma warning(disable:4996)

int counter = 0;
int main(int argc, char* argv[])
{
	
	
	HANDLE read_Handle, write_Handle;

	write_Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	read_Handle = GetStdHandle(STD_INPUT_HANDLE);


	if ((write_Handle == INVALID_HANDLE_VALUE) || (read_Handle == INVALID_HANDLE_VALUE))
	{
		exit(0);
	}

	
	if (argc != 1)
	{
		exit(0);
	}
	
	while (1) {
		
		counter++;
		
		char message_from_Parent[BUFSIZE];
		
		int bytesToWrite;
		bytesToWrite = 100;
		bytesToWrite++;
		write_Handle = GetStdHandle(STD_OUTPUT_HANDLE);
		read_Handle = GetStdHandle(STD_INPUT_HANDLE);
		// Read from parent process to know schedule order and to know check finish 
		if (!ReadFile(read_Handle, message_from_Parent, BUFSIZE, &bytesToWrite, NULL)) {
			exit(0);
			break;
		}
		else {

			// Generede random number  and send them with pipe
			srand(time(NULL));
			
			Sleep(2000);
			float random_number = (rand() % (300 - 50)) + 50;
			
			
			char message_To_Parent[1000];
			sprintf(message_To_Parent, "%.2f", random_number);
			bytesToWrite = 0;
			int bytesWritten = 0;
			bytesToWrite = strlen(message_To_Parent);
			

			
			if (!WriteFile(write_Handle, message_To_Parent, bytesToWrite, &bytesWritten, NULL)) {
				exit(0);

			}
			else {
				// If message from parent pipe equals to 1 break the loop and finish .
				int check =  atoi(message_from_Parent);
				if (check == 1) {
					break;
				}
			}

		}

	
	}



	CloseHandle(write_Handle);
	CloseHandle(read_Handle);
	return 1;
}