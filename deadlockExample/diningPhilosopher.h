#ifndef _diningPhilosopher_H
#define _diningPhilosopher_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>
#include "requests.h"

class diningPhilosopher : public SST::Component {

public:
	diningPhilosopher( SST::ComponentId_t id, SST::Params& params );
	~diningPhilosopher();

	void setup();

	bool clockTick( SST::Cycle_t currentCycle );
	// bool waitingTick( SST::Cycle_t currentCycle );
    void handleEvent(SST::Event *ev, std::string from);
	void outputTickInfo();

	// Register the component
	SST_ELI_REGISTER_COMPONENT(
		diningPhilosopher, // class
		"deadlock", // element library
		"diningPhilosopher", // component
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"philosopher to model the dining philosophers problem",
		COMPONENT_CATEGORY_UNCATEGORIZED
	)

	// Parameter name, description, default value
	SST_ELI_DOCUMENT_PARAMS(
		{ "thinkingDuration", "How long to wait between thinking and trying to eat", "15s" },
		{ "waitingClock", "How long to wait between checking if I have both chopsticks", "13s" },
		{ "randomseed", "Random Seed for car type generation", "151515" },
		{ "eatingduration", "length of time to eat", "2000" },
		{ "id", "id for dining table", "1" },
	)

	// Statistic name, description, unit, enable level
	SST_ELI_DOCUMENT_STATISTICS(
        { "eatingCounterStats", "Number of times a philosopher ate", "counter", 1 },  
        { "thinkingCounterStats", "Number of times a philosopher thought", "counter", 1 },
        { "hungryCounterStats", "Number of times a philosopher was hungry", "counter", 1 }
    )

	// Port name, description, event type
	SST_ELI_DOCUMENT_PORTS(
		{ "diningTable", "Connecting port to the dining table", {"sst.Interfaces.StringEvent", "leftPort"}},
	)

private:
	SST::Output output;
	SST::RNG::MarsagliaRNG* rng;
	std::string clock;
	// std::string waitingClock;
	int64_t RandomSeed;
	bool firstPass;
    int64_t eatingDuration;
  	int64_t startEating;
	int8_t philid;

	int eatingCounter;
	int thinkingCounter;
	int hungryCounter;

	SST::Link *diningTable;

    philosopherStatus status;
	chopstickStatus chopStatus;
    bool holdingLeftChopstick;
    bool holdingRightChopstick;

	// Statistics
	Statistic<int>* eatingCounterStats;
	Statistic<int>* thinkingCounterStats;
	Statistic<int>* hungryCounterStats;
};

#endif