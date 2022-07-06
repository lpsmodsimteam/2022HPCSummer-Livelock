import sst

philosopherOne = sst.Component("Philosopher 1", "test.dine")
philosopherOne.addParams(
    {
        "thinkingDuration": "15s",  # how long one thinks after eating
        "waitingClock": "13s",      # how long to wait between checking chopsticks
        "randomseed": "151515",     # random seed
        "eatingduration": "1",      # how long one holds chopsticks while eating
        "id": "1"                   # id for each philosopher
    }
)

philosopherTwo = sst.Component("Philosopher 2", "test.dine")
philosopherTwo.addParams({"thinkingDuration": "15s", "waitingClock": "13s", "randomseed": "123", "eatingduration": "1", "id": "2"})

philosopherThree = sst.Component("Philosopher 3", "test.dine")
philosopherThree.addParams({"thinkingDuration": "15s", "waitingClock": "13s", "randomseed": "348734", "eatingduration": "1", "id": "3"})

philosopherFour = sst.Component("Philosopher 4", "test.dine")
philosopherFour.addParams({"thinkingDuration": "15s", "waitingClock": "13s", "randomseed": "123", "eatingduration": "1", "id": "4"})

philosopherFive = sst.Component("Philosopher 5", "test.dine")
philosopherFive.addParams({"thinkingDuration": "15s", "waitingClock": "13s", "randomseed": "348734", "eatingduration": "1", "id": "5"})

L1R2 = sst.Component("L1R2", "test.chopsticks")
L1R2.addParams(
    {
        "name": "L1R2"              # name for each chopstick
    }
)
L2R3 = sst.Component("L2R3", "test.chopsticks")
L2R3.addParams({"name": "L2R3"})
L3R4 = sst.Component("L3R4", "test.chopsticks")
L3R4.addParams({"name": "L3R4"})
L4R5 = sst.Component("L4R5", "test.chopsticks")
L4R5.addParams({"name": "L4R5"})
L5R1 = sst.Component("L5R1", "test.chopsticks")
L5R1.addParams({"name": "L5R1"})

### Enable statistics
sst.setStatisticLoadLevel(2)

# Output statistics to CSV. 
sst.setStatisticOutput("sst.statOutputCSV", { "filepath" : "statsoutput.csv", "separator" : "," } ) 

## Enable statistics on the components
sst.enableAllStatisticsForComponentType("test.dine")


# Connect philosopher one's chopsticks
sst.Link("philosopher_one_left").connect(
    (philosopherOne, "leftChopstick", "1ps"), (L1R2, "rightPhilosopher", "1ps")
)
sst.Link("philosopher_one_right").connect(
    (philosopherOne, "rightChopstick", "1ps"), (L5R1, "leftPhilosopher", "1ps")
)

# Connect philosopher two's chopsticks
sst.Link("philosopher_two_left").connect(
    (philosopherTwo, "leftChopstick", "1ps"), (L2R3, "rightPhilosopher", "1ps")
)
sst.Link("philosopher_two_right").connect(
    (philosopherTwo, "rightChopstick", "1ps"), (L1R2, "leftPhilosopher", "1ps")
)

# Connect philosopher three's chopsticks
sst.Link("philosopher_three_left").connect(
    (philosopherThree, "leftChopstick", "1ps"), (L3R4, "rightPhilosopher", "1ps")
)
sst.Link("philosopher_three_right").connect(
    (philosopherThree, "rightChopstick", "1ps"), (L2R3, "leftPhilosopher", "1ps")
)

# Connect philosopher four's chopsticks
sst.Link("philosopher_four_left").connect(
    (philosopherFour, "leftChopstick", "1ps"), (L4R5, "rightPhilosopher", "1ps")
)
sst.Link("philosopher_four_right").connect(
    (philosopherFour, "rightChopstick", "1ps"), (L3R4, "leftPhilosopher", "1ps")
)

# Connect philosopher five's chopsticks
sst.Link("philosopher_five_left").connect(
    (philosopherFive, "leftChopstick", "1ps"), (L5R1, "rightPhilosopher", "1ps")
)
sst.Link("philosopher_five_right").connect(
    (philosopherFive, "rightChopstick", "1ps"), (L4R5, "leftPhilosopher", "1ps")
)
