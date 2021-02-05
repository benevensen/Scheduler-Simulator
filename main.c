
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//enum for process state
typedef enum Process_State
{
    NEW,       // process is new
    READY,     // process is ready to run
    RUNNING,   // process is currently running
    WAITING,   // process is currently waiting on IO
    TERMINATED // process is finished its task and terminated
} States;

//data structure for process
typedef struct process
{
    int pid;                               //Process ID
    int arrival_time;                      //Time of arrival
    int total_CPU_time;                    //Total execution time
    int current_CPU_time_needed;           //amount of CPU time needed to finish the task
    int IO_frequency;                      //How frequently IO is accessed
    int current_time_until_IO;             // amount of time until I/O is needed by the process
    int IO_duration;                       //How long IO is accessed
    int current_time_until_IO_is_finished; //amount of time until I/O is needed by the process
    States state;                          //Current state of a process
} process_t;

// ************************************************************

//Implementation for Queue

//Node for queue
typedef struct Node
{
    struct Node *Next;  // pointer to next node
    process_t *process; // pointer to the node's process

} Node_t;

//Queue implemented with a linked list
typedef struct Queue
{
    Node_t *Head; //pointer to head node
    Node_t *Tail; //pointer to tail node
    int size;     //size of the queue
} Queue_t;

//constructing function for queue
Queue_t *initReadyQueue();

//clean up function for queue
void cleanReadyQueue(Queue_t *ReadyQueue);

//general queue commands
void enqueue(Queue_t *ReadyQueue, process_t *process);
process_t *dequeue(Queue_t *ReadyQueue);
int getQueueSize(Queue_t *ReadyQueue);

//Initializes queue and sets its members to an initial state
Queue_t *initReadyQueue()
{

    // allocates memory for queue on the heap
    Queue_t *ReadyQueue = (Queue_t *)malloc(sizeof(Queue_t));

    //sets head and tail to null and size to 0
    ReadyQueue->Head = NULL;
    ReadyQueue->Tail = NULL;
    ReadyQueue->size = 0;

    //returns the queue
    return ReadyQueue;
}

//cleans up the queue (deallocates the memory on the heap of the queue)
void cleanReadyQueue(Queue_t *ReadyQueue)
{
    free(ReadyQueue);
}

//function enqueues a process onto the queue
//parameters are: a queue, and a process to enqueue
//returns nothing
void enqueue(Queue_t *ReadyQueue, process_t *process)
{

    // if the queue is empty, the first node becomes the head and tail
    if (ReadyQueue->Tail == NULL)
    {

        //allocates memory for a node
        Node_t *node = (Node_t *)malloc(sizeof(Node_t));

        //creates a node and sets its process to the process parameter
        node->process = process;

        //sets next to null
        node->Next = NULL;

        //sets head and tail to current node
        ReadyQueue->Head = (Node_t *)node;
        ReadyQueue->Tail = (Node_t *)node;

        //increments size of queue
        ReadyQueue->size++;
    }
    // queue is not empty
    else
    {
        //allocates memory for a node
        Node_t *node = (Node_t *)malloc(sizeof(Node_t));

        //creates a node and sets its process to the process parameter
        node->process = process;

        //sets next to null
        node->Next = NULL;

        //updates old tail node to point to current node
        ReadyQueue->Tail->Next = (Node_t *)node;

        //sets tail to current node (current node becomes the new tail)
        ReadyQueue->Tail = node;

        //increments size of the queue
        ReadyQueue->size++;
    }
}

//function dequeues a process off the queue
//parameters are: a queue
//returns a process struct
process_t *dequeue(Queue_t *ReadyQueue)
{

    //exits program if dequeue is attempted on an empty queue
    if (ReadyQueue->Head == NULL || ReadyQueue->size == 0)
    {
        perror("Error nothing is in the queue, Returning an empty process");
        exit(-1);

        
    }
    //dequeues a process off the queue
    else
    {

        //gets the process of the head node of the queue
        process_t *frontProcess = ReadyQueue->Head->process;

        // gets a reference to the head node
        Node_t *ptr = ReadyQueue->Head;

        // checks if enqueue has more than 1 node
        if (ReadyQueue->Head != ReadyQueue->Tail)
        {

            //updates head node to point to the next node
            ReadyQueue->Head = ReadyQueue->Head->Next;

            
        }
        // if queue has only 1 node, sets head and tail to null
        else
        {
            ReadyQueue->Head = NULL;
            ReadyQueue->Tail = NULL;
        }

        //decrements the size of the queue
        ReadyQueue->size--;

        //frees up memory allociated for the head node
        free(ptr);

        //returns process that was at the front of the queue
        return frontProcess;
    }
}

