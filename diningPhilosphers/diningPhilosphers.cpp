//Jacob Simonek | CSC 4730 | Week Of: 11/08/21
#include <iostream>
#include <getopt.h>
#include <thread>
#include <mutex>

using namespace std;

const int NUM_THREADS = 5;  //Constant declared for number of threads (players) which is 5
thread* threads[NUM_THREADS]; //Declaration of 5 threads (these will act as our players)
mutex threadLocks[NUM_THREADS]; //Declaration of 5 mutexes (these will act as our fugelhorns)
mutex coutLock; //Declaration of mutex in order to prevent cout stomping

void HandleOptions(int argc, char* argv[]); //Handle options is in charge of the command line
void JoinThreads(); //Method created to join the threads together after launching
void LaunchThreads(int val); //This method is used to the launch our 5 threads (players)
void FleegmansAlgorithm(int tid); //This method will implement Fleegman's Algorithm which will end in a deadlock
void TryLockSolution(int tid); //This implements the first solution (not resulting  in a deadlock) using try_lock()s
void OtherLockSolution(int tid); //This implements the second solution (not resulting in a deadlock) using another lock

int main(int argc,char* argv[])
{
    HandleOptions(argc, argv); //Calls on HandleOptions function to set everything in motion
    return 0;
}

void HandleOptions(int argc, char* argv[]) //Handle Options deals with the command line options entered by the user
{
   	int option; //option used to hold value from getopt()
	while ((option = getopt(argc, argv, "hgto")) != -1) //While loop handles whether a user enters a getopt option
	{
		switch(option) //Switch based on the case the user selected
		{
			case 'h': //This case will run if the user types '-h' and print what the program does
				cout << "help:" << endl; 
				cout << "-h         prints this." << endl; 
                cout << "-g         Fleegman's Algorithm (will deadlock)." << endl;
                cout << "-t         try_lock based solution." << endl;
                cout << "-o         other lock based solution." << endl;
				exit(0); 
			case 'g':
                LaunchThreads(0);
				JoinThreads();
				break; 
			case 't':
                LaunchThreads(1);
				JoinThreads();
				break; 
            case 'o': 
                LaunchThreads(2);
				JoinThreads();
                break;
			default: 
				break;  
		}
	} 
}

void FleegmansAlgorithm(int tid) //This method will implement Fleegman's Algorithm which will end in a deadlock
{
	while (true) //Loops through continously, however, this should deadlock
	{
		if (tid !=  4) //If the thread ID is not 4, otherwise we will need to wrap around
		{
			threadLocks[tid].lock(); //Player n aquires fuggelhorn n (thread n locks mutex n)
			threadLocks[tid+1].lock(); //Player n aquires fuggelhorn n+1 (thread n locks mutex n+1)
			coutLock.lock(); //Lock used to protect cout is locked
			cout << "Fleegman's Algorithm, player:" << tid << endl; //Cout prints out which player is picking up his horns
			coutLock.unlock(); //Lock used to protect cout is unlocked
			threadLocks[tid+1].unlock(); //Player n puts down fuggelhorn n (thread n unlocks mutex n)
			threadLocks[tid].unlock(); //Player n puts down fuggelhorn n+1 (thread n unlocks mutex n+1)
			sched_yield();  //sched_yield called and player plays again
		}
		else
		{
			threadLocks[tid].lock(); //Player n aquires fuggelhorn n (thread n locks mutex n)
			threadLocks[0].lock(); //Player n aquires fuggelhorn n+1 (thread n locks mutex n+1) since this wraps around it is the first index
			coutLock.lock();  //Lock used to protect cout is locked
			cout << "Fleegman's Algorithm, player:" << tid << endl; //Cout prints out which player is picking up his horns
			coutLock.unlock();  //Lock used to protect cout is locked
			threadLocks[0].unlock(); //Player n aquires fuggelhorn n+1 (thread n locks mutex n+1) since this wraps around it is the first index
			threadLocks[tid].unlock(); //Player n puts down fuggelhorn n (thread n unlocks mutex n)
			sched_yield(); //sched_yield called and player plays again
		}
	}
}

