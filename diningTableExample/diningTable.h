#ifndef _diningTable_H
#define _diningTable_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>
#include "requests.h"

class diningTable : public SST::Component {

public:
	diningTable( SST::ComponentId_t id, SST::Params& params );
	~diningTable();
	SST::Link * returnPhilosopherLink(int philid);
	Chopstick sendOverChopstick(Chopstick chopstick, int philid, chopstickAvailability side);

    void handleEvent(SST::Event *ev, std::string from);
	holdingStatus convertIDToStatus(int philid);
	Chopstick updateChopstick(Chopstick chopstick, int philid);


	// Register the component
	SST_ELI_REGISTER_COMPONENT(
		diningTable, // class
		"livelock", // element library
		"diningTable", // component
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"table to hold set of chopsticks",
		COMPONENT_CATEGORY_UNCATEGORIZED
	)

	// Parameter name, description, default value
	SST_ELI_DOCUMENT_PARAMS(
		{ "thinkingDuration", "How long to wait between thinking an trying to eat", "15s" },
		{ "randomseed", "Random Seed for car type generation", "151515" },
		{ "eatingduration", "length of time to eat", "2000" }
	)

	// Port name, description, event type
	SST_ELI_DOCUMENT_PORTS(
		{ "philosopherOne", "Communication to philosopher one", {"sst.Interfaces.StringEvent", "one"}},
        { "philosopherTwo", "Communication to philosopher one", {"sst.Interfaces.StringEvent", "two"}},
        { "philosopherThree", "Communication to philosopher one", {"sst.Interfaces.StringEvent", "three"}},
        { "philosopherFour", "Communication to philosopher one", {"sst.Interfaces.StringEvent", "four"}},
        { "philosopherFive", "Communication to philosopher one", {"sst.Interfaces.StringEvent", "five"}},
	)

private:
	SST::Output output;
	SST::RNG::MarsagliaRNG* rng;
	int64_t RandomSeed;

	SST::Link *philosopherOne;
    SST::Link *philosopherTwo;
    SST::Link *philosopherThree;
    SST::Link *philosopherFour;
    SST::Link *philosopherFive;

    Chopstick L1R2;
    Chopstick L2R3;
    Chopstick L3R4;
    Chopstick L4R5;
    Chopstick L5R1;
};

#endif