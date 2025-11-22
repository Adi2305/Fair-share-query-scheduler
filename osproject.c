#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXQ 120
#define SESSION_MINUTES 120  

typedef enum {FACULTY = 1, STUDENT = 2} QTYPE;

typedef struct {
    char id[8];        
    int arrival;       
    int burst;         
    int remaining;     
    int completion;    
    int waiting;       
    int tat;           
    QTYPE type;       
} Query;

int TimeQuantum = 0;
Query Faculty[MAXQ], Student[MAXQ], Mix[MAXQ];
int FacultyCount = 0, StudentCount = 0, MixCount = 0, TotalQueries = 0;

int hhmm_to_minutes_since_10(int hhmm, int *out_min) {
    int hh = hhmm / 100;
    int mm = hhmm % 100;
    if (mm < 0 || mm > 59) return 0;
    if (hh < 10 || hh > 11) return 0; 
    int absolute_minutes = hh * 60 + mm;
    int since_10 = absolute_minutes - 600; 
    if (since_10 < 0 || since_10 >= SESSION_MINUTES) return 0;
    *out_min = since_10;
    return 1;
}

int read_int_with_prompt(const char *prompt) {
    int x;
    printf("%s", prompt);
    while (scanf("%d", &x) != 1) {
        while (getchar() != '\n'); 
        printf("Invalid input, try again: ");
    }
    return x;
}

void InputsForProcess() {
    printf("\nEnter total number of Queries (1..%d): ", MAXQ);
    while (scanf("%d", &TotalQueries) != 1 || TotalQueries <= 0 || TotalQueries > MAXQ) {
        while (getchar() != '\n');
        printf("Enter a valid number between 1 and %d: ", MAXQ);
    }
    printf("\nEnter Time Quantum for each query (in minutes, >0): ");
    while (scanf("%d", &TimeQuantum) != 1 || TimeQuantum <= 0) {
        while (getchar() != '\n');
        printf("Enter a valid positive Time Quantum: ");
    }

    int remaining_session = SESSION_MINUTES; 
    for (int i = 0; i < TotalQueries; i++) {
        int qtype;
        printf("\nQuery %d:\n", i+1);
        printf(" Type of Query (1 for Faculty, 2 for Student): ");
        while (scanf("%d", &qtype) != 1 || (qtype != 1 && qtype != 2)) {
            while (getchar() != '\n');
            printf("Enter 1 for Faculty or 2 for Student: ");
        }

        char qid[16];
        printf(" Enter Query ID (string, no spaces): ");
        scanf("%15s", qid);

        
        int AT;
        int arrival_min;
        printf(" Enter Query Arrival Time (HHMM) between 1000 and 1159: ");
        while (scanf("%d", &AT) != 1 || !hhmm_to_minutes_since_10(AT, &arrival_min)) {
            while (getchar() != '\n');
            printf("Invalid time. Enter in HHMM format between 10:00 and 11:59 (e.g. 1025): ");
        }

       
        int BT;
        printf(" Enter Burst Time (in minutes): ");
        while (scanf("%d", &BT) != 1 || BT <= 0 || arrival_min + BT > SESSION_MINUTES) {
            while (getchar() != '\n');
            if (BT <= 0) printf("Burst Time must be > 0. Try again: ");
            else printf("Invalid burst -> arrival + burst must be <= %d minutes from 10:00. Try again: ", SESSION_MINUTES);
        }

        
        if (qtype == 1) {
            strncpy(Faculty[FacultyCount].id, qid, sizeof(Faculty[FacultyCount].id)-1);
            Faculty[FacultyCount].id[sizeof(Faculty[FacultyCount].id)-1] = '\0';
            Faculty[FacultyCount].arrival = arrival_min;
            Faculty[FacultyCount].burst = BT;
            Faculty[FacultyCount].remaining = BT;
            Faculty[FacultyCount].completion = -1;
            Faculty[FacultyCount].waiting = 0;
            Faculty[FacultyCount].tat = 0;
            Faculty[FacultyCount].type = FACULTY;
            FacultyCount++;
        } else {
            strncpy(Student[StudentCount].id, qid, sizeof(Student[StudentCount].id)-1);
            Student[StudentCount].id[sizeof(Student[StudentCount].id)-1] = '\0';
            Student[StudentCount].arrival = arrival_min;
            Student[StudentCount].burst = BT;
            Student[StudentCount].remaining = BT;
            Student[StudentCount].completion = -1;
            Student[StudentCount].waiting = 0;
            Student[StudentCount].tat = 0;
            Student[StudentCount].type = STUDENT;
            StudentCount++;
        }
        remaining_session -= BT;
        
    }
}


