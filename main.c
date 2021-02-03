
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//ENUM FOR PROCESS STATES
typedef enum Process_State {
    NEW,
    READY, // process is ready to run
    RUNNING, // process is currently running
    WAITING, // process is currently waiting on IO
    TERMINATED
} States;

//DATA STRUCTURE IN MEMORY
struct process{
    int pid; //Process ID
    int arrival_time; //Time of arrival
    int total_CPU_time; //Total execution time
    int IO_frequency; //How frequently IO is accessed
    int IO_duration; //How long IO is accessed
    States state; //Current state of a process
};


// ************************************************************

//Implementation for Queue

typedef struct Node
{
    struct Node* Next;
    struct process* process; 

}Node_t;

typedef struct Queue {
    Node_t* Head;
    Node_t* Tail;
    int size;
}Queue_t;

struct Queue ReadyQueue;

Queue_t* initReadyQueue();

void cleanReadyQueue();

Queue_t* initReadyQueue(){

    Queue_t * ReadyQueue = (Queue_t *) malloc(sizeof(Queue_t));

    ReadyQueue->Head = NULL;
    ReadyQueue->Tail = NULL;
    ReadyQueue->size = 0;

    return ReadyQueue;
}

void cleanReadyQueue(Queue_t* ReadyQueue){
    free(ReadyQueue);
}

void enqueue(Queue_t* ReadyQueue, struct process* process);

void enqueue(Queue_t* ReadyQueue, struct process* process){
    if (ReadyQueue->Tail == NULL)
    {
        printf(" IN ENQUEUE FOR EMPTY QUEUE \n");
        //Node_t new_node;
       // Node_t *node = &new_node;
        Node_t *node = (Node_t*) malloc(sizeof(Node_t));

        node->process = process;
        node->Next = NULL;

        ReadyQueue->Head = (Node_t *) node;
        ReadyQueue->Tail = (Node_t *) node;
        ReadyQueue->size++;

       /*  printf("Size is currently %d \n", ReadyQueue.size);
        printf("Enqueued process id %d \n", node->process->pid);
        printf("Current tail is porcess %d \n", ReadyQueue.Tail->process->pid);
        printf("Current head is porcess %d \n", ReadyQueue.Head->process->pid); */
    }
    else if (ReadyQueue->Tail == NULL)
    {
        exit(-1);
    }
    else
    {

        printf(" IN ENQUEUE FOR NONE EMPTY QUEUE \n");

        /* Node_t new_node;
        Node_t *node = &new_node; */
        Node_t *node = (Node_t*) malloc(sizeof(Node_t));


        node->process = process;
        node->Next = NULL;

        ReadyQueue->Tail->Next = (Node_t *) node;
        ReadyQueue->Tail = node;
        ReadyQueue->size++;
       /*  printf("Size is currently %d \n", ReadyQueue.size);
        printf("Enqueued process id %d \n", node->process->pid);
        printf("Current tail is porcess %d \n", ReadyQueue.Tail->process->pid);
        printf("Current head is porcess %d \n", ReadyQueue.Head->process->pid); */
    }
}

struct process* dequeue(Queue_t* ReadyQueue);

struct process* dequeue(Queue_t* ReadyQueue){
    if( ReadyQueue->Head == NULL || ReadyQueue->size == 0){
       /*  printf(" IN DEQUEUE FOR EMPTY QUEUE \n"); */

        printf("Error nothing is in the queue, Returning an empty process");
        exit(-1);
    }else{

        printf(" IN DEQUEUE FOR NONE EMPTY QUEUE \n");

       /*  struct process frontProcess = *(ReadyQueue->Head->process); */

        struct process* frontProcess = ReadyQueue->Head->process;

       // printf("dequeued process id %d \n", frontProcess.pid);

        /* printf("old head process id %d \n", ReadyQueue.Head->process->pid); */

        Node_t * ptr = ReadyQueue->Head;

        if(ReadyQueue->Head != ReadyQueue->Tail){
            ReadyQueue->Head = ReadyQueue->Head->Next;

            //Empty Queue
        }else{
            ReadyQueue->Head = NULL;
            ReadyQueue->Tail = NULL;
        }

        ReadyQueue->size--;

        

      /*   printf("new head process id %d \n", ReadyQueue.Head->process->pid); */

       /*  printf("Size is currently %d \n", ReadyQueue.size);
        printf("dequeued process id %d \n", frontProcess.pid);
        printf("Current tail is porcess %d \n", ReadyQueue.Tail->process->pid);
        printf("Current head is porcess %d \n", ReadyQueue.Head->process->pid); */
        
        free(ptr);

        return frontProcess;
    }
}

int getQueueSize(Queue_t* ReadyQueue);

int getQueueSize(Queue_t* ReadyQueue){
    return ReadyQueue->size;
}


// ************************************************************

/* struct process* input_file;
int processCtr = 0; //How many process there are

int clock = 0; //current clock of the system
 */

void scheduler(); //Schedules jobs, first in - first out
void readInputFile(struct process* processes); //To parse input file
void writeFile(); //To write our output

