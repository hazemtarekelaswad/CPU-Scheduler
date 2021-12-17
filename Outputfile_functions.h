//In the start
void OutFile_Begining (const char* filePath)
{
    FILE* logFile = fopen(filePath, "w");		//open the file
    if (logFile == NULL) {                    //can't open the file
        printf("ERROR! Could not open file %s\n", filePath);
        return;  // ERROR occured
    }
    fprintf(f, "#At time \t x \t process \t y \t state \t arr \t w \t total \t z \t remain \t y \t wait \t \n");
    fclose(f);
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
FILE* logFile = fopen(filePath, "w");		//open the file
    if (logFile == NULL) {                    //can't open the file
        printf("ERROR! Could not open file %s\n", filePath);
        return;  // ERROR occured
    }

if(status == 1)
{
fprintf(logFile, "At time \t %d \t process \t %d \t started \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \n",clk,id_curProc,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc);
fclose(logFile);
}
else if (stauts == 2)
{
	//parameters are previous process not currently
fprintf(logFile, "At time \t %d \t process \t %d \t stopped \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \n",clk, id_curProc,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc);
fclose(logFile);
}
else if (stauts == 3)
{
fprintf(logFile, "At time \t %d \t process \t %d \t resumed \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \n",clk,id_curProc,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc);
fclose(logFile);
}
else if (stauts == 4)
{
fprintf(logFile, "At time \t %d \t process \t %d \t finished \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \t TA \t %d \t WTA \t %.2f\n",clk,id_curProc,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc,getClk()-AT_curProc,(getClk()-AT_curProc)/(RunT_curProc))
fclose(logFile);
}
else
{
  printf("ERROR! Could not open file %s\n", filePath);
  return;  // ERROR occured
}
	
}


//When Starting a process
/*void OutFile_Starting (file* f, int id_curProc, int AT_curProc, int RunT_curProc, int RemT_curProc, int WaitT_curProc)
{
	f = fopen("logs/scheduler.log", "a");
	fprintf(f, "At time \t %d \t process \t %d \t started \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \n",
	getClk(),id_curProc
	,AT_curProc,RunT_curProc
	,RemT_curProc,WaitT_curProc);
	fclose(f);
}
//When stopping a process
void OutFile_Stopping (file* f, int id_prevProc, int AT_prevProc, int RunT_prevProc, int RemT_prevProc, int WaitT_prevProc)
{
	f = fopen("logs/Round_Robin/scheduler2.log", "a");
	fprintf(f, "At time \t %d \t process \t %d \t stopped \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \n",
	getClk(), id_prevProc
	,AT_prevProc,RunT_prevProc,RemT_prevProc,WaitT_prevProc);
	fclose(f);
}
//When resuming a process
void OutFile_Resuming (file* f , int id_curProc, int AT_curProc, int RunT_curProc, int RemT_curProc, int WaitT_curProc)
{
	f = fopen("logs/scheduler.log", "a");
	fprintf(f, "At time \t %d \t process \t %d \t resumed \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \n",
	getClk(),id_curProc
	,AT_curProc,RunT_curProc,RemT_curProc,WaitT_curProc);
	fclose(f)
}
//When finishing a process
void OutFile_Finishing (file* f, , int id_curProc, int AT_curProc, int RunT_curProc, int RemT_curProc, int WaitT_curProc)
{
	f = fopen("logs/scheduler.log", "a");
	fprintf(f, "At time \t %d \t process \t %d \t finished \t arr \t %d \t total \t %d \t remain \t %d \t wait \t %d \t TA \t %d \t WTA \t %.2f\n",
	getClk(),id_curProc
	,AT_curProc,RunT_curProc,RemT_curProc,
	WaitT_curProc,getClk()-AT_curProc,(getClk()-AT_curProc)/(RunT_curProc));
	fclose(f);
}*/
