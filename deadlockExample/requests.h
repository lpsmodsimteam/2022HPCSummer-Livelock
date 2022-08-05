/**
 * @file requests.h
 * @brief  This file defines objects that allow us to send requests back and 
 * forth between the dining philosophers and the dining table
 * 
 */

/**
 * @brief This is used for our diningPhilosopher components, and it keeps track 
 * of which state they are currently in
 * 
 */
enum philosopherStatus {
	THINKING,
	HUNGRY,
	EATING
};

/**
 * @brief This is used by the philosopher in their request to the dining table, 
 * and lets the table know if they are requesting a chopstick to hold, or if 
 * they're sending a chopstick back  
 * 
 */
enum chopstickStatus {
	REQUESTING,
	SENDING
};

/**
 * @brief This is used by both dining table and philosopher requests, and 
 * distinguishes if a left or right chopstick is being sent or requested over 
 * a particular link
 * 
 */
enum chopstickAvailability{
	LEFT,
	RIGHT
};

/**
 * @brief This is used by our chopstick struct, and allows it to keep track 
 * of if a philosopher is holding it, or if it's on the table
 * 
 */
enum holdingStatus {
	TABLE,
	ONE,
	TWO,
    THREE,
    FOUR,
    FIVE
};

/**
 * @brief This is a request sent by the philosopher to the dining table, 
 * detailing which chopstick they currently need
 * 
 */
struct PhilosopherRequest {
	int8_t id;
	chopstickAvailability side;
	chopstickStatus status;
};

/**
 * @brief These are the chopsticks that are passed around in the simulation
 * 
 */
struct Chopstick {
    holdingStatus status;
    bool available;
	std::string name;
};

/**
 * @brief This is a request sent by the dining table to the dining philosophers, 
 * and lets them know if the chopstick they requested is actually free for 
 * them to take
 * 
 */
struct ChopstickRequest {
	bool available;
	chopstickAvailability leftOrRight;
};

/*! 
 * This class creates a message that the dining table sends over to one of the 
 * philosophers when they request a chopstick.  The ChopstickRequest object is 
 * created separatley, then passed into this class and serialized.  By doing so, 
 * the data is organized in such a way that after its sent over its link to 
 * the philosopher, that component knows how to read over and extract 
 * information from this event.
 *
 */
class ChopstickRequestEvent : public SST::Event {

public:
	void serialize_order(SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & chopreq.available;
		ser & chopreq.leftOrRight;
	}
	
	/**
	 * @brief Construct a new Chopstick Request Event object that wraps the 
	 * ChopstickRequest struct
	 * 
	 * @param chopreq Chopstick Request Struct that contains information 
	 * regarding whether or not the philosopher can pick up a chopstick
	 */
	ChopstickRequestEvent(ChopstickRequest chopreq) :
		Event(),
		chopreq(chopreq)
	{}
	
	ChopstickRequestEvent() {} // For Serialization only
	ChopstickRequest chopreq; 

	// this serializes the event we created so it can be sent over a link
	ImplementSerializable(ChopstickRequestEvent); 
};

/*! 
 * This class creates a message that the dining philosopher sends over to the 
 * table when they request a chopstick.  The PhilosopherRequest object is 
 * created separatley, then passed into this class and serialized.  By doing so, 
 * the data is organized in such a way that after its sent over its link to 
 * the table, that component knows how to read over and extract 
 * information from this event.
 *
 */
class PhilosopherRequestEvent : public SST::Event {

public:
	void serialize_order(SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & philreq.id;
		ser & philreq.side;
		ser & philreq.status;
	}
	
	/**
	 * @brief Construct a new Philosopher Request Event object that wraps the 
	 * PhilosopherRequest struct
	 * 
	 * @param philreq Philosopher Request struct that contains information 
	 * regarding which chopstick is being requested or sent over the link
	 */
	PhilosopherRequestEvent(PhilosopherRequest philreq) :
		Event(),
		philreq(philreq)
	{}

	PhilosopherRequestEvent() {} // For Serialization only
	PhilosopherRequest philreq; 

	// this serializes the event we created so it can be sent over a link
	ImplementSerializable(PhilosopherRequestEvent); 
};