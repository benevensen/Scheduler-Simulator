
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//DATA STRUCTURE IN MEMORY
struct processes {
    int PID; //Process ID
    int arrival_time; //Time of arrival
    int total_CPU_time; //Total execution time
    int IO_frequency; //How frequently IO is accessed
    int IO_duration; //How long IO is accessed
};

struct processes *input_file;

void scheduler(); //Schedules jobs, first in - first out
void readFile(); //To parse input file
void writeFile(); //To write our output

int main() {
    //some code...
}


//FUNCTION TO SCHEDULE OUR JOBS (FIFO)
void scheduler(){
    //some code...
}

//FUNCTION TO PARSE OUR .TXT FILES
void readFile(){
    char *str; //To store the text contained in each line
    const char truncate[2] = " "; //In-line separator
    char *token; //To store the token for each line
    int ctr = 0;

    FILE *file = fopen("tester.txt", "r"); //Open input file, and read, "r", it


    //some code...
}

void writeFile(){
    printf("Transition time:-\t PID:-\t Old State:-\t New State:-");

    //some code...
}



//REFERENCES:-
//1. https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
//2.
