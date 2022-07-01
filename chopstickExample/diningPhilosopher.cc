/**
Implementation of the dining philosophers problem to model livelock

Trial and Error for Parameters:
- The more randomization you do to your time, the less likely you are to run into livelock
- By increasing how often we check if we're waiting, we're more likely to introduce livelock
- Shortening the duration of eating helps introduce livelock
- By removing the dropping of chopsticks in the waitingTick function, we introduce deadlock to the system instead
- As long as waitingClock < clock, we should have livelock

TODO:
- livelock detection
- clean up comments
*
*/

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/stopAction.h>
#include <sst/core/simulation.h>
#include "diningPhilosopher.h"

using SST::Interfaces::StringEvent;

diningPhilosopher::diningPhilosopher( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {
	
	output.init("dine-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

	// Get parameters
	clock = params.find<std::string>("thinkingDuration", "15s");
    waitingClock = params.find<std::string>("waitingClock", "15s");
	RandomSeed = params.find<int64_t>("randomseed", 151515);
    eatingDuration = params.find<int64_t>("eatingduration", 2000);
    philid = params.find<int8_t>("id", 1);

	// Register the clock
	registerClock(clock, new SST::Clock::Handler<diningPhilosopher>(this, &diningPhilosopher::clockTick));
	registerClock(waitingClock, new SST::Clock::Handler<diningPhilosopher>(this, &diningPhilosopher::waitingTick));

	// Initialize variables
	rng = new SST::RNG::MarsagliaRNG(15, RandomSeed);
    holdingLeftChopstick = false;
    holdingRightChopstick = false;
    status = HUNGRY;
    chopStatus = REQUESTING;

    // randomize first chopstick grab between left and right
    int temp = (int)(rng->generateNextInt32()); // Generate a random 32-bit integer
	firstPass = abs((int)(temp % 2)); // Generate a integer 0-1.
    output.output(CALL_INFO, "First Pass: %d\n", firstPass);

    // register philosopher as a primary component
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();
	
	// Configure our port
	leftChopstick = configureLink("leftChopstick", "1ns", new SST::Event::Handler<diningPhilosopher, std::string>(this, &diningPhilosopher::handleEvent, "leftChopstick"));
	if ( !leftChopstick ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'leftChopstick'\n");
	}
    rightChopstick = configureLink("rightChopstick", "1ns", new SST::Event::Handler<diningPhilosopher, std::string>(this, &diningPhilosopher::handleEvent, "rightChopstick"));
	if ( !rightChopstick ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'rightChopstick'\n");
	}
}

diningPhilosopher::~diningPhilosopher() {

}

void diningPhilosopher::setup() {
    // randomize first chopstick grab to vary simulation
	output.verbose(CALL_INFO, 1, 0, "Component is being setup.\n");
    if (firstPass) {
        output.output(CALL_INFO, "is now requesting a left chopstick with id %d\n", this->philid);
		struct PhilosopherRequest chopreq = { philid, LEFT, REQUESTING };
		leftChopstick->send(new PhilosopherRequestEvent(chopreq));
    } else {
        output.output(CALL_INFO, "is now requesting a right chopstick\n");
		struct PhilosopherRequest chopreq = { philid, RIGHT, REQUESTING };
		rightChopstick->send(new PhilosopherRequestEvent(chopreq));
    } 
}

// check philosopher's current status --> called every delay
bool diningPhilosopher::clockTick( SST::Cycle_t currentCycle ) {
    // Output current status
    outputTickInfo();

    // decide if/who to request a chopstick
    // only request one chopstick per tick
    if (status == HUNGRY) {
        output.output(CALL_INFO, "is now hungry\n");
        if (!holdingLeftChopstick) {
            output.output(CALL_INFO, "is now requesting a left chopstick with id %d\n", philid);
		    struct PhilosopherRequest chopreq = { philid, LEFT, REQUESTING };
		    leftChopstick->send(new PhilosopherRequestEvent(chopreq));
            // return false;
        } else if (!holdingRightChopstick) {
            output.output(CALL_INFO, "is now requesting a right chopstick with id %d\n", philid);
		    struct PhilosopherRequest chopreq = { philid, RIGHT, REQUESTING };
		    rightChopstick->send(new PhilosopherRequestEvent(chopreq));
            // return false;
        } 

        // both hungry and has both chopsticks, can eat
        if (holdingLeftChopstick && holdingRightChopstick) {
            status = EATING;
            startEating = getCurrentSimTimeNano();
            output.output(CALL_INFO, "is now eating\n");
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
        if (getCurrentSimTimeNano() - startEating >= eatingDuration) {
            output.output(CALL_INFO, "is now full, switched to thinking\n");
            status = THINKING;
            if (holdingLeftChopstick) {
                output.output(CALL_INFO, "is now sending back a left chopstick with id %d\n", philid);
		        struct PhilosopherRequest chopreq = { philid, LEFT, SENDING };
		        leftChopstick->send(new PhilosopherRequestEvent(chopreq));
                holdingLeftChopstick = false;
            } else if (holdingRightChopstick) {
                output.output(CALL_INFO, "is now sending back a right chopstick with id %d\n", philid);
		        struct PhilosopherRequest chopreq = { philid, RIGHT, SENDING };
		        rightChopstick->send(new PhilosopherRequestEvent(chopreq));
                holdingRightChopstick = false;
            }
        } else {
            output.output(CALL_INFO, "is still eating\n");
        }
    }
	return false;
}

bool diningPhilosopher::waitingTick( SST::Cycle_t currentCycle ) {
    // Output current status
    outputTickInfo();

    // we use exclusive or to check if we're only holding one chopstick
    if (holdingLeftChopstick ^ holdingRightChopstick) {
            if (holdingLeftChopstick) {
                output.output(CALL_INFO, "is now sending back a left chopstick with id %d\n", philid);
		        struct PhilosopherRequest chopreq = { philid, LEFT, SENDING };
		        leftChopstick->send(new PhilosopherRequestEvent(chopreq));
                holdingLeftChopstick = false;
            } else if (holdingRightChopstick) {
                output.output(CALL_INFO, "is now sending back a right chopstick with id %d\n", philid);
		        struct PhilosopherRequest chopreq = { philid, RIGHT, SENDING };
		        rightChopstick->send(new PhilosopherRequestEvent(chopreq));
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
    // comparing two philosophers, one from request event, and this philosopher

    // event decides if we need to hand off chopsticks
    ChopstickRequestEvent *chopev = dynamic_cast<ChopstickRequestEvent*>(ev);
    output.output(CALL_INFO, "recieved a chopstick request from %s\n", from.c_str());
	if ( chopev != NULL ) {

        bool available = chopev->chopreq.available;
        chopstickAvailability chopstickAvailable = chopev->chopreq.leftOrRight;
        if (available) {
            switch (chopstickAvailable) {
            case LEFT:
                output.output(CALL_INFO, "Obtained left chopstick\n");
                holdingLeftChopstick = true;
                break;
            case RIGHT:
                output.output(CALL_INFO, "Obtained right chopstick\n");
                holdingRightChopstick = true;
                break;
            default:
                break;
            }
        }
        output.output(CALL_INFO, "Left chopstick status: %s\n", returnStatus(holdingLeftChopstick).c_str());
        output.output(CALL_INFO, "Right chopstick status: %s\n", returnStatus(holdingRightChopstick).c_str());
        // if we've now obtained both, we can start to eat
        if (holdingLeftChopstick && holdingRightChopstick) {
            output.output(CALL_INFO, "obtained both chopsticks\n");
            status = EATING;
            startEating = getCurrentSimTimeNano();
        } 
    } 
}

void diningPhilosopher::outputTickInfo() {
    std::cout << "Sim-Time: " << getCurrentSimTimeNano() << std::endl;
    output.output(CALL_INFO, "Waiting clock\n");
    output.output(CALL_INFO, "Left chopstick status: %s\n", returnStatus(holdingLeftChopstick).c_str());
    output.output(CALL_INFO, "Right chopstick status: %s\n", returnStatus(holdingRightChopstick).c_str());
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
}

std::string diningPhilosopher::returnStatus(bool chopstickStatus) {
    if (chopstickStatus) {
        return "holding";
    } else {
        return "not holding";
    }
}