
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//DATA STRUCTURE IN MEMORY
struct processes {
    int pid; //Process ID
    int arrival_time; //Time of arrival
    int total_CPU_time; //Total execution time
    int IO_frequency; //How frequently IO is accessed
    int IO_duration; //How long IO is accessed
};

struct processes *input_file;
int processCtr = 0; //How many processes there are

void scheduler(); //Schedules jobs, first in - first out
void readFile(); //To parse input file
void writeFile(); //To write our output

int main() {
    readFile();
    //some code...
}


//FUNCTION TO SCHEDULE OUR JOBS (FIFO)
void scheduler(){
    //some code...
}

void numberOfProcesses(){
    processCtr = 0;

    FILE* file = fopen("C:\Users\benni\CLionProjects\SYSC4001-Assignment1.2\tester.txt");
    if(file == NULL){ //Check for file
        perror("Could not open file.");
        exit(1);
    }

    char input_line[100];
    while (fgets(input_line, sizeof (input_line), file)){
        processCtr++; //Increment processCtr
    }

    input_file = (struct processes *)malloc(processCtr); //Allocate memory for size of processCtr

    fclose(file);
}

//FUNCTION TO PARSE OUR .TXT FILES
void readFile(){
    char *str; //To store the text contained in each line
    const char truncate[2] = " "; //In-line separator
    char *token; //To store the token for each line
    int output_parameter = 0; //To traverse through the different output parameters (i.e., execution time, etc.)
    int word_position = 0; //To traverse through the words in each line

    FILE* file = fopen("C:\\Users\\benni\\CLionProjects\\SYSC4001-Assignment1.2\\tester.txt", "r"); //Open input file, and read, "r", it
    if(file == NULL){
        perror("Could not open file.");
        exit(1);
    }

    char input_line[100];
    while (fgets(input_line, sizeof(input_line), file)){
        str = input_line; //Point to our line
        token = strtok(str, truncate); //Break input into a series of tokens

        while (token != NULL){
            int int_token = atoi(token); //Converts string to integer representation

            if (output_parameter == 0){
                input_line[word_position].PID = int_token;
            }
        }

    }


    //some code...
}

void writeFile(){
    printf("Transition time:-\t PID:-\t Old State:-\t New State:-");

    //some code...
}



//REFERENCES:-
//1. https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
//2. https://www.tutorialspoint.com/c_standard_library/c_function_atoi.htm
//3.
