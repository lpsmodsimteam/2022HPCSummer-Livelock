import sst

process_memory = sst.Component("Process Memory", "livelock.processMemory")

process_one = sst.Component("Process 1", "livelock.process")
process_one.addParams(
    {
        "maxSubProcesses": "100",  # max message queue size.
        "tickFreq": "1ns",  # simulated time node runs at.
        "processID": "1" # ID for processMemory to identify ports
    }
)

process_two = sst.Component("Process 2", "livelock.process")
process_two.addParams({"maxSubProcesses": "100", "tickFreq": "2ns", "processID": "2"})

process_three = sst.Component("Process 3", "livelock.process")
process_three.addParams({"maxSubProcesses": "100", "tickFreq": "10s", "processID": "3"})

process_four = sst.Component("Process 4", "livelock.process")
process_four.addParams({"maxSubProcesses": "100", "tickFreq": "10s", "processID": "4"})


# Connect the nodes by their ports.
sst.Link("Message_Link_One").connect(
    (process_one, "memoryPort", "1ps"), (process_memory, "process1port", "1ps")
)
sst.Link("Message_Link_Two").connect(
    (process_two, "memoryPort", "1ps"), (process_memory, "process2port", "1ps")
)
sst.Link("Message_Link_Three").connect(
    (process_three, "memoryPort", "1ps"), (process_memory, "process3port", "1ps")
)
sst.Link("Message_Link_Four").connect(
    (process_four, "memoryPort", "1ps"), (process_memory, "process4port", "1ps")
)