//function that returns the size of the queue
//parameters a pointer to a queue
//returns the size of the queue inputted
int getQueueSize(Queue_t *ReadyQueue)
{
    return ReadyQueue->size;
}

// ************************************************************

//Implementation of the kernel simulator

//input file related functions
int countNumberOfProcesses();
void readInputFile(process_t *processes, char *inputFile);

//helper function for getting the string equivalent of enums
const char *getStringFromState(States state);

//output file related functions
FILE *outputFileInit();
void printTransition(FILE *outputFile, int clock, process_t process, States prevState);

// function for printing details of the process (for debugging)
void print_process_details(process_t process);

//checks if kernel simulator is done
bool isDone(process_t *processes, int processCount);

//clean up
void cleanOutputFile(FILE *outputFile);

// Main function that runs the kernel simulator
// Parameters are: the amount of commandline arguements , and an array of strings representing the arguments
int main(int argc, char *argv[])
{

    //variables for the inputfile and outputfile names
    char inputFileName[20];
    char outputFileName[20];

    //no command line argument given -> uses default names for input and output files
    if (argc == 1)
    {
        strcpy(inputFileName, "input.txt");
        strcpy(outputFileName, "output.txt");
    }
    //1 command line argument given -> uses given name for input file and uses default name for output file
    else if (argc == 2)
    {
        strcpy(inputFileName, argv[1]);
        strcpy(outputFileName, "output.txt");
    }
    //2 command line argument given -> uses first given name for input file and second given name for output file
    else if (argc == 3)
    {
        
        strcpy(inputFileName, argv[1]);
        strcpy(outputFileName, argv[2]);

        // exits if there is too much command line arguments
    }
     // exits if there is too much command line arguments
    else
    {
        perror("too many arguments!\n");
        exit(-1);
    }

    //initializes the ready queue for keeping track of the order of processes
    Queue_t *ReadyQueue = initReadyQueue();

    // gets the current number of processes from the input file
    int numberOfProcesses = countNumberOfProcesses(inputFileName);

    //dynamically allocates an array of process structs to hold all the processes
    process_t *processes = (process_t *)malloc(sizeof(process_t) * numberOfProcesses); //Allocate memory for all process

    // reads the input file and constructs the process structs with the correct values
    readInputFile(processes, inputFileName);

    //variable to represent the clock in ticks
    int clock = 0;

    //initializes output file handle (for interacting with the output file)
    FILE *outputFile = outputFileInit(outputFileName);

    //initializes the current running process id to -1
    //ASSUMES process id will never be -1
    int RunningProcess_ID = -1;

    //while loop that runs until all processes are in a terminated state
    //ASSUMES processes can not have multiple transisitons between states in 1 tick
    //ASSUMES processes on arrival will go into a ready state before being able to become the running process
    while (!isDone(processes, numberOfProcesses))
    {

        //Checks if there is no running process and the ready queue is not empty.
        //If conditions are met, the process within the head node of the ready queue becomes the new process
        //NOTE: only processes in the READY state can enter the ready queue, and transition to RUNNING when the 
        //condiions above are met.
        if (getQueueSize(ReadyQueue) > 0 && RunningProcess_ID == -1)
        {

            //dequeues process from ready queue
            process_t *process = dequeue(ReadyQueue);

            //updates the running process id variable
            RunningProcess_ID = process->pid;

            //resets process's current_time_until_IO variable to its IO frequency
            process->current_time_until_IO = process->IO_frequency;

            //saves process's old state
            States prevState = process->state;

            //updates the process's state
            process->state = RUNNING;

            //prints transition to output file
            printTransition(outputFile, clock, *process, prevState);
        }

        //for loop that iterates through all the processes in the array of processes
        for (int i = 0; i < numberOfProcesses; i++)
        {

            //if the process is in the NEW state
            if (processes[i].state == NEW)
            {
                //if its the processes arrival time, the process transitions to the READY state
                if (processes[i].arrival_time == clock)
                {
                    //saves process's old state
                    States prevState = processes[i].state;

                    //updates the process's state
                    processes[i].state = READY;

                    //enqueues process onto the ready queue
                    enqueue(ReadyQueue, &processes[i]);

                    //prints transition to output file
                    printTransition(outputFile, clock, processes[i], prevState);
                }
            }
            //if the process is in the RUNNING state
            else if (processes[i].state == RUNNING)
            {
                // if a running process finishes it's task, it transitions to the TERMINATED state
                if (processes[i].current_CPU_time_needed == 0)
                {
                    //saves process's old state
                    States prevState = processes[i].state;

                    //updates the process's state
                    processes[i].state = TERMINATED;

                    //resets the current running process id to -1, symbolizing there is currently no running process
                    RunningProcess_ID = -1;

                    //prints transition to output file
                    printTransition(outputFile, clock, processes[i], prevState);
                }
                //if a running process needs IO, it transitions to the WAITING state
                else if (processes[i].current_time_until_IO == 0)
                {
                    //saves process's old state
                    States prevState = processes[i].state;

                    //updates the process's state
                    processes[i].state = WAITING;

                    //resets the current running process id to -1, symbolizing there is currently no running process
                    RunningProcess_ID = -1;

                    //prints transition to output file
                    printTransition(outputFile, clock, processes[i], prevState);
                }
                // if it isn't time to request IO and the process's task has not finished
                // decrements the process's current_time_until_IO and current_CPU_time_needed variables
                else
                {
                    processes[i].current_time_until_IO--;
                    processes[i].current_CPU_time_needed--;
                }
            }
            //if the process is in the WAITING state
            else if (processes[i].state == WAITING)
            {

                // if a waiting process is done with IO, it transitions to the READY state and gets enqueued
                if (processes[i].current_time_until_IO_is_finished == 0)
                {
                    //saves process's old state
                    States prevState = processes[i].state;

                    //updates the process's state
                    processes[i].state = READY;

                    //enqueues process onto the ready queue
                    enqueue(ReadyQueue, &processes[i]);

                    //prints transition to output file
                    printTransition(outputFile, clock, processes[i], prevState);
                }
                // if a waiting process is not done with IO, decrements process's current_time_until_IO_is_finished variable
                else
                {
                    processes[i].current_time_until_IO_is_finished--;
                }
            }
            
            //if the process is in the READY state
            else if (processes[i].state == READY)
            {
                //Mechanism for switching the current running process is out of the for loop.
                //Waits and does nothing while process is ready until process is at the front of the ready queue
                //and the current running process transistions out of the RUNNING state (requests I/O or finishes it's task) 
                continue;
            }
            //if the process is in the TERMINATED state
            else if (processes[i].state == TERMINATED)
            {

                // Nothing to do with a terminated process
                continue;
            }
            //if a state is reached that is not defined, exits the program and prints the error
            else
            {
                perror("ERROR: Process state is unknown!\n");
                exit(-1);
            }
        }

        //increments the value of the clock to represent time has elapsed
        clock++;
    }

    //Calls functions to clean up any dynamically allocated resources.
    cleanOutputFile(outputFile);
    cleanReadyQueue(ReadyQueue);
    free(processes);

    return 0;
}

