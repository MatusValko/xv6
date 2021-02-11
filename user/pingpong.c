pingpong.c 

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
close_array(int *ar, int arn)
{
    int i;
    for(i = 0; i < arn; i++){
        close(ar[i]);
    }
}

int
main(int argc, char *argv[])
{
    int parent_fd[2], child_fd[2];

    char byte = '\n';

    pipe(parent_fd);
    pipe(child_fd);

    if(fork() == 0){
        read(parent_fd[0], &byte, 1);
        printf("%d: received ping\n", getpid());
        write(child_fd[1], &byte, 1);
        close_array(parent_fd, 2);
        close_array(child_fd, 2);
        exit();
    } else {
        write(parent_fd[1], &byte, 1);
        read(child_fd[0], &byte, 1);
        printf("%d: received pong\n", getpid());
        close_array(parent_fd, 2);
        close_array(child_fd, 2);
        wait();
    }
    exit();
} 
 53  user/primes.c 
@@ -0,0 +1,53 @@
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX 36

int
prime_print(int n)
{
    printf("prime %d\n", n);
    return 0;
}

void
prime(int *arr)
{
    int p[2];
    pipe(p);
    if(fork() == 0){
        read(p[0], arr, (MAX + 1) * sizeof(int));
        close(p[0]);
        close(p[1]);
        if(arr[MAX] == MAX) exit();
        prime(arr);
    } else {
        int si = 1;
        while(arr[++si] && si < MAX);
        //prime_print(si);
        int i = 1;
        while(i * si < MAX){
            arr[i*si] = 1;
            i++;
        }
        arr[MAX] = si;
        if(si != MAX) {
            prime_print(si);
        }
        write(p[1], arr, (MAX + 1) * sizeof(int));
        close(p[0]);
        close(p[1]);
        wait();
        exit();
    }
}

int
main(int argc, char *argv[])
{
    int arr[MAX + 1] = {0};
    arr[0] = arr[1] = 1;
    prime(arr);
    exit();
} 