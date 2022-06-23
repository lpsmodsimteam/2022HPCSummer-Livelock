import sst

philosopherOne = sst.Component("Philosopher 1", "livelock.diningPhilosopher")
philosopherOne.addParams(
    {
        "thinkingDuration": "15s",  # how long one thinks after eating
        "randomseed": "151515",  # random seed
        "eatingduration": "2000" # how long one holds chopsticks while eating
    }
)

philosopherTwo = sst.Component("Philosopher 2", "livelock.diningPhilosopher")
philosopherTwo.addParams({"thinkingDuration": "15s", "randomseed": "123", "eatingduration": "2000"})

philosopherThree = sst.Component("Philosopher 3", "livelock.diningPhilosopher")
philosopherThree.addParams({"thinkingDuration": "15s", "randomseed": "348734", "eatingduration": "2000"})

philosopherFour = sst.Component("Philosopher 4", "livelock.diningPhilosopher")
philosopherFour.addParams({"thinkingDuration": "15s", "randomseed": "123", "eatingduration": "2000"})

philosopherFive = sst.Component("Philosopher 5", "livelock.diningPhilosopher")
philosopherFive.addParams({"thinkingDuration": "15s", "randomseed": "348734", "eatingduration": "2000"})


# Connect the nodes by their ports.
sst.Link("Chopstick_One_Link").connect(
    (philosopherOne, "philosopherRight", "1ps"), (philosopherTwo, "philosopherLeft", "1ps")
)
sst.Link("Chopstick_Two_Link").connect(
    (philosopherTwo, "philosopherRight", "1ps"), (philosopherThree, "philosopherLeft", "1ps")
)
sst.Link("Chopstick_Three_Link").connect(
    (philosopherThree, "philosopherRight", "1ps"), (philosopherFour, "philosopherLeft", "1ps")
)
sst.Link("Chopstick_Four_Link").connect(
    (philosopherFour, "philosopherRight", "1ps"), (philosopherFive, "philosopherLeft", "1ps")
)
sst.Link("Chopstick_Five_Link").connect(
    (philosopherFive, "philosopherRight", "1ps"), (philosopherOne, "philosopherLeft", "1ps")
)