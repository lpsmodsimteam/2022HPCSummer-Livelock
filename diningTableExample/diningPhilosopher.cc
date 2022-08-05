/**
 * Implementation of the dining philosophers problem to model livelock
 *
 */

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/stopAction.h>
#include <sst/core/simulation.h>
#include "diningPhilosopher.h"

diningPhilosopher::diningPhilosopher( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {
	
    // initalizes the name of each philosopher for our output
	output.init("diningPhilosopher-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

	// This grabs the parameters that were defined in the python test file in 
    // order to initalize our component
	clock = params.find<std::string>("thinkingDuration", "15s");
    waitingClock = params.find<std::string>("waitingClock", "15s");
	RandomSeed = params.find<int64_t>("randomseed", 151515);
    eatingDuration = params.find<int64_t>("eatingduration", 2);
    philid = params.find<int8_t>("id", 1);
    livelockCycle = params.find<int64_t>("livelockCycle", 10000);
    windowSize = params.find<int64_t>("windowSize", 100);

    /*
     * The register clock functions take in a duration of time that was defined 
     * by the parameters above, and ties a function to it that is called 
     * every time this cycle passes.  In this simulation, we have two 
     * different clocks that run on different cycles.  The reasoning behind this 
     * is defined in more detail in the report file.
     */
	registerClock(clock, new SST::Clock::Handler<diningPhilosopher>(this, &diningPhilosopher::clockTick));
	registerClock(waitingClock, new SST::Clock::Handler<diningPhilosopher>(this, &diningPhilosopher::waitingTick));
	
	// Initialize private variables
	rng = new SST::RNG::MarsagliaRNG(15, RandomSeed);
    holdingLeftChopstick = false;
    holdingRightChopstick = false;
    status = HUNGRY;
    eatingCounter = 0;
    thinkingCounter = 0;
    hungryCounter = 0;

    // randomize first chopstick grab between left and right
    int temp = (int)(rng->generateNextInt32());                 // Generate a random 32-bit integer
	firstPass = abs((int)(temp % 2));                           // Generate a integer 0-1.

    // register philosopher as a primary component
    // this function means that in order for our simulation to exit, every
    // component that was registered as "primary" has to okay an exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();
	
	// Configure our port, which links us to other components in the simulation
	diningTable = configureLink("diningTable", "1ns", new SST::Event::Handler<diningPhilosopher>(this, &diningPhilosopher::handleEvent));
	if ( !diningTable ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'diningTable'\n");
	}

    // Setup statistics, which output a counter for the amount of times 
    // each philosopher enters one of these states
	eatingCounterStats = registerStatistic<int>("eatingCounterStats");
	thinkingCounterStats = registerStatistic<int>("thinkingCounterStats");
	hungryCounterStats = registerStatistic<int>("hungryCounterStats");
    
    // Enabling SST CSV File Output to track livelock likelihood
    std::string outputName = getName() + "-livelock.csv";
    csvout.init("CSVOUT", 1, 0, SST::Output::FILE, outputName);
    csvout.output("Cycle,State Progress,Thinking Rate,Hungry Rate\n");
}

diningPhilosopher::~diningPhilosopher() {

}

void diningPhilosopher::setup() {
    // randomize first chopstick grab to vary simulation
    // create a PhilosopherRequest event to be sent to the dining table
    if (firstPass) {
        output.verbose(CALL_INFO, 2, 0, "is now requesting a left chopstick with id %d\n", this->philid);
		struct PhilosopherRequest chopreq = { philid, LEFT, REQUESTING };
		diningTable->send(new PhilosopherRequestEvent(chopreq));
    } else {
        output.verbose(CALL_INFO, 2, 0, "is now requesting a right chopstick\n");
		struct PhilosopherRequest chopreq = { philid, RIGHT, REQUESTING };
		diningTable->send(new PhilosopherRequestEvent(chopreq));
    } 
}

bool diningPhilosopher::clockTick( SST::Cycle_t currentCycle ) {
    // Output current status
    outputTickInfo();

    // track state changes every clock cycle
    output.verbose(CALL_INFO, 4, 0, "current clock cycle: %ld\n", currentCycle);
    performStatisticOutput(eatingCounterStats);
    performStatisticOutput(thinkingCounterStats);
    performStatisticOutput(hungryCounterStats);

    // livelock detection
    if (currentCycle == livelockCycle) {
        // marks this philosopher as ready to exit the simulation
        // does not end until everyone gives an okay
        primaryComponentOKToEndSim();
    } else if ((currentCycle % windowSize) == 1) { // marks beginning of window
        // note down current state of the system at the beginning of a window
        eatingCounterInitial = eatingCounter;
	    thinkingCounterInitial = thinkingCounter;
	    hungryCounterInitial = hungryCounter;
    } else if ((currentCycle % windowSize) == 0) { // marks end of window
        // compare to initial state, make decision about livelock
        // stateProgress checks whether or not this philosopher ate during the window
        // this would quantify whether or not we made any meaningul progress
        bool stateProgress = (eatingCounterInitial == eatingCounter);

        // these 2 rates check how frequently we were making "non meaningful state changes"
        float thinkingRate = (thinkingCounter - thinkingCounterInitial);
        float hungryRate = ((hungryCounter - hungryCounterInitial));
        
        // this is divided by 2 to take into consideration how long it takes a state to switch
        // in a true livelock scenario, you spend half the time thinking, and the other half hungry
        // by dividing the window size, we see how much of this expected time we spend in each state
        int windowSizeIndividual = windowSize/2;
        thinkingRate = thinkingRate/windowSizeIndividual;
        hungryRate = hungryRate/windowSizeIndividual;

        // multiply by 100 to create a percentage
        thinkingRate = thinkingRate*100;
        hungryRate = hungryRate*100;

        // output final rates to terminal and csv file for analysis
        output.verbose(CALL_INFO, 1, 0, "Final Eating Progress Status: %d\n", stateProgress);
        output.verbose(CALL_INFO, 1, 0, "Final Rate for thinking status switches: %lf%% \n", thinkingRate);
        output.verbose(CALL_INFO, 1, 0, "Final Rate for hungry status switches: %lf%% \n", hungryRate);
        csvout.output("%ld,%d,%f,%f\n", currentCycle, stateProgress, thinkingRate, hungryRate);
    }

    // decide if/who to request a chopstick
    // only request one chopstick per tick
    if (status == HUNGRY) {
        output.verbose(CALL_INFO, 4, 0, "is now hungry\n");
        if (!holdingLeftChopstick) {
            output.verbose(CALL_INFO, 2, 0, "is now requesting a left chopstick with id %d\n", philid);
		    struct PhilosopherRequest chopreq = { philid, LEFT, REQUESTING };
		    diningTable->send(new PhilosopherRequestEvent(chopreq));
        } else if (!holdingRightChopstick) {
            output.verbose(CALL_INFO, 2, 0, "is now requesting a right chopstick with id %d\n", philid);
		    struct PhilosopherRequest chopreq = { philid, RIGHT, REQUESTING };
		    diningTable->send(new PhilosopherRequestEvent(chopreq));
        } 

        // both hungry and has both chopsticks, can eat
        if (holdingLeftChopstick && holdingRightChopstick) {
            status = EATING;
            eatingCounter++;
            eatingCounterStats->addData(eatingCounter);
            startEating = currentCycle;
            output.verbose(CALL_INFO, 3, 0,  "is now eating\n");
        } 
        return false;
    } 
    
    // since there's been a delay before this call, we can switch status
    else if (status == THINKING) {
        output.verbose(CALL_INFO, 4, 0, "is now thinking\n");
        status = HUNGRY;
        hungryCounter++;
        hungryCounterStats->addData(hungryCounter);
    }

    // check whether or not we're done eating
    else if (status == EATING) {
        output.verbose(CALL_INFO, 4, 0, "current cycle: %ld\n", currentCycle);
        output.verbose(CALL_INFO, 4, 0, "start eating: %ld\n", startEating);
        output.verbose(CALL_INFO, 4, 0, "eating duration %ld\n", eatingDuration);
        if ((currentCycle - startEating) >= eatingDuration) {
            output.verbose(CALL_INFO, 3, 0, "is now full, switched to thinking\n");
            status = THINKING;
            thinkingCounter++;
            thinkingCounterStats->addData(thinkingCounter);
            if (holdingLeftChopstick) {
                output.verbose(CALL_INFO, 2, 0, "is now sending back a left chopstick with id %d\n", philid);
		        struct PhilosopherRequest chopreq = { philid, LEFT, SENDING };
		        diningTable->send(new PhilosopherRequestEvent(chopreq));
                holdingLeftChopstick = false;
            } else if (holdingRightChopstick) {
                output.verbose(CALL_INFO, 2, 0, "is now sending back a right chopstick with id %d\n", philid);
		        struct PhilosopherRequest chopreq = { philid, RIGHT, SENDING };
		        diningTable->send(new PhilosopherRequestEvent(chopreq));
                holdingRightChopstick = false;
            }
        } else {
            output.verbose(CALL_INFO, 4, 0, "is still eating\n");
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
                output.verbose(CALL_INFO, 2, 0, "is now sending back a left chopstick with id %d\n", philid);
		        struct PhilosopherRequest chopreq = { philid, LEFT, SENDING };
		        diningTable->send(new PhilosopherRequestEvent(chopreq));
                holdingLeftChopstick = false;
                status = THINKING;
                thinkingCounter++;
                thinkingCounterStats->addData(thinkingCounter);
            } else if (holdingRightChopstick) {
                output.verbose(CALL_INFO, 2, 0, "is now sending back a right chopstick with id %d\n", philid);
		        struct PhilosopherRequest chopreq = { philid, RIGHT, SENDING };
		        diningTable->send(new PhilosopherRequestEvent(chopreq));
                holdingRightChopstick = false;
                status = THINKING;
                thinkingCounter++;
                thinkingCounterStats->addData(thinkingCounter);
            }
    }
    return false;
}

// this is where we attempt to grab one/both chopsticks
void diningPhilosopher::handleEvent(SST::Event *ev) {
    // if holding both, set status to eat and wait a variable amount of time
    // if missing one, set potential livelock timer and resend event
    // if missing, set both chopsticks down, reset thinking status/timer
    // comparing two philosophers, one from request event, and this philosopher

    // event decides if we need to hand off chopsticks
    ChopstickRequestEvent *chopev = dynamic_cast<ChopstickRequestEvent*>(ev);
    output.verbose(CALL_INFO, 4, 0, "recieved a chopstick request\n");
	if ( chopev != NULL ) {

        bool available = chopev->chopreq.available;
        chopstickAvailability chopstickAvailable = chopev->chopreq.leftOrRight;
        if (available) {
            switch (chopstickAvailable) {
            case LEFT:
                output.verbose(CALL_INFO, 4, 0, "Obtained left chopstick\n");
                holdingLeftChopstick = true;
                break;
            case RIGHT:
                output.verbose(CALL_INFO, 4, 0, "Obtained right chopstick\n");
                holdingRightChopstick = true;
                break;
            default:
                break;
            }
        }
    } 
}

void diningPhilosopher::outputTickInfo() {
    output.verbose(CALL_INFO, 4, 0, "Sim-Time: %ld\n", getCurrentSimTimeNano());
    output.verbose(CALL_INFO, 4, 0, "Waiting clock\n");
    output.verbose(CALL_INFO, 4, 0, "Left chopstick status: %d\n", holdingLeftChopstick);
    output.verbose(CALL_INFO, 4, 0, "Right chopstick status: %d\n", holdingRightChopstick);
    output.verbose(CALL_INFO, 4, 0, "Eating counter: %d\n", eatingCounter);
    output.verbose(CALL_INFO, 4, 0, "Thinking counter: %d\n", thinkingCounter);
    output.verbose(CALL_INFO, 4, 0, "Hungry counter: %d\n", hungryCounter);
    switch (status) {
    case THINKING: 
        output.verbose(CALL_INFO, 4, 0, "Status: THINKING\n");
        break;
    case HUNGRY:
        output.verbose(CALL_INFO, 4, 0, "Status: HUNGRY\n");
        break;
    case EATING:
        output.verbose(CALL_INFO, 4, 0, "Status: EATING\n");
        break;
    default:
        output.verbose(CALL_INFO, 4, 0, "Status: other\n");
        break;
    } 
}