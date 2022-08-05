import sst

philosopherOne = sst.Component("Philosopher 1", "deadlock.diningPhilosopher")
philosopherOne.addParams(
    {
        "thinkingDuration": "15s",  # how long one thinks after eating
        "randomseed": "151515",     # random seed
        "eatingduration": "1",      # how long one holds chopsticks while eating
        "id": "1",                  # id for table to identify philosopher
        "livelockCheck": "1000",    # length of cycles to wait before checking for livelock
        "windowSize": "100"         # size of window for livelock detection
    }
)

philosopherTwo = sst.Component("Philosopher 2", "deadlock.diningPhilosopher")
philosopherTwo.addParams({"thinkingDuration": "15s", "randomseed": "123", "eatingduration": "1", "id": "2", "livelockCheck": "1000", "windowSize": "100"})

philosopherThree = sst.Component("Philosopher 3", "deadlock.diningPhilosopher")
philosopherThree.addParams({"thinkingDuration": "15s", "randomseed": "348734", "eatingduration": "1", "id": "3", "livelockCheck": "1000", "windowSize": "100"})

philosopherFour = sst.Component("Philosopher 4", "deadlock.diningPhilosopher")
philosopherFour.addParams({"thinkingDuration": "15s", "randomseed": "123", "eatingduration": "1", "id": "4", "livelockCheck": "1000", "windowSize": "100"})

philosopherFive = sst.Component("Philosopher 5", "deadlock.diningPhilosopher")
philosopherFive.addParams({"thinkingDuration": "15s", "randomseed": "348734", "eatingduration": "1", "id": "5", "livelockCheck": "1000", "windowSize": "100"})

diningTable = sst.Component("Dining Table", "deadlock.diningTable")
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
sst.enableAllStatisticsForComponentType("deadlock.diningPhilosopher")


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