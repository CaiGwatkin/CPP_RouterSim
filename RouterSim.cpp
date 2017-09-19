//////////////////////////////////////////////
//	Cai Gwatkin								//
//////////////////////////////////////////////
//	This application simulates the queue	//
//	of a router and outputs the state of	//
//	the queue when the simulation ends		//
//////////////////////////////////////////////

#include "RouterSim.h"


int main(int argc, char** argv)
{
	// returns error code if not enough arguments
	if (argc != 2) return errArguments();

	Queue inputQ[N];
	Queue outputQ[N];
	int numberOfPorts = 0;

	// gets number of ports and stores input packets in queues
	int err = getInput(argv[1], inputQ, numberOfPorts);
	if (err != 0) return err;

	int congestionSnapshot[N];
	initialiseCongestionSnapshot(congestionSnapshot);

	// runs simulation
	err = runSim(inputQ, outputQ, numberOfPorts, congestionSnapshot);
	if (err != 0) return err;	

	// print to console the congestion size for each output port
	for (int i = 0; i < numberOfPorts; i++) cout << "output port " << i + 1 << ": " << congestionSnapshot[i] << " packets" << endl;

	return 0;
}

// constructor
Queue::Queue()
{
	front = NULL;
	back = NULL;
	counter = 0;
}

// destructor
Queue::~Queue()
{
	if (front == NULL) return;
	while (front != back)
	{
		Node *temp = front;
		front = front->next;
		delete temp;
	}
	delete front;
}

// adds packet to tail of queue
void Queue::Join(const int newData)
{
	Node *temp;
	temp = new Node;
	temp->data = newData;
	temp->next = NULL;
	if (back != NULL) back->next = temp;
	back = temp;
	if (front == NULL) front = temp;
	counter++;
}

// removes packet from head of queue
void Queue::Leave()
{
	if (front == NULL) return;
	Node *temp = front;
	front = front->next;
	if (front == NULL) back = NULL;
	delete temp;
	counter--;
}

// returns true if queue is empty
bool Queue::IsEmpty()
{
	if (front == NULL) return true;
	return false;
}

// returns the value of the front packet in the queue
int Queue::Front()
{
	return front->data;
}

// returns the number of packets in the queue
int Queue::Length()
{
	return counter;
}

// initialises congestion size array to 0 to begin similation
void initialiseCongestionSnapshot(int *congestionSnapshot)
{
	for (int i = 0; i < N; i++) congestionSnapshot[i] = 0;
}

// returns error code if not enough arguments
int errArguments()
{
	cout << "Error: requires one file name as argument" << endl;
	return 0;
}

// returns error code if file cannot be opened
int errFile(const char *file)
{
	cout << "Error: could not read file \"" << file << "\"" << endl;
	return 0;
}

// returns error code if format of data in file incorrect
int errFileFormat()
{
	cout << "ERROR in the format of the text file" << endl;
	return 0;
}

// returns sum of queue lengths
int sumPacketLengths(Queue *Q)
{
	int sum = 0;
	for (int i = 0; i < N; i++) sum += Q[i].Length();
	return sum;
}

// updates congestion snapshot array if at max congestion
void updateCongestionSnapshot(int &maxCongestion, int *congestionSnapshot, Queue *Q)
{
	int sum = sumPacketLengths(Q);
	if (sum > maxCongestion)
	{
		maxCongestion = sum;
		for (int i = 0; i < N; i++) congestionSnapshot[i] = Q[i].Length();
	}
}

// gets number of ports and stores input packets in queues
int getInput(const char *file, Queue *Q, int &numberOfPorts)
{
	// returns error code if file cannot be opened
	ifstream inFile(file);
	if (!inFile) return errFile(file);

	int portNumber = 0;

	while (!inFile.eof())
	{
		// gets line from file
		string expression;
		getline(inFile, expression);
		stringstream line(expression);

		// ignores comments
		if (expression[0] == '#') continue;

		// gets number of ports
		if (expression[0] == 'P')
		{
			string ports;
			line >> ports >> ports;
			numberOfPorts = stoi(ports);

			continue;
		}

		// create queue from port
		string token;
		while (getline(line, token, ' '))
		{
			int destination = stoi(token);

			// checks format is correct
			if (destination < 0 || destination > numberOfPorts || numberOfPorts < portNumber) return errFileFormat();

			// add packet to current port's input queue
			Q[portNumber].Join(destination);
		}

		// dealing with next port
		portNumber++;
	}

	return 0;
}

// runs router simulation
int runSim(Queue *inputQ, Queue *outputQ, const int numberOfPorts, int *congestionSnapshot)
{
	unsigned long int clock = 0;
	unsigned long int inputQLengths = 1;
	int portNumber = 0;
	int maxCongestion = 0;

	while (inputQLengths > 0)
	{
		// move packets from input queues to output queues
		if (!inputQ[portNumber].IsEmpty())
		{
			// move packet from input to output queue corresponding to packet data
			outputQ[inputQ[portNumber].Front() - 1].Join(inputQ[portNumber].Front());

			// remove packet from input queue
			inputQ[portNumber].Leave();

			// updates congestion snapshot array if at max congestion
			updateCongestionSnapshot(maxCongestion, congestionSnapshot, outputQ);
		}

		// go to next port
		portNumber++;

		// if all ports dealt with: update congestion size array and go back to first port
		if (portNumber == numberOfPorts) portNumber = 0;

		// update tick
		clock++;

		// removes packets from output queue if on correct tick determined by timedelay and number of ports
		if (clock % (TIMEDELAY * numberOfPorts) == 0 && clock != 0)
		{
			// remove one packet from all non-empty output queues
			for (int i = 0; i < numberOfPorts; i++) if (!outputQ[i].IsEmpty()) outputQ[i].Leave();
		}

		// compute sum of input queue lengths
		inputQLengths = sumPacketLengths(inputQ);
	}

	return 0;
}
