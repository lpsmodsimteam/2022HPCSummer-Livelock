/**
 * @Shared memory for each process to access, and store their children processes
 * 
 */

#include <sst/core/sst_config.h> 
#include <sst/core/interfaces/stringEvent.h> // Include stringEvent event type.
#include <sst/core/simulation.h>
#include <sst/core/stopAction.h>
#include "processMemory.h"  
#include "process.h"                 // Element header file.

using SST::Interfaces::StringEvent; 

#define MEMORY_NOT_FULL 1
#define MEMORY_FULL 0

// Constructor definition
processMemory::processMemory ( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
    output.init("processMemory-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

	// Get parameters
	// clock = params.find<std::string>("tickFreq", "15s");
	randSeed = params.find<int64_t>("randomseed", 151515);
    memory_size = 1000;
    memory_available = 1000;
    processesComplete = 0;

	// Register the clock
	// registerClock(clock, new SST::Clock::Handler<processMemory>(this, &processMemory::tick));

	// Initialize random
	rng = new SST::RNG::MarsagliaRNG(11, randSeed);
    
    // Configure our port
	process1port = configureLink("process1port", new SST::Event::Handler<processMemory>(this, &processMemory::handleEvent));
	if ( !process1port ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'process1port'\n");
	}
    process2port = configureLink("process2port", new SST::Event::Handler<processMemory>(this, &processMemory::handleEvent));
	if ( !process2port ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'process2port'\n");
	}
    process3port = configureLink("process3port", new SST::Event::Handler<processMemory>(this, &processMemory::handleEvent));
	if ( !process3port ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'process3port'\n");
	}
    process4port = configureLink("process4port", new SST::Event::Handler<processMemory>(this, &processMemory::handleEvent));
	if ( !process4port ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'process4port'\n");
	}
}

// Destructor Definition
processMemory::~processMemory() {
    
}

// bool processMemory::tick( SST::Cycle_t currentCycle ) {
//     output.output(CALL_INFO, "Process complete value: %d\n", processesComplete);
//     if (processesComplete == 16) {
//         std::cout << getName() << ": All processes found space. ending simulation." << std::endl;
// 	    SST::StopAction exit;
// 	    exit.execute();
//     }
//     return false;
// }

void processMemory::handleEvent(SST::Event *ev) {
    // recieves memory requests, sends out whether or not it had space
    // StringEvent *se = dynamic_cast<StringEvent*>(ev);
    MemoryRequestEvent *memev = dynamic_cast<MemoryRequestEvent*>(ev);
	if ( memev != NULL ) {
        int processID;
        int returnValue;
        processID = memev->memreq.pid;
        if (memev->memreq.status == COMPLETE) {
            output.output(CALL_INFO, "Has enough children\n");
            processesComplete++;
            output.output(CALL_INFO, "Process complete value: %d\n", processesComplete);
            if (processesComplete >= 4) {
                std::cout << getName() << ": All processes found space. ending simulation." << std::endl;
	            SST::StopAction exit;
	            exit.execute();
            }
            return;
        } else if (!memoryFull()) {
            memory_available--;
            output.output(CALL_INFO, "Found a slot. This many left: %d more children\n", memory_available);
            returnValue = MEMORY_NOT_FULL;
        } else {
            output.output(CALL_INFO, "memory is full\n");
            returnValue = MEMORY_FULL;
        }
        
        // switch case to make sure we alert the right process port
        switch (processID)
        {
        case 1:
            process1port->send(new StringEvent(std::to_string(returnValue)));
            break;
        case 2:
            process2port->send(new StringEvent(std::to_string(returnValue)));
            break;
        case 3:
            process3port->send(new StringEvent(std::to_string(returnValue)));
            break;
        case 4:
            process4port->send(new StringEvent(std::to_string(returnValue)));
            break;
        default:
            break;
        }
    }
}

// hard-coded size for memory at the moment
int processMemory::memorySize() {
    // return sizeof(memorySpace)/sizeof(memorySpace[0]);
    return memory_size;
}

int processMemory::memoryAvailable() {
    return memory_available;
}

bool processMemory::memoryFull() {
    return memory_available <= 0;
}