FILE* outputFileInit();
void outputFileCleanUp();
void printTransition();

const char * getStringFromState(States state);
void print_process_details(struct process process);
void printTransition(FILE* outputFile, int clock, struct process process, States prevState);
bool isDone(struct process* processes, int processCount);

int countNumberOfProcesses();
void cleanOutputFile(FILE* outputFile);

int main(int argc, char* argv[]) {


    //readFile();

  /*   outputFileInit();
    printTransition();
    outputFileCleanUp(); */

    //testing my queue implementation

    Queue_t* ReadyQueue = initReadyQueue();

    int numberOfProcesses = countNumberOfProcesses();

    printf("number of processes is %d\n", numberOfProcesses);
    
    struct process* processes = (struct process *) malloc( sizeof(struct process) * numberOfProcesses); //Allocate memory for all process
    readInputFile(processes);

    int clock = 0;
    int IO_Duration = processes[0].IO_duration;  // Due to the assumption that processes will have the same duration

    printf("IO duration is %d\n", IO_Duration);

    //**********************************

    FILE* outputFile = outputFileInit();

    //running process swap needs to include setting the IO frequency and the making sure the next switch happens at the next iteration of the while loop

    int RunningProcess_IO_Frequency = -1;
    int RunningProcess_ID = -1;
    int time_until_IO;
    bool canSwitch = false;

    int count = 0;

    while (!isDone(processes, numberOfProcesses))
    {
        printf("count is %d \n", count++);
        getchar();
        if( getQueueSize(ReadyQueue) > 0 &&  RunningProcess_ID == -1){
            struct process* process = dequeue(ReadyQueue);

            RunningProcess_ID = process->pid;
            time_until_IO = process->IO_frequency;

            States prevState = process->state;

            process->state = RUNNING;

            printTransition(outputFile, clock, *process, prevState);
        }

        for (int i = 0; i < numberOfProcesses; i++)
        {
            struct process process = processes[i];

            print_process_details(processes[i]);

            if (processes[i].state == NEW)
            {

                printf("clock is %d and process arrival time is %d\n", clock, processes[i].arrival_time);

                if (processes[i].arrival_time == clock)
                {
                    States prevState = processes[i].state;

                    processes[i].state = READY;
                    enqueue(ReadyQueue, &processes[i]);

                    printTransition(outputFile, clock, processes[i], prevState);
                }
            }
            else if (processes[i].state == RUNNING)
            {

                printf("time_until_IO is %d \n", time_until_IO);

                processes[i].total_CPU_time--;

                if(processes[i].total_CPU_time == 0){
                    States prevState = processes[i].state;

                    processes[i].state = TERMINATED;
                    RunningProcess_ID = -1;

                    printTransition(outputFile, clock, processes[i], prevState);
                }else if(time_until_IO == 0)
                {
                    States prevState = processes[i].state;

                    processes[i].state = WAITING;
                     RunningProcess_ID = -1;

                    printTransition(outputFile, clock, processes[i], prevState);
                }
                else
                {
                    time_until_IO--;
                }
                
            }
            else if (processes[i].state == WAITING)
            {

                if (processes[i].IO_duration == 0)
                {
                    States prevState = processes[i].state;

                    processes[i].state = READY;
                    enqueue(ReadyQueue, &processes[i]);

                    printTransition(outputFile, clock, processes[i], prevState);
                }
                else
                {
                    processes[i].IO_duration--;
                }
            }
            // mechanism for switching the current running process is out of the for loop
            else if (processes[i].state == READY)
            {
                print_process_details(processes[i]);
                continue;
               /*  //timing the switch
                if (canSwitch && RunningProcess_ID == -1)
                {
                    States prevState = processes[i].state;

                    process.state = READY;
                    enqueue(ReadyQueue, &processes[i]);

                    printTransition(outputFile, clock, processes[i], prevState);
                } */

            }
            //
            else if (processes[i].state == TERMINATED)
            {

                // Nothing to do with a terminated process
                continue;
            }
            else
            {
                //print_process_details(processes[i]);
                printf("ERROR: Process state is unknown!\n");
                exit(-1);
            }
        }

        clock++;
    }

    /* 
    BLOCK USED TO TEST QUEUE

    struct process process[5];
    for(int i = 0 ; i<5 ; i++){
        process[i].pid = i;
        process[i].arrival_time = 5;
        process[i].IO_frequency = 10;
        process[i].total_CPU_time = 20;
        process[i].IO_duration = 1;
        process[i].state = READY;

        enqueue(ReadyQueue,&process[i]);
    }

    Node_t * pt = ReadyQueue->Head;
    printf("Current head node is process with id %d \n", pt->process->pid);

    Node_t * pt2 = ReadyQueue->Tail;
    printf("Current tail node is process with id %d \n", pt2->process->pid);

    /* while(pt != NULL ){
        printf("Current node is process with id %d\n", pt->process->pid);
        pt = pt->Next;
    }

    for (int i = 0; i < 5; i++)
    {
        struct process process = dequeue(ReadyQueue);
        printf("Dequeueing Round %d \n", i);
        print_process_details(process);
    }
    //input_file = (struct process *) malloc( sizeof(struct process) * processCtr); //Allocate memory for all process
    */

    //Calls functions to clean up any dynamically allocated resources.
    cleanOutputFile(outputFile);
    cleanReadyQueue(ReadyQueue);
    free(processes);
    return 0;
    //some code...
}

