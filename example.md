# Dining Philosophers Example

## Prerequisites
```
git clone https://github.com/lpsmodsim/2022HPCSummer-Livelock.git
```

# Demonstrate livelock
Use the following python driver file.

Copy to 2022HPCSummer-Livelock/diningTableExample/tests/diningPhilosopher.py to 
simulate with the diningTable component as a global node

Copy to 2022HPCSummer-Livelock/chopstickExample/tests/diningPhilosopher.py to 
simulate with the chopstick components as individual objects connected to 
each of the philosophers

```
# Reference: http://sst-simulator.org/SSTPages/SSTUserPythonFileFormat/

import sst # Use SST Library

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
```

# Demonstrate livelock not occuring.
Use the following python driver file.

Copy to 2022HPCSummer-Livelock/diningTableExample/tests/diningPhilosopher.py to 
simulate with the diningTable component as a global node

Copy to 2022HPCSummer-Livelock/chopstickExample/tests/diningPhilosopher.py to 
simulate with the chopstick components as individual objects connected to 
each of the philosophers

```
# Reference: http://sst-simulator.org/SSTPages/SSTUserPythonFileFormat/

import sst # Use SST Library

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
philosopherTwo.addParams({"thinkingDuration": "7s", "waitingClock": "16s", "randomseed": "123", "eatingduration": "5", "id": "2", "livelockCheck": "1000", "windowSize": "100"})

philosopherThree = sst.Component("Philosopher3", "livelock2.diningPhilosopher")
philosopherThree.addParams({"thinkingDuration": "13s", "waitingClock": "16s", "randomseed": "348734", "eatingduration": "5", "id": "3", "livelockCheck": "1000", "windowSize": "100"})

philosopherFour = sst.Component("Philosopher4", "livelock2.diningPhilosopher")
philosopherFour.addParams({"thinkingDuration": "3s", "waitingClock": "16s", "randomseed": "123", "eatingduration": "5", "id": "4", "livelockCheck": "1000", "windowSize": "100"})

philosopherFive = sst.Component("Philosopher5", "livelock2.diningPhilosopher")
philosopherFive.addParams({"thinkingDuration": "11s", "waitingClock": "16s", "randomseed": "348734", "eatingduration": "5", "id": "5", "livelockCheck": "1000", "windowSize": "100"})

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
```
The difference between these 2 simulations is that the one with livelock has each philosopher's lock as being synchroized, so they all send requests for chopsticks at the same time

However, the example without livelock gives the philosophers more randomized cycles, so they don't send requests at the same time.

# Running
This is assuming the user is on a system running a Ubuntu-Based Linux Distro.

Prerequisites
```
sudo apt install singularity black mypi
git clone https://github.com/tactcomplabs/sst-containers.git
```
Follow the instructions in the git repo to build the container "sstpackage-11.1.0-ubuntu-20.04.sif".
```
cp sst-containers/singularity/sstpackage-11.1.0-ubuntu-20.04.sif /usr/local/bin/
git clone https://github.com/lpsmodsim/2022HPCSummer-SST.git
sudo ./2022HPCSummer-SST/additions.def.sh
```

Running the model
```
cd 2022HPCSummer-Livelock/diningTableExample
make > output.txt
```

Re-run the model
```
make clean
make > output.txt
```

Simulation output is generated in 2022HPCSummer-Livelock/diningTableExample/output.txt

# Plotting

The plots for these simulations were generated within LibreOffice Calc.  When you run this simulation, you should see a  CSV output for each philosopher.  From here, you can open these CSV files within a spreadsheet application of your choosing. We plotted within LibreOffice using an X-Y scatter plot, and making our X-axis the cycle, and the Y-axis a percentage out of 100.  We also plotted the boolean state of eating on top of this graph, scaling the output to be 0 and 100 so we could compare how eating states lined up with rises and falls in the expected percentages.  To observe what these graphs would look like, you can refer to the report within this repository.
