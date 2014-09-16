#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


static inline void set_sched_fifo()
{
    struct sched_param param; 
    int maxpri; 
    maxpri = sched_get_priority_max(SCHED_FIFO);
    if(maxpri == -1) {
        perror("sched_get_priority_max() failed"); 
        exit(1); 
    } 
    param.sched_priority = maxpri; 
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) { 
        perror("sched_setscheduler() failed"); 
        exit(1); 
    } 
}

static inline void set_cpu_bind()
{
    cpu_set_t mask;  

    CPU_ZERO(&mask);      
    CPU_SET(0, &mask);

    if(sched_setaffinity(0, sizeof(mask), &mask) == -1) {  
        perror("set affinity failed..");  
        exit(1);
    } 
}

static inline uint64_t rdtsc()
{
    uint32_t  eax, edx;

    asm volatile (
        "rdtsc"
    : "=a" (eax), "=d" (edx));

    return eax + ((uint64_t)edx << 32);
}

int main()
{
    uint64_t clock1;
    uint64_t clock2;
    uint64_t clock3;

    /* 消除cpu调度的误差 */
    set_sched_fifo();
    set_cpu_bind();

    /* 算两次，是为了消除sleep本身的误差，精度不高的话，可以只算一次 */
    clock1 = rdtsc();
    sleep(1);
    clock2 = rdtsc();
    sleep(2);
    clock3 = rdtsc();

    printf("cpu freq: %lu\n", clock3 - clock2 - (clock2 - clock1));
    
    return 0;
}
