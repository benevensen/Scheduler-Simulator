
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//ENUM FOR PROCESS STATES
typedef enum Process_State {
    NEW,
    READY, // process is ready to run
    RUNNING, // process is currently running
    WAITING // process is currently waiting on IO
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

struct Queue {
    Node_t* Head;
    Node_t* Tail;
    int size;
};

struct Queue ReadyQueue;

void initReadyQueue();

void initReadyQueue(){
    ReadyQueue.Head = NULL;
    ReadyQueue.Tail = NULL;
    ReadyQueue.size = 0;
}

void enqueue();

void enqueue(struct process* process){
    if (ReadyQueue.Tail == NULL)
    {
       /*  printf(" IN ENQUEUE FOR EMPTY QUEUE \n"); */
        //Node_t new_node;
       // Node_t *node = &new_node;
        Node_t *node = (Node_t*) malloc(sizeof(Node_t));

        node->process = process;
        node->Next = NULL;

        ReadyQueue.Head = (Node_t *) node;
        ReadyQueue.Tail = (Node_t *) node;
        ReadyQueue.size++;

       /*  printf("Size is currently %d \n", ReadyQueue.size);
        printf("Enqueued process id %d \n", node->process->pid);
        printf("Current tail is porcess %d \n", ReadyQueue.Tail->process->pid);
        printf("Current head is porcess %d \n", ReadyQueue.Head->process->pid); */
    }
    else if (ReadyQueue.Tail == NULL)
    {
        exit(-1);
    }
    else
    {

       /*  printf(" IN ENQUEUE FOR NONE EMPTY QUEUE \n"); */

        /* Node_t new_node;
        Node_t *node = &new_node; */
        Node_t *node = (Node_t*) malloc(sizeof(Node_t));


        node->process = process;
        node->Next = NULL;

        ReadyQueue.Tail->Next = (Node_t *) node;
        ReadyQueue.Tail = node;
        ReadyQueue.size++;
       /*  printf("Size is currently %d \n", ReadyQueue.size);
        printf("Enqueued process id %d \n", node->process->pid);
        printf("Current tail is porcess %d \n", ReadyQueue.Tail->process->pid);
        printf("Current head is porcess %d \n", ReadyQueue.Head->process->pid); */
    }
}

struct process dequeue();

struct process dequeue(){
    if( ReadyQueue.Head == NULL || ReadyQueue.size == 0){
       /*  printf(" IN DEQUEUE FOR EMPTY QUEUE \n"); */

        printf("Error nothing is in the queue, Returning an empty process");
        exit(-1);
    }else{

        //printf(" IN DEQUEUE FOR NONE EMPTY QUEUE \n");

        struct process frontProcess = *(ReadyQueue.Head->process);

       // printf("dequeued process id %d \n", frontProcess.pid);

        /* printf("old head process id %d \n", ReadyQueue.Head->process->pid); */

        Node_t * ptr = ReadyQueue.Head;

        if(ReadyQueue.Head != ReadyQueue.Tail){
            ReadyQueue.Head = ReadyQueue.Head->Next;
        }

        ReadyQueue.size--;

        

      /*   printf("new head process id %d \n", ReadyQueue.Head->process->pid); */

       /*  printf("Size is currently %d \n", ReadyQueue.size);
        printf("dequeued process id %d \n", frontProcess.pid);
        printf("Current tail is porcess %d \n", ReadyQueue.Tail->process->pid);
        printf("Current head is porcess %d \n", ReadyQueue.Head->process->pid); */
        
        free(ptr);

        return frontProcess;
    }
}


// ************************************************************

struct process* input_file;
int processCtr = 0; //How many process there are

int clock = 0; //current clock of the system

FILE* outputFIle;

void scheduler(); //Schedules jobs, first in - first out
void readFile(); //To parse input file
void writeFile(); //To write our output

void outputFileInit();
void outputFileCleanUp();
void printTransition();

const char * getStringFromState(States state);
void print_process_details(struct process process);

int main(int argc, char* argv[]) {


    //readFile();

  /*   outputFileInit();
    printTransition();
    outputFileCleanUp(); */

    //testing my queue implementation

    initReadyQueue();

    struct process process[5];

    for(int i = 0 ; i<5 ; i++){
        process[i].pid = i;
        process[i].arrival_time = 5;
        process[i].IO_frequency = 10;
        process[i].total_CPU_time = 20;
        process[i].IO_duration = 1;
        process[i].state = READY;

        enqueue(&process[i]);
    }

    Node_t * pt = ReadyQueue.Head;
    printf("Current head node is process with id %d \n", pt->process->pid);

    Node_t * pt2 = ReadyQueue.Tail;
    printf("Current tail node is process with id %d \n", pt2->process->pid);

    /* while(pt != NULL ){
        printf("Current node is process with id %d\n", pt->process->pid);
        pt = pt->Next;
    } */

    for (int i = 0; i < 5; i++)
    {
        struct process process = dequeue();
        printf("Dequeueing Round %d \n", i);
        print_process_details(process);
    }
    

   // writeFile();
    return 0;
    //some code...
}

void outputFileInit(){

outputFIle = fopen("output.txt", "w"); // opens file in current working directory
    if(outputFIle == NULL){ //Check for file
        perror("Could not open file.");
        exit(1);
    }
}

void outputFileCleanUp(){
    fclose(outputFIle);
}

void printTransition(){

    //for testing purposes
    for(int i = 0; i < 4; i++){
        fprintf(outputFIle, " %d %d %s %s \n", clock, 12, getStringFromState( (States) READY), getStringFromState((States) RUNNING));
    }

};

/* 
 FUNCTION THAT COUNTS THE NUMBER OF process IN INPUT FILE 
 AND MAKES AN ARRAY OF PROCESS STRUCTS SIZED TO THE COUNT
 */
void numberOfprocess(){
    processCtr = 0;

    FILE* file = fopen("tester.txt", "r"); // opens file in current working directory
    if(file == NULL){ //Check for file
        perror("Could not open file.");
        exit(1);
    }

    char input_line[100];
    while (fgets(input_line, sizeof (input_line), file)){
        processCtr++; //Increment processCtr
        printf("Current process counter is %d \n", processCtr);
    }

    input_file = (struct process *) malloc( sizeof(struct process) * processCtr); //Allocate memory for all process

    fclose(file);
}

//FUNCTION TO GET STRING FROM ENUM
const char * getStringFromState(States state){
    //array of strings corresponding to the Process State enum 
    static const char *states[] = {"READY", "RUNNING", "WAITING"};

    //returns string of the state enum
    return states[state];
}

//FUNCTION TO PRINT DETAILS OF A PROCESS
void print_process_details(struct process process){

    //prints the state of a process for debugging
    printf("Pid: %d \t, arrival_time: %d \t, total_CPU_time: %d \t, IO_frequency: %d \t, IO_duration: %d \t, state: %s \t \n", \
            process.pid, process.arrival_time, process.total_CPU_time, process.IO_frequency, process.IO_duration, getStringFromState(process.state));
}

void writeFile(){
    printf("Transition time:-\t PID:-\t Old State:-\t New State:-");
    for (int i = 0; i < 3; i++){
        printf("\n %i\t %i\t %i", input_file[i].pid, input_file[i].arrival_time, input_file[i].total_CPU_time);
    }
    //some code...
}

//FUNCTION TO PARSE OUR .TXT FILES
void readFile(){
    char *str; //To store the text contained in each line
    const char truncate[2] = " "; //In-line separator
    char *token; //To store the token for each line
    int output_parameter = 0; //To traverse through the different output parameters (i.e., execution time, etc.) Set to zero for first word
    int word_position = 0; //To traverse through the words in each line

    numberOfprocess();

    FILE* file = fopen("tester.txt", "r"); //Open input file in current working directory, and read, "r", it
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

            if (output_parameter == 0){
                input_file[word_position].pid = int_token;
            }
            else if (output_parameter == 1){
                input_file[word_position].arrival_time = int_token;
            }
            else if (output_parameter == 2){
                input_file[word_position].total_CPU_time = int_token;
            }
             else if (output_parameter == 3){
                input_file[word_position].IO_frequency = int_token;
            }
             else if (output_parameter == 4){
                input_file[word_position].IO_duration = int_token;
            } 
            else if (output_parameter == 5){
                input_file[word_position].state = NEW;
            }
            

//          NOTE:- PRIORITY NOT NEEDED
//            else if (output_parameter == 3){
//                input_file[word_position].some_priority_method = int_token;
//            }

            token = strtok(NULL, truncate); //Reset token
            output_parameter++; //Increment output parameter counter

        }

     //   printf("number of process %ld\n",sizeof(input_file)/sizeof(input_file[0])); doesnt work because its a pointer with a struct
        print_process_details(input_file[word_position]);
        output_parameter = 0; //Reset output parameter ctr
        word_position++; //Increment word position

    }

    fclose(file);

    //some code...
}







//REFERENCES:-
//1. https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
//2. https://www.tutorialspoint.com/c_standard_library/c_function_atoi.htm
//3.
