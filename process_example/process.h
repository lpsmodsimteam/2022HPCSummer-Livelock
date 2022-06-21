#ifndef _process_H
#define _process_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>

// Status types
enum CompletionStatus {
	COMPLETE,
	INCOMPLETE,
	RESETTING
};

// Struct for a Message
struct MemoryRequest {
	int pid;
	std::string processType;
	CompletionStatus status;
};


class MemoryRequestEvent : public SST::Event {

public:
	void serialize_order(SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & memreq.pid;
		ser & memreq.processType;
		ser & memreq.status;
	}
	
	MemoryRequestEvent(MemoryRequest memreq) :
		Event(),
		memreq(memreq)
	{}

	
	MemoryRequestEvent() {} // For Serialization only

	MemoryRequest memreq; 

	ImplementSerializable(MemoryRequestEvent); //
};

class process : public SST::Component {
    public:
        process( SST::ComponentId_t id, SST::Params& params ); // Constructor
	    ~process(); // Deconstructor

	    bool tick( SST::Cycle_t currentCycle);
	    void handleEvent(SST::Event *ev);

		void liveLockDetect();		// checks if process is constantly checking memory
        bool hasAllSubProcesses(); 	// lets us know if process is ready to continue

        // Register the component for lookup via sst-info
	    SST_ELI_REGISTER_COMPONENT(
		    process, 		// class
		    "livelock", 	// element library
		    "process", 		// component
		    SST_ELI_ELEMENT_VERSION( 1, 0, 0 ), // current element version
		    "processes that need to allocate space for subprocesses.", // description of component.
		    COMPONENT_CATEGORY_UNCATEGORIZED // * Not grouped in a category. (No category to filter with via sst-info).
	    )
	
	    // Parameters, description, default value
	    SST_ELI_DOCUMENT_PARAMS(
	    	{"maxSubProcesses", "The number of children a process needs.", "10"},
			{"tickFreq", "The frequency the process is called at.", "10ns"}
	    )

	    // Port name, description, event type
	    SST_ELI_DOCUMENT_PORTS(
		    {"memoryPort", "Port to connect to shared memory", {"sst.Interfaces.StringEvent"}},
	    )

    private:
		int64_t randSeed; 			// Seed for MarsagliaRNG
		SST::RNG::MarsagliaRNG *rng; 

        SST::Output output;				
		int processID;				// ID for processMemory to identify ports
        int numSubProcesses; 		// number of children prcesses held
        int maxSubProcesses; 		// number of children processes required for process to continue  
        bool cantFindSpace;  		// process has potentially entered a livelock  

		std::string clock; 			// Defining a clock which can be described via unit math as a string (?).
		SST::SimTime_t memoryFilled;// keeps track of how long we've been requesting for memory
		SST::SimTime_t startTask;
		SST::SimTime_t taskLength;
		SST::Link *memoryPort; 		// Pointer to processMemeory port  

};
#endif