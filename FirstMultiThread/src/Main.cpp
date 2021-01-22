#include <iostream>
#include <thread>
#include <algorithm>
#include <vector>
#include <functional>
#include <numeric>
#include <iterator>
//#include <string>


/*
// Single thread----------------------------------------------------------------------------------------
void print(int n, const std::string& str)
{
	std::cout << "Printing int: " << n << std::endl;
	std::cout << "Printing str: " << str << std::endl;
}

int main() {
	std::thread t1(print, 10, "Harry"); // init thread with a function 
	t1.join(); // join pauses the main func till t1 has finished its func

	return 0;
}
_______________________________________________________________________________________________________
//MultiThread------------------------------------------------------------------------------------------

void print(int n, const std::string& str)
{
	std::string msg = std::to_string(n) + " : " + str;
	std::cout << msg << std::endl;
}


int main() {
	std::vector<std::string> s =
	{
		"Hazzies",
		"first",
		"time",
		"multithreading"
	};
	std::vector<std::thread> threads;

	for (int i = 0; i < s.size(); i++)
		threads.push_back(std::thread(print, i, s[i]));
	
	for (auto& th : threads)
		th.join();

	return 0;
}
_________________________________________________________________________________________________________
*/

using namespace std;
// Setting a dynamic number of threads at runtime dependant on user hardware

template<typename Iterator, typename T>

struct accumulate_block
{
	void operator() (Iterator first, Iterator last, T& result)
	{
		result = accumulate(first, last, result);
	}
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
	unsigned long const length = std::distance(first, last);
	//return initial val if range is empty
	if (!length) return init;

	unsigned long const minPerThread = 50;

	//else there is an element in the range. Divide num of elements to process by min block size in order to give max threads
	unsigned long const maxThreads = (length + minPerThread - 1) / minPerThread;

	//this give max num of threads that can run concurrently 
	unsigned int long const hardwareThreads = thread::hardware_concurrency();

	//num of threads to run is min of max and num of hardware threads. if concurreny returns 0 sub with '2'
	unsigned long const numThreads = std::min(hardwareThreads != 0 ? hardwareThreads : 2, maxThreads);
	
	unsigned long const block_size = length / numThreads;
	//create vec of threads
	vector<T> results(numThreads);
	//launch 1 fewer than max as we already have one
	vector<thread> threads(numThreads - 1);

	Iterator block_start = first;

	//launch threads with loop
	for (unsigned long i = 0; i < numThreads - 1; ++i)
	{
		Iterator block_end = block_start;
		//advance block iterator to end of block
		std::advance(block_end, block_size);
		//launch new thread to accumulate result for block
		threads[i] = thread(accumulate_block<Iterator, T>(), block_start, block_end, std::ref(results[i]));
		//start of the next block is end of this one
		block_start = block_end;
	}

	//after launch of all trheads, this threads does the final block
	accumulate_block<Iterator, T>() (block_start, last, results[numThreads - 1]);

	//once have results for last block wait for all threads to finish
	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
	// add up all results
	cout << "results begin: " << results.begin() << "results end: " << results.end() << "init: " << init << "\n";
	return std::accumulate(results.begin(), results.end(), init);
}

int main() {
	cout <<  std::thread::hardware_concurrency();
	

	return 0;

}
