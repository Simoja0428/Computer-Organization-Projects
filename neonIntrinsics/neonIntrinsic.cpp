//Name: Jacob Simonek   Date: 2/27/21   Class: Computer Org
#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <iomanip>
#include <cassert>
#include <arm_neon.h>
#include <vector>
#include <thread>
#include <sys/time.h>
#include <cstring>

using namespace std; 

void printArray(float* array, int size); //Used for testing purposes prints values in arrays
float randomNumber(); //Used to generate randomNumbers to fill array
void initializeToZero(float* array, int size); //Used to fill array c with all 0.0s
void SingleCore(float* a, float* b, float* c, int size); //Single Core function declaration
double SumofSums(float* p, int size); //SumofSums function declaration which is used for check
void Intrinsic(float* a, float* b, float* c, int size); //Intrinsic function declaration; NEON version of SingleCore

int main (int argc, char* argv[]) //Main function which makes use of command line
{
    srand(time(NULL)); //Generates random number based on internal clock

	int size = 128; //Declares size variable; which has a default value of 128
	int numIterations = 1; //Declares number of iteratons variable; default value of 1
	int option; //Holds the value of the option by the user for getopt

	while ((option = getopt(argc, argv, "hs:i:")) != -1) //Loop needed for getopt based on tutorial
	{
		switch(option) //Switch statement required based on tutorial inputs option
		{
			case 'h': //If the user typed -h then this case will run and display the help menu
				cout << "help:" << endl; //Displays title for instructions
				cout << "-h      prints this" << endl; //Displays the instruction for -h
				cout << "-s size sets size (default is 128 - will be made divisible by 16)"	<< endl; //Displays instruction for -s
				cout << "-i iter sets iterations (default is 1)" << endl; //Displays the instruction for -i
				return 0; //Exits the program
			case 's': //If the user typed -s then this case will run and set the size
				size = atoi(optarg); //Sets size by converting value from getopt in optarg
				break; //Breaks out after size is set
			case 'i': //If the user typed -i then this case will run and set the numIterations
				numIterations = atoi(optarg); //Sets iterations by converting value from getopt in optarg
				break; //Breaks out after numIterations is set
			default: //If the user didn't select any of these three options it breaks out
				break;  //Breaks out no option selected
		}
	}

	if (size % 16 != 0) //If the inputed size by the user is not a multiple of 16
	{
		size = size / 16; //Divides size by 16 to get a whole number
		size = (size + 1) * 16; //Rounds the size that we originally started with up
	}

	float * a = (float *) aligned_alloc(16, size * sizeof(float)); //Allocates float array a which is the size specified
	float * b = (float *) aligned_alloc(16, size * sizeof(float)); //Allocates float array b which is the size specified
	float * c = (float *) aligned_alloc(16, size * sizeof(float)); //Allocates float array c which is the size specified

	for (int i  = 0; i < size; i++) //Loops through and fills the first array a with random numbers
	{
		a[i] =  randomNumber(); //Sets index of float array a to be random number
	}
	for (int p = 0; p < size; p++) //Loops through and fills the second array b with random numbers
    {
		b[p]  = randomNumber(); //Sets index of float array b to be random number
	}

	initializeToZero(c, size); //Initializes the third array c to be filled with all zeros
	
	int numCores = thread::hardware_concurrency(); //Used to get the number of avaliable cores

	cout << "Array size: " << size << " total size in MB: " << (size  / 1048576) * 3<< endl; //Prints size of arrays, MB divides by 2^20 = 1048576
	cout << "Iterations: " << numIterations << endl; //Prints number of iterations
	cout << "Available cores: " << numCores << endl; //Prints number of cores
	cout << "Single core timings..." << endl; //Prints loading display message

	timeval start; //Creates the start variable which is of type timeval
	timeval end; //Creates the end variable which is of type timeval
	timeval elapsed; //Creates the elapsed time variable which is of type timeval

	gettimeofday(&start, NULL); //The initial get time of day call starting time

	for ( int r = 0; r < numIterations; r++) //Loops through adding values in a and b into c
	{
		SingleCore(a, b, c, size); //Loops through adding the values of a and b together using SingleCore()
	}

	gettimeofday(&end, NULL); //The initial get time of day ending time
	
	timersub(&end, &start, &elapsed); //Subtracts start time from end time stores in elapsed

	double printVal = SumofSums(c, size); //Holds double value from sumofsums() for print
	cout << "Naive: " << fixed << setprecision(6) << (elapsed.tv_usec / (float)(numIterations * 1000000)) << " Check: " << fixed << setprecision(6) << printVal << endl; //Calls on time and check using sum of sums sets decimal place out up to 6
    
	initializeToZero(c, size); //Empties out c so that we can reuse it with intrinsic function

	gettimeofday(&start, NULL); //The Neon get time of day call starting time

	for ( int q  = 0; q < numIterations; q++) //Loops through adding values in a and b into c
	{
		Intrinsic(a, b, c, size); //Loops through adding the values of a and b together using Intrinsic()
	}

	gettimeofday(&end, NULL); //The Neon get time of day call ending time

	timersub(&end, &start, &elapsed); //Subtracts start time from end time stores in elapsed

	printVal = SumofSums(c, size); //Holds doouble value from sumofsums() for print 
	cout << "NEON:  " << fixed << setprecision(6) << (elapsed.tv_usec / (float)(numIterations * 1000000)) << " Check: " << fixed << setprecision(6) << printVal << endl; //Calls on time and check using sum of sums sets decimal place out up to 6

	cout << "Threaded timing..." << endl; //Prints the threaded timing line statement

	thread* threads[numCores]; //Array which is the size of the number of cores will hold all threads
	
	int sizeOfThreads[numCores]; //Establishes an int array of sizes of threads

	for (int fo = 0; fo < numCores; fo++) //Loops through the sizeofthreads array
	{
		sizeOfThreads[fo] = 0; //Initializes all the indexes to 0
	}

	int tracker = 0; //Sets the tracker to start at 0

	for (int re = 0; re < (size / 16); re++) //Loops through to calculate the size of each thread
	{
		sizeOfThreads[tracker] = sizeOfThreads[tracker] + 16; //Adds 16 to size of the thread at the respective index in the array of threads
		tracker++; //Increments the tracker
		if (tracker == numCores)
		{
			tracker = 0; //If the tracker equals the number of cores then reset it so it doesnt go out of bounds
		}
	}

	int offsetsOfThreads[numCores]; //Establishs an int array of offsets for each respective thread

	for (int fo = 0; fo < numCores; fo++) //Loops through the offsetsofthreads array
	{
		offsetsOfThreads[fo] = 0;  //Initializes all indexes to 0
	}

	for (int of = 1; of < numCores; of++) //Loops through to calculate the num of offsets for each thread
	{
		offsetsOfThreads[of] = offsetsOfThreads[of-1] + (sizeOfThreads[of-1] + 1); //Offset set to previous offset value plus the previous array size + 1
	}

	initializeToZero(c, size); //Empties out c so that we can reuse it with single core function

	gettimeofday(&start, NULL); //The second singlecore get time of day call starting time
	
	for ( int we = 0; we < numIterations; we++) //Loops through joining the threads together
	{
		for ( int x0 = 0; x0 < numCores; x0++) //Sets each of the threads
		{
			threads[x0] = new thread(SingleCore, (a + offsetsOfThreads[x0]), (b + offsetsOfThreads[x0]), (c + offsetsOfThreads[x0]), sizeOfThreads[x0]);
			//The thread is set by running the single core function using the inputs of float a,b,c added to their respective offsets and an input for the size of each thread
		}
		for (int x1 = 0; x1 < numCores; x1++) //Joins the threads together
		{
			threads[x1]->join(); //Joins the thread using pointer operands
		}
	}

	gettimeofday(&end, NULL); //The second singlecore get time of day ending time

	timersub(&end, &start, &elapsed); //Subtracts start time from end time stores in elapsed

	printVal = SumofSums(c, size); //Holds double value from sumofsums() for print
	cout << "Naive: " << fixed << setprecision(6) << (elapsed.tv_usec / (float)(numIterations * 1000000)) << " Check: " << fixed << setprecision(6) << printVal << endl; //Calls on time and check using sum of sums sets decimal place out up to 6

	initializeToZero(c, size); //Empties out c so that we can reuse it with NEON

	gettimeofday(&start, NULL); //The second NEON get timee of day call starting time

	for ( int weNeon = 0; weNeon < numIterations; weNeon++) //Loops through joining the threads together
	{
		for ( int x2 = 0; x2 < numCores; x2++) //Sets each of the threads
		{
			threads[x2] = new thread(Intrinsic, (a + offsetsOfThreads[x2]), (b + offsetsOfThreads[x2]), (c + offsetsOfThreads[x2]), sizeOfThreads[x2]);
			//The thread is set by running the intrinsic function using the inputs of float a,b,c added to their respective offsets and an input for the size of each thread
		}
		for (int x3 = 0; x3 < numCores; x3++) //Joins the threads together
		{
			threads[x3]->join(); //Joins the thread using pointer operands
		}
	}

	gettimeofday(&end, NULL); //The second Neon get time of day ending time

	timersub(&end, &start, &elapsed); //Subtracts start time from end time stores in elapsed

	printVal = SumofSums(c, size); //Holds doouble value from sumofsums() for print 
	cout << "NEON:  " << fixed << setprecision(6) << (elapsed.tv_usec / (float)(numIterations * 1000000)) << " Check: " << fixed << setprecision(6) << printVal << endl; //Calls on time and check using sum of sums sets decimal place out up to 6

	free(a); //Frees a to avoid memory leak
	free(b); //Frees b to avoid memory leak
	free(c); //Frees c to avoid memory leak

	return 0; //Returns from program
}

