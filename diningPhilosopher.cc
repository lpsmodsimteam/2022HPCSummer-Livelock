/**
Implementation of the dining philosophers problem to model livelock
*
*/

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/stopAction.h>
#include <sst/core/simulation.h>
#include "diningPhilosopher.h"

using SST::Interfaces::StringEvent;

diningPhilosopher::diningPhilosopher( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {
	
	output.init("diningPhilosopher-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

	// Get parameters
	clock = params.find<std::string>("thinkingDuration", "15s");
	RandomSeed = params.find<int64_t>("randomseed", 151515);
    eatingDuration = params.find<int64_t>("eatingduration", 2000);

	// Register the clock
	registerClock(clock, new SST::Clock::Handler<diningPhilosopher>(this, &diningPhilosopher::clockTick));
	
	// Initialize variables
	rng = new SST::RNG::MarsagliaRNG(15, RandomSeed);
    holdingLeftChopstick = false;
    holdingRightChopstick = false;
    status = HUNGRY;
    chopStatus = REQUESTING;

    int temp = (int)(rng->generateNextInt32()); // Generate a random 32-bit integer
	firstPass = abs((int)(temp % 2)); // Generate a integer 0-1.
    output.output(CALL_INFO, "First Pass: %d\n", firstPass);

    // register philosopher as a primary component
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();
	
	// Configure our port
	philosopherLeft = configureLink("philosopherLeft", "1ns", new SST::Event::Handler<diningPhilosopher, std::string>(this, &diningPhilosopher::handleEvent, "leftPort"));
	if ( !philosopherLeft ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'philosopherLeft'\n");
	}
    philosopherRight = configureLink("philosopherRight", "1ns", new SST::Event::Handler<diningPhilosopher, std::string>(this, &diningPhilosopher::handleEvent, "rightPort"));
	if ( !philosopherRight ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'philosopherRight'\n");
	}
}

diningPhilosopher::~diningPhilosopher() {

}

void diningPhilosopher::setup() {
    // randomize first chopstick grab to vary simulation
	output.verbose(CALL_INFO, 1, 0, "Component is being setup.\n");
    if (firstPass) {
        output.output(CALL_INFO, "is now requesting a left chopstick\n");
		struct ChopstickRequest chopreq = { holdingLeftChopstick, holdingRightChopstick, status, REQUESTING };
		philosopherLeft->send(new ChopstickRequestEvent(chopreq));
    } else {
        output.output(CALL_INFO, "is now requesting a right chopstick\n");
		struct ChopstickRequest chopreq = { holdingLeftChopstick, holdingRightChopstick, status, REQUESTING };
		philosopherRight->send(new ChopstickRequestEvent(chopreq));
    } 
}

// check philosopher's current status --> called every delay
bool diningPhilosopher::clockTick( SST::Cycle_t currentCycle ) {
    // Output current status
    std::cout << "Sim-Time: " << getCurrentSimTimeNano() << std::endl;
    output.output(CALL_INFO, "Left chopstick status: %d\n", holdingLeftChopstick);
    output.output(CALL_INFO, "Right chopstick status: %d\n", holdingRightChopstick);
    switch (status) {
    case THINKING: 
        output.output(CALL_INFO, "Status: THINKING\n");
        break;
    case HUNGRY:
        output.output(CALL_INFO, "Status: HUNGRY\n");
        break;
    case EATING:
        output.output(CALL_INFO, "Status: EATING\n");
        break;
    default:
        output.output(CALL_INFO, "Status: other\n");
        break;
    } 

    // decide if/who to request a chopstick
    if (status == HUNGRY) {
        output.output(CALL_INFO, "is now hungry\n");
        if (!holdingLeftChopstick) {
            output.output(CALL_INFO, "is now requesting a left chopstick\n");
		    struct ChopstickRequest chopreq = { holdingLeftChopstick, holdingRightChopstick, status, REQUESTING };
		    philosopherLeft->send(new ChopstickRequestEvent(chopreq));
            // return false;
        } else if (!holdingRightChopstick) {
            output.output(CALL_INFO, "is now requesting a right chopstick\n");
		    struct ChopstickRequest chopreq = { holdingLeftChopstick, holdingRightChopstick, status, REQUESTING };
		    philosopherRight->send(new ChopstickRequestEvent(chopreq));
            // return false;
        } 

        // both hungry and has both chopsticks, can eat
        if (holdingLeftChopstick && holdingRightChopstick) {
            status = EATING;
            startEating = getCurrentSimTimeNano();
            output.output(CALL_INFO, "is now eating\n");
        // }
        } else if (!(holdingLeftChopstick && holdingRightChopstick)) {
            output.output(CALL_INFO, "is now releasing chopsticks and thinking\n");
            holdingLeftChopstick = false;
            holdingRightChopstick =false;
            status = THINKING;
        }
        return false;
    } 
    
    // since there's been a delay before this call, we can switch status
    else if (status == THINKING) {
        output.output(CALL_INFO, "is now thinking\n");
        status = HUNGRY;
    }

    // check whether or not we're done eating
    else if (status == EATING) {
        output.output(CALL_INFO, "is now eating\n");
        if (getCurrentSimTimeNano() - startEating >= eatingDuration) {
            output.output(CALL_INFO, "finished eating, is now thinking\n");
            status = THINKING;
            holdingLeftChopstick = false;
            holdingRightChopstick = false;
        }
    }
	return false;
}

// this is where we attempt to grab one/both chopsticks
void diningPhilosopher::handleEvent(SST::Event *ev, std::string from) {
    // if holding both, set status to eat and wait a variable amount of time
    // if missing one, set potential livelock timer and resend event
    // if missing, set both chopsticks down, reset thinking status/timer
    // when grabbing right chopstick, check philosopherRight.holdingLeftChopstick
    // when grabbing left chopstick, check philosopherLeft.holdingRightChopstick
    // comparing two philosophers, one from request event, and this philosopher

    // event decides if we need to hand off chopsticks
    ChopstickRequestEvent *chopev = dynamic_cast<ChopstickRequestEvent*>(ev);
    output.output(CALL_INFO, "recieved a chopstick request\n");
	if ( chopev != NULL ) {
        philosopherStatus requesterStatus = chopev->chopreq.status;
        // check whether request is from left or right
        if (chopev->chopreq.chopStatus == REQUESTING) {
            // if we're not holding the chopstick, allow neighbor to hold it
            if (from == "leftPort") {
                if (!holdingLeftChopstick) {
                    output.output(CALL_INFO, "is sending left chopstick\n");
                    struct ChopstickRequest chopreq = { chopev->chopreq.holdingLeftChopstick, true, chopev->chopreq.status, SENDING };
		            philosopherLeft->send(new ChopstickRequestEvent(chopreq));
                }
            } else if (from == "rightPort") {
                if (!holdingRightChopstick) {
                    output.output(CALL_INFO, "is sending right chopstick\n");
                    struct ChopstickRequest chopreq = { true, chopev->chopreq.holdingRightChopstick, chopev->chopreq.status, SENDING };
		            philosopherRight->send(new ChopstickRequestEvent(chopreq));
                }
            }
        } else if (chopev->chopreq.chopStatus == SENDING) {
            // someone sent over a chopstick, we can update our hands
            output.output(CALL_INFO, "setting chopsticks\n");
            holdingLeftChopstick = chopev->chopreq.holdingLeftChopstick;
            holdingRightChopstick = chopev->chopreq.holdingRightChopstick;
            output.output(CALL_INFO, "Left chopstick status: %d\n", holdingLeftChopstick);
            output.output(CALL_INFO, "Right chopstick status: %d\n", holdingRightChopstick);
            // if we've now obtained both, we can start to eat
            if (holdingLeftChopstick && holdingRightChopstick) {
                output.output(CALL_INFO, "obtained both chopsticks\n");
                status = EATING;
                startEating = getCurrentSimTimeNano();
            } else if (!(holdingLeftChopstick && holdingRightChopstick)) {
                // if we don't have both, allow someone else to grab them
                output.output(CALL_INFO, "is now releasing chopsticks and thinking\n");
                holdingLeftChopstick = false;
                holdingRightChopstick =false;
                status = THINKING;
             }
        }
    } 
}