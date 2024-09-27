#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void prime(int last_p[2])
{
    printf("pipe: %d %d \n", last_p[0], last_p[1]);
    int divisor;
    // 读取管道 管道中有数据时 创建子进程
    if (read(last_p[0], &divisor, sizeof(int)) == 0)
    { // 终止条件
        exit(0);
    }
    // 创建子进程  进行素数判断
    if (fork() == 0)
    {
        printf("prime %d\n", divisor);
        int next_p[2];
        pipe(next_p);
        int num;
        while (read(last_p[0], &num, sizeof(int)) > 0)
        {
            if (num % divisor != 0)
            { // 不为divisor的倍数
                write(next_p[1], &num, sizeof(int));
            }
        }
        close(next_p[1]); // 写入下个管道入口后之后 将入口关闭
        close(last_p[0]); // 读取完上个管道之后 将其关闭
        prime(next_p);
    }
    wait(0);
    exit(0); // 这两个是必须的   每个子进程都要exit 每个exit都要父进程wait回收
}
int main()
{
    int p[2];
    pipe(p);
    for (int i = 2; i < 35; i++)
    {
        write(p[1], &i, sizeof(int));
    }
    close(p[1]); // 写入完毕后 将入口关闭
    prime(p);
    return 0;
}
