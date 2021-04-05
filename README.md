Solving a test task at one of the interviews.

The main differences from the solutions of other candidates:
* In the resolve access to the IP is carried out directly without additional problems from memory, as in the solutions of other candidates like unordered_map<string,shared_ptr<...>> or unordered_map<string,iterator>
* Memory reservation in a hash during class initialization.
* There is no deallocation, memory allocation when an item is preempted from the cache.
* The code reads well enough.
* 100% test coverage.
