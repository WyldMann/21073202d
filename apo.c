#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h> 
#include <sys/types.h>
#include <sys/wait.h>

struct User{
    int pid;
    char name[20];
};

struct Appointment {
    char appointmentType[14];
    char caller[20];
    char callees[10];      // split string into buffer and save them in this
    int date;
    int time;
    float duration;
};

int main(int argc, char *argv[]){
    // Initialize variables
    struct User user[10]; 
    struct Appointment childAppointment;
    int startDate, endDate;
    char command[14];
    int n;
    int userCounter, i, j, ctr;
    int fd1[2]; // First pepe to send input string from parent
    int fd2[2]; // Second pepe to send informations form child
    char buffer[200];
    char splitString[15][20];

    // Define variables
    userCounter = argc - 3;
    startDate = atoi(argv[1]);
    endDate = atoi(argv[2]);

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
        n = fork();
        // assign pid and name to each child process and turn the first letter to capital letter
        if(n == 0){
            strcpy(user[i].name , argv[i+3]);
            user[i].name[0] = toupper(user[i].name[0]);
            user[i].pid = getpid();

            /*
            Debug seesion
            printf("user: %s \n", user[i].name); 
            printf("userId: %d, userPid: %d \n",user[i].pid,test);
            */
            
            break;
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
            /* Debug session
            for(i=0;i<sizeof(callees);i++){
                if(callees[i] == NULL){
                    break;
                }
                printf("Debug session: %s \n",callees[i]);
            } */
            
            if(strcmp(command, "endProgram") == 0){
                printf("-> Bye!\n");
                wait(NULL);
                break;
            }
            else if(strcmp(command, "privateTime") == 0){
                // Pipe it to child process
                close(fd1[0]);
                write(fd1[1], &childAppointment, sizeof(struct Appointment));
                close(fd1[1]);
            }
            else if(strcmp(command, "projectMeeting") == 0){
                // Pipe it to child process                
            }
            else if(strcmp(command, "groupStudy") == 0){
                // Pipe it to child process
            }
            else if(strcmp(command, "gathering") == 0){
                // Pipe it to child process
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
        // keep get the value from parent
        /*while(n = read(fd1[0],&childAppointment,sizeof(childAppointment)) > 0){
            for(int i=0;i<userCounter;i++){
                if(childAppointment.caller == user[i].name || childAppointment.callees == user[i].name){
                    // dosomethings
                }
            }*/
            
        // Test is it the data transfer correctly
        struct Appointment appointment;
        while(n = read(fd1[0],&appointment,sizeof(childAppointment)) > 0){
            for(i=0;i<userCounter;i++){
                if(strcmp(appointment.caller, user[i].name)){
                    printf("meetingType: %s\ncaller: %s\ndate: %d\ntime: %d\nduration: %f\n",appointment.appointmentType,appointment.caller,appointment.date,appointment.time,appointment.duration);
                    break;
                }    
            }
        }
        close(fd1[0]);
        exit(0);
    }
        
}
