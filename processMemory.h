#ifndef _processMemory_H
#define _processMemory_H

#include <sst/core/component.h>
#include <sst/core/subcomponent.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>

// #include<queue>

class processMemory : public SST::Component {
    public:

        processMemory( SST::ComponentId_t id, SST::Params& params ); // Constructor
	    ~processMemory(); // Deconstructor

        void setup();
	    void finish();

	    bool tick( SST::Cycle_t currentCycle);

	    void handleEvent(SST::Event *ev);

        // *** for now, just increment/decrement space, not actually
        // using the array to store info *******
        // queue<int> q;
        // int memorySpace [25];
        int memorySize();

        int memoryAvailable();

        bool memoryFull();

        // Register the component for lookup via sst-info
	    SST_ELI_REGISTER_COMPONENT(
		    processMemory, // class
		    "livelock", // element library
		    "processMemory", // component
		    SST_ELI_ELEMENT_VERSION( 1, 0, 0 ), // current element version
		    "shared memory for subprocesses", // description of component.
		    COMPONENT_CATEGORY_UNCATEGORIZED // * Not grouped in a category. (No category to filter with via sst-info).
	    )

        // Port name, description, event type
	    SST_ELI_DOCUMENT_PORTS(
		    {"process1port", "Port to connect to individual process", {"sst.Interfaces.StringEvent"}},
            {"process2port", "Port to connect to individual process", {"sst.Interfaces.StringEvent"}},
            {"process3port", "Port to connect to individual process", {"sst.Interfaces.StringEvent"}},
            {"process4port", "Port to connect to individual process", {"sst.Interfaces.StringEvent"}},
	    )

;
    private:
        SST::Output output;
        int64_t randSeed; // Seed for MarsagliaRNG
		SST::RNG::MarsagliaRNG *rng; //

        int memory_size;
        int memory_available;
        bool memory_full;
        std::string clock; // Defining a clock which can be described via unit math as a string (?).

		SST::Link *process1port; // Pointer to queue port  
        SST::Link *process2port; // Pointer to queue port  
        SST::Link *process3port; // Pointer to queue port  
        SST::Link *process4port; // Pointer to queue port  
};
#endif