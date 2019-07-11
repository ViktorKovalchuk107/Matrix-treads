#Problem
We have two matrix that we want to multiply in two different ways:
 1) Usual one-tread multiplication as we all know 
 2) Multiplication by dividing our matrix on smaller and multiply smaller blocks by using treads.
In solution I use vector<std::thread> 
In my case threads do not cross which allows me do not worry about their work and 
use only join() to measure right multiplicatoin time. 
