#ifndef _process_H
#define _process_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>

class process : public SST::Component {
    public:
        process( SST::ComponentId_t id, SST::Params& params ); // Constructor
	    ~process(); // Deconstructor

	    bool tick( SST::Cycle_t currentCycle);

	    void handleEvent(SST::Event *ev);

        bool hasAllSubProcesses(); // lets us know if process is ready to continue

        // Register the component for lookup via sst-info
	    SST_ELI_REGISTER_COMPONENT(
		    node, // class
		    "livelock", // element library
		    "process", // component
		    SST_ELI_ELEMENT_VERSION( 1, 0, 0 ), // current element version
		    "processes that need to allocate space for subprocesses.", // description of component.
		    COMPONENT_CATEGORY_UNCATEGORIZED // * Not grouped in a category. (No category to filter with via sst-info).
	    )
	
        /** *******NEED TO FINISH CHANGING THE TWO BELOW********* */
	    // Parameters, description, default value
	    SST_ELI_DOCUMENT_PARAMS(
	    	{"queueMaxSize", "The size of the node's queue.", "50"},
	    )

	    // Port name, description, event type
	    SST_ELI_DOCUMENT_PORTS(
		    {"nextPort", "Port which outputs a message into the next nodes queue", {"sst.Interfaces.StringEvent"}},
		    {"prevPort", "Port which sends credit info to previous node.", {"sst.Interfaces.StringEvent"}}
	    )

    private:
        SST::Output output;
        int numSubProcesses; // number of children prcesses held
        int maxSubProcesses; // number of children processes required for process to continue        


}