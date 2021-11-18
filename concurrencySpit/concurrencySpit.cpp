//Jacob Simonek || Computer Org || Week of April 4th, 2021
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <random>
#include <algorithm>
#include <cinttypes>
#include <ctime>
#include <getopt.h>
#include <cstdlib>

using namespace std;

void HandleOptions(int argc, char** argv, uint64_t& size, uint64_t& seed, uint32_t& numThreads); //Handle options is in charge of the command line stuff
void InitializeNumbers(vector <uint32_t>& numbers, uint64_t& seed, uint64_t& size); //Initialize numbers creates vector of all numbers and shuffles it
void LaunchThreads(vector <uint32_t>& numbers, uint32_t& numThreads, uint64_t& size, thread** threads); //Splits vector of numbers up and launches threads with a thread worker
void JoinThreads(thread** threads, uint32_t& numThreads); //Joins the threads together

mutex mtx; //Mutex variable is declared as global so it can be accessed by all the threads used to protect order
uint64_t counter; //Counter is going to be a large positive number used amongst the threads so it is unsigned 64 bits

int main(int argc, char **argv) //Main function standard parameters
{
    srand(time(nullptr)); //Calls srand() for rand() later on, uses the time of day as the seed for srand()
	counter = 0; //Sets the global variable counter which is used in the thread worker
	uint64_t size = (1 << 20); // Sets default size of vector of numbers, will be large positive number so its unsigned 64 bit (1 << 20 = 2^20)
	uint32_t numThreads = 4; //Sets the default number of threads (4), this is going to be a small positive number so its unsigned 32 bit
	uint64_t seed = rand(); //Sets the seed to random number, this number will be large and positive so its unsigned 64 bit 
	vector <uint32_t> numbers; //Creates a vector that will hold all of the numbers of the deck of cards, type is unsigned 32 because the numbers inside will be relatively small positive
	HandleOptions(argc, argv, size, seed, numThreads); //Calls HandleOptions() which will deal with the command line options inputted by the user
	thread* threads[numThreads]; //Establishes an array of thread* that is the size of the updated number of threads
	InitializeNumbers(numbers, seed, size); //Calls initialize numbers which fills the vector<uint32_t> numbers with values 0-(size-1) shuffled
	LaunchThreads(numbers, numThreads, size, threads); //Calls LaunchThreads() which splits the vector of numbers between the threads and launches them
	JoinThreads(threads, numThreads); //Calls JoinThreads() which joins the threads together afeer launching them
	return 0; 
}

void HandleOptions(int argc, char** argv, uint64_t& size, uint64_t& seed, uint32_t& numThreads) //Handle Options deals with the command line options entered by the user and sets all of our variables
{
   	int32_t option; //The option will be a small positive or negative number so it is signed 32 bits
	while ((option = getopt(argc, argv, "hz:s:t:")) != -1) //While loop handles whether a user enters a getopt option
	{
		switch(option) //Switch based on the case the user selected
		{
			case 'h': //This case will run if the user types '-h' and print what the program does
				cout << "help:" << endl; 
				cout << "-h         prints this" << endl; 
                cout << "-z size    sets size of vector of uint32_t (Minimum: 1 << 8 Default: 1 << 20)" << endl;
                cout << "-s seed    sets seed for random number generator (Default: time-of-day)" << endl;
                cout << "-t threads sets number of threads to create (Minimum/Default: 4 Maximum: 30)" << endl;
				return; 
			case 'z': //This case will run if the user types '-z' and sets the size of the vector
                if ( atoi(optarg)  >= (1 << 8) ) // Makes sure input is larger than the minimum (1 << 8 = 256)
				    size = atoi(optarg); //Sets size variable for the vector of nums based on user input
				break; 
			case 's': //This case will run if the user types '-s' and sets the seed to shuffle the vector
				seed = atoi(optarg); //Sets seed variable for the vector of nums based on user input
				break; 
            case 't': //This case will run if the user types '-t' and sets t of the vector
                if ( atoi(optarg)  >= 4 && atoi(optarg)  <= 30) //Makes sure the user input of numThreads is between 4-30
                    numThreads = atoi(optarg); //Sets numThreads variable which is the number of threads created based on user input
                break;
			default: //Default case just breaks away since it shouldn't change anything
				break;  
		}
	} 
}

void InitializeNumbers(vector <uint32_t>& numbers, uint64_t& seed, uint64_t& size) //Initialize numbers creates vector of all numbers and shuffles it
{
	for (uint32_t i = 0; i < size; i++) //Loops through the vector to insert numbers
		numbers.push_back(i); //The range of numbers inserted into the vector is 0-(size-1)
	shuffle(numbers.begin(), numbers.end(), std::default_random_engine(seed)); //Calls the shuffle() method using our seed and the numbers vector to shuffle the numbers
}

void ThreadWorker(vector<uint32_t> hands, uint32_t threadID,  uint64_t size) //Thread worker is called when threads are launched it displays output of program
{
	while(counter < size) //While loop used to track when the threads have reached the last number
	{
		lock_guard<mutex> lockGuard(mtx); //Uses lock_guard<mutex> to lock the lock will unlock when we exit scopee
		if (find(hands.begin(), hands.end(), counter) != hands.end())//Uses the find() function to see if the indivual thread's vector has the number we want
		{
			cout << "Thread: " << threadID  << " hit on: " << counter << endl; //Output for program
			hands.erase(find(hands.begin(), hands.end(), counter)); //Erases the value from hands to shorten vector and increase speed
			counter++; //Increments counter because we found our number to move on
		}
		else //This case runs if the current thread's vector doesn't have our numbe
			this_thread::yield(); //Tells the program our thread doesn't have the number so come back to it later 
	}  
}

void LaunchThreads(vector <uint32_t>& numbers, uint32_t& numThreads, uint64_t& size, thread** threads)  //Splits vector of numbers up and launches threads with a thread worker
{
	vector<vector<uint32_t>> smallVectors(numThreads); //Establishes a vector of vectors based on the number of threads, each thread has a corresponding vector
	uint64_t rangeOfNums = size / numThreads; //Sets the range (size of each of the smaller vectors), this number could be large and will be positive so its unsigned 64 bit
	uint64_t tracker = 0; //Sets tracker  to 0 which will be used to keep track of our index in the big vector; will be large and positive so its unsigned 64 bit
	for (uint32_t i = 0; i < numThreads; i++) //Loops through to assign each thread's vector its approriate numbers
	{
		if (i != numThreads - 1) //If the thread isn't the last thread then it doesn't get the remainder numbers
			smallVectors[i].assign(numbers.begin() + tracker, numbers.begin() + tracker + rangeOfNums); //Calls assign() to assign the appropriate range of numbers to the smallVector
		else //This case only runs if its the vector for the last thread
			smallVectors[i].assign(numbers.begin() + tracker, numbers.end()); //Calls assign() to assign the appropriate and extra numbers to the final thread's vectoor
		tracker += rangeOfNums; //Increments tracker so we don't losee our index in the big vector when the loop restarts
	}
	for(uint32_t r = 0; r < numThreads; r++) //Loops through to launch the threads
		threads[r] = new thread(ThreadWorker, smallVectors[r], r, size); //Launches the threads and calls threadworker() to get output for program
}

void JoinThreads(thread** threads, uint32_t& numThreads) //Joins the threads together after they've been launched
{
	for(uint32_t i = 0; i < numThreads; i++) //Loops through and joins each of the threads
		threads[i]->join(); //Uses pointer notation '->' to call join since threads is a pointer to a pointer
}