#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUF_SIZE 100
int main()
{
    /*
    双管道
    子进程  p_to_c[0]读端  c_to_p[1]写端
    父进程  c_to_p[0]读端  p_to_c[1]写端
    */
    int parent_to_child[2], child_to_parent[2];
    pipe(parent_to_child);
    pipe(child_to_parent);
    char buf[BUF_SIZE];
    // printf("parent_to_child[2] :%d,%d\n", parent_to_child[0], parent_to_child[1]);
    // printf("child_to_parent[2] :%d,%d\n", child_to_parent[0], child_to_parent[1]);
    if (fork() == 0)
    { // child process
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        read(parent_to_child[0], buf, BUF_SIZE);
        printf("%d: received %s\n", getpid(), buf);
        write(child_to_parent[1], "pong", strlen("pong"));
        exit(0);
    }
    else
    { // parent process
        close(parent_to_child[0]);
        close(child_to_parent[1]);
        write(parent_to_child[1], "ping", strlen("ping"));
        read(child_to_parent[0], buf, BUF_SIZE);
        printf("%d: received %s\n", getpid(), buf);
        wait(0);
        exit(0);
    }
}