//Function to clean up and close the output file
//parametesr are: a pointer to the output file
//returns nothing
FILE *outputFileInit(char *outputFile)
{

    FILE *outputFIle = fopen(outputFile, "w"); // opens file in current working directory in write mode
                                               // if the file does not exist, it will create the file

    if (outputFIle == NULL)
    { //Check if file opened properly
        perror("Could not open file.");
        exit(1);
    }

    //returns pointer to output file
    return outputFIle;
}

//Function to clean up and close the output file
//parametesr are: a pointer to the output file
//returns nothing
void cleanOutputFile(FILE *outputFile)
{
    //closes the output file
    fclose(outputFile);
}

//Function to print transitions in a processes state to an output file
//parametesr are: a pointer to the output file, the current tick, the process that had a transition in state, and a State enum for the previous state
//returns nothing
void printTransition(FILE *outputFile, int clock, process_t process, States prevState)
{
    //prints process transisiton with the current time and process id to the output file
    fprintf(outputFile, " %d %d %s %s \n", clock, process.pid, getStringFromState((States)prevState), getStringFromState((States)process.state));
};

//Function to count the number of processes in the input file
//parameter is a string of the name of the inputFile
//returns the number of processes found in the input file (assuming each line represents a process)
//ASSUMES : there is no blank lines in the input file
int countNumberOfProcesses(char *inputFile)
{
    int processCtr = 0; // initializes process counter

    FILE *file = fopen(inputFile, "r"); // opens input file in current working directory in read mode

    if (file == NULL)
    { //Checks for file if file opened properly
        perror("Could not open file.");
        exit(1);
    }

    char input_line[100]; // input buffer for reading from the file

    //while loop that reads the input file (line by line) until the end of file is
    while (fgets(input_line, sizeof(input_line), file))
    {
        processCtr++; //Increment process counter for each line read
    }

    //closes input file when finished
    fclose(file);

    //returns the number of processes found in the input file
    return processCtr;
}

