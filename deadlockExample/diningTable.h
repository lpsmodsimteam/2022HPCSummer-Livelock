#ifndef _diningTable_H
#define _diningTable_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>
#include "requests.h"

/**
 * @file diningTable.h
 * @brief This creates a dining table component that stores all of the shared 
 * chopsticks for the simulation, as well as who is allowed to hold each one
 * 
 */
class diningTable : public SST::Component {

public:
	/**
	 * @brief Construct a new dining Table object
	 * 
	 * @param id The id for the component, this is passed in by SST. Usually 
	 * just need to pass it to the base SST::Component constructor
	 * @param params The params set by the project driver
	 */
	diningTable( SST::ComponentId_t id, SST::Params& params );

	/**
	 * @brief Destroy the dining Table object
	 * 
	 */
	~diningTable();

	/**
	 * @brief Takes in the numerical id passed to the dining table, and returns 
	 * the correct port for the corresponding philosopher
	 * 
	 * @param philid internal ID for each dining philosopher
	 * @return SST::Link* returns the port to connect to that philosopher
	 */
	SST::Link * returnPhilosopherLink(int philid);

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
	 * @brief This function recieves chopstick requests from the philosophers, 
	 * and checks whether or not these chopsticks are available to be passed 
	 * back onto them
	 * 
	 * @param ev An event object that contains the details of the chopstick 
	 * request
	 * @param from The name of the philosopher so that the dining table can 
	 * distinguish different requests
	 * @return void
	 */
    void handleEvent(SST::Event *ev, std::string from);

	/**
	 * @brief This function takes in a philosopher's id, and returns the 
	 * corresponding status value that the chopstick can store
	 * 
	 * @param philid The id of the philosopher
	 * @return holdingStatus Which object is holding this chopstick
	 */
	holdingStatus convertIDToStatus(int philid);

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
		diningTable, // class
		"deadlock", // element library
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
	/**
	 * \endcond
	 */

private:
	/* Output about the simulation to display in the terminal */
	SST::Output output;

	/* A collection of links to each of the philosophers */
	SST::Link *philosopherOne;
    SST::Link *philosopherTwo;
    SST::Link *philosopherThree;
    SST::Link *philosopherFour;
    SST::Link *philosopherFive;

	/* A list of the chopsticks that the dining table keeps track of, which 
	 * are knowledgable of which philosophers can hold them */ 
    Chopstick L1R2;
    Chopstick L2R3;
    Chopstick L3R4;
    Chopstick L4R5;
    Chopstick L5R1;
};

#endif