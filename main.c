
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//The different possible states a process can have ("TERMINATED" is synonymous for EXIT)
typedef enum Process_State
{
    NEW,       // Process is NEW
    READY,     // Process is READY to run
    RUNNING,   // Process is RUNNING
    WAITING,   // Process is WAITING for I/O and such
    TERMINATED // Process is finished its task and TERMINATED/EXIT
} States;

//The data structure for the PCB
typedef struct process
{
    int pid;                               // Process ID.
    int arrival_time;                      // Time of arrival.
    int total_CPU_time;                    // Total execution time.
    int current_CPU_time_needed;           // Amount of CPU time needed to finish the task.
    int IO_frequency;                      // How frequently IO is accessed.
    int current_time_until_IO;             // Amount of time until I/O is needed by the process.
    int IO_duration;                       // How long IO is accessed.
    int current_time_until_IO_is_finished; // Amount of time until I/O is needed by the process.
    int initial_priority;                  // Priority when the process first came into the queue.
    int effective_priority;                // Effective priority the process currently has.
    int memory_needed;                     // The amount of memory need for the process.
    int partition_used;                    // The partition that the process is stored on, set to -1 is not in memory
    States state;                          // Current state of a process.
} process_t;

/* ======================================================
 * QUEUE IMPLEMENTATION
 * ======================================================
 *
 * Used a LinkedList because:-
 * 1. May have to manipulate it often (Doubly LinkedList's are faster).
 * 2. We will only be adding process sequentially (their order is not necessarily linear, though).
 */

//A Node for the queue.
typedef struct Node
{
    struct Node *Next;  // Pointer to next Node.
    process_t *process; // Pointer to the Node's PCB (named process).

} Node_t;

//Queue implementation with a LinkedList.
typedef struct Queue
{
    Node_t *Head; // Pointer to head Node.
    Node_t *Tail; // Pointer to tail Node.
    int size;     // Size of the queue.
} Queue_t;

//Constructing function for queue.
Queue_t *initReadyQueue();

//Clean up function for queue.
void cleanReadyQueue(Queue_t *ReadyQueue);

//Commands for enqueue-ing, dequeue-ing, and accessor method for queue size.
void enqueue(Queue_t *ReadyQueue, process_t *process);
process_t *dequeue(Queue_t *ReadyQueue);
int getQueueSize(Queue_t *ReadyQueue);

//Commands for enqueue-ing with priority scheduler algorithm.
//To prevent starvation aging has been recognised.
void priority_enqueue(Queue_t *ReadyQueue, process_t *process);
void age_priority_queue(Queue_t *ReadyQueue);


//Initializing the queue and setting its member's initial state.
Queue_t *initReadyQueue()
{

    //Allocating memory for queue on the heap.
    Queue_t *ReadyQueue = (Queue_t *)malloc(sizeof(Queue_t));

    //Setting head and tail to NULL and size to 0.
    ReadyQueue->Head = NULL;
    ReadyQueue->Tail = NULL;
    ReadyQueue->size = 0;

    //Returning the queue.
    return ReadyQueue;
}

//Function ages all processes in the priority queue.
//Method to print out the queue as it stands.
//Parameter:- ReadyQueue, a queue.
//Return:- N/A.
void print_queue(Queue_t *ReadyQueue){

    Node_t* current_node = ReadyQueue->Head;

    while (current_node != NULL)
    {
        printf("%d (priority: %d) -> ", current_node->process->pid, current_node->process->effective_priority);

        current_node = current_node->Next;
    }

    printf("\n");
}

//Method to clean up the queue (deallocates the memory on the heap of the queue).
//Parameter:- ReadyQueue, a queue.
//Return:- N/A.
void cleanReadyQueue(Queue_t *ReadyQueue)
{
    free(ReadyQueue); //Freeing the provided queue.
}