void TryLockSolution(int tid) //This implements the first solution (not resulting in a deadlock) using try_lock()s
{
	while (true) //Loops through continously, however, this should not deadlock
	{
		if (tid !=  4) //If the thread ID is not 4, otherwise we will need to wrap around
		{
			if(threadLocks[tid].try_lock()) //Calls try_lock and attempts to unlock lock n, will lock if it can
			{
				if (threadLocks[tid+1].try_lock()) //Calls try_lock and attempts to unlock lock n+1, will lock if it can
				{
					coutLock.lock(); //Lock used to protect cout is locked
					cout << "Try_Lock Solution, player:" << tid << endl; //Cout prints out which player is picking up his horns
					coutLock.unlock(); //Lock used to protect cout is unlocked
					threadLocks[tid+1].unlock(); //Unlocks lock n+1 which was locked with the try_lock call
				}
				threadLocks[tid].unlock(); //Unlocks lock n which was locked with the try_lock call
				sched_yield();  //sched_yield called and player plays again
			}
		}
		else
		{
			if(threadLocks[tid].try_lock()) //Calls try_lock and attempts to unlock lock n, will lock if it can
			{
				if (threadLocks[0].try_lock()) //Calls try_lock and attempts to unlock lock n+1, will lock if it can
				{
					coutLock.lock(); //Lock used to protect cout is locked
					cout << "Try_Lock Solution, player:" << tid << endl; //Cout prints out which player is picking up his horns
					coutLock.unlock(); //Lock used to protect cout is unlocked
					threadLocks[0].unlock(); //Unlocks lock n+1 which was locked with the try_lock call
				}
				threadLocks[tid].unlock();  //Unlocks lock n which was locked with the try_lock call
				sched_yield();  //sched_yield called and player plays again
			}
		}
	}
}

void OtherLockSolution(int tid) //This implements the second solution (not resulting in a deadlock) using another lock
{
	while (true) //Loops through continously, however, this should not deadlock
	{
		if (tid == 0) //If it is the first player, meaning there is no n-1
		{
			threadLocks[tid + 1 % NUM_THREADS].lock(); //Player n aquires fuggelhorn n-1 (thread n locks mutex n-1)
			threadLocks[tid].lock(); //Player n aquires fuggelhorn n (thread n locks mutex n)
			coutLock.lock(); //Lock used to protect cout is locked
			cout << "Other Solution, player:" << tid << endl; //Cout prints out which player is picking up his horns
			coutLock.unlock(); //Lock used to protect cout is unlocked
			threadLocks[tid].unlock(); //Player n puts down fuggelhorn n (thread n unlocks mutex n)
			threadLocks[tid + 1 % NUM_THREADS].unlock(); //Player n puts down fuggelhorn n-1 (thread n unlocks mutex n-1)
			sched_yield(); //sched_yield() yields to play again
		}
		else
		{
			threadLocks[tid].lock(); //Player n aquires fuggelhorn n (thread n locks mutex n)
			threadLocks[tid-1].lock(); //Player n aquires fuggelhorn n-1 (thread n locks mutex n-1)
			coutLock.lock(); //Lock used to protect cout is locked
			cout << "Other Solution, player:" << tid << endl; //Cout prints out which player is picking up his horns
			coutLock.unlock(); //Lock used to protect cout is unlocked
			threadLocks[tid-1].unlock(); //Player n puts down fuggelhorn n-1 (thread n unlocks mutex n-1)
			threadLocks[tid].unlock(); //Player n puts down fuggelhorn n (thread n unlocks mutex n)
			sched_yield();  //sched_yield() yields to play again
		}
	}
}

void LaunchThreads(int val) //This method is used to the launch our 5 threads (players)
{
	if  (val == 0) //If the user selected the 'g' case in getopt then val is 0 and Fleegmans Algorithm starts
	{
		for( int r = 0; r < NUM_THREADS; r++) //Loops through to launch the threads
			threads[r] = new thread(FleegmansAlgorithm, r); //Launches the threads and calls FleegmansAlgorithm() to get output for program
	}
	else if (val == 1) //If the user selected the 't' case in getopt then val is 1 and try_lock solution starts
	{
		for( int r = 0; r < NUM_THREADS; r++) //Loops through to launch the threads
			threads[r] = new thread(TryLockSolution, r); //Launches the threads and calls TryLockSolution() to get output for program
	}
	else  //If the user selected the 'o' case in getopt then val is 2 and the other lock solution starts
	{
		for( int r = 0; r < NUM_THREADS; r++) //Loops through to launch the threads
			threads[r] = new thread(OtherLockSolution, r); //Launches the threads and calls OtherLockSolution() to get output for program
	}
}

void  JoinThreads() //Method created to join the threads together after launching
{
	for(int i = 0; i < NUM_THREADS; i++) //Loops through and joins each of the threads
		threads[i]->join(); //Uses pointer notation '->' to call join since threads is a pointer to a pointer
}
