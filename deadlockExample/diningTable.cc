#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/stopAction.h>
#include <sst/core/simulation.h>
#include "diningTable.h"

using SST::Interfaces::StringEvent;

diningTable::diningTable( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {
    
    // initalizes the name of each philosopher for our output
    output.init("diningTable-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

    // initalize the table to be holding all of the chopsticks
    L1R2 = { TABLE, true, "L1R2" };
    L2R3 = { TABLE, true, "L2R3" };
    L3R4 = { TABLE, true, "L3R4" };
    L4R5 = { TABLE, true, "L4R5" };
    L5R1 = { TABLE, true, "L5R1" };
	
	// Configure our ports
	philosopherOne = configureLink("philosopherOne", "1ns", new SST::Event::Handler<diningTable, std::string>(this, &diningTable::handleEvent, "one"));
	if ( !philosopherOne ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'philosopherOne'\n");
	}
    philosopherTwo = configureLink("philosopherTwo", "1ns", new SST::Event::Handler<diningTable, std::string>(this, &diningTable::handleEvent, "two"));
	if ( !philosopherTwo ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'philosopherTwo'\n");
	}
    philosopherThree = configureLink("philosopherThree", "1ns", new SST::Event::Handler<diningTable, std::string>(this, &diningTable::handleEvent, "three"));
	if ( !philosopherThree ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'philosopherThree'\n");
	}
    philosopherFour = configureLink("philosopherFour", "1ns", new SST::Event::Handler<diningTable, std::string>(this, &diningTable::handleEvent, "four"));
	if ( !philosopherFour ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'philosopherFour'\n");
	}
    philosopherFive = configureLink("philosopherFive", "1ns", new SST::Event::Handler<diningTable, std::string>(this, &diningTable::handleEvent, "five"));
	if ( !philosopherFive ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'philosopherFive'\n");
	}
}

diningTable::~diningTable() {

}

// table is recieving a request from a philosopher
// then sending an update about chopstick status
void diningTable::handleEvent(SST::Event *ev, std::string from) {
    // we know that we only send PhilosopherRequestEvents to the dining table, 
    // so we cast the parameter as such so we can read it properly
    PhilosopherRequestEvent *philev = dynamic_cast<PhilosopherRequestEvent*>(ev);
	if ( philev != NULL ) {
        // extract information from request event
        int philid = philev->philreq.id;
        chopstickAvailability chopNeeded = philev->philreq.side;
        chopstickStatus status = philev->philreq.status;
        output.verbose(CALL_INFO, 2, 0, "recieved a chopstick request from philosopher %d\n", philid);
        
        // check whether request is from left or right
        if (status == REQUESTING) {
            if ((philid == 1 && chopNeeded == LEFT) || (philid == 2 && chopNeeded == RIGHT)) {          // L1R2 Chopstick
                L1R2 = sendOverChopstick(L1R2, philid, chopNeeded);
            } else if ((philid == 2 && chopNeeded == LEFT) || (philid == 3 && chopNeeded == RIGHT)) {   // L2R3 Chopstick
                L2R3 = sendOverChopstick(L2R3, philid, chopNeeded);
            } else if ((philid == 3 && chopNeeded == LEFT) || (philid == 4 && chopNeeded == RIGHT)) {   // L3R4 Chopstick
                L3R4 = sendOverChopstick(L3R4, philid, chopNeeded);
            } else if ((philid == 4 && chopNeeded == LEFT) || (philid == 5 && chopNeeded == RIGHT)) {   // L4R5 Chopstick
                L4R5 = sendOverChopstick(L4R5, philid, chopNeeded);
            } else if ((philid == 5 && chopNeeded == LEFT) || (philid == 1 && chopNeeded == RIGHT)) {   // L5R1 Chopstick
                L5R1 = sendOverChopstick(L5R1, philid, chopNeeded);
            } else {
                // not a valid request
                output.fatal(CALL_INFO, -1, " recieved an invalid request\n");
            }

        } else if (status == SENDING ) {
            // dining table recieved a used chopstick from a philosopher
            if ((philid == 1 && chopNeeded == LEFT) || (philid == 2 && chopNeeded == RIGHT)) {          // L1R2 Chopstick
                L1R2 = updateChopstick(L1R2, philid);
            } else if ((philid == 2 && chopNeeded == LEFT) || (philid == 3 && chopNeeded == RIGHT)) {   // L2R3 Chopstick
                L2R3 = updateChopstick(L2R3, philid);
            } else if ((philid == 3 && chopNeeded == LEFT) || (philid == 4 && chopNeeded == RIGHT)) {   // L3R4 Chopstick
                L3R4 = updateChopstick(L3R4, philid);
            } else if ((philid == 4 && chopNeeded == LEFT) || (philid == 5 && chopNeeded == RIGHT)) {   // L4R5 Chopstick
                L4R5 = updateChopstick(L4R5, philid);
            } else if ((philid == 5 && chopNeeded == LEFT) || (philid == 1 && chopNeeded == RIGHT)) {   // L5R1 Chopstick
                L5R1 = updateChopstick(L5R1, philid);
            } else {
                // not a valid request
                output.fatal(CALL_INFO, -1, " recieved an invalid request\n");
            }
        }
        
    }
}

holdingStatus diningTable::convertIDToStatus (int philid) {
    if (philid == 1) {
        return ONE;
    } else if (philid == 2) {
        return TWO;
    } else if (philid == 3) {
        return THREE;
    } else if (philid == 4) {
        return FOUR;
    } else if (philid == 5) {
        return FIVE;
    } else {
        return TABLE;
    }
}

SST::Link * diningTable::returnPhilosopherLink(int philid) {
    switch (philid) {
    case 1:
        return philosopherOne;
        break;
    case 2:
        return philosopherTwo;
        break;
    case 3:
        return philosopherThree;
        break;
    case 4:
        return philosopherFour;
        break;
    case 5:
        return philosopherFive;
        break;
    default:
        return 0;   // unexpected id, return null
        break;
    }
}

Chopstick diningTable::sendOverChopstick(Chopstick chopstick, int philid, chopstickAvailability side) {
    // only send chopstick if the table has ownership of it at the time
    if (chopstick.available && chopstick.status == TABLE) {
        chopstick.available = false;
        chopstick.status = convertIDToStatus(philid);   // update ownership
        // send a message back to the philosopher over the correct port
        if (side == LEFT) {
            output.output(CALL_INFO, "sending a left chopstick to philosopher %d\n", philid);
            struct ChopstickRequest chopreq = { true, LEFT };
            returnPhilosopherLink(philid)->send(new ChopstickRequestEvent(chopreq));
        } else {
            output.output(CALL_INFO, "sending a right chopstick to philosopher %d\n", philid);
            struct ChopstickRequest chopreq = { true, RIGHT };
            returnPhilosopherLink(philid)->send(new ChopstickRequestEvent(chopreq));
        }
    }
    return chopstick;
}

Chopstick diningTable::updateChopstick(Chopstick chopstick, int philid) {
    output.output(CALL_INFO, "%s is returned by philosopher %d\n", chopstick.name.c_str(), philid);
    chopstick.available = true;
    chopstick.status = TABLE;
    return chopstick;
}