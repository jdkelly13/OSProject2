#include "..\includes\Dispatcher.h"
#include "..\includes\constants.h"
#include "..\includes\file_io.h"
//#include "..\Proj2_Main.cpp"
#include <queue>

PCB runningPCB;
std::queue<PCB> ready_Q;
std::queue<PCB> blocked_Q;
//Timeslice test.
const int TIMESLICE = 4;


//Initializes the dispatcher.  Can also be used as a reset.
void dispatcher::init() {
	std::queue<PCB> empty1;
	std::queue<PCB> empty2;
	std::swap(ready_Q, empty1);
	std::swap(blocked_Q, empty2);

	runningPCB.cpu_time = UNINITIALIZED;
	runningPCB.io_time = UNINITIALIZED;
	runningPCB.process_number = UNINITIALIZED;
	runningPCB.start_time = UNINITIALIZED;
}

//Gets the current job.
PCB dispatcher::getCurrentJob() {
	return runningPCB;
}

//Adds job to the queue.
void dispatcher::addJob(PCB &myPCB) {
	ready_Q.push(myPCB);
}

//Handles process interupts.
int dispatcher::processInterrupt(int interrupt) {
	//Checks to see if the interrupt is a time slice interrupt and handles it if it isn't.
	if (interrupt != SWITCH_PROCESS) {
		//Checks to see if its an io complete interrupt.
		if (interrupt != IO_COMPLETE) {
			return PCB_UNIMPLEMENTED;
		}
		//Adds elements from the blocked queue to the ready queue.
		while (!(blocked_Q.empty())) {
			ready_Q.push(blocked_Q.front());
			blocked_Q.pop();
		}

		return PCB_BLOCKED_QUEUE_EMPTY;
	}
	else {
		//Handles if the interrupt is a time slice interrupt.

		//Checks to see if the queues are empty.
		if (ready_Q.empty()) {
			if (blocked_Q.empty()) {
				return NO_JOBS;
			}
			return BLOCKED_JOBS;
		}


		//Switches the processes.
		PCB temp = ready_Q.front();
		ready_Q.pop();
		if (runningPCB.process_number != UNINITIALIZED) {
			ready_Q.push(runningPCB);
		}
		runningPCB = temp;

		return PCB_SWITCHED_PROCESSES;
	}
}

//Handles the tick of the CPU timer.
int dispatcher::doTick() {
	std::queue<PCB> blarge = ready_Q;
	//Checks to see if there is a running PCB and handles if there isnt.
	if(runningPCB.process_number == UNINITIALIZED){

		//Handles ready_Q empty being true.
		if(ready_Q.empty()){
			if(blocked_Q.empty()){
				return NO_JOBS;
			}
			return BLOCKED_JOBS;
		}
		//Handles ready_Q empty being false.
		runningPCB = ready_Q.front();
		ready_Q.pop();
		return PCB_MOVED_FROM_READY_TO_RUNNING;
	}

	//Subtracts 1 from the PCB cpu time.
	(&runningPCB)->cpu_time--;

	if (runningPCB.process_number == 7) {
		std::cout << "";
	}

	//Checks to see if the runningPCB is not finished and handles if it isn't.
	if(runningPCB.cpu_time >=1){
		return PCB_CPUTIME_DECREMENTED;
	}

	//Handles cpu time is 0;.
	int returnval = PCB_FINISHED;

	//Checks to see if the runningPCB makes a blocked IO call and handles it.
	if(runningPCB.io_time == 1){
		runningPCB.io_time = 0;
		blocked_Q.push(runningPCB);
		returnval = PCB_ADDED_TO_BLOCKED_QUEUE;
	}

	//Unloads PCB and returns it as finished or invalid based on IO blocking.
	//ready_Q.pop();
	runningPCB.cpu_time = UNINITIALIZED;
	runningPCB.io_time = UNINITIALIZED;
	runningPCB.process_number = UNINITIALIZED;
	runningPCB.start_time = UNINITIALIZED;
	return returnval;	
}