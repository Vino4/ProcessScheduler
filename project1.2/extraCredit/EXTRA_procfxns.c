#include "EXTRA_procNode.h"
#include "EXTRA_procList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "EXTRA_p1fxns.h"
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct procList pList;
typedef struct procNode pNode;

/*
*   p1strcpy - copies char * 'dst' and a char * 'dst' until it reaches a '\0'
*/
void p1strcpy(char *dst, char *src){
    //while(*dst++=*src++);
    while(*src != '\0'){
        *dst++ = *src++;
    }
    *dst = '\0';
}

/*
*   parsePid - takes a pid_t 'pid' and parses it into the buffer buf[]
*/
void parsePid(char buf[], pid_t pid) {
    char pTempBuffer[1024] = "";
    int i = 0; 
    while (pid > 0){
        pTempBuffer[i++] = pid%10 + '0';
        pid /= 10;
    }

    char * ePtr = &pTempBuffer[0];
    ePtr += p1strlen(pTempBuffer);
    char * dPtr = &buf[0];
    int j = 0;
    for(j = strlen(pTempBuffer) - 1; j > -1; j--)
        *dPtr++ = pTempBuffer[j];
    *dPtr = '\0';
}

/*
*   strToLong - takes a char buffer 'buf[]' and returns the content as a long
*       contant must be a number, otherwise it will break 
*/
long strToLong(char buf[]) {

    long result = 0;
    int idx = p1strlen(buf) - 1;
    int scaler = 1;
    for (; idx > -1; idx--){
        int temp = buf[idx] - '0';
        result += scaler * temp;
        scaler *= 10;
    }
    return result;
}

/*
 *	pid_getProcFileInfo - takes a destination char * 'dst', a pid_t 'pid', a char* 'filename' and the index on the information 
 *      copies the result to 'dst' (over-write)
 *	
 */
void pid_getProcFileInfo(char * dst, pid_t pid, char * fileName, int lineIndex, int wordIndex) {
    char lBuffer[1024];
    char * wBuffer = dst;
    char path[1024];
    char parsedPid[1024];
    int i = 0;

    parsePid(parsedPid, pid);
    p1strcpy(path, "/proc/");
    p1strcpy(&path[p1strlen(path)], parsedPid);
    p1strcpy(&path[p1strlen(path)], "/");
    p1strcpy(&path[p1strlen(path)], fileName);

    int fd = open(path, O_RDONLY);
    if (fd < 0){
        p1perror(1, "pid_getProcFileInfo: failed to open.\n");
    }

    for (i = 0; i < lineIndex; i++)
        p1getline(fd, lBuffer, 1024);
    int j = 0;
    for (i = 0; i < wordIndex; i++)
        j = p1getword(lBuffer, j, wBuffer);
    if(wBuffer[strlen(wBuffer) - 1] == '\n'){
        wBuffer[strlen(wBuffer) - 1] = '\0';
    }
    int fd2 = close(fd);    
    if (fd2 < 0){
        p1perror(1, "pid_getProcFileInfo: failed to close.\n");
    }
}


int procTimer = 0; // for repeating the number description
/*
*   proc_updatePrintInfo - takes a pNode 'node' and updates its proc info then prints relavent information
*   Depends on global variable procTimer (=0)
*/
void proc_updatePrintInfo(pNode * node){
    pid_t x = pNode_getPID(node);
    char PID[1024];
    char name[1024];
    char cpuUsrTime[1024];
    char cpuKrnlTime[1024];
    char reads[1024];
    char writes[1024];
    char pages[1024];

    /* proccess proc */
    pid_getProcFileInfo(PID, x, "stat", 1, 1);
    pid_getProcFileInfo(name, x, "stat", 1, 2);
    pid_getProcFileInfo(cpuUsrTime, x, "stat", 1, 14);
    pid_getProcFileInfo(cpuKrnlTime, x, "stat", 1, 15);
    pid_getProcFileInfo(reads, x, "io", 3, 2);
    pid_getProcFileInfo(writes, x, "io", 4, 2);
    pid_getProcFileInfo(pages, x, "statm", 1, 1);

    /* get previous quantum data */
    long long previousReads = pNode_getReads(node);
    long long previousWrites = pNode_getWrites(node);
    long long previousCpuUsrTime = pNode_getCPUUsrTime(node);
    long long previousCpuKrnlTime = pNode_getCPUKrnlTime(node);

    /* parse current data */
    long long currentCpuUsrTime = strToLong(cpuUsrTime);
    long long currentCpuKrnlTime = strToLong(cpuKrnlTime);
    long long currentReads = strToLong(reads);
    long long currentWrites = strToLong(writes);
    long long currentPages = strToLong(pages);

    /* calcuate additional current data */
    long long currentTotalIO = currentReads + currentWrites;
    long long currentTotalCPU = currentCpuUsrTime + currentCpuKrnlTime;

    /* calculate addition previous data */
    long long previousTotalIO = previousReads + previousWrites;
    long long previousTotalCPU = previousCpuKrnlTime + previousCpuUsrTime;

    /* calculate data for this quantum only*/
    long long readsThisQuantum = currentReads - previousReads;
    long long writesThisQuantum = currentWrites - previousWrites;
    long long cpuKrnlThisQuantum = currentCpuKrnlTime - previousCpuKrnlTime;
    long long cpuUsrThisQuantum = currentCpuUsrTime - previousCpuUsrTime;
    long long totalIOThisQuantum = currentTotalIO - previousTotalIO;
    long long totalCPUThisQuantum = currentTotalCPU - previousTotalCPU;

    /* Print stats */
    pNode_setStats(node, currentReads, currentWrites, currentCpuUsrTime, currentCpuKrnlTime);


    if ((procTimer++ % 20) == 0){
        p1putstr(1, "[PID]");
        p1putstr(1, "\t");
        p1putstr(1, "Size (pages)");
        p1putstr(1, "\t");
        p1putstr(1, "IO reads");
        p1putstr(1, "\t");
        p1putstr(1, "IO writes");
        p1putstr(1, "\t");
        p1putstr(1, "Total IO");
        p1putstr(1, "\t");
        p1putstr(1, "CPU (User, Krnl) [HZ]");
        p1putstr(1, "\t");
        p1putstr(1, "\t");
        p1putstr(1, "(Name)");
        p1putstr(1, "\n");
    }

    p1putstr(1, "[");
    p1putstr(1, PID);
    p1putstr(1, "]\t");
    p1putint(1, currentPages);
    p1putstr(1, "\t");
    p1putstr(1, "\t");
    p1putint(1, readsThisQuantum);
    p1putstr(1, "\t");
    p1putstr(1, "\t");
    p1putint(1, writesThisQuantum);
    p1putstr(1, "\t");
    p1putstr(1, "\t");
    p1putint(1, totalIOThisQuantum);
    p1putstr(1, "\t");
    p1putstr(1, "\t");
    p1putint(1, totalCPUThisQuantum);
    p1putstr(1, " (");
    p1putint(1, cpuUsrThisQuantum);
    p1putstr(1, ", ");
    p1putint(1, cpuKrnlThisQuantum);
    p1putstr(1, ")");
    p1putstr(1, "\t");
    p1putstr(1, "\t");
    p1putstr(1, "\t");
    p1putstr(1, name);
    p1putstr(1, "\n");
}