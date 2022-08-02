#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/stat.h"

void find(char *path, char *fname) {
    int fd;
    struct dirent de;
    struct stat st;
    char buf[512], *p;

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type) {
        case T_FILE:
            fprintf(2, "find: %s is a file, not a directory.\n", path);
        break;
        case T_DIR:
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
                fprintf(2, "find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';

            // read all directory entries until there is none
            while(read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0)
                    continue;

                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if (stat(buf, &st) < 0) {
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }

                if (st.type == T_FILE && strcmp(de.name, fname) == 0) {
                    fprintf(1, "%s\n", buf);
                }

                if (st.type == T_DIR) {
                    // do not recursive on "." and ".."
                    if (strcmp(".", de.name) == 0 || strcmp("..", de.name) == 0) {
                        continue;
                    }
                    find(buf, fname);
                }
            }
        break;
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(2, "usage: find [directory path] [file name].\n");
        exit(1);
    }

    find(argv[1], argv[2]);

    exit(0);
}