-> Normal merge sort ran much faster than the 
   concurent(using processes) and concurent(using threaded) merge sorts.

-> When are creating two processes and making parent to wait for these processes end.
  I think this increases Cpu waiting time and it takes time for creation also.

-> In concurrent merge sort using multiple processes . At an instant of time 
  we divide the array and in one process we try to acces left part so left array
  is loaded into cache mem and simultaneously we try to acces right array in 
  right process which creates a cache miss. as this happens multiple times execution time will be increased.

-> For threaded it takes time for creating threads may be.