#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/stopAction.h>
#include <sst/core/simulation.h>
#include "chopsticks.h"

using SST::Interfaces::StringEvent;

chopsticks::chopsticks( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {
    output.init("chopsticks-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

    // set the table to be holding all of the chopsticks
    chopstickInfo = { TABLE, true, "L1R2" };
    chopstickInfo.name = params.find<std::string>("name", "L1R2");

	// Configure our ports
	leftPhilosopher = configureLink("leftPhilosopher", "1ns", new SST::Event::Handler<chopsticks, std::string>(this, &chopsticks::handleEvent, "one"));
	if ( !leftPhilosopher ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'leftPhilosopher'\n");
	}
    rightPhilosopher = configureLink("rightPhilosopher", "1ns", new SST::Event::Handler<chopsticks, std::string>(this, &chopsticks::handleEvent, "two"));
	if ( !rightPhilosopher ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'rightPhilosopher'\n");
	}
}

chopsticks::~chopsticks() {

}

// table is recieving a request from a philosopher
// then sending an update about chopstick status
void chopsticks::handleEvent(SST::Event *ev, std::string from) {
    PhilosopherRequestEvent *philev = dynamic_cast<PhilosopherRequestEvent*>(ev);
	if ( philev != NULL ) {
        int philid = philev->philreq.id;
        chopstickAvailability chopNeeded = philev->philreq.side;
        chopstickStatus status = philev->philreq.status;
        output.output(CALL_INFO, "recieved a %s chopstick request from philosopher %d\n", returnSide(chopNeeded).c_str(), philid);
        
        // check whether request is from left or right
        if (status == REQUESTING) {
            if (chopNeeded == LEFT) {
                chopstickInfo = sendOverChopstick(chopstickInfo, philid, chopNeeded);
            } else if (chopNeeded == RIGHT) {
                chopstickInfo = sendOverChopstick(chopstickInfo, philid, chopNeeded);
            } else {
                // not a valid request
                output.output(CALL_INFO, " recieved an invalid request\n");
            }
        } else if (status == SENDING ) {
            // philosopher is placing down their chopstick
            if (chopNeeded == LEFT) {
                chopstickInfo = updateChopstick(chopstickInfo, philid);
            } else if (chopNeeded == RIGHT) {
                chopstickInfo = updateChopstick(chopstickInfo, philid);
            } else {
                // not a valid request
                output.output(CALL_INFO, " recieved an invalid request\n");
            }
        }
    }
}

holdingStatus chopsticks::convertIDToStatus (int philid) {
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

Chopstick chopsticks::sendOverChopstick(Chopstick chopstick, int philid, chopstickAvailability side) {
    if (chopstick.available && chopstick.status == TABLE) {
        chopstick.available = false;
        if (side == LEFT) {
            output.output(CALL_INFO, "sending a left chopstick to philosopher %d\n", philid);
            struct ChopstickRequest chopreq = { true, LEFT };
            leftPhilosopher->send(new ChopstickRequestEvent(chopreq));
        } else {
            output.output(CALL_INFO, "sending a right chopstick to philosopher %d\n", philid);
            struct ChopstickRequest chopreq = { true, RIGHT };
            rightPhilosopher->send(new ChopstickRequestEvent(chopreq));
        }
    }
    return chopstick;
}

Chopstick chopsticks::updateChopstick(Chopstick chopstick, int philid) {
    output.output(CALL_INFO, "%s is returned by philosopher %d\n", chopstick.name.c_str(), philid);
    chopstick.available = true;
    chopstick.status = TABLE;
    return chopstick;
}

std::string chopsticks::returnSide(chopstickAvailability chopNeeded) {
    if (chopNeeded == LEFT) {
        return "left";
    } else {
        return "right";
    }
}