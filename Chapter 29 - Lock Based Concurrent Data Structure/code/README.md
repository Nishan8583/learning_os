# Some notes on the code

1. build

```clang -Wall -Werror -Wextra -pthread 1_basic_concurrent_lock.c -o test```.

2. Valgrind test
- `valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./your_program`
- This checks for memory leaks.
- Example output
```==2788== 48 bytes in 1 blocks are definitely lost in loss record 
1 of 1 ==2788== at 0x4846828: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so) 
==2788== by 0x1091D1: init (in /mnt/c/Users/Nisha/OneDrive/Documents/NIshanGamingPCVault/NishanGamingPC/Chapter 29 - Lock Based Concurrent Data Structure/code/test) 
==2788== by 0x1093DC: main (in /mnt/c/Users/Nisha/OneDrive/Documents/NIshanGamingPCVault/NishanGamingPC/Chapter 29 - Lock Based Concurrent Data Structure/code/test)
```
- This means that there main -> init -> malloc(), so the memory allocated in by malloc inside init() is not freed.
3. For race conditon detection `valgrind --tool=helgrind ./main-signal`.