```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>


// simple reverse_string without allocating a new char
void reverse_string(char *input) {
  int start = 0;
  int end = strlen(input)-1;

  while (start < end) {
    char tmp1 = input[start];
    char tmp2 = input[end];

    input[start]=tmp2;
    input[end]=tmp1;

    start++;
    end--;
  }
}

int main(int argc, char *argv[]) {

  char *filename = "README.md";

  int count = 0;
  int c;

  while ((c = getopt(argc,argv,"n:")) != -1) {
    switch (c) {
      case 'n':
        count = count + atoi(optarg); // optarg is the global value that stores the value of -n
        break;
      default:
        printf("unknown flag");
        return -1;
    }
  }

  // optind is the global variable that will point to the next arguement after all switches have been parsed
  if (optind < argc) {
    filename = argv[optind];
  } else {
    printf("Please provide filename\n");
    return -1;
  }

  int fd = open(filename,O_RDONLY);
  if (fd == -1) {
    perror("while opening file");
    return -1;
  }
  
  // INITIAL buffer to hold data
  char *buffer = calloc(1024,sizeof(char));
  unsigned long cb = 0;

  // Seek to 0 starting from SEEK_END, i.e. go to end of the file
  off_t file_size = lseek(fd,0,SEEK_END);
  if (file_size == -1) {
    perror("while setting offset");
    goto err_cleanup;
  }

  // next offset is going to be one less than the end
  off_t offset = file_size - 1;

// while we do not reach at the top of the file
  while (offset >= 0) {

    // if unable to seek 
    if (lseek(fd,offset,SEEK_SET) == -1) {
      perror("reading error again");
      goto err_cleanup;
    }

    // if we have reached the max amount of \n occurences
    if (count < 0) {
      break;
    }

    // if we cant put anymore data into buffer realloc to get more memory, realloc automatically frees previous pointer if it moves data around
    if (cb >= strlen(buffer)) {
      char *tmp=realloc(buffer,1024);
      if (tmp == NULL){
        perror("reallocation");
        goto err_cleanup;
      }
      buffer = tmp;
    }

    // read 
    char temp;
    if (read(fd,&temp,1) != 1) {
      perror("reading bytes");
      goto err_cleanup;
    }

    // if \n encountered decrease 
    if (temp == '\n') {
      count--;
    }

    buffer[cb]=temp;

    // decrease offset
    offset--;
    // increase number of bytes written
    cb++;
  }

  reverse_string(buffer);
  printf("value %s\n",buffer);
  free(buffer);

  close(fd);
  exit(EXIT_SUCCESS);

err_cleanup:
  free(buffer);
  close(fd);
  exit(EXIT_FAILURE);
}



```