//Function to check if all the processes have finished executing
//parameters are an array of processes and the total number of processes in the array
//returns true if all processes are in a TERMINATED state and false otherwise
bool isDone(process_t *processes, int processCount)
{

    //for loop to iterate through the array of process structs
    for (int i = 0; i < processCount; i++)
    {
        //retuns false if a process is not in the TERMINATED state
        if (processes[i].state != TERMINATED)
        {
            return false;
        }
    }

    //returns true if all processes are in the TERMINATED state
    return true;
}

//function for getting the string equivalent of a States Enum
//parameter is an a State Enum
//returns a string
const char *getStringFromState(States state)
{
    //array of strings corresponding to the Process State Enum s
    static const char *states[] = {"NEW", "READY", "RUNNING", "WAITING", "TERMINATED"};

    //returns string of the state enum
    return states[state];
}

//function for printing the details of a process (for debugging)
//parameters is a process struct
//returns nothing
void print_process_details(process_t process)
{
    //prints the state of a process for debugging
    printf("Pid: %d \t, arrival_time: %d \t, total_CPU_time: %d \t, IO_frequency: %d \t, IO_duration: %d \t, state: %s \t \n",
           process.pid, process.arrival_time, process.total_CPU_time, process.IO_frequency, process.IO_duration, getStringFromState(process.state));
}

//function for reading and practing an input file
//parameters are an array of processes and a string for the name of the input file
//returns nothing
//ASSUMES : there is no blank lines in the input file
void readInputFile(process_t *processes, char *inputFile)
{
    char str[100];                //To store the text contained in each line
    const char truncate[2] = " "; //In-line separator
    char *token;                  //To store the token for each line
    int input_parameter = 0;      //To traverse through the different output parameters (i.e., execution time, etc.) Set to zero for first token
    int process_position = 0;     //To traverse through the words in each line

    FILE *file = fopen(inputFile, "r"); //Opens the input file within current working directory in read mode

    // If file could not open, prints the error and exits
    if (file == NULL)
    {
        perror("Could not open file.");
        exit(1);
    }

    //while loop that keeps reading the file (line by line) until the end is reached, stores the contents of the line in str
    while (fgets(str, sizeof(str), file))
    {

        token = strtok(str, truncate); //Break input into a series of tokens

        while (token != NULL)
        {
            int int_token = atoi(token); //Converts string to integer representation

            // if statements to set the members of the process struct according to the input from the file
            if (input_parameter == 0)
            {
                processes[process_position].pid = int_token;
            }
            else if (input_parameter == 1)
            {
                processes[process_position].arrival_time = int_token;
            }
            else if (input_parameter == 2)
            {
                processes[process_position].total_CPU_time = int_token;
            }
            else if (input_parameter == 3)
            {
                processes[process_position].IO_frequency = int_token;
            }
            else if (input_parameter == 4)
            {
                processes[process_position].IO_duration = int_token;
            }

            token = strtok(NULL, truncate); //Reset token
            input_parameter++;              //Increment input parameter position counter
        }

        //sets new processes to initially be in a NEW state
        processes[process_position].state = NEW;

        //initializes variables used to track cpu time, time until IO, and current IO duration
        processes[process_position].current_CPU_time_needed = processes[process_position].total_CPU_time;
        processes[process_position].current_time_until_IO_is_finished = processes[process_position].IO_duration;
        processes[process_position].current_time_until_IO = processes[process_position].IO_frequency;

        input_parameter = 0; //Reset input parameter counter
        process_position++;  //Increment the process position counter to point to the next process in the array of processes
    }

    //closes the input file
    fclose(file);
}

//REFERENCES:-
//1. https://www.tutorialspoint.com/c_standard_library/