int cmp_query_for_sort(const void *a, const void *b) {
    const Query *qa = (const Query*)a;
    const Query *qb = (const Query*)b;
    if (qa->arrival != qb->arrival) return qa->arrival - qb->arrival;
    
    return (int)qb->type - (int)qa->type; 
}

void MergeAndSort() {
    MixCount = 0;
    for (int i = 0; i < FacultyCount; i++) Mix[MixCount++] = Faculty[i];
    for (int i = 0; i < StudentCount; i++) Mix[MixCount++] = Student[i];
    if (MixCount > 0) qsort(Mix, MixCount, sizeof(Query), cmp_query_for_sort);
}

void RoundRobinSimulation() {
    if (MixCount == 0) {
        printf("\nNo queries to process.\n");
        return;
    }

    int completed = 0;
    int current_time = Mix[0].arrival; 
    while (completed < MixCount) {
        int progress = 0; 
        for (int i = 0; i < MixCount && completed < MixCount; i++) {
            if (Mix[i].remaining > 0 && Mix[i].arrival <= current_time) {
                progress = 1;
                int use = (Mix[i].remaining <= TimeQuantum) ? Mix[i].remaining : TimeQuantum;
                Mix[i].remaining -= use;
                current_time += use;
                if (Mix[i].remaining == 0) {
                    Mix[i].completion = current_time;
                    Mix[i].tat = Mix[i].completion - Mix[i].arrival;
                    Mix[i].waiting = Mix[i].tat - Mix[i].burst;
                    completed++;
                }
            }
            
        }
        if (!progress) {
            
            int next_arrival = 1000000;
            for (int j = 0; j < MixCount; j++) {
                if (Mix[j].remaining > 0 && Mix[j].arrival > current_time) {
                    if (Mix[j].arrival < next_arrival) next_arrival = Mix[j].arrival;
                }
            }
            if (next_arrival != 1000000) current_time = next_arrival;
            else break; 
        }
    }

    
    printf("\n==> Times printed in hh:mm (24h) and minutes\n");
    printf("QueryID\tType\tArrival\tBurst\tWaiting\tTurnAround\tCompletion\n");
    for (int i = 0; i < MixCount; i++) {
        int absArr = 600 + Mix[i].arrival;   
        int absCT = 600 + Mix[i].completion;
        int arr_h = absArr / 60, arr_m = absArr % 60;
        int ct_h = absCT / 60, ct_m = absCT % 60;
        printf("%s\t%s\t%02d:%02d\t%d\t%d\t%d\t\t%02d:%02d\n",
               Mix[i].id,
               Mix[i].type == FACULTY ? "FAC" : "STU",
               arr_h, arr_m,
               Mix[i].burst,
               Mix[i].waiting,
               Mix[i].tat,
               ct_h, ct_m);
    }
}

void PrintSummary() {
    if (MixCount == 0) return;
    int earliestArrival = Mix[0].arrival;
    int latestCompletion = Mix[0].completion;
    int totalWait = 0, totalTAT = 0;
    for (int i = 0; i < MixCount; i++) {
        if (Mix[i].completion > latestCompletion) latestCompletion = Mix[i].completion;
        totalWait += Mix[i].waiting;
        totalTAT += Mix[i].tat;
    }
    int totalTimeSpent = latestCompletion - earliestArrival; 
    double avgWait = (double)totalWait / MixCount;
    double avgTAT = (double)totalTAT / MixCount;

    printf("\nSummary of Execution:\n");
    printf(" Total Time Spent on handling Queries: %d minutes\n", totalTimeSpent);
    printf(" Average TurnAround Time: %.2f minutes\n", avgTAT);
    printf(" Average Waiting Time   : %.2f minutes\n", avgWait);
    printf("\nProgram Execution Completed!\n");
}

int main() {
    printf("Welcome to the FAIR SHARE QUERY SCHEDULING SYSTEM\n"
           "Session: 10:00 AM to 11:59 AM inclusive (minutes 0..119 from 10:00).\n"
           "Please enter arrival times in HHMM format (e.g., 1025 for 10:25).\n");

    InputsForProcess();
    MergeAndSort();
    RoundRobinSimulation();
    PrintSummary();
    return 0;
}
