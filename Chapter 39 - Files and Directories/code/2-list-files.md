NOTE: Some format specifier in printf might be different because different OS seems to have different type for fields.
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
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

void getlsinfo(char *filename) {
   struct stat statbuf = {0};

  if (stat(filename,&statbuf) != 0) {
    perror("main(): stat() syscall failed");
    exit(EXIT_FAILURE);
  }

  printf("-----------------%s---------------------\n",filename);
  printf("file_size=%lld bytes\n",statbuf.st_size);
  printf("Device_id=%d\n",statbuf.st_dev);
  printf("Inode Num=%llu\n",statbuf.st_ino);
  printf("Number of Hard Links=%hu\n",statbuf.st_nlink);
  printf("last status change=%s",ctime(&statbuf.st_ctime));
  printf("last file access=%s",ctime(&statbuf.st_atime));
  printf("last modification time=%s",ctime(&statbuf.st_mtime));
  //printf("Permission=%lo (octal)\n",(unsigned long) statbuf.st_mode);
  printf("Permission: %s\n",pretty_file_perm(statbuf.st_mode));
}

int main(int argc, char *argv[]) {
  
  char dest[1024] = {0};
  int dols = 0;

  if (argc >= 2){
    for (int i = 1; i < argc; i++) {
      if (strncmp("-ls",argv[i],3) == 0 ) {
        dols = 1;
        continue;
      }
      strncpy(dest,argv[i],1024);
    }
  }

  printf("DO ls %d\n",dols);
  if (strlen(dest) == 0) {
    if (getcwd(dest,1024) == NULL) {
      perror("could not get current directory");
      exit(EXIT_FAILURE);
    }
  }

  DIR *dir = opendir(dest);
  if (dir == NULL) {
    perror("could not open directory due to error");
    exit(EXIT_FAILURE);

  }

  while (1) {
    struct dirent *d = readdir(dir);
    if (d == NULL) {
      break;
    }
    if (dols) {
      getlsinfo(d->d_name);
      continue;
    }
    printf("%s\n",d->d_name);


  }

  printf("Current directory = %s, file=%s\n",dest,argv[0]);
}
```