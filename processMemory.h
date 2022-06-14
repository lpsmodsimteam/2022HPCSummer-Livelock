#ifndef _processMemory_H
#define _processMemory_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>

#include<queue>


class processMemory : public SST::Component {
    public:

        processMemory( SST::ComponentId_t id, SST::Params& params ); // Constructor
	    ~processMemory(); // Deconstructor

	    bool tick( SST::Cycle_t currentCycle);

	    void handleEvent(SST::Event *ev);

        queue<int> q;
        int memorySpace [25];
        int memorySize()
;
    private:
        SST::Output output;
        int memory_size;
}