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

// Constructor definition
processMemory::processMemory ( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {

}

// Destructor Definition
processMemory::~processMemory() {

}

bool processMemory::tick( SST::Cycle_t currentCycle ) {

}

int processMemory::memorySize() {
    return sizeof(memorySpace)/sizeof(memorySpace[0]);
}