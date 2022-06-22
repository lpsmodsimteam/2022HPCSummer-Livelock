/**
Car Generator for simpleCarWash simulator

This component generates "cars" which are sent to the carwash
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
	rng = new SST::RNG::MarsagliaRNG(11, RandomSeed);
    holdingLeftChopstick = false;
    holdingRightChopstick = false;

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

// check philosopher's current status --> called every delay
bool diningPhilosopher::clockTick( SST::Cycle_t currentCycle ) {
    // Output current status
    output.output(CALL_INFO, "Status: %d\n", status);
	std::cout << "Sim-Time: " << getCurrentSimTimeNano() << std::endl;

    // decide if/who to request a chopstick
    if (status == HUNGRY) {
        output.output(CALL_INFO, "is now hungry\n");
        if (!holdingLeftChopstick) {
		    struct ChopstickRequest chopreq = { holdingLeftChopstick, holdingRightChopstick, status, REQUESTING };
		    philosopherLeft->send(new ChopstickRequestEvent(chopreq));
        } else if (!holdingRightChopstick) {
		    struct ChopstickRequest chopreq = { holdingLeftChopstick, holdingRightChopstick, status, REQUESTING };
		    philosopherRight->send(new ChopstickRequestEvent(chopreq));
        } else if (holdingLeftChopstick && holdingRightChopstick) {
            // both hungry and has both chopsticks, can eat
            status = EATING;
            startEating = getCurrentSimTimeNano();
            output.output(CALL_INFO, "is now eating\n");
        }
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

    // recieves memory requests, sends out whether or not it had space
    ChopstickRequestEvent *chopev = dynamic_cast<ChopstickRequestEvent*>(ev);
	if ( chopev != NULL ) {
        // int processID;
        // int returnValue;
        // processID = memev->memreq.pid;
        philosopherStatus requesterStatus = chopev->chopreq.status;
        // check whether request is from left or right
        if (chopev->chopreq.chopStatus == REQUESTING) {
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
            output.output(CALL_INFO, "setting chopsticks\n");
            holdingLeftChopstick = chopev->chopreq.holdingLeftChopstick;
            holdingRightChopstick = chopev->chopreq.holdingRightChopstick;
            if (holdingLeftChopstick && holdingRightChopstick) {
                output.output(CALL_INFO, "obtained both chopsticks\n");
                status = EATING;
            }
        }
        
    }
        
}