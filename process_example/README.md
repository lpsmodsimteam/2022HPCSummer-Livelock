This is code for an initial example to analyze livelock, where the goal was to model 
processes within a computer system.  Each process would need to request a certain 
amount of space to run, and would occupy that space for a certain amount of time 
until they completed, and freed up space.  While working on it, we realized that 
due to the fact that space would eventually free up, this wouldn't truly 
model livelock, and simplifying the model to just have processes occupy space 
forever and switch from waiting to requesting states infinitely wouldn't be 
as realistic.  Due to this, we stopped working on this example, but left it within 
this repository in case we ever needed to refer back to it.  The example still 
runs, but has been left in a work in progress state. 