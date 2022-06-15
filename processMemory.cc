/**
 * @Shared memory for each process to access, and store their children processes
 * 
 */

#include <sst/core/sst_config.h> 
#include <sst/core/interfaces/stringEvent.h> // Include stringEvent event type.
#include <sst/core/simulation.h>
#include <sst/core/stopAction.h>
#include "processMemory.h" // Element header file.

using SST::Interfaces::StringEvent; 

#define MEMORY_NOT_FULL 0
#define MEMORY_FULL 1

// Constructor definition
processMemory::processMemory ( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
    output.init("processMemory-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

	// Get parameters
	// clock = params.find<std::string>("tickFreq", "15s");
	randSeed = params.find<int64_t>("randomseed", 151515);
    memory_size = 25;
    memory_available = 25;

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

void processMemory::handleEvent(SST::Event *ev) {
    // mainly recieves events, send back true or false to process
    // if they successfully found space
    StringEvent *se = dynamic_cast<StringEvent*>(ev);
	if ( se != NULL ) {
        int processID;
        int returnValue;
        processID = atoi(&(se->getString().c_str()[0]));
        if (!memoryFull()) {
            memory_available--;
            output.output(CALL_INFO, "Found a slot. This many left: %d more children\n", memory_available);
            returnValue = MEMORY_NOT_FULL;
        } else {
            output.output(CALL_INFO, "memory is full\n");
            returnValue = MEMORY_FULL;
        }
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

// hard-coded size for memory
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