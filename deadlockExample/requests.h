// holds information for requests to and from dining table to philosophers

enum philosopherStatus {
	THINKING,
	HUNGRY,
	EATING
};

enum chopstickStatus {
	REQUESTING,
	SENDING
};

enum chopstickAvailability{
	LEFT,
	RIGHT,
	NONE
};

enum holdingStatus {
	TABLE,
	ONE,
	TWO,
    THREE,
    FOUR,
    FIVE
};

struct PhilosopherRequest {
	int8_t id;
	chopstickAvailability side;
	chopstickStatus status;
};

struct Chopstick {
    holdingStatus status;
    bool available;
	std::string name;
};

struct ChopstickRequest {
	bool available;
	chopstickAvailability leftOrRight;
};

class ChopstickRequestEvent : public SST::Event {

public:
	void serialize_order(SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & chopreq.available;
		ser & chopreq.leftOrRight;
	}
	
	ChopstickRequestEvent(ChopstickRequest chopreq) :
		Event(),
		chopreq(chopreq)
	{}
	
	ChopstickRequestEvent() {} // For Serialization only
	ChopstickRequest chopreq; 
	ImplementSerializable(ChopstickRequestEvent); 
};


class PhilosopherRequestEvent : public SST::Event {

public:
	void serialize_order(SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & philreq.id;
		ser & philreq.side;
		ser & philreq.status;
	}
	
	PhilosopherRequestEvent(PhilosopherRequest philreq) :
		Event(),
		philreq(philreq)
	{}

	PhilosopherRequestEvent() {} // For Serialization only
	PhilosopherRequest philreq; 
	ImplementSerializable(PhilosopherRequestEvent); 
};