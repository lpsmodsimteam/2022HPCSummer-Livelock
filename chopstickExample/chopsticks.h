#ifndef _chopsticks_H
#define _chopsticks_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>
#include "requests.h"

class chopsticks : public SST::Component {

public:
	chopsticks( SST::ComponentId_t id, SST::Params& params );
	~chopsticks();
	Chopstick sendOverChopstick(Chopstick chopstick, int philid, chopstickAvailability side);

    void handleEvent(SST::Event *ev, std::string from);
	holdingStatus convertIDToStatus(int philid);
	std::string returnSide(chopstickAvailability chopNeeded);

	Chopstick updateChopstick(Chopstick chopstick, int philid);


	// Register the component
	SST_ELI_REGISTER_COMPONENT(
		chopsticks, // class
		"test", // element library
		"chopsticks", // component
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"table to hold set of chopsticks",
		COMPONENT_CATEGORY_UNCATEGORIZED
	)

	// Parameter name, description, default value
	SST_ELI_DOCUMENT_PARAMS(
		{ "name", "Name of chopstick", "L1R2" },
	)

	// Port name, description, event type
	SST_ELI_DOCUMENT_PORTS(
		{ "leftPhilosopher", "Communication to left philosopher", {"sst.Interfaces.StringEvent", "leftPhilosopher"}},
        { "rightPhilosopher", "Communication to right philosopher", {"sst.Interfaces.StringEvent", "rightPhilosopher"}},
	)

private:
	SST::Output output;

	SST::Link *leftPhilosopher;
    SST::Link *rightPhilosopher;

	Chopstick chopstickInfo;
};

#endif