# MarkovDecisionProcessesCS463G
A path value and policy generator with a fully viewable decision and state tree environment using a simple Markov Decision Process

Variations:

With probability 0.15, the car will end up in reverse, and go backwards from the intended direction.  With probability 0.15, it will stall out and go nowhere.  With probability 0.7, it goes where you send it.  If it hits an edge of the grid world, it stays in its square.

Algorithms:

Value Iteration
Policy Iteration
Epsilon Greedy Q learning

Parameters:

For all algorithms, the discount rate gamma will be 0.95.
For VI and PI, run each of the algorithms with time horizons 50 and 100.