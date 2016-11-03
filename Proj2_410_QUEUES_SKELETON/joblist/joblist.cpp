#include <iostream>
#include "..\includes\joblist.h"
#include "..\includes\file_io.h"
#include "..\includes\constants.h"
#include "..\includes\Dispatcher.h"

//assumme the worst
bool joblistHasJobs = false;

int joblist::init(const char* filename) {
	int wasSuccessful = loadData(filename);
	if(wasSuccessful == COULD_NOT_OPEN_FILE){
		return COULD_NOT_OPEN_FILE;
	}
	sortData(START_TIME);
	return SUCCESS;
}

PCB joblist::getNextJob() {
	return getNext();
}

int joblist::doTick(int currentTick) {
	if(size() == 0){
		return NO_JOBS;
	}
	if(currentTick >= peekNextStartTime()){
		return ADD_JOB_TO_DISPATCHER;
	}

	return WAITING_TO_ADD_JOB_TO_DISPATCHER;
}
