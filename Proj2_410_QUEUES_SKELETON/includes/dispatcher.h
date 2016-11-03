#pragma once
#include "constants.h"
namespace dispatcher {
	//clears ready_Q and blocked_Q these are queues of PCB structures,
	//initializes runningPCB to default values in constants (see PCB structure)
	void init();

	//add a job to the ready queue
	void addJob(PCB &myPCB);

	//see flowchart
	int doTick();

	//see flowchart
	int processInterrupt(int interrupt);

	//used for testing, return a copy of runningPCB
	PCB getCurrentJob();
}

