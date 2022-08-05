import sst

philosopherOne = sst.Component("Philosopher1", "livelock2.diningPhilosopher")
philosopherOne.addParams(
    {
        "thinkingDuration": "15s",  # how long one thinks after eating
        "waitingClock": "16s",      # how long to wait between checking if they need to put down their chopsticks
        "randomseed": "151515",     # random seed
        "eatingduration": "5",      # how many cycles one holds chopsticks while eating
        "id": "1",                  # id for table to identify philosopher
        "livelockCheck": "1000",    # length of cycles to wait before checking for livelock
        "windowSize": "100"         # size of window for livelock detection
    }
)

philosopherTwo = sst.Component("Philosopher2", "livelock2.diningPhilosopher")
philosopherTwo.addParams({"thinkingDuration": "15s", "waitingClock": "16s", "randomseed": "123", "eatingduration": "5", "id": "2", "livelockCheck": "1000", "windowSize": "100"})

philosopherThree = sst.Component("Philosopher3", "livelock2.diningPhilosopher")
philosopherThree.addParams({"thinkingDuration": "15s", "waitingClock": "16s", "randomseed": "348734", "eatingduration": "5", "id": "3", "livelockCheck": "1000", "windowSize": "100"})

philosopherFour = sst.Component("Philosopher4", "livelock2.diningPhilosopher")
philosopherFour.addParams({"thinkingDuration": "15s", "waitingClock": "16s", "randomseed": "123", "eatingduration": "5", "id": "4", "livelockCheck": "1000", "windowSize": "100"})

philosopherFive = sst.Component("Philosopher5", "livelock2.diningPhilosopher")
philosopherFive.addParams({"thinkingDuration": "15s", "waitingClock": "16s", "randomseed": "348734", "eatingduration": "5", "id": "5", "livelockCheck": "1000", "windowSize": "100"})

diningTable = sst.Component("Dining Table", "livelock2.diningTable")
diningTable.addParams(
    {
        "randomseed": "151515"  # random seed
    }
)

### Enable statistics
sst.setStatisticLoadLevel(2)

# Output statistics to CSV. 
sst.setStatisticOutput("sst.statOutputCSV", { "filepath" : "statsoutput.csv", "separator" : "," } ) 

## Enable statistics on the components
sst.enableAllStatisticsForComponentType("livelock2.diningPhilosopher")


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