FILE* outputFileInit(){

    FILE* outputFIle = fopen("output.txt", "w"); // opens file in current working directory
    if(outputFIle == NULL){ //Check for file
        perror("Could not open file.");
        exit(1);
    }

    return outputFIle;
}

void cleanOutputFile(FILE* outputFile){
    fclose(outputFile);
}

void printTransition(FILE* outputFile, int clock, struct process process, States prevState){

    //for testing purposes
        fprintf(outputFile, " %d %d %s %s \n", clock, process.pid,  getStringFromState((States) prevState) ,getStringFromState( (States) process.state));

};

/* 
 FUNCTION THAT COUNTS THE NUMBER OF process IN INPUT FILE 
 AND MAKES AN ARRAY OF PROCESS STRUCTS SIZED TO THE COUNT
 */
int countNumberOfProcesses(){
    int processCtr = 0;

    FILE* file = fopen("input.txt", "r"); // opens file in current working directory
    
    if(file == NULL){ //Check for file
        perror("Could not open file.");
        exit(1);
    }

    char input_line[100];

    while (fgets(input_line, sizeof (input_line), file)){
        processCtr++; //Increment processCtr
       /*  printf("Current process counter is %d \n", processCtr); */
    }

    //input_file = (struct process *) malloc( sizeof(struct process) * processCtr); //Allocate memory for all process

    fclose(file);

    return processCtr;
}

bool isDone(struct process* processes, int processCount){
    for(int i = 0; i < processCount; i++){
        if(processes[i].state != TERMINATED){
            return false;
        }
    }

    return true;
}


//FUNCTION TO GET STRING FROM ENUM
const char * getStringFromState(States state){
    //array of strings corresponding to the Process State enum 
    static const char *states[] = {"NEW","READY", "RUNNING", "WAITING","TERMINATED"};

    //returns string of the state enum
    return states[state];
}

//FUNCTION TO PRINT DETAILS OF A PROCESS
void print_process_details(struct process process){

    //prints the state of a process for debugging
    printf("Pid: %d \t, arrival_time: %d \t, total_CPU_time: %d \t, IO_frequency: %d \t, IO_duration: %d \t, state: %s \t \n", \
            process.pid, process.arrival_time, process.total_CPU_time, process.IO_frequency, process.IO_duration, getStringFromState(process.state));
}

/* 
void writeFile(){
    printf("Transition time:-\t PID:-\t Old State:-\t New State:-");
    for (int i = 0; i < 3; i++){
        printf("\n %i\t %i\t %i", input_file[i].pid, input_file[i].arrival_time, input_file[i].total_CPU_time);
    }
    //some code...
} */

//FUNCTION TO PARSE OUR .TXT FILES
void readInputFile(struct process* processes){
    char *str; //To store the text contained in each line
    const char truncate[2] = " "; //In-line separator
    char *token; //To store the token for each line
    int input_parameter = 0; //To traverse through the different output parameters (i.e., execution time, etc.) Set to zero for first word
    int process_position = 0; //To traverse through the words in each line

    /* numberOfprocess(); */

    FILE* file = fopen("input.txt", "r"); //Open input file in current working directory, and read, "r", it

    if(file == NULL){
        perror("Could not open file.");
        exit(1);
    }

    //printf("Opened file!\n");

    char input_line[100];
    while (fgets(input_line, sizeof(input_line), file)){
        str = input_line; //Point to our line

        printf("%s \n",str);
        token = strtok(str, truncate); //Break input into a series of tokens

        while (token != NULL){
            int int_token = atoi(token); //Converts string to integer representation

            if (input_parameter == 0){
                processes[process_position].pid = int_token;
            }
            else if (input_parameter == 1){
                processes[process_position].arrival_time = int_token;
            }
            else if (input_parameter == 2){
                processes[process_position].total_CPU_time = int_token;
            }
             else if (input_parameter == 3){
                processes[process_position].IO_frequency = int_token;
            }
             else if (input_parameter == 4){
                processes[process_position].IO_duration = int_token;
            } 
            else if (input_parameter == 5){
                processes[process_position].state = NEW;
            }
            

//          NOTE:- PRIORITY NOT NEEDED
//            else if (output_parameter == 3){
//                input_file[word_position].some_priority_method = int_token;
//            }

            token = strtok(NULL, truncate); //Reset token
            input_parameter++; //Increment output parameter counter

        }

     //   printf("number of process %ld\n",sizeof(input_file)/sizeof(input_file[0])); doesnt work because its a pointer with a struct
        print_process_details(processes[process_position]);
        input_parameter = 0; //Reset output parameter ctr
        process_position++; //Increment word position

    }

    fclose(file);

    //some code...
}







//REFERENCES:-
//1. https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
//2. https://www.tutorialspoint.com/c_standard_library/c_function_atoi.htm
//3.
