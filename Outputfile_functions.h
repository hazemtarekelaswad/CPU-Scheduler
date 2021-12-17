//When the Process is started
void OutFile_Begining (file* f)
{
f = fopen("logs/scheduler.log", "w");
fprintf(f, "#At time\t x \t process \t y \t state \t arr \t w \t total \t z \t remain \t y \t wait \t \n");
fflush(f);
fclose(f);
}
//When Starting a process
void OutFile_Starting (file* f)
{
f = fopen("logs/scheduler.log", "a");
fprintf(f, "At time\t%d\tprocess\t%d\tstarted\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",
getClk(),cur_process->id
,cur_process->arrivalTime,cur_process->runTime
,cur_process->remainingTime,cur_process->wait_time);
fflush(f);
fclose(f);
}
//When stopping a process
void OutFile_Stopping (file* f)
{
	f = fopen("logs/Round_Robin/scheduler2.log", "a");
     fprintf(f, "At time\t%d\tprocess\t%d\tstopped\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",getClk(),prev_process->id
     ,prev_process->arrivalTime,prev_process->runTime,prev_process->remainingTime,prev_process->wait_time);
     // fflush(fp);
     fclose(f);
}
//When resuming a process
void OutFile_Resuming (file* f)
{
f = fopen("logs/scheduler.log", "a");
fprintf(f, "At time\t%d\tprocess\t%d\tresumed\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",getClk(),cur_process->id
       ,cur_process->arrivalTime,cur_process->runTime,cur_process->remainingTime,cur_process->wait_time);
       // fflush(fp);
       fclose(f)
}
//When finishing a process
void OutFile_Finishing (file* f)
{
f = fopen("logs/scheduler.log", "a");
fprintf(f, "At time\t%d\tprocess\t%d\tfinished\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%.2f\n",
                getClk(),cur_process->id
                ,cur_process->arrivalTime,cur_process->runTime,cur_process->remainingTime,
                cur_process->wait_time,getClk()-(cur_process->arrivalTime),WTA);
                    fclose(f);
}

