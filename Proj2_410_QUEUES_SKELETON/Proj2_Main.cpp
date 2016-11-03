// Proj3_SOL.cpp : Defines the entry point for the console application.
//

#include ".\includes\constants.h"
#include ".\includes\file_io.h"
#include ".\includes\joblist.h"
#include ".\includes\dispatcher.h"
#include ".\includes\logger_single_thread.h"

//this is the amount of time each process can run before it is swapped out
const int	TIMESLICE = 4;
const int	NUMBER_CYCLES_BETWEEN_IO_INTERRUPTS = 20;

int main()
{
	int	timeslice_time_used = TIMER_BEGIN;			//used per process goes from 0 to TIMESLICE and then restarts
	int tickcount			= TIMER_BEGIN;			//used by io interrupts can be a large number, BTW the first person who
													//tells me if this is an overflow risk gets 2 points added to this project
	int iRetDispatcher		= UNINITIALIZED;
	bool switchProcess		= false;				//when true the dispatcher will switch the running process for another
	bool jobsWaitingForIoInterrupt	= false;		//assumme best
	bool showJobListEmptyMessage	= true;

	//load all jobs from job file
	int iRetJobs			= joblist::init("testdata.txt");		//load file
	if (iRetJobs < SUCCESS)
		return FAIL;

	//this loop runs until there are no jobs to execute anywhere
	do {
		tickcount++;			//tracks all time used
		PRINT2("Tickcount=", tickcount);
		timeslice_time_used++;	

		//are there jobs on the blocked_Q that need an interrupt to wake them?
		//if so then periodically set this interrupt off to clear them
		if (jobsWaitingForIoInterrupt) {
			if (tickcount % NUMBER_CYCLES_BETWEEN_IO_INTERRUPTS == 0) {
				int iret = dispatcher::processInterrupt(IO_COMPLETE);
				switch (iret) {
					case PCB_MOVED_FROM_BLOCKED_TO_READY: 
						PRINT1("	  PCB moved from blocked to ready");
						break;
					
					case PCB_BLOCKED_QUEUE_EMPTY:
						PRINT1("	  PCB moved from blocked to ready");
						break;
					default:
						break;
				}
				jobsWaitingForIoInterrupt = false;
			}
		}

		//do joblist stuff
		iRetJobs = joblist::doTick(tickcount);
		switch (iRetJobs) {
			case NO_JOBS:
				if (showJobListEmptyMessage)
					PRINT1("	Joblist: is empty");
				showJobListEmptyMessage = false;
				break;
			case ADD_JOB_TO_DISPATCHER:
			{
				//a job is ready pull from joblist and add to dispatcher
				PCB newJob = joblist::getNextJob();
				dispatcher::addJob(newJob);
				PRINT4("	JobList: Added job:", newJob.process_number, " at time:", tickcount);
			}
			break;
			case WAITING_TO_ADD_JOB_TO_DISPATCHER:
				PRINT2("	JobList: waiting to add job at time:", tickcount);
				break;
			default:
				PRINT2("	JobList: returned:", iRetJobs);
				break;
		}

		//do dispatcher stuff
		iRetDispatcher = dispatcher::doTick();
		switch (iRetDispatcher) {
			case NO_JOBS:
				PRINT1("	Queues are empty, no running process");
				break;
			case BLOCKED_JOBS:
				PRINT1("	blocked_Q has jobs, ready_Q is empty");
				break;
			case PCB_FINISHED:
				PRINT1("	Finished executing a process");
				//if finished give up rest of timeslice!!!!
				timeslice_time_used = 0;	
				break;
			case PCB_CPUTIME_DECREMENTED:
				PRINT2("	Working on process:", dispatcher::getCurrentJob().process_number);
				break;
			case PCB_ADDED_TO_BLOCKED_QUEUE:
				jobsWaitingForIoInterrupt = true;
				//if finished give up rest of timeslice!!!!
				timeslice_time_used = 0;
				PRINT1("	Process added to blocked queue");
				break;
			case PCB_MOVED_FROM_READY_TO_RUNNING:
				timeslice_time_used = 0;
				PRINT3("	Process:", dispatcher::getCurrentJob().process_number, " is now running");
				break;
				
			default:
				PRINT3("	ERROR:", iRetDispatcher, "In dispatcher");
				break;
		}

		//computes the remainder of tickcount % TimeSlice
		//if 0 then time to switch
		if (timeslice_time_used % TIMESLICE == 0)
		{
			timeslice_time_used = 0;
			dispatcher::processInterrupt(SWITCH_PROCESS);
		}

		//if everything is done do not log the event
		if (iRetJobs != NO_JOBS || iRetDispatcher != NO_JOBS) {
			ST_LOG::log(tickcount, dispatcher::getCurrentJob().process_number);
			std::cout << iRetJobs << " " << iRetDispatcher << std::endl;
		}
	} while (iRetJobs != NO_JOBS || iRetDispatcher != NO_JOBS);

	ST_LOG::save();
}

