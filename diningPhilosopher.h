#ifndef _diningPhilosopher_H
#define _diningPhilosopher_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>

// Status types
enum philosopherStatus {
	THINKING,
	HUNGRY,
	EATING
};

enum chopstickStatus {
	REQUESTING,
	SENDING
};

// Struct for a Message
struct ChopstickRequest {
	bool holdingLeftChopstick;
    bool holdingRightChopstick;
	philosopherStatus status;
	chopstickStatus chopStatus;
};


class ChopstickRequestEvent : public SST::Event {

public:
	void serialize_order(SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & chopreq.holdingLeftChopstick;
		ser & chopreq.holdingRightChopstick;
		ser & chopreq.status;
		ser & chopreq.chopStatus;
	}
	
	ChopstickRequestEvent(ChopstickRequest chopreq) :
		Event(),
		chopreq(chopreq)
	{}

	
	ChopstickRequestEvent() {} // For Serialization only
	ChopstickRequest chopreq; 
	ImplementSerializable(ChopstickRequestEvent); //
};

class diningPhilosopher : public SST::Component {

public:
	diningPhilosopher( SST::ComponentId_t id, SST::Params& params );
	~diningPhilosopher();

	bool clockTick( SST::Cycle_t currentCycle );
    void handleEvent(SST::Event *ev, std::string from);

	// Register the component
	SST_ELI_REGISTER_COMPONENT(
		diningPhilosopher, // class
		"livelock", // element library
		"diningPhilosopher", // component
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"philosopher to model the dining philosophers problem",
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
		{ "philosopherLeft", "Connecting port to the philosopher on one's left", {"sst.Interfaces.StringEvent", "leftPort"}},
        { "philosopherRight", "Connecting port to the philosopher on one's right", {"sst.Interfaces.StringEvent", "rightPort"}},
	)

private:
	SST::Output output;
	SST::RNG::MarsagliaRNG* rng;
	std::string clock;
	int64_t RandomSeed;
    SST::SimTime_t eatingDuration;
    SST::SimTime_t startEating;
	SST::Link *philosopherLeft;
    SST::Link *philosopherRight;

    philosopherStatus status;
	chopstickStatus chopStatus;
    bool holdingLeftChopstick;
    bool holdingRightChopstick;
};

#endif