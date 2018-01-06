Dual-Core Cache Simulator
=======

Written by Jesse Bannon
-----------

### Building and Compiling

In the current directory, enter 'make' to produce the executable **cacheSimulator**.

### Running

To run the program, execute the executable by entering './cacheSimulator'.

### Modifying the Memory Hierarchy

Within *CacheSimulator.h* are multiple defines that modify the memory heiarchy
and metrics. Simply edit these values with respect to log base two if they are part of
associated with the cache (latencies can be any positive integer). 

### Modifying the Address Trace file

Modify the Address Trace file (.csv) within *CacheSimulator.h* by changing the 
**FILE_NAME** string.




