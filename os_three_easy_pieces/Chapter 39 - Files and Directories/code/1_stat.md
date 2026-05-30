```c
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

char* pretty_file_perm(mode_t mode) {
  char *perms = (char *) malloc(11);
  

  // filetype
  // This is basically terary operation
  // condition ? if true this value : else this value
  // we are chaining, final one is default else case
  // Alll these are macrsos, that does the respective AND operations
  perms[0]= S_ISREG(mode) ? '-':
            S_ISDIR(mode) ? 'd':
            S_ISLNK(mode) ? 'l':
            S_ISCHR(mode) ? 'c':
            S_ISBLK(mode) ? 'b':
            S_ISFIFO(mode) ? 'p':
            S_ISSOCK(mode) ? 's': '?';

  
    // Owner permissions
    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_ISUID)
           ? ((mode & S_IXUSR) ? 's' : 'S')
           : ((mode & S_IXUSR) ? 'x' : '-');

    // Group permissions
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    // Group execute bit or setgid
    perms[6] = (mode & S_ISGID)
           ? ((mode & S_IXGRP) ? 's' : 'S')
           : ((mode & S_IXGRP) ? 'x' : '-');

    // Others permissions
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    // Others execute bit or sticky
    perms[9] = (mode & S_ISVTX)
           ? ((mode & S_IXOTH) ? 't' : 'T')
           : ((mode & S_IXOTH) ? 'x' : '-');
    perms[10] = '\0';
             

  // NOTE: even though we are showing setuid, setguid and setgui bit info on 3,6 and 9 position 
  // The values are actually present in high bit. We are just putting it there because it becomse easier to understand
  //
  // bit 11   bit 10   bit 9      bits 8–6      bits 5–3      bits 2–0
//┌────────┬────────┬────────┬─────────────┬─────────────┬──────────────┐
//│ setuid │ setgid │ sticky │ user (rwx)  │ group (rwx) │ other (rwx)  │
//└────────┴────────┴────────┴─────────────┴─────────────┴──────────────┘
  //
  //
    return perms;

}
int main(int argc, char *argv[]) {

  // we need at least one arguement i.e. the file and directory we want to stat() on
  if (argc < 2) {
    printf("Usage: \n./a.out <file_name>\n");
    exit(EXIT_FAILURE);
  }
  
  struct stat statbuf = {0};

  if (stat(argv[1],&statbuf) != 0) {
    perror("main(): stat() syscall failed");
    exit(EXIT_FAILURE);
  }

  printf("-----------------%s---------------------\n",argv[1]);
  printf("file_size=%ld bytes\n",statbuf.st_size);
  printf("Device_id=%lu\n",statbuf.st_dev);
  printf("Inode Num=%lu\n",statbuf.st_ino);
  printf("Number of Hard Links=%lu\n",statbuf.st_nlink);
  printf("last status change=%s",ctime(&statbuf.st_ctime));
  printf("last file access=%s",ctime(&statbuf.st_atime));
  printf("last modification time=%s",ctime(&statbuf.st_mtime));
  //printf("Permission=%lo (octal)\n",(unsigned long) statbuf.st_mode);
  printf("Permission: %s\n",pretty_file_perm(statbuf.st_mode));
}
```