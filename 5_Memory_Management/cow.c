#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>

#define BUFFER_SIZE     (100 * 1024 * 1024)
#define PAGE_SIZE       (4 * 1024)
#define COMMAND_SIZE    4096

static char* p;
static char command[COMMAND_SIZE];

static void child_fn(char* p)
{
    printf("*** child ps info before memory access ***:\n");
    fflush(stdout);

    snprintf(command, COMMAND_SIZE, "ps -o pid,comm,vsz,rss,min_flt,maj_flt | grep %d", getpid());
    system(command);

    printf("*** free memory info before memory access ***:\n");
    fflush(stdout);
    system("free -h");

    int i;
    for(i = 0; i < BUFFER_SIZE; i += PAGE_SIZE) {
        p[i] = 0;
    }

    printf("*** child ps info after memory access ***:\n");
    fflush(stdout);

    snprintf(command, COMMAND_SIZE, "ps -o pid,comm,vsz,rss,min_flt,maj_flt | grep %d", getpid());
    system(command);

    printf("*** free memory info after memory access ***:\n");
    fflush(stdout);
    system("free -h");

    exit(EXIT_SUCCESS);
}

static void parent_fn(void)
{
    wait(NULL);
    exit(EXIT_SUCCESS);
}

int main(void) 
{
    char* buf;
    p = malloc(BUFFER_SIZE);
    if(p == NULL) {
        err(EXIT_FAILURE, "malloc() failed");
    }

    int i;
    for(i = 0; i < BUFFER_SIZE; i += PAGE_SIZE) {
        p[i] = 0;
    }

    printf("*** free memory info before fork ***:\n");
    fflush(stdout);
    system("free -h");

    pid_t child1, child2;
    child1 = fork();
    child2 = fork();
    if((child1 == -1) || (child2 == -1)) {
        err(EXIT_FAILURE, "fork() failed");
    }

    if(child1 == 0)
        child_fn(p);
    else
        parent_fn();

    err(EXIT_FAILURE, "shouldn't reach here");
}
