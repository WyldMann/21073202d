#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_USER_NAME_CHAR 20

struct User {
    int pid;
    char name[MAX_USER_NAME_CHAR];
};


struct Appointment {
    enum Type {APPOINTMENT, CHECK, REJECTED, PRINT} type;   // type PRINT is used for communication (not an appointment)
    bool isFail;
    char appointmentType[14];
    char caller[MAX_USER_NAME_CHAR];
    char callees[9][MAX_USER_NAME_CHAR];
    int date;
    int time;
    float duration;
    int numberOfCallees;
};

bool haveTimeConflict(int dateA, int dateB ,int startTimeA, float durationA, int startTimeB, float durationB){
    int durationHourA = (int) durationA;
    int durationHourB = (int) durationB;
    float durationMinA = 60 * (durationA - durationHourA);
    float durationMinB = 60 * (durationB - durationHourB);
    int endTimeA = durationHourA * 100 + durationMinA;
    int endTimeB = durationHourB * 100 + durationB;
    if(dateA != dateB){
        return false;
    }
    else if((endTimeA >= startTimeB && endTimeA <= endTimeB) || (startTimeA >= startTimeB && startTimeA <= endTimeB )){
        return true;
    }
    else{
        return false;
    }
}

struct Appointment createAppointment(char userInput[15][20], int userCounter){
    struct Appointment newAppoinment;
    int i;
    int numberOfCallees = 0;
    strcpy(newAppoinment.appointmentType, userInput[0]);
    strcpy(newAppoinment.caller, userInput[1]);
    newAppoinment.date = atoi(userInput[2]);
    newAppoinment.time = atoi(userInput[3]);
    newAppoinment.duration = atof(userInput[4]);
    newAppoinment.type = CHECK;
    numberOfCallees = arrLength(userInput) - 5;
    if(numberOfCallees != 0){
        for(i=0;i<numberOfCallees;i++){
            strcpy(newAppoinment.callees[i], userInput[i+5]);
        }
    }
    newAppoinment.numberOfCallees = numberOfCallees;
    return newAppoinment;
}

int searchUser(char* userName, int userCounter, struct User user[]) {
    int i;
    for(i=0;i<userCounter;i++){
        if(user[i].name == userName){
            return i;
        }
    }
    return 0;
}

int arrLength(char arr[][MAX_USER_NAME_CHAR]){
    int i;
    int count = 0;
    for(i=0;arr[i][0]!='\0';i++){
        count++;
    }
    return count;
}

void printAppointment(int printCount,char scheduling[],int startDate,int endDate, struct Appointment appointment){
    FILE *fp;
    fp = fopen(("Ggg_%d_%s.txt",printCount,scheduling),"w");
    fprintf(fp, "Period: %d to %d\n",startDate,endDate);
    fprintf(fp, "Algorithm used: %s\n",scheduling);
    fprintf(fp, "***Appointment Schedule***\n\n");
    fprintf(fp, "")

}