void printArray(float* array, int size) //Used for testing purposes to check values in array
{
	for (int i = 0; i < size; i++)
	{
		cout << "Array at index: " << i << " is: " << array[i] << endl; //Print statement to check values
	}
}

float randomNumber() //This function returns a random number between 0 to 1 inclusive
{
	float randomNum = ((float)rand() / (float)(RAND_MAX)); //Creates the random number between 0 and 1, RAND_MAX is a constant
	return randomNum; //Returns the random number that was generated
}

void initializeToZero(float* array, int size) //Function used to fill array with all zeros
{
	memset(array, 0, size); //Sets the third array to null using memset
}

void SingleCore(float* a, float* b, float* c, int size) //Single Core function provided
{
	//cout << __FUNCTION__ << " " << hex << size << dec << " " << size << endl;
	// Ensure compatibility with a 16-times unrolled loop.
	assert((size & 0xF) == 0); //Assert used to make sure if bug program terminates
	size = size / 16; //Divides size by 16 then assigns size

	for (int i = 0; i < size; i++)  //Loops through filling in c and then incrementing
    {
		*(c++) = *(a++) + *(b++); 
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
	}
}

double SumofSums(float* p, int size) //SumsofSums function adds first size floats at given address returns sum
{
    double theSum = 0.0; //Holds the value of the double sum of the first size floats

    for (int i = 0; i < size; i++) //Loops through til it reaches one less than size of float*
    {
        theSum = theSum + (double)p[i]; //Adds current value of theSum to float, casts as double, stores in theSum
    }
    
    return theSum; //Returns the value of the sum
}

