#ifndef _process_H
#define _process_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>

#include "processMemory.h"

class process : public SST::Component {
    public:
        process( SST::ComponentId_t id, SST::Params& params ); // Constructor
	    ~process(); // Deconstructor

	    bool tick( SST::Cycle_t currentCycle);

	    void handleEvent(SST::Event *ev);

		// void setup();
		// void finish();

		void liveLockDetect();

        bool hasAllSubProcesses(); // lets us know if process is ready to continue

        // Register the component for lookup via sst-info
	    SST_ELI_REGISTER_COMPONENT(
		    process, // class
		    "livelock", // element library
		    "process", // component
		    SST_ELI_ELEMENT_VERSION( 1, 0, 0 ), // current element version
		    "processes that need to allocate space for subprocesses.", // description of component.
		    COMPONENT_CATEGORY_UNCATEGORIZED // * Not grouped in a category. (No category to filter with via sst-info).
	    )
	
	    // Parameters, description, default value
	    SST_ELI_DOCUMENT_PARAMS(
	    	{"maxSubProcesses", "The number of children a process needs.", "10"},
			{"tickFreq", "The frequency the process is called at.", "10s"}
	    )

	    // Port name, description, event type
	    SST_ELI_DOCUMENT_PORTS(
		    {"memoryPort", "Port to connect to shared memory", {"sst.Interfaces.StringEvent"}},
	    )

    private:
		int64_t randSeed; // Seed for MarsagliaRNG
		SST::RNG::MarsagliaRNG *rng; //

        SST::Output output;
		int processID;
        int numSubProcesses; // number of children prcesses held
        int maxSubProcesses; // number of children processes required for process to continue  
        bool cantFindSpace;  // process has potentially entered a livelock  

		std::string clock; // Defining a clock which can be described via unit math as a string (?).

		SST::Link *memoryPort; // Pointer to queue port  

};
#endif