//Method for enqueue-ing a process onto the queue.
//Parameter:- ReadyQueue, a queue.
//Parameter:- process, a PCB to enqueue.
//Return:- N/A.
void enqueue(Queue_t *ReadyQueue, process_t *process)
{

    //If queue is empty; make first Node the head and the tail.
    if (ReadyQueue->Tail == NULL)
    {
        //Allocating memory for a Node.
        Node_t *node = (Node_t *)malloc(sizeof(Node_t));

        //Creating a Node and setting its PCB to the process parameter.
        node->process = process;

        //Setting the next to NULL.
        node->Next = NULL;

        //Setting the head and the tail to the current Node.
        ReadyQueue->Head = (Node_t *)node;
        ReadyQueue->Tail = (Node_t *)node;
    }

    //If queue is not empty; make Node the tail.
    else
    {
        //Allocating memory for a Node.
        Node_t *node = (Node_t *)malloc(sizeof(Node_t));

        //Creating a Node and setting its PCB to the process parameter.
        node->process = process;

        //Setting next to NULL.
        node->Next = NULL;

        //Updating the old tail Node to point to the current Node.
        ReadyQueue->Tail->Next = (Node_t *)node;

        //Setting the tail to current Node (current Node becomes the new tail).
        ReadyQueue->Tail = node;
    }

    //Incrementing the size of the queue.
    ReadyQueue->size++;

    //TESTING
    //print_queue(ReadyQueue);
}

//Method for enqueue-ing a process onto the queue.
//Parameters:- ReadyQueue, a queue.
//Parameter:- process, a PCB to enqueue.
//Return:- N/A.
// TODO: implement the priority enqueue
void priority_enqueue(Queue_t *ReadyQueue, process_t *process)
{
    //If the queue is empty; create a new one.
    //The first Node becomes the head and the tail.
    if (ReadyQueue->Tail == NULL)
    {
        //Allocating memory for a Node.
        Node_t *node = (Node_t *)malloc(sizeof(Node_t));

        //Creating a Node and setting its PCB to the process parameter.
        node->process = process;

        //Setting next to NULL.
        node->Next = NULL;

        //Setting the head and the tail to the current Node.
        ReadyQueue->Head = (Node_t *)node;
        ReadyQueue->Tail = (Node_t *)node;

        //Incrementing the size of the queue.
        ReadyQueue->size++;
    }

    //If the queue is not empty; compare priorities and add where needed.
    else
    {
        //Allocating memory for a Node.
        Node_t *node = (Node_t *) malloc(sizeof(Node_t));

        //Creating a Node and setting its PCB to the process parameter.
        node->process = process;

        //Pointer to track the current Node.
        Node_t* current_node = ReadyQueue->Head;

        //Pointer to track the previous Node.
        Node_t* prev_node = ReadyQueue->Head;

        //Iterating through the processes in the priority queue comparing priorities until
        //a process with a higher priority (i.e., a lower number) than the process being enqueued is found.
        while(current_node->process->effective_priority <= node->process->effective_priority){

            //If the iteration reaches the head of the queue; stop iterating.
            if(current_node == ReadyQueue->Tail){
                break;
            }

            //Updating the previous Node and the current Node to the next Nodes in the LinkedList.
            prev_node = current_node;
            current_node = current_node->Next;
        }

        //printf("\n current node: %d , previous node: %d\n\n", current_node->process->pid,prev_node->process->pid );

        //If there is only one process on the queue; check for higher priority and add where neccessary.
        if(ReadyQueue->Head == ReadyQueue->Tail){

            //printf("IN ONE ELEMENT QUEUE\n");

            //If the priority of the new Node is lesser than the present Node; add the new Node to the tail of the LinkedList.
            if(ReadyQueue->Head->process->effective_priority <= node->process->effective_priority){

                //Make head Node next point to new Node.
                ReadyQueue->Head->Next = node;

                //Make new Node next point to NULL as it is the new tail.
                node->Next = NULL;

                //Updating the ready queue's tail.
                ReadyQueue->Tail = node;
            }

            //If the priority of the new Node is greater than the present Node; add the new Node to the head of the LinkedList.
            else{

                //Make new Node point to the old head Node.
                node->Next = ReadyQueue->Head;
                
                //Updating the ready queue's head.
                ReadyQueue->Head = node;
            }
        }

        //If the process being enqueued has a higher priority than the head; make it the new head.
        else if(current_node == ReadyQueue->Head){

            //printf("Current node is the head \n");
            
            //Setting new Node's next to the previous head Node, it is the new head.
            node->Next = ReadyQueue->Head;

            /* //set head node to point to the new node
            current_node->Next = (Node_t *) node; */

            //Setting the head of the queue to be the new Node.
            ReadyQueue->Head = node;


        //If the process being enqueued has a higher priority than the tail; make it the new tail.
        }else if(current_node == ReadyQueue->Tail){
            //printf("Current node is the tail \n");


            if(ReadyQueue->Tail->process->effective_priority <= node->process->effective_priority){

                ReadyQueue->Tail->Next = node;

                //Setting the new Node's next to the tail as it will be the new tail.
                node->Next = NULL;

                //Setting the tail of the queue to be the new Node.
                ReadyQueue->Tail = (Node_t *) node;

            }else{

                node->Next = prev_node->Next;

                prev_node->Next = node;
            }

        //If the Node has a medium-level priority; it will be inserted in the middle.
        }else{

            //printf("Current node is being inserted in the middle of the queue \n");
           /*  //sets next to current node's next
            node->Next = current_node->Next;

            current_node->Next = node; */

          /*   //updates old tail node to point to current node
            prev_node->Next = (Node_t *) node; */
            //Seting the new Node's next to the current Node.
            //node->Next = current_node;

            //Setting the previous Node's next to be the new Node.
            //prev_node->Next = node;

            node->Next = prev_node->Next;

            prev_node->Next = node;

        }



        //Incrementing the size of the queue.
        ReadyQueue->size++;
    }
        //TESTING
        printf("Priority enqueue \n");
        print_queue(ReadyQueue);
}

