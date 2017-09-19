#include <iostream>
#include <fstream>
#include <locale>
#include <sstream>
#include <string>

#define TIMEDELAY 3
#define N 128

using namespace std;


struct Node                                                                     // to be used as linked list to implement queue
{
	int data;
	Node *next;
};

class Queue                                                                     // queue implemented with linked lists
{
private:
	Node *front, *back;
	int counter;
public:
	Queue();
	~Queue();
	void Join(int newData);
	void Leave();
	bool IsEmpty();
	int Front();
	int Length();
};

void initialiseCongestionSnapshot(int *congestionSnapshot);                     // initialises congestion size array to 0 to begin similation
int errArguments();                                                             // returns error code if not enough arguments
int errFile(const char *file);                                                  // returns error code if file cannot be opened
int errFileFormat();                                                            // returns error code if format of data in file incorrect
int sumPacketLengths(Queue *Q);                                                 // returns sum of queue lengths
void updateCongestionSnapshot(int &maxCongestion, int *congestionSnapshot, Queue *Q);   // updates congestion snapshot array if at max congestion
int getInput(const char *file, Queue *Q, int &numberOfPorts);                   // gets number of ports and stores input packets in queues
int runSim(Queue *inputQ, Queue *outputQ, const int numberOfPorts, int *congestionSnapshot);    // runs router simulation
