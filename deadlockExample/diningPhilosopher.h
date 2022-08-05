#ifndef _diningPhilosopher_H
#define _diningPhilosopher_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>
#include "requests.h"

/**
 * @file diningPhilosopher.h
 * @brief This creates a dining Philosopher component that cycles between eating, 
 * thinking, and hungry states, and communicates with the dining table component
 * when it needs a chopstick to eat
 * 
 */
class diningPhilosopher : public SST::Component {

public:
	/**
	 * @brief Construct a new dining Philosopher object
	 * 
	 * @param id The id for the component, this is passed in by SST. Usually 
	 * just need to pass it to the base SST::Component constructor
	 * @param params The params set by the project driver
	 */
	diningPhilosopher( SST::ComponentId_t id, SST::Params& params );

	/**
	 * @brief Destroy the dining Philosopher object
	 * 
	 */
	~diningPhilosopher();

	/**
	 * @brief This function randomizes which chopstick each philosopher grabs 
	 * in the beginning of the simulation in order to randomize it.
	 * 
	 * Our simulation defines a random seed for each philosopher, and this seed 
	 * dictates whether or not the first request to the dining table is sent 
	 * for the chopstick on the left, or for the one on the right
	 * 
	 * @return void
	 */
	void setup();

	/**
	 * @brief This first clock function checks the status of the philosopher, 
	 * and updates their needs accordingly.  It also performs livelock 
	 * detection
	 * 
	 * This function contains the main functionality of the entire simulation.  
	 * It starts out by checking if we need to update our livelock window, which
	 * checks the status of the system and notes down the current livelock 
	 * metrics.  After this, we check the status of the philosopher, sending a 
	 * request to the dining table for a chopstick if we're hungry, or seeing if 
	 * we've elapsed out eating or thinking durations to switch states.
	 * 
	 * @param currentCycle This tells us what cycle of the simulation we're on, 
	 * where each philosopher's cycle may or may not have the same frequency.
	 * @return This returns whether or not the simulation should continue 
	 */
	bool clockTick( SST::Cycle_t currentCycle );

	/** 
	 * @brief handles messages sent to the dining philosophers 
	 * back from dining table
	 * 
	 * This function unwraps the information that was sent back from the 
	 * request to the dining table, and checks to see whether or not the
	 * chopstick that was asked for is actually available.  
 	 *
 	 * @param ev An event object that holds the contents of the message
	 * sent back from the dining table
 	 * @return void
 	*/
    void handleEvent(SST::Event *ev, std::string from);

	/** @brief function to output current status of simulation
	 * 
	 * This function places a majority of the status outputs into one function
	 * that can be called anywhere to see where chopsticks are being held
	 *  @return void
 	*/
	void outputTickInfo();

	/**
	 * \cond
	 */
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
	/**
	 * \endcond
	 */

private:
	SST::Output output;				/* Standard output to the terminal */
	SST::Output csvout; 			/* Data output to create graphs for livelock analysis */
	SST::RNG::MarsagliaRNG* rng; 	/* Random number generator for our first chopstick grab */
	std::string clock;				/* Clock that checks and updates the state of the philosopher every cycle */
	int64_t RandomSeed;				/* Seed pased into the MarsagliaRNG */
	bool firstPass;					/* Determines if the first chopstick grab is left or right */
    SST::Cycle_t eatingDuration;	/* How many cycles a philosopher holds both chopsticks while eating */
  	SST::Cycle_t startEating;		/* The first cycle a philosopher started eating */
	int8_t philid;					/* Internal ID so dining table can distinguish philosophers */
	SST::Cycle_t livelockCycle;		/* How many cycles our simulation should run for */
	SST::Cycle_t windowSize;		/* How often we should probe the system for livelock data */

	SST::Link *diningTable;			/* Link to connect philosopher with dining table */

    philosopherStatus status;		/* Notes what state the philosopher is in */
    bool holdingLeftChopstick;		
    bool holdingRightChopstick;

	// Statistics
	Statistic<int>* eatingCounterStats;
	Statistic<int>* thinkingCounterStats;
	Statistic<int>* hungryCounterStats;

	// State count at end of window
	int eatingCounter;
	int thinkingCounter;
	int hungryCounter;

	// State count at beginning of window
	int eatingCounterInitial;
	int thinkingCounterInitial;
	int hungryCounterInitial;
};

#endif