void Intrinsic(float* a, float* b, float* c, int size) //Intrinsinc function does same as SingleCore but instead uses SIMD Instructions
{
	assert((size & 0xF) == 0); //Assert used to make sure if bug program terminates; same as SingleCore
	size = size / 16; //Divides size by 16 then assigns size; same as SingleCore

	float32x4_t * aNeon = (float32x4_t*) a; //Creates variable of float array aNeon that has 4, 32 bit floats
	float32x4_t * bNeon = (float32x4_t*) b; //Creates variable of float array bNeon that has 4, 32 bit floats
	float32x4_t * cNeon = (float32x4_t*) c; //Creates variable of float array cNeon that has 4, 32 bit floats

	for (int i = 0; i < size; i++) //Loops through size of array but uses SIMD instructions instead
	{
		*(cNeon++) = vaddq_f32(*(aNeon++), *(bNeon++)); //Adds together respective values of the first 32 bit float in a to b stores in c.
		*(cNeon++) = vaddq_f32(*(aNeon++), *(bNeon++)); //Adds together respective values of the second 32 bit float in a to b stores in c.
		*(cNeon++) = vaddq_f32(*(aNeon++), *(bNeon++)); //Adds together respective values of the third 32 bit float in a to b stores in c.
		*(cNeon++) = vaddq_f32(*(aNeon++), *(bNeon++)); //Adds together respective values of the fourth 32 bit float in a to b stores in c.
	}
}