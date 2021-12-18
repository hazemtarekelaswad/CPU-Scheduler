#ifndef OUTPUT
#define OUTPUT

#include <stdio.h>
#include <math.h>

//In the start
void OutFile_Begining (const char* filePath)
{
    FILE* logFile = fopen(filePath, "w");		//open the file
    if (logFile == NULL) {                    //ca open the file
        printf("ERROR! Could not open file %s\n", filePath);
        return;  // ERROR occured
    }
    fprintf(logFile, "#At time x process y state arr w total z remain y wait \n");
    fclose(logFile);
}

//Integrated Function for the 4 cases of processes
void OutFile_Starting (FILE* logFile, int status, int id_curProc, int AT_curProc, int RunT_curProc, int RemT_curProc, int WaitT_curProc, int clk)
{
	/*
	status =1 -> start
	status =2 -> stop
	status =3 -> resume
	status =4 -> finish
	*/
	

	if(status == 1)
	{
		fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d \n",clk,id_curProc,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc);
	}
	else if (status == 2)
	{
		//parameters are previous process not currently
		fprintf(logFile, "At time %d process %d stopped arr %d total %d remain %d wait %d \n",clk, id_curProc,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc);
	}
	else if (status == 3)
	{
		fprintf(logFile, "At time %d process %d resumed arr %d total %d remain %d wait %d \n",clk,id_curProc,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc);
	}
	else if (status == 4)
	{
		fprintf(logFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",clk,id_curProc,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc,clk-AT_curProc,(clk-AT_curProc)/(1.0 * (RunT_curProc)));
	}
	else
	{
		// printf("ERROR! Could not open file %s\n", filePath);
	}
	// fclose(logFile);
		
}

void printPerfFile(const char* filePath, float utilization, float avgWTA, float avgWaiting, float stdWTA) {
    FILE* perfFile = fopen(filePath, "w");		//open the file
    if (perfFile == NULL) {                    //ca open the file
        printf("ERROR! Could not open file %s\n", filePath);
        return;  // ERROR occured
    }
    fprintf(perfFile, "CPU utilization = %.2f%%\n", utilization * 100);
    fprintf(perfFile, "Avg WTA = %.2f\n", avgWTA);
    fprintf(perfFile, "Avg Waiting = %.2f\n", avgWaiting);
    fprintf(perfFile, "Std WTA = %.2lf", ((double)stdWTA));

    fclose(perfFile);
}

#endif


//When Starting a process
/*void OutFile_Starting (file* logFile, int id_curProc, int AT_curProc, int RunT_curProc, int RemT_curProc, int WaitT_curProc)
{
	logFile = fopen("logs/scheduler.log", "a");
	fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d \n",
	getClk(),id_curProc
	,AT_curProc,RunT_curProc
	,RemT_curProc,WaitT_curProc);
	fclose(logFile);
}
//When stopping a process
void OutFile_Stopping (file* logFile, int id_prevProc, int AT_prevProc, int RunT_prevProc, int RemT_prevProc, int WaitT_prevProc)
{
	logFile = fopen("logs/Round_Robin/scheduler2.log", "a");
	fprintf(logFile, "At time %d process %d stopped arr %d total %d remain %d wait %d \n",
	getClk(), id_prevProc
	,AT_prevProc,RunT_prevProc,RemT_prevProc,WaitT_prevProc);
	fclose(logFile);
}
//When resuming a process
void OutFile_Resuming (file* logFile , int id_curProc, int AT_curProc, int RunT_curProc, int RemT_curProc, int WaitT_curProc)
{
	logFile = fopen("logs/scheduler.log", "a");
	fprintf(logFile, "At time %d process %d resumed arr %d total %d remain %d wait %d \n",
	getClk(),id_curProc
	,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc);
	fclose(logFile)
}
//When finishing a process
void OutFile_Finishing (file* logFile, , int id_curProc, int AT_curProc, int RunT_curProc, int RemT_curProc, int WaitT_curProc)
{
	logFile = fopen("logs/scheduler.log", "a");
	fprintf(logFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",
	getClk(),id_curProc
	,AT_curProc,RunT_curProc,RemT_curProc,
	WaitT_curProc,getClk()-AT_curProc,(getClk()-AT_curProc)/(RunT_curProc));
	fclose(logFile);
}*/