int main(int argc, char *argv[]){
    // ====================================================
    // # Parent's variables
    // ====================================================
    // Initialize variables
    struct User user[10]; 
    struct Appointment childAppointment[10][100];
    int startDate, endDate;
    char command[14];
    int n;
    int userCounter, i, j, ctr;
    int userLocation;
    int fd1[10][2]; // parent to child (with channels)
    int fd2[2]; // child to parent
    char buffer[200];
    char splitString[15][MAX_USER_NAME_CHAR];
    userCounter = argc - 3;
    startDate = atoi(argv[1]);
    endDate = atoi(argv[2]);
    char scheduling[8];
    int printCount;

    // ====================================================
    // # Children's variables
    // ====================================================
    char userName[MAX_USER_NAME_CHAR];

    // ====================================================

    // Create pipe
    for(i=0;i<userCounter;i++){
        if(pipe(fd1[i]) == -1){
            printf("pipe failed\n");
        }
    }

    // Fork child for handling serval users
    for(i=0;i<userCounter;i++){
        // assign pid and name to each child process and turn the first letter to capital letter
        strcpy(userName , argv[i+3]);
        strcpy(user[i].name , userName);
        user[i].name[0] = toupper(user[i].name[0]);
        n = fork();    
        if (n == 0) {
            break;
        }
        else {
            user[i].pid = i;
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

        // Display welcome message and inquires input method
        printf("~~WELCOME TO APO~~\n");
        printf("Please enter input method (0 = Manual Commands, 1 = BAT File): ");
        char inputmethod = 'x';
        scanf("%c", &inputmethod);

        while (inputmethod != '0' && inputmethod != '1') {
            printf("Input invalid\nPlease reenter input method (0 = Manual Commands, 1 = BAT File): ");
            scanf("%c", &inputmethod);
        }

        // file opening for BAT method
        FILE *fptr;
        if (inputmethod == '1') {
            printf("Enter file name: ");
            char fname[20];
            scanf("%s",fname);

            while ((fptr = fopen(fname,"r")) == NULL) {
                printf("File not found \n");    
                printf("Re-enter file name: ");
                char *fname;
                scanf("%s",fname);
            }
        }

        // file opening to write log files
        FILE *log;
        if ((log = fopen("log.txt","w")) == NULL) printf("Error initiating log file");

        // Get user input
        while(1){
            // Input methods
            struct Appointment appointment;
            struct Appointment appointmentCheckList[10];
            bool isReject;
            j=0; ctr=0;
            
            // fetching the input, either from file or stdin
            if (inputmethod == '1') {
                fgets(buffer,sizeof(buffer),fptr);
            }
            else {
                printf("Please enter appointment:\n");
                fgets(buffer, sizeof(buffer), stdin);
            }
            
            // record input into log.txt
            fputs(buffer,log);
            
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
                // command endProgram 
                printf("-> Bye!\n");
                wait(NULL);
                break;
            }
            else if(strcmp(command, "printSchd") == 0){
                // command printSchd
                strcpy(scheduling,splitString[1]);
                appointment.appointmentType = PRINT;
                for(i=0;i<userCounter;i++){
                    // send to all user
                    write(fd1[i][1],&appointment,sizeof(struct Appointment));
                }
                for(i=0;i<userCounter;i++){
                    // read all user appointment
                    read(fd2[0],&childAppointment,sizeof(struct Appointment));
                }
                
                printAppointment();
                printCount++;
            }
            else if(strcmp(command, "privateTime") != 0){
                // command except privateTime
                // Pipe it to child process
                appointment = createAppointment(splitString, userCounter);
                for(i=0;i<appointment.numberOfCallees;i++){
                    userLocation = searchUser(appointment.callees[i],userCounter,user);
                    write(fd1[userLocation][1], &appointment, sizeof(struct Appointment));
                }
                userLocation = searchUser(appointment.caller,userCounter,user);
                write(fd1[userLocation][1], &appointment, sizeof(struct Appointment));
                printf("-> [Recorded]\n");
                
            }
            else if(strcmp(command, "privateTime") == 0){
                // command privateTime
                // Pipe it to child process
                appointment = createAppointment(splitString, userCounter);
                userLocation = searchUser(appointment.caller,userCounter,user);
                write(fd1[userLocation][1], &appointment, sizeof(struct Appointment));
                printf("-> [Recorded]\n");
            }

            // check all user appointment wheater it isFail is true
            close(fd2[1]);
            isReject = false;
            for(i=0;i<appointment.numberOfCallees+1;i++){
                read(fd2[0], &appointment, sizeof(struct Appointment));
                appointmentCheckList[i] = appointment;
                if(appointmentCheckList[i].isFail == true){
                    // if there is one child can't join the meet
                    // meeting rejected
                    isReject = true;
                }
            }
            for(i=0;i<appointment.numberOfCallees+1;i++){
                if(isReject == true){
                    appointmentCheckList[i].type = REJECTED;
                    userLocation = searchUser(appointmentCheckList[0].callees[i],userCounter,user);
                    write(fd1[userLocation][1],&appointmentCheckList[i],sizeof(struct Appointment));
                }
                else{
                    appointmentCheckList[i].type = APPOINTMENT;
                    userLocation = searchUser(appointmentCheckList[0].callees[i],userCounter,user);
                    write(fd1[userLocation][1],&appointmentCheckList[i],sizeof(struct Appointment));
                }
            }
        }
        
        // close files
        if (inputmethod=='1') fclose(fptr);
        fclose(log);
        

    }
    else {
        // Child Process
        // Pipe infomations from parent to child
        int location;
        int calleesCounter;
        int appointmentSize = 0;
        int appointmentFailedCounter = 0;
        struct Appointment confirmedAppointments[100];
        struct Appointment failedAppointments[100];
        // keep get the value from parent
        while(1){
            struct Appointment appointment;
            location = searchUser(userName,userCounter,user);
            read(fd1[location][0],&appointment,sizeof(struct Appointment));    // read own channel
            // check for their own availability 
            if(appointment.type == PRINT) {
                write(fd2[1], &confirmedAppointments, sizeof(struct Appointment));
            }
            if(appointment.type == CHECK){
                if(appointmentSize == 0){
                    appointment.isFail = false;
                    write(fd2[1], &appointment, sizeof(struct Appointment));
                }
                else{
                    for(i=0;i<appointmentSize;i++){
                        if(haveTimeConflict(appointment.date,confirmedAppointments[i].date,appointment.time,appointment.duration,confirmedAppointments[i].time,confirmedAppointments[i].duration)){
                            appointment.isFail = true;
                            write(fd2[1],&appointment,sizeof(struct Appointment));
                            break;
                        }
                        else{
                            appointment.isFail = false;
                        }
                    }
                    if(appointment.isFail == false){
                        write(fd2[1],&appointment,sizeof(struct Appointment));
                    }
                }
                // read again from parent
                read(fd1[location][0],&appointment,sizeof(struct Appointment));
            }
            // Add to the appointment
            if(appointment.numberOfCallees != 0){        // callees are exist(loop is needed)
                for(i=0;i<appointment.numberOfCallees;i++){
                    if(appointment.type == REJECTED){
                        failedAppointments[appointmentFailedCounter] = appointment;
                        appointmentFailedCounter++;
                    }
                    else if(appointment.type == APPOINTMENT){
                        confirmedAppointments[appointmentSize] = appointment;
                        appointmentSize++;
                    }
                }
            }
            else if(appointment.numberOfCallees == 0) {     // caller only
                if(appointment.type == REJECTED){
                    failedAppointments[appointmentFailedCounter] = appointment;
                    appointmentFailedCounter++;
                }
                else if(appointment.type == APPOINTMENT){
                    confirmedAppointments[appointmentSize] = appointment;
                    appointmentSize++;
                }
            }
        }
    }
}

