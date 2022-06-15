/**
 * Simple process simulation
 * 
 * WIP example of an infinite execution livelock scenario
 * Goal: create a set of process that all require a certain amount of space
 * in order to continue, but a limited amount of space prohibits
 * this condition from ever being met
 * 
 * TODO: 
 * Randomize size needs of each process on initialization
 * 
 * Implement way to clear number of children after they've filled
 * so that the simulation runs for longer (give subprocesss a lifespan)
 * 
 * Detection: set a bool once one process fails to find space
 * If after a certain amount of time, this bool hasn't switched
 * End simulation, declare livelock
 * 
 */

#include <sst/core/sst_config.h> 
#include <sst/core/interfaces/stringEvent.h>    // Include stringEvent event type.
#include <sst/core/simulation.h>
#include <sst/core/stopAction.h>
#include "process.h"                            // Element header file.

using SST::Interfaces::StringEvent; 

// Constructor definition
process::process( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
    output.init("livelockSim-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

	// Get parameters
    maxSubProcesses = params.find<int64_t>("maxSubProcesses", 10);
	clock = params.find<std::string>("tickFreq", "15s");
    processID = params.find<int64_t>("processID", "1");
	randSeed = params.find<int64_t>("randomseed", 151515);

	// Register the clock
	registerClock(clock, new SST::Clock::Handler<process>(this, &process::tick));
	
    // tell the simulator not to end without us
	registerAsPrimaryComponent();
	primaryComponentDoNotEndSim();

	// Initialize random
	rng = new SST::RNG::MarsagliaRNG(11, randSeed);

    // // Initalize maxSize --> TODO: move randomization to .py
    // maxSubProcesses = (int)(rng->generateNextInt32()); // Generate a random 32-bit integer
	// maxSubProcesses = abs((int)(maxSubProcesses % 10)); // Generate a integer 0-9.
    // maxSubProcesses++; // ensure that our max size is greater than 0
    numSubProcesses = 0;
	
	// Configure our port for requesting space from memory
	memoryPort = configureLink("memoryPort", new SST::Event::Handler<process>(this, &process::handleEvent));
	if ( !memoryPort ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'memoryPort'\n");
	}
}

// Destructor Definition
process::~process() {

}

bool process::tick( SST::Cycle_t currentCycle ) {
    // Output current status
    output.output(CALL_INFO, "Number of children: %d\n", numSubProcesses);
    output.output(CALL_INFO, "Max number of children: %d\n", maxSubProcesses);
	std::cout << "Sim-Time: " << getCurrentSimTimeNano() << std::endl;

    // check if al children have space allocated
    if ( this->hasAllSubProcesses() ) {
        output.output(CALL_INFO, "***COMPLETE***\n");
    } 

    else {
        // call addSubProcess in processMemory
        output.output(CALL_INFO, "visiting processMemory\n");
        memoryPort->send(new StringEvent(std::to_string(processID)));
    }

    // find a new spot in memory for another child
    // send space request (event) to processMemory
    return false; // temp
}

void process::liveLockDetect() {
    // for now, immediatley exits
    // in the future, should wait some time and recheck
    std::cout << getName() << " detected a livelock. Ending simulation." << std::endl;
	SST::StopAction exit;
	exit.execute();
}

void process::handleEvent(SST::Event *ev) {
    // if memory was full, wait 5 seconds and send another event request
    // if memory was full:
    // cantFindSpace = true;
    // create new function to check livelock scenario
    StringEvent *se = dynamic_cast<StringEvent*>(ev);
	if ( se != NULL ) {
        int spaceFree;
        spaceFree = atoi(&(se->getString().c_str()[0]));
        if(!spaceFree) {
            numSubProcesses++;
            output.output(CALL_INFO, "Found a slot. Needs %d more children\n", maxSubProcesses - numSubProcesses);
        } else {
            output.output(CALL_INFO, "ran out of space\n");
            cantFindSpace = true;
            liveLockDetect();
        }
    }
}

bool process::hasAllSubProcesses() {
    return numSubProcesses >= maxSubProcesses;
}