#ifndef _chopsticks_H
#define _chopsticks_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>
#include "requests.h"

/**
 * @file chopsticks.h
 * @brief This file creates the chopstick component for the dining philosopher 
 * simulation.  Each of the chopsticks created connects to two philosophers, 
 * who all communicate with who is able to hold the chopstick at any given 
 * cycle.
 * 
 */

class chopsticks : public SST::Component {

public:
	/**
 	 * @brief Construct a new chopsticks object
	 * 
	 * @param id 
	 * @param params 
	 */
	chopsticks( SST::ComponentId_t id, SST::Params& params );

	/**
	 * @brief Destroy the chopsticks object
	 * 
	 */
	~chopsticks();

	/**
	 * @brief This function verifies that a chopstick requested by a philosopher
	 * is available, and sends it over after marking it as taken
	 * 
	 * In this function, once we've confirmed that the requested chopstick is 
	 * available, we have to construct a message object to send over to 
	 * our philosopher, letting them know that they are now in possesion of 
	 * this chopstick
	 * 
	 * @param chopstick The current chopstick object that is being requested
	 * @param philid Notes which philosopher is requesting a chopstick
	 * @param side Notes if the philosopher is requesting a left or right 
	 * chopstick
	 * @return Chopstick An updated chopstick object that is marked as taken
	 */
	Chopstick sendOverChopstick(Chopstick chopstick, int philid, chopstickAvailability side);

	/**
	 * @brief This function takes in requests from the philosophers, and 
	 * decides what needs to be done with the chopsticks as a result.
	 * 
	 * This function has to differentiate between seeing if the philosopher has 
	 * requested to hold a chopstick, or if they're sending one back after 
	 * being done holding it.  From there, it updates the availability and 
	 * ownership of said chopstick if need be.
	 * 
	 * @param ev This is an SST event object that contains information from 
	 * the request that the dining philosophers had sent over.
	 */
    void handleEvent(SST::Event *ev);

	/**
	 * @brief This function takes in a philosopher's id, and returns the 
	 * corresponding status value that the chopstick can store
	 * 
	 * @param philid The id of the philosopher
	 * @return holdingStatus Which object is holding this chopstick
	 */
	holdingStatus convertIDToStatus(int philid);

	/**
	 * @brief This function returns whether or not the philosopher wanted a 
	 * left or right chopstick as a string so we can output this information.
	 * 
	 * @param chopNeeded Contains left or right request status
	 * @return std::string Returns left or right as a string
	 */
	std::string returnSide(chopstickAvailability chopNeeded);

	/**
	 * @brief This function is called when a chopstick is being passed back to 
	 * the dining table, and marks it as available
	 * 
	 * @param chopstick The chopstick being returned
	 * @param philid The philosopher who returned the chopstick
	 * @return Chopstick Chopstick object with an updated availability
	 */
	Chopstick updateChopstick(Chopstick chopstick, int philid);

	/**
	 * \cond
	 */
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
	/**
	 * \endcond
	 */

private:
	/* Outputs status of simulaion to terminal */
	SST::Output output;

	/* Each chopstick has a link to their left and right philosopher, the 
	 * only ones who are allowed to hold this particular chopstick 
	 */
	SST::Link *leftPhilosopher;
    SST::Link *rightPhilosopher;

	/* Holds information regarding who's holding this chopstick, and if
	 * if it's available or not.
	 */
	Chopstick chopstickInfo;
};

#endif