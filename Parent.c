// Ýsmail Burak Kurhan
//220201055

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#define NO_OF_PROCESS 5
#define BUFSIZE 100 
#pragma warning(disable:4996)


int schedule_Array[5] = { 0, 0, 0, 0, 0 }; 
float burst_Time_Array[5] = { 300, 220, 180, 45, 255 };
float Previus_Burst_Time_Array[5] = { 300, 220, 180, 45, 255 };  // to compare burst_time_array when burst time array sorted it holds last burst time values

int counter = 5;  // For finish loop.
float alpha = 0.5;
float predicted = 0;
int check_finish = 0; 


int main(int argc, char* argv[])
{
	STARTUPINFO si[NO_OF_PROCESS];
	PROCESS_INFORMATION pi[NO_OF_PROCESS];
	SECURITY_ATTRIBUTES sa[NO_OF_PROCESS];

	HANDLE parent_write_handle[NO_OF_PROCESS];
	HANDLE child_read_handle[NO_OF_PROCESS];
	HANDLE child_write_handle[NO_OF_PROCESS];
	HANDLE parent_read_handle[NO_OF_PROCESS];
	HANDLE processHandles[NO_OF_PROCESS];

	char* lpCommandLine[NO_OF_PROCESS] = { "Child.exe","Child.exe","Child.exe","Child.exe","Child.exe" };
	int i = 0;

	for (i = 0; i < NO_OF_PROCESS; i++)
	{
		SecureZeroMemory(&sa[i], sizeof(SECURITY_ATTRIBUTES));
		sa[i].bInheritHandle = TRUE;
		sa[i].lpSecurityDescriptor = NULL;
		sa[i].nLength = sizeof(SECURITY_ATTRIBUTES);
		
		if (!CreatePipe(&parent_read_handle[i], &child_write_handle[i], &sa[i], 0)) 
		{
			printf("Error in create pipe\n");
			system("pause");
			exit(0);
		}
		

		if (!CreatePipe(&child_read_handle[i], &parent_write_handle[i], &sa[i], 0))
		{
			printf("Error in create pipe\n");
			system("pause");
			exit(0);
		}
		

		SecureZeroMemory(&si[i], sizeof(STARTUPINFO));
		si[i].cb = sizeof(STARTUPINFO);
		si[i].hStdInput = child_read_handle[i];
		si[i].hStdOutput = child_write_handle[i];
		si[i].hStdError = child_write_handle[i];
		si[i].dwFlags = STARTF_USESTDHANDLES;


		
		SecureZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));
		if (!CreateProcess(NULL,
			lpCommandLine[i], 
			NULL, 
			NULL,  
			TRUE, 
			CREATE_NEW_CONSOLE, 
			NULL, 
			NULL,
			&si[i], 
			&pi[i])) 
		{
			printf("Error in  Create child process: %d\n", i);
			system("pause");
			exit(0);
		}
		processHandles[i] = pi[i].hProcess;
	}



	while (counter>0) {

		int size = 5;
		int i, j;
		// Sorting Burst times
		for (int i = 1; i<size; i++)
		{
			float value = burst_Time_Array[i];
			int hole = i;
			while (hole>0 && burst_Time_Array[hole - 1]>value)
			{
				burst_Time_Array[hole] = burst_Time_Array[hole - 1];
				hole--;
			}
			burst_Time_Array[hole] = value;
		}

		// Compare burst times with previus burst time for schedule
		for (i = 0; i<size; i++)
		{
			for (j = 0; j<size; j++)
			{

				if (burst_Time_Array[j] == Previus_Burst_Time_Array[i]) {
					schedule_Array[j] = i;

				}
			}
		}
		printf(" Schedule for next execution : ");
		for (i = 0; i<size; i++)
		{
			printf(" %d - ", schedule_Array[i]);
			
		}



		for (i = 0; i<size; i++)
		{	
			
			int order;
			int bytesToWrite = 0;
			int bytesWritten = 0;
			char message_To_Child[1000];
			bytesToWrite = strlen(message_To_Child);
			bytesToWrite++;
			// If counter = 1 means last loop check finish = 1 and send check_finish to child process via pipe else send check_finish =0 
			order = schedule_Array[i];
			if (counter == 1) {
				check_finish = 1;
				sprintf(message_To_Child, "%d", check_finish);
			}
			else {
				check_finish = 0;
				sprintf(message_To_Child, "%d", check_finish);
			}
			
			if (!WriteFile(parent_write_handle[order], message_To_Child, bytesToWrite, &bytesWritten, NULL))
			{
				printf("Unable to write to pipe \n");
				system("pause");
				exit(0);
			}
			printf("Process (%d) started\n", order);
			Sleep(2000);

			char message_from_child[BUFSIZE]; 
			bytesToWrite = 0;
			int bytesWrittenn = 0;
			bytesToWrite = strlen(message_from_child);
			bytesToWrite++;
			// Read the burst time of child processes via pipe
			if (!ReadFile(parent_read_handle[order], message_from_child, bytesToWrite, &bytesWrittenn, NULL)) {
				exit(0);

			}
			// Gets the burst time and calculate predicted burst time for next execution
			printf("Process (%d) ended\n", order);
			float random_number = 0;
			random_number = atof(message_from_child);
			printf("(%d). process previus burst time = %.2f \n", order, Previus_Burst_Time_Array[order]);
			printf("(%d). process  burst time (%2.f)\n", order, random_number);
			predicted = (alpha*random_number) + ((1 - alpha) * (burst_Time_Array[order]));
			printf("(%d). process predicted next burst time  (%2.f)\n", order, predicted);
			
			burst_Time_Array[order] = predicted;
			Previus_Burst_Time_Array[order] = predicted;

		}
	
		counter--;
	} 
	// Wait all child proccess and finish the job
	WaitForMultipleObjects(NO_OF_PROCESS, processHandles, TRUE, INFINITE);
	printf("End of the program, child processess are closed. ");
	system("pause");
	for (i = 0; i < NO_OF_PROCESS; i++)
	{

		CloseHandle(child_read_handle[i]);
		CloseHandle(pi[i].hThread);
		CloseHandle(pi[i].hProcess);
	}


	return 1;
}


