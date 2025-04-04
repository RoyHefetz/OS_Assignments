#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ALLOC_SIZE  (20 * 1024) // 20k Bytes

int
main(int argc, char *argv[]) {
    
    int before, after, final;
    void *mem;

    before = memsize();
    if (before < 0){
        printf("Memsize syscall failed\n", before);
        exit(1, "");
    }
    printf("Memory size of the current process: %d bytes\n", before);
    
    mem = malloc(ALLOC_SIZE);
    if (mem == 0){
        printf("Memory allocation failed\n");
        exit(1, "");
    }
    after = memsize();
    printf("Memory size after memory allocation of the process: %d bytes\n", after);
    
    free(mem);

    final = memsize();
    printf("Memory size after freeing memory: %d bytes\n", final);

    exit(0, "");
}

