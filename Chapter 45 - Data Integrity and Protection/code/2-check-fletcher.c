/*
1. Read file as arguement
2. Read line by line
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("usage: ./a.out <file_name>\n");
    return -1;
  }

  int fd = open(argv[1],O_RDONLY);
  if (fd == -1) {
    perror("unable to open file");
    return -1;
  }

  // we XOR one byte at a time
  unsigned int sum1 = 0;
  unsigned int sum2 = 0;

  unsigned char buf= 0;

  int exit_status = 0;

  while (1) {
    int br = read(fd,&buf,1);

    if (br == -1) {
      perror("while reading\n");
      exit_status = -1;
      goto cleanup;
    }

    if (br == 0) {
      printf("EOF reached\n");
      break;
    }

    sum1 = (sum1 + ((int) buf)) % 255;
    sum2 = (sum1 + sum2) % 255;
    buf = 0;

  }

  unsigned int checksum  = sum2 << 8 | sum1;
  printf("Checksum %u\n",checksum);
  goto cleanup;

cleanup:
  close(fd);
  return exit_status;
}