//function ages all processes in the priority queue
//parameters are: a queue
//returns nothing
void age_priority_queue(Queue_t *ReadyQueue)
{

    // if the queue is empty, there is nothing to age
    if (ReadyQueue->Tail == NULL)
    {
        return;
    }
    // queue is not empty
    else
    {
     
        // variable current_node
        Node_t* current_node = ReadyQueue->Head;

        //iterate throughout the entire priority queue and decrement the effective priority to age the processes
        while(current_node != NULL){

            if (current_node->process->effective_priority > 0)
            {
                current_node->process->effective_priority--;
            }

            current_node = current_node->Next;
        }

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

        //TESTING
       /*  printf("Dequeue \n");
        print_queue(ReadyQueue); */

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
void readInputFile(process_t *processes, char *inputFile, int memory_scheme);

//helper function for getting the string equivalent of enums
const char *getStringFromState(States state);

//output file related functions
FILE *outputFileInit();
void printTransition(FILE *outputFile, int clock, process_t process, States prevState);
void print_memory_information(FILE *outputFile, int partitions[][2] , int memory_scheme);

// function for printing details of the process (for debugging)
void print_process_details(process_t process);

//checks if kernel simulator is done
bool isDone(process_t *processes, int processCount);

//clean up
void cleanOutputFile(FILE *outputFile);

//function for dispatching processes based on the schedule algorithm
//mode is 1 for FCFS, mode is 2 for priority scheduling, mode is 3 for round robin with 10ms timeout
process_t * dispatcher(int mode);

//function for allocating memory to processes or freeing memory,
//it will allocate a free partition if a partition is available, otherwise it will return -1
//the function will also free memory when a process is terminated 
int memory_manager(int partitions[][2], int command, process_t* process, int memory_scheme );

//variable for 100ms timeout assuming 1 tick is 1 ms
const int TIMEOUT_AMOUNT = 100; 

//Commands for the memory manager
const int ALLOCATE = 0;
const int FREE = 1;


const int MEMORY_SCHEME_1[][2] = {{500,-1},{250,-1},{150,-1},{100,-1}};

const int MEMORY_SCHEME_2[][2] = {{300,-1},{300,-1},{350,-1},{50,-1}};


// Main function that runs the kernel simulator
// Parameters are: the amount of commandline arguements , and an array of strings representing the arguments
int main(int argc, char *argv[])
{

    //variables for the inputfile and outputfile names
    char inputFileName[20];
    char outputFileName[20];

    //variable for checking which scheduling algorithm is being used
    // 1 for FCFS, 2 for Priority Scheduling, 3 for Round Robin with 100ms timeout
    int mode;

    //variable for which memory_scheme to use
    //set to 0 if unused
    int memory_scheme = 0;

    //no command line argument given -> uses default values for mode, memory_scheme, input and output files
    if (argc == 1)
    {
        strcpy(inputFileName, "input.txt");
        strcpy(outputFileName, "output.txt");

        mode = 1;
        memory_scheme = 0;

    }
    //1 command line argument given -> uses given value for mode, 
    //and default values for  memory_scheme, input and output filenames
    else if (argc == 2)
    {

        //strcpy(inputFileName, argv[1]);

        mode = atoi(argv[1]);

        memory_scheme = 0;

        strcpy(inputFileName, "input.txt");
        strcpy(outputFileName, "output.txt");
    }
    //2 command line argument given -> uses given values for mode and memory_scheme
    //and default values for input and output filenames
    else if (argc == 3)
    {
        
        mode = atoi(argv[1]);

        memory_scheme = atoi(argv[2]);

        strcpy(inputFileName, "input.txt");
        strcpy(outputFileName, "output.txt");

    //3 command line argument given -> uses given values for mode, memory_scheme, and input filename
    //and default values for output filename
    }else if (argc == 4)
    {

        mode = atoi(argv[1]);

        memory_scheme = atoi(argv[2]);

        strcpy(inputFileName, argv[3]);
        strcpy(outputFileName,"output.txt");
    } 
    //4 command line argument given -> uses given values for mode, memory_scheme, input filename and output filename
    else if (argc == 5)
    {

        mode = atoi(argv[1]);

        memory_scheme = atoi(argv[2]);

        strcpy(inputFileName, argv[3]);
        strcpy(outputFileName, argv[4]);
    }
     // exits if there is too much command line arguments
    else
    {
        perror("too many arguments!\n");
        exit(-1);
    }

    //variable to hold partition information
    int partitions[4][2];

    //double for loop to initial the partitions array to the correct sizes
    if(memory_scheme == 1 || memory_scheme == 2 ){

        for(int j = 0; j < 4; j++){

            for(int k = 0; k < 2; k++){

                if (memory_scheme == 1)
                {
                    partitions[j][k] = MEMORY_SCHEME_1[j][k];
                }
                else
                {
                    partitions[j][k]  = MEMORY_SCHEME_2[j][k];
                }
            }
        }
    }


    if(memory_scheme == 1 || memory_scheme == 2 ){

        printf("Printing memory partition... \n");

        for(int j = 0; j < 4; j++){

            for(int k = 0; k < 2; k++){

               printf("partition [%d] [%d]  is %d\n", j, k, partitions[j][k]);
            }
        }
    }

    //initializes the ready queue for keeping track of the order of processes
    Queue_t *ReadyQueue = initReadyQueue();

    // gets the current number of processes from the input file
    int numberOfProcesses = countNumberOfProcesses(inputFileName);

    //dynamically allocates an array of process structs to hold all the processes
    process_t *processes = (process_t *)malloc(sizeof(process_t) * numberOfProcesses); //Allocate memory for all process

    // reads the input file and constructs the process structs with the correct values
    readInputFile(processes, inputFileName, memory_scheme);

    //variable to represent the clock in ticks
    int clock = 0;

    //initializes output file handle (for interacting with the output file)
    FILE *outputFile = outputFileInit(outputFileName);

    //initializes the current running process id to -1
    //ASSUMES process id will never be -1
    int RunningProcess_ID = -1;

    //variable for tracking if a process will timeout within the Round Robin Algorithm
    int timeout = 0;

    //variable for tracking the amount of ticks that have happened
    int tick_until_aging = 10;

    //while loop that runs until all processes are in a terminated state
    //ASSUMES processes can not have multiple transisitons between states in 1 tick
    //ASSUMES processes on arrival will go into a ready state before being able to become the running process
    while (!isDone(processes, numberOfProcesses))
    {

        //if the scheduing algorithm is the priority scheduler
        if(mode == 2){

            //update the tick_until_aging variable 
            tick_until_aging--;

            //if tick_until_aging is less than 10, then it is time to 
            if(tick_until_aging <= 0){

                //reset tick_until_aging 
                tick_until_aging = 10;

                //age processes in the priority queue
                age_priority_queue(ReadyQueue);
            }
        }


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

            //resets priority of the process transitioning to running
            //can be done in this transition because this algorithm does not use preemption so 
            //the priority of the running is not important.
            if(mode == 2){
                //resets process's effective priority variable to its initial priority
                process->effective_priority = process->initial_priority;

                //if mode is Round Robin
            }else if(mode == 3){
                
                //reset timeout
                timeout = 0;
            }

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

                //printf("new process with id %d\n" , processes[i].pid);

                //if its the processes arrival time, the process transitions to the READY state
                if (processes[i].arrival_time <= clock )
                {


                    if(memory_scheme != 0 && memory_manager(partitions,ALLOCATE,&processes[i], memory_scheme) == -1){
                        continue;
                    }

                    printf("new process with id %d has been allocated!\n" , processes[i].pid);

                    for (int j = 0; j < 4; j++)
                    {

                        for (int k = 0; k < 2; k++)
                        {

                            printf("partition [%d] [%d]  is %d\n", j, k, partitions[j][k]);
                        }
                    }

                    getchar();

                    //saves process's old state
                    States prevState = processes[i].state;

                    //updates the process's state
                    processes[i].state = READY;

                    //if scheduling algorithm is priority scheduling 
                    if(mode == 2){

                        //enqueues process onto a priority ready queue
                        priority_enqueue(ReadyQueue, &processes[i]);

                    // enqueues process onto normal queue if the scheduling algorithm is FCFS or Round Robin
                    }else{

                        //enqueues process onto the ready queue
                        enqueue(ReadyQueue, &processes[i]);

                    }

                    //prints transition to output file
                    printTransition(outputFile, clock, processes[i], prevState);

                    if( memory_scheme != 0){
                        print_memory_information(outputFile, partitions, memory_scheme);
                    }
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

                    memory_manager(partitions,FREE, &processes[i], memory_scheme);

                    printf("Process %d is terminating", processes[i].pid);

                    printf("Printing memory partition... \n");

                    for (int j = 0; j < 4; j++)
                    {

                        for (int k = 0; k < 2; k++)
                        {

                            printf("partition [%d] [%d]  is %d\n", j, k, partitions[j][k]);
                        }
                    }

                    getchar();
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

                    //if schedule algorithm being used is the Round Robin algorithm
                    if (mode == 3)
                    {

                        //if timeout variable is greater or equal to 100ms, then the process gets timed out and
                        //goes back to the ready queue
                        if (timeout == TIMEOUT_AMOUNT && processes[i].current_CPU_time_needed != 0)
                        {

                            //saves process's old state
                            States prevState = processes[i].state;

                            //updates the process's state
                            processes[i].state = READY;

                            //resets the current running process id to -1, symbolizing there is currently no running process
                            RunningProcess_ID = -1;

                            //TESTING
                            //printf("time left in process: %d  is %d \n", processes[i].pid, processes[i].current_CPU_time_needed);

                            //
                            enqueue(ReadyQueue, &processes[i]);

                            //prints transition to output file
                            printTransition(outputFile, clock, processes[i], prevState);

                            continue;
                        }

                        //increment the timeout variable
                        timeout++;
                    }

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

                    if(mode == 2){

                        //enqueues process onto a priority ready queue
                        priority_enqueue(ReadyQueue, &processes[i]);

                    // enqueues process onto normal queue if the scheduling algorithm is FCFS or Round Robin
                    }else{

                        //enqueues process onto the ready queue
                        enqueue(ReadyQueue, &processes[i]);

                    }

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



void print_memory_information(FILE *outputFile, int partitions[4][2] , int memory_scheme){
    

    //
    //  **********************
    //  Total Memory Used: %d MB | Free Memory Available: %d MB | Usable Memory: MB
    //
    //  Partitions Available: 
    //  Partition 1 (%d MB) Available? Yes/No
    //  Partition 2 (%d MB) Available? Yes/No
    //  Partition 3 (%d MB) Available? Yes/No
    //  Partition 4 (%d MB) Available? Yes/No
    //  ***************************
    //
    //
    //


        printf("Printing memory partition... \n");

        for(int j = 0; j < 4; j++){

            for(int k = 0; k < 2; k++){

               printf("partition [%d] [%d]  is %d\n", j, k, partitions[j][k]);
            }
        }
    

    //variable for calculating the amount of free memory available
    int total_free_memory_available = 0;

    //variable for calculating the amount of usable memory available
    int total_usable_memory_available = 0;

    //variable for calculating the amount of used memory
    int memory_used = 0;

    //for loop that iterates through the partitions
    for (int i = 0; i < 4; i++)
    {
        //adds the remaining space in each partition to total_free_memory_available
        total_free_memory_available += partitions[i][0];

        //adds the memory used by calculating the difference between the capacity of each partition and 
        //the current remaining space (depends on the memory portions assigned)
        if(memory_scheme == 1){
            memory_used += (MEMORY_SCHEME_1[i][0] - partitions[i][0]);
        }else{
            memory_used += (MEMORY_SCHEME_2[i][0] - partitions[i][0]);
        }

        //adds the remaining space in each availble partition to total_usable_memory_available
        //after checking if the partition is currently usable
        if(partitions[i][1] == -1){
            total_usable_memory_available += partitions[i][0];
        }
    }


    //prints formatted information to the file
    fprintf(outputFile,"\n************************ \n");

    fprintf(outputFile,"Total Memory Used: %d MB | Free Memory Available: %d MB | Usable Memory: %d MB \n", memory_used, total_free_memory_available, total_usable_memory_available);

    fprintf(outputFile,"Partitions Available: \n");

    // string for printing availability
    char availability[10];

    //for loop that iterates through all the partions
    for(int i = 0; i < 4 ; i++){

        //sets the availability to "Yes" if the partion is available and "No" if the partition is not available.
        if(partitions[i][1] == -1){
            strcpy(availability,"Yes");
        }else{
            strcpy(availability,"No");
        }

        //prints out the availablitiy information of the partition
        if(memory_scheme == 1){
           fprintf(outputFile,"Partition %d  %d MB/(%d MB) Available? %s \n", i , partitions[i][0] , MEMORY_SCHEME_1[i][0],  availability );
        }else{
           fprintf(outputFile,"Partition %d  %d MB/(%d MB) Available? %s \n", i , partitions[i][0] , MEMORY_SCHEME_2[i][0], availability );
        }
        
    }

    fprintf(outputFile,"************************ \n\n");

}


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


//function for allocating memory or freeing memory from partitions
//parameters are a partition array, the command the partition is doing, and the process
//that is being allocated or freed from memory
//returns 0 if successful in allocating memory and returns -1 if there was no space for the process in memory
//returns 2 if successful in freeing memory
int memory_manager(int partitions[][2], int command, process_t* process, int memory_scheme ){

    //memory manager attempts to allocates memory if it receives the allocate command
    if(command == ALLOCATE){

        printf("Allocating for process %d\n ", process->pid);

        //sets initial value for partition_for_process
        int partition_for_process = -1;

        for(int i = 0; i < 4; i++){

            if( partitions[i][1] == -1 && process->memory_needed <= partitions[i][0]){
                partition_for_process = i;

                break;
            }
        }

        // if partition_for_process is at the initial value, the memory manager couldn't find space for the process
        if(partition_for_process == -1){

            printf("couldn't find memory for process %d \n", process->pid);

            //returns -1 because the memory manager couldn't find space for the process
            return -1;


            //memory manager found space for the process
        }else{
            
            printf("Found memory for process %d in partition %d\n", process->pid, partition_for_process);
            //memory manager updates the information about memory usage within the partition
            partitions[partition_for_process][0] -= process->memory_needed;

            //memory manager updates the information about availability within the partition
            partitions[partition_for_process][1] = 1;

            //memory manager updates the information about memory position within the process
            process->partition_used = partition_for_process;

            return 0;
        }


    //memory manager attempts to free memory if it receives the allocate command
    //used for freeing memory from terminated processes
    }else if(command == FREE){

        //frees memory based on the current memory scheme
        if(memory_scheme == 1){
            partitions[process->partition_used][0] = MEMORY_SCHEME_1[process->partition_used][0]; 
            partitions[process->partition_used][1] = -1;
        }else{
            partitions[process->partition_used][0] = MEMORY_SCHEME_2[process->partition_used][0]; 
            partitions[process->partition_used][1] = -1;
        }

        return 2;

    //if the memory manager gets an unknown command
    }else{
        perror("Memory manager could not handle unknown command");
        exit(1);
    }

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
//parameters are an array of processes , a string for the name of the input file, and the memory scheme
//returns nothing
//ASSUMES : there is no blank lines in the input file
//If memory scheme is 0, ASSUMES no memory requirement for the processes
void readInputFile(process_t *processes, char *inputFile, int memory_scheme)
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
            else if (input_parameter == 5)
            {
                processes[process_position].initial_priority = int_token;
            }
            else if ( memory_scheme != 0 && input_parameter == 6)
            {
                processes[process_position].memory_needed = int_token;
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
        processes[process_position].effective_priority = processes[process_position].initial_priority;
        processes[process_position].partition_used = -1;

        input_parameter = 0; //Reset input parameter counter
        process_position++;  //Increment the process position counter to point to the next process in the array of processes
    }

    //closes the input file
    fclose(file);
}

//REFERENCES:-
//1. https://www.tutorialspoint.com/c_standard_library/
