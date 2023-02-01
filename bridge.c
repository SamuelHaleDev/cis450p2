#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>



typedef struct Bridge {
    pthread_mutex_t mut;
    pthread_cond_t cond;

    int currentDirec;
    int currentNumber;
} bridge_t;

bridge_t monitor;

void *OneVehicle(int carNum);
void CrossBridge(int direc, int carNum);
void ArriveBridge(int direc, int carNum);
void ExitBridge(int direc, int carNum);
bool isSafe(int direc);

//@desc: constructor for Bridge Monitor
void bridgeInit(bridge_t* monitor, int direc, int currentNum) {
    pthread_mutex_t mut;
    pthread_mutex_init(&mut, NULL);
    pthread_cond_t cond;
    pthread_cond_init(&cond, NULL);
    bridge_t temp = { .mut = mut, .cond = cond, .currentDirec = direc, .currentNumber = currentNum };
    *monitor = temp;
}

void *OneVehicle(int carNum) {
    int direc = rand() % 2;
    ArriveBridge(direc, carNum);
    CrossBridge(direc, carNum);
    ExitBridge(direc, carNum);
    sched_yield();
}

void ArriveBridge(int direc, int carNum) {
    while(!isSafe(direc)) {
        pthread_cond_wait(&monitor->cond, &monitor->mut);
    }
    printf("Car %d entering bridge heading %d", carNum, direc);
    monitor->currentNumber = monitor->currentNumber + 1;
    monitor->currentDirec = direc;
    sched_yield();
}

void ExitBridge(int direc, int carNum) {
    printf("Car %d exited bridge heading $d", carNum, direc);
    monitor->currentNumber = monitor->currentNumber - 1;
    pthread_cond_broadcast(&monitor->cond);
    sched_yield();
}

bool isSafe(int direc) {
    if (monitor->currentNumber == 0) return true;
    else if((monitor->currentNumber < 3) && (monitor->currentDirec == direc)) return true;
    else return false;
}

void CrossBridge(int direc, int carNum) {
    printf("Car %d heading %d along bridge", carNum, direc); // will print heading 0 or 1 for which ever direction
    sched_yield();
}




int main() {
    bridgeInit(&monitor, (rand() % 2), 0); // Create monitor
    int numChildren = 50;
    pthread_t id[numChildren];
    
    for(int i = 0; i < numChildren; i++) {
        pthread_create(&id[i], NULL, OneVehicle(i), NULL); // create 50 threads one for each car
    }
    pthread_exit(0);
    return 0;
}