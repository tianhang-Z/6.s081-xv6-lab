#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

/*
xargs常与管道配合使用
其需要从stdin读取前一个命令的输出 并将其作为参数传递给下一个命令
遇到\n时，则立马执行xargs后的下一个指令，
执行完之后，继续读取stdin剩余的内容，作为下一个命令的参数
eg: echo -e "1\n2" |xargs -n 1 echo line
这是带缓存的实现
*/

int main(int argc, char *argv[])
{

    char *params[MAXARG];       // 用记录参数的位置 后面传递给exec
    int buf_size = MAXARG * 10; //
    char buf[buf_size];         // 用于保存所有参数
    char *cmd = argv[1];        // 第一个参数是命令名

    // 先处理自带的参数
    for (int idx = 0; idx < argc - 1; idx++)
    {
        params[idx] = argv[idx + 1];
    }
    int params_idx = argc - 1;

    int para_idx_in_buf = 0; // 记录buf中参数的位置
    int buf_idx = 0;         // 记录在单个字符buf中的位置

    char temp_buf[10]; // 临时buf 缓存
    int read_sz = 0;   // 记录读取的字节数
    while ((read_sz = read(0, temp_buf, 10)) > 0)
    {
        for (int i = 0; i < read_sz; i++)
        { // 逐个处理字符
            char cur_char = temp_buf[i];
            buf[buf_idx] = cur_char;
            if (cur_char == ' ')
            {
                buf[buf_idx] = 0;
                params[params_idx] = &buf[para_idx_in_buf]; // 记录param
                params_idx++;
                para_idx_in_buf = buf_idx + 1; // 更新下一个param的位置
            }
            else if (cur_char == '\n')
            {
                buf[buf_idx] = 0;
                params[params_idx] = &buf[para_idx_in_buf]; // 记录param
                params_idx++;
                params[params_idx] = 0; // 终止params
                if (fork() == 0)
                {
                    exec(cmd, params);
                }
                wait(0); // 等子进程执行完继续处理后续字符
                buf_idx = 0;
                para_idx_in_buf = 0;
                params_idx = argc - 1; // 重置参数位置
            }
            buf_idx++;
        }
    }
    exit(0);
}
