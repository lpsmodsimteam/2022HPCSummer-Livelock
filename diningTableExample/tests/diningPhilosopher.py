import sst

philosopherOne = sst.Component("Philosopher 1", "livelock.diningPhilosopher")
philosopherOne.addParams(
    {
        "thinkingDuration": "15s",  # how long one thinks after eating
        "waitingClock": "13s", # how long to wait between checking chopsticks
        "randomseed": "151515",  # random seed
        "eatingduration": "1", # how long one holds chopsticks while eating
        "id": "1"
    }
)

philosopherTwo = sst.Component("Philosopher 2", "livelock.diningPhilosopher")
philosopherTwo.addParams({"thinkingDuration": "15s", "waitingClock": "13s", "randomseed": "123", "eatingduration": "1", "id": "2"})

philosopherThree = sst.Component("Philosopher 3", "livelock.diningPhilosopher")
philosopherThree.addParams({"thinkingDuration": "15s", "waitingClock": "13s", "randomseed": "348734", "eatingduration": "1", "id": "3"})

philosopherFour = sst.Component("Philosopher 4", "livelock.diningPhilosopher")
philosopherFour.addParams({"thinkingDuration": "15s", "waitingClock": "13s", "randomseed": "123", "eatingduration": "1", "id": "4"})

philosopherFive = sst.Component("Philosopher 5", "livelock.diningPhilosopher")
philosopherFive.addParams({"thinkingDuration": "15s", "waitingClock": "13s", "randomseed": "348734", "eatingduration": "1", "id": "5"})

diningTable = sst.Component("Dining Table", "livelock.diningTable")
diningTable.addParams(
    {
        "randomseed": "151515"  # random seed
    }
)


# Connect the nodes by their ports.
sst.Link("Chopstick_One_Link").connect(
    (philosopherOne, "diningTable", "1ps"), (diningTable, "philosopherOne", "1ps")
)
sst.Link("Chopstick_Two_Link").connect(
    (philosopherTwo, "diningTable", "1ps"), (diningTable, "philosopherTwo", "1ps")
)
sst.Link("Chopstick_Three_Link").connect(
    (philosopherThree, "diningTable", "1ps"), (diningTable, "philosopherThree", "1ps")
)
sst.Link("Chopstick_Four_Link").connect(
    (philosopherFour, "diningTable", "1ps"), (diningTable, "philosopherFour", "1ps")
)
sst.Link("Chopstick_Five_Link").connect(
    (philosopherFive, "diningTable", "1ps"), (diningTable, "philosopherFive", "1ps")
)