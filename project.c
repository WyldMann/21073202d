#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h> 
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_USER_NAME_CHAR 20

bool haveTimeConflict(int startTimeA, float durationA, int startTimeB, float durationB){
    int durationHourA = (int) durationA;
    int durationHourB = (int) durationB;
    float durationMinA = 60 * (durationA - durationHourA);
    float durationMinB = 60 * (durationB - durationHourB);
    int endTimeA = durationHourA * 100 + durationMinA;
    int endTimeB = durationHourB * 100 + durationB;

}


struct User {
    int pid;
    char name[MAX_USER_NAME_CHAR];
};


/*
struct Communication {
    enum type {APPOINTMENT, PRINT, GET, DOTHIS } typeOfThisComm;
    Appointment* appointment;
}
*/

struct Appointment {
    enum Type {APPOINTMENT, PRINT} type;
    bool fail;
    char appointmentType[14];
    char caller[MAX_USER_NAME_CHAR];
    char callees[9][MAX_USER_NAME_CHAR];
    int date;
    int time;
    float duration;
};

int main(int argc, char *argv[]){
    // ====================================================
    // # Parent's variables
    // ====================================================
    // Initialize variables
    struct User user[10]; 
    struct Appointment childAppointment[100];
    int startDate, endDate;
    char command[14];
    int n;
    int userCounter, i, j, ctr;
    int fd1[2]; // First pepe to send input string from parent
    int fd2[2]; // Second pepe to send informations form child
    char buffer[200];
    char splitString[15][MAX_USER_NAME_CHAR];
    userCounter = argc - 3;
    startDate = atoi(argv[1]);
    endDate = atoi(argv[2]);

    // ====================================================
    // # Children's variables
    // ====================================================
    char userName[50];

    // ====================================================

    // Create pipe
    if(pipe(fd1) == -1){
        printf("Pipe Failed\n");
        exit(0);
    }
    if(pipe(fd2) == -1){
        printf("Pipe Failed\n");
        exit(0);
    }

    // Fork child for handling serval users
    for(i=0;i<userCounter;i++){
        // assign pid and name to each child process and turn the first letter to capital letter
        strcpy(userName , argv[i+3]);
        strcpy(user[i].name , userName);
        user[i].name[0] = toupper(user[i].name[0]);
        n = fork();    
        if (n == 0) {
            // user[i].pid = getpid();
            break;
        }
        else {
            user[i].pid = n;
        }
    }

    // Parent Process
    if (n > 0) {
        sleep(1);
        // Check the number of users
        if(userCounter < 3 || userCounter > 10){
            printf("Error Message: There are at least three users and the maximum number is ten.\n");
            exit(0);
        }

        // Display welcome message
        printf("~~WELCOME TO APO~~\n");

        // Get user input
        while(1){
            // Input methods
            j=0; ctr=0;
            printf("Please enter appointment:\n");
            fgets(buffer, sizeof(buffer), stdin);
            for(i=0;i<=(strlen(buffer));i++){
                if(buffer[i] == ' ' || buffer[i] == '\0'){
                    splitString[ctr++][j]='\0';
                    j=0;
                }
                else{
                    splitString[ctr][j++] = buffer[i];
                }
            }
            strcpy(command, splitString[0]);
            
            if(strcmp(command, "endProgram") == 0){
                printf("-> Bye!\n");
                wait(NULL);
                break;
            }
            else if(strcmp(command, "privateTime") == 0){
                // Pipe it to child process
                strcpy(childAppointment.appointmentType, command);
                strcpy(childAppointment.caller, splitString[1]);
                childAppointment.date = atoi(splitString[2]);
                childAppointment.time = atoi(splitString[3]);
                childAppointment.duration = atof(splitString[4]);
                for(i=0;i<userCounter;i++){
                    strcpy(childAppointment.callees[i], splitString[i+5]);
                }
                childAppointment->appointmentType = APPOINTMENT;
                write(fd1[1], &childAppointment, sizeof(struct Appointment));
                close(fd1[1]);
                printf("-> [Recorded]\n");
                
            }
            else if(strcmp(command, "projectMeeting") == 0){
                // Pipe it to child process
                strcpy(childAppointment.appointmentType, command);
                strcpy(childAppointment.caller, splitString[1]);
                childAppointment.date = atoi(splitString[2]);
                childAppointment.time = atoi(splitString[3]);
                childAppointment.duration = atof(splitString[4]);
                for(i=0;i<userCounter;i++){
                    strcpy(childAppointment.callees[i], splitString[i+5]);
                }
                childAppointment->appointmentType = APPOINTMENT;
                write(fd1[1], &childAppointment, sizeof(struct Appointment));
                close(fd1[1]);
                printf("-> [Recorded]\n");
            }
            else if(strcmp(command, "groupStudy") == 0){
                // Pipe it to child process
                strcpy(childAppointment.appointmentType, command);
                strcpy(childAppointment.caller, splitString[1]);
                childAppointment.date = atoi(splitString[2]);
                childAppointment.time = atoi(splitString[3]);
                childAppointment.duration = atof(splitString[4]);
                for(i=0;i<userCounter;i++){
                    strcpy(childAppointment.callees[i], splitString[i+5]);
                }
                childAppointment->appointmentType = APPOINTMENT;
                write(fd1[1], &childAppointment, sizeof(struct Appointment));
                close(fd1[1]);
                printf("-> [Recorded]\n");
            }
            else if(strcmp(command, "gathering") == 0){
                // Pipe it to child process
                strcpy(childAppointment.appointmentType, command);
                strcpy(childAppointment.caller, splitString[1]);
                childAppointment.date = atoi(splitString[2]);
                childAppointment.time = atoi(splitString[3]);
                childAppointment.duration = atof(splitString[4]);
                for(i=0;i<userCounter;i++){
                    strcpy(childAppointment.callees[i], splitString[i+5]);
                }
                childAppointment->appointmentType = APPOINTMENT;
                write(fd1[1], &childAppointment, sizeof(struct Appointment));
                close(fd1[1]);
                printf("-> [Recorded]\n");
            }
            else if(strcmp(command, "printSchd") == 0){
                // Pipe from child process
                printf("Pipe from child\n");
            }

        }

    }
    else {
        // Child Process
        // Pipe infomations from parent to child
        close(fd1[1]);
        int n;
        int calleesCounter;
        int appointmentSize = 0;
        struct Appointment appointments[100];
        // keep get the value from parent
        while(1){
            struct Appointment appointment;
            read(fd1[0],&appointment,sizeof(appointment));
            if (appointment.type == APPOINTMENT) { 
                // record appointment
                for(i=0;i<appointmentSize;i++){
                    // check time slot
                    
                    if()
                }
                memcpy(appointments[appointmentSize], appointment, sizeof(struct Appointment));
                appointmentSize++;
            }
            else {
                // print

            }
            
        close(fd1[0]);
        exit(0);
        }
    }
        
}
