/**
 * Simple process simulation
 * 
 * WIP example of an infinite execution livelock scenario
 * Goal: create a set of process that all require a certain amount of space
 * in order to continue, but a limited amount of space prohibits
 * this condition from ever being met
 * 
 */

#include <sst/core/sst_config.h> 
#include <sst/core/interfaces/stringEvent.h> // Include stringEvent event type.
#include <sst/core/simulation.h>
#include <sst/core/stopAction.h>
#include "process.h" // Element header file.

using SST::Interfaces::StringEvent; 

// Constructor definition
process::process( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
    output.init("livelockSim-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

	// Get parameters
	clock = params.find<std::string>("delay", "60s");
	RandomSeed = params.find<int64_t>("randomseed", 151515);

	// Register the clock
	registerClock(clock, new SST::Clock::Handler<carGenerator>(this, &process::tick));
	
	// Initialize random
	rng = new SST::RNG::MarsagliaRNG(11, RandomSeed);

    // Initalize maxSize
    maxSubProcesses = (int)(rng->generateNextInt32()); // Generate a random 32-bit integer
	maxSubProcesses = abs((int)(maxSubProcesses % 10)); // Generate a integer 0-9.
    maxSubProcesses++; // ensure that our max size is greater than 0
    numbSubProcesses = 0;
	
	// Configure our port
	port = configureLink("port");
	if ( !port ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
	}
}

// Destructor Definition
process::~process() {

}

bool node::tick( SST::Cycle_t currentCycle ) {
    // Output current status
    output.output(CALL_INFO, "Number of children: %d\n", numSubProcesses);
    output.output(CALL_INFO, "Max number of children: %d\n", maxSubProcesses);
	std::cout << "Sim-Time: " << getCurrentSimTimeNano() << std::endl;

    // check if al children have space allocated
    if ( this.hasAllSubProcesses() ) {
        output.output(CALL_INFO, "Ready to continue\n");
    }

    // find a new spot in memory for another child
    
}

void node::handleEvent(SST::Event *ev) {

}

bool node::hasAllSubProcesses() {
    return numSubProcesses >= maxSubProcesses;
}