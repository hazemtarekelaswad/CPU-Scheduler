#ifndef OUTPUT
#define OUTPUT

#include <stdio.h>

//In the start
void OutFile_Begining (const char* filePath)
{
    FILE* logFile = fopen(filePath, "w");		//open the file
    if (logFile == NULL) {                    //can't open the file
        printf("ERROR! Could not open file %s\n", filePath);
        return;  // ERROR occured
    }
    fprintf(logFile, "#At time \t x \t process \t y \t state \t arr \t w \t total \t z \t remain \t y \t wait \t \n");
    fclose(logFile);
}

//Integrated Function for the 4 cases of processes
void OutFile_Starting (const char* filePath,int status, int id_curProc, int AT_curProc, int RunT_curProc, int RemT_curProc, int WaitT_curProc, int clk)
{
	/*
	status =1 -> start
	status =2 -> stop
	status =3 -> resume
	status =4 -> finish
	*/
	FILE* logFile = fopen(filePath, "a");		//open the file
    if (logFile == NULL) {                    //can't open the file
        printf("ERROR! Could not open file %s\n", filePath);
        return;  // ERROR occured
    }

	if(status == 1)
	{
		fprintf(logFile, "At time \t %d \t process \t %d \t started \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \n",clk,id_curProc,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc);
	}
	else if (status == 2)
	{
		//parameters are previous process not currently
		fprintf(logFile, "At time \t %d \t process \t %d \t stopped \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \n",clk, id_curProc,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc);
	}
	else if (status == 3)
	{
		fprintf(logFile, "At time \t %d \t process \t %d \t resumed \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \n",clk,id_curProc,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc);
	}
	else if (status == 4)
	{
		fprintf(logFile, "At time \t %d \t process \t %d \t finished \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \t TA \t %d \t WTA \t %.2f\n",clk,id_curProc,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc,clk-AT_curProc,(clk-AT_curProc)/(RunT_curProc));
	}
	else
	{
		printf("ERROR! Could not open file %s\n", filePath);
	}
	fclose(logFile);
		
}

void printPerfFile(const char* filePath, float utilization, float avgWTA, float avgWaiting, float stdWTA) {
    FILE* perfFile = fopen(filePath, "w");		//open the file
    if (perfFile == NULL) {                    //can't open the file
        printf("ERROR! Could not open file %s\n", filePath);
        return;  // ERROR occured
    }
    fprintf(perfFile, "CPU utilization = %.2f%%\n", utilization);
    fprintf(perfFile, "Avg WTA = %.2f\n", avgWTA);
    fprintf(perfFile, "Avg Waiting = %.2f\n", avgWaiting);
    fprintf(perfFile, "Std WTA = %.2f", stdWTA);

    fclose(perfFile);
}

#endif


//When Starting a process
/*void OutFile_Starting (file* logFile, int id_curProc, int AT_curProc, int RunT_curProc, int RemT_curProc, int WaitT_curProc)
{
	logFile = fopen("logs/scheduler.log", "a");
	fprintf(logFile, "At time \t %d \t process \t %d \t started \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \n",
	getClk(),id_curProc
	,AT_curProc,RunT_curProc
	,RemT_curProc,WaitT_curProc);
	fclose(logFile);
}
//When stopping a process
void OutFile_Stopping (file* logFile, int id_prevProc, int AT_prevProc, int RunT_prevProc, int RemT_prevProc, int WaitT_prevProc)
{
	logFile = fopen("logs/Round_Robin/scheduler2.log", "a");
	fprintf(logFile, "At time \t %d \t process \t %d \t stopped \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \n",
	getClk(), id_prevProc
	,AT_prevProc,RunT_prevProc,RemT_prevProc,WaitT_prevProc);
	fclose(logFile);
}
//When resuming a process
void OutFile_Resuming (file* logFile , int id_curProc, int AT_curProc, int RunT_curProc, int RemT_curProc, int WaitT_curProc)
{
	logFile = fopen("logs/scheduler.log", "a");
	fprintf(logFile, "At time \t %d \t process \t %d \t resumed \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \n",
	getClk(),id_curProc
	,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc);
	fclose(logFile)
}
//When finishing a process
void OutFile_Finishing (file* logFile, , int id_curProc, int AT_curProc, int RunT_curProc, int RemT_curProc, int WaitT_curProc)
{
	logFile = fopen("logs/scheduler.log", "a");
	fprintf(logFile, "At time \t %d \t process \t %d \t finished \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \t TA \t %d \t WTA \t %.2f\n",
	getClk(),id_curProc
	,AT_curProc,RunT_curProc,RemT_curProc,
	WaitT_curProc,getClk()-AT_curProc,(getClk()-AT_curProc)/(RunT_curProc));
	fclose(logFile);
}*/
