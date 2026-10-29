// C program for implementation of Simulation 
#include<stdio.h> 
#include<limits.h>
#include<stdlib.h>
#include<string.h>
#include "process.h"
#include "util.h"


// Round Robin waiting times.
// Assumes every process arrives at time 0 (as the lab specifies), so a
// process's waiting time is its finish time minus its burst time.
// rem_bt[] holds each process's remaining burst time; we cycle through the
// list, giving each unfinished process at most one quantum per pass.
void findWaitingTimeRR(ProcessType plist[], int n,int quantum) 
{ 
    int *rem_bt = malloc(n * sizeof(int));
    if (!rem_bt) {
        fprintf(stderr, "Error: out of memory\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        rem_bt[i] = plist[i].bt;
        plist[i].wt = 0;
    }

    int t = 0;          // current time
    int done = 0;
    while (!done) {
        done = 1;
        for (int i = 0; i < n; i++) {
            if (rem_bt[i] > 0) {
                done = 0;   // at least one process still has work left
                if (rem_bt[i] > quantum) {
                    // Not the last slice: run a full quantum
                    t += quantum;
                    rem_bt[i] -= quantum;
                } else {
                    // Last slice: process finishes at time t
                    t += rem_bt[i];
                    plist[i].wt = t - plist[i].bt;
                    rem_bt[i] = 0;
                }
            }
        }
    }
    free(rem_bt);
} 

// SJF (preemptive, i.e. Shortest Remaining Time First) waiting times.
// Simulates one time unit at a time: among processes that have arrived and
// still have work left, run the one with the smallest remaining time for one
// tick. A newly arrived shorter job therefore preempts the running one.
// Linear scan per tick (O(n * total time)), fine for this lab's input sizes.
// If nothing has arrived yet the CPU idles and time advances.
void findWaitingTimeSJF(ProcessType plist[], int n)
{ 
    int *rem = malloc(n * sizeof(int));
    if (!rem) {
        fprintf(stderr, "Error: out of memory\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        rem[i] = plist[i].bt;
        plist[i].wt = 0;
    }

    int complete = 0, t = 0;
    while (complete < n) {
        int idx = -1, min = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (plist[i].art <= t && rem[i] > 0 && rem[i] < min) {
                min = rem[i];
                idx = i;
            }
        }
        if (idx == -1) {   // nothing has arrived yet: CPU idle
            t++;
            continue;
        }

        rem[idx]--;        // run the chosen process for one time unit
        t++;
        if (rem[idx] == 0) {
            complete++;
            // completion time = t, so wt = completion - arrival - burst
            plist[idx].wt = t - plist[idx].art - plist[idx].bt;
        }
    }
    free(rem);
} 

// FCFS waiting times (also reused by Priority after sorting).
// Processes run in list order. A process starts at the later of (a) when the
// previous one finished and (b) its own arrival time, so idle gaps between
// arrivals are handled. With all arrivals at 0 this reduces to
// wt[i] = wt[i-1] + bt[i-1].
void findWaitingTime(ProcessType plist[], int n)
{ 
    int t = 0;   // time at which the CPU becomes free
    for (int i = 0; i < n; i++) {
        if (t < plist[i].art)
            t = plist[i].art;          // CPU idles until this process arrives
        plist[i].wt = t - plist[i].art;
        t += plist[i].bt;
    }
} 
  
// Function to calculate turn around time 
void findTurnAroundTime( ProcessType plist[], int n)
{ 
    // calculating turnaround time by adding bt[i] + wt[i] 
    for (int  i = 0; i < n ; i++) 
        plist[i].tat = plist[i].bt + plist[i].wt; 
} 
  
// Comparator for qsort: higher priority value first.
// Ties are broken by pid because qsort is not stable; this keeps equal-
// priority processes in their original (FCFS) order.
int my_comparer(const void *this, const void *that)
{ 
    ProcessType *a = (ProcessType *)this;
    ProcessType *b = (ProcessType *)that;

    if (a->pri < b->pri) return 1;    // b goes first
    if (a->pri > b->pri) return -1;   // a goes first
    return a->pid - b->pid;           // tie: keep original order
} 

//Function to calculate average time 
void findavgTimeFCFS( ProcessType plist[], int n) 
{ 
    //Function to find waiting time of all processes 
    findWaitingTime(plist, n); 
  
    //Function to find turn around time for all processes 
    findTurnAroundTime(plist, n); 
  
    //Display processes along with all details 
    printf("\n*********\nFCFS\n");
}

//Function to calculate average time 
void findavgTimeSJF( ProcessType plist[], int n) 
{ 
    //Function to find waiting time of all processes 
    findWaitingTimeSJF(plist, n); 
  
    //Function to find turn around time for all processes 
    findTurnAroundTime(plist, n); 
  
    //Display processes along with all details 
    printf("\n*********\nSJF\n");
}

//Function to calculate average time 
void findavgTimeRR( ProcessType plist[], int n, int quantum) 
{ 
    //Function to find waiting time of all processes 
    findWaitingTimeRR(plist, n, quantum); 
  
    //Function to find turn around time for all processes 
    findTurnAroundTime(plist, n); 
  
    //Display processes along with all details 
    printf("\n*********\nRR Quantum = %d\n", quantum);
}

//Function to calculate average time 
void findavgTimePriority( ProcessType plist[], int n) 
{ 
    // 1- Sort the processes according to priority
    qsort(plist, n, sizeof(ProcessType), my_comparer);

    // 2- Now simply apply FCFS algorithm
    findWaitingTime(plist, n);
    findTurnAroundTime(plist, n);
  
    //Display processes along with all details 
    printf("\n*********\nPriority\n");
}

void printMetrics(ProcessType plist[], int n)
{
    int total_wt = 0, total_tat = 0; 
    float awt, att;
    
    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n"); 
  
    // Calculate total waiting time and total turn  
    // around time 
    for (int  i=0; i<n; i++) 
    { 
        total_wt = total_wt + plist[i].wt; 
        total_tat = total_tat + plist[i].tat; 
        printf("\t%d\t\t%d\t\t%d\t\t%d\n", plist[i].pid, plist[i].bt, plist[i].wt, plist[i].tat); 
    } 
  
    awt = ((float)total_wt / (float)n);
    att = ((float)total_tat / (float)n);
    
    printf("\nAverage waiting time = %.2f", awt); 
    printf("\nAverage turn around time = %.2f\n", att); 
} 

ProcessType * initProc(char *filename, int *n) 
{
  	FILE *input_file = fopen(filename, "r");
	  if (!input_file) {
		    fprintf(stderr, "Error: Invalid filepath\n");
		    fflush(stdout);
		    exit(0);
	  }

    ProcessType *plist = parse_file(input_file, n);
  
    fclose(input_file);
  
    return plist;
}

// Make a fresh copy of the original process list so each algorithm starts
// from untouched data (the file is parsed only once).
ProcessType * copyProcs(ProcessType *orig, int n)
{
    ProcessType *copy = malloc(n * sizeof(ProcessType));
    if (!copy) {
        fprintf(stderr, "Error: out of memory\n");
        exit(1);
    }
    memcpy(copy, orig, n * sizeof(ProcessType));
    return copy;
}
  
// Driver code 
int main(int argc, char *argv[]) 
{ 
    int n; 
    int quantum = 2;

    ProcessType *orig_list;
    ProcessType *proc_list;
  
    if (argc < 2) {
		   fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
		   fflush(stdout);
		   return 1;
	   }

    // Parse the input file once
    n = 0;
    orig_list = initProc(argv[1], &n);
    
  // FCFS
    proc_list = copyProcs(orig_list, n);
    findavgTimeFCFS(proc_list, n);
    printMetrics(proc_list, n);
    free(proc_list);
  
  // SJF
    proc_list = copyProcs(orig_list, n);
    findavgTimeSJF(proc_list, n); 
    printMetrics(proc_list, n);
    free(proc_list);
  
  // Priority
    proc_list = copyProcs(orig_list, n);
    findavgTimePriority(proc_list, n); 
    printMetrics(proc_list, n);
    free(proc_list);
    
  // RR
    proc_list = copyProcs(orig_list, n);
    findavgTimeRR(proc_list, n, quantum); 
    printMetrics(proc_list, n);
    free(proc_list);

    free(orig_list);
    
    return 0; 
}