#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 获取filename的格式化字符串 如./dir/a.txt 格式化为 a.txt
char *
fmtname(char *path)
{
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    // 倒着搜第一个'/'
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
    return buf;
}

void find(char *cur_path, char *filename)
{
    int fd;
    char buf[512], *p;
    struct dirent de;
    struct stat st;

    if ((fd = open(cur_path, 0)) < 0)
    { // 打开当前path  也就是打开目录
        printf("find: cannot open %s\n", cur_path);
        return;
    }
    if (fstat(fd, &st) < 0)
    { // 获取文件状态信息
        printf("find: cannot stat %s\n", cur_path);
        return;
    }

    if (strlen(cur_path) + 1 + DIRSIZ + 1 > sizeof buf)
    { // 确保不会超出buf
        printf("cur_path too long\n");
        exit(1);
    }
    strcpy(buf, cur_path);
    p = buf + strlen(buf);
    *p++ = '/';
    // 一个目录包含多个目录项 对其进行read可以逐个获取目录项

    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        // 读取目录项 判断是目录还是文件
        // 如果是目录则递归 是文件则判断是否为要找的文件
        // de.inum==0表示这是一块已经初始化并且可以用来创建文件或者文件夹的位置，
        // 所以在读取的过程中应当无视这一块空间。

        // 不对..和.进行递归
        if (strcmp(de.name, ".") == 0 ||
            strcmp(de.name, "..") == 0)
        {
            // printf("%s inum:%d\n", de.name, de.inum);
            continue;
        }
        if (de.inum == 0)
        {
            // printf("%s inum:%d\n", de.name, de.inum);
            continue;
        }

        memmove(p, de.name, DIRSIZ); // de.name加到cur_path之后
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0)
        {
            printf("find: cannot stat %s\n", buf);
            continue;
        }
        // 要么是文件 要么是目录
        if (st.type == T_FILE && strcmp(de.name, filename) == 0)
        {
            printf("%s\n", buf);
            // printf("inum:%d\n", de.inum);
            // printf("inode:%d\n", st.ino);
        }
        else if (st.type == T_DIR)
        {
            find(buf, filename);
        }
    }
    close(fd);
    return;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage : find <dir> <filenam>...\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
