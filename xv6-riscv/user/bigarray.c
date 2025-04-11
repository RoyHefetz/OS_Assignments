#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ARRAY_SIZE (1 << 16)  // 2^16 = 65536
#define DEFAULT_NUM_CHILDREN 4

int array[ARRAY_SIZE];

void initialize_array() {
  for (int i = 0; i < ARRAY_SIZE; i++) {
    array[i] = i;
  }
}

int main(int argc, char *argv[]) {
  int num_children = DEFAULT_NUM_CHILDREN;
  
  // Initialize the array with consecutive numbers
  initialize_array();
  
  // Calculate elements per child
  int elements_per_child = ARRAY_SIZE / num_children;
  
  // Create an array to store child PIDs
  int child_pids[16];
  
  // Fork n children
  int ret = forkn(num_children, child_pids);
  
  if (ret < 0) {
    printf("Error: forkn failed\n");
    exit(1, "");
  } else if (ret == 0) {
    // Parent process
    for (int i = 0; i < num_children; i++) {
      printf("%d ", child_pids[i]);
    }
    printf("\n");
    
    // Wait for all children to finish
    int finished_children;
    int statuses[64]; // NPROC = 64 by default
    
    if (waitall(&finished_children, statuses) < 0) {
      printf("Error: waitall failed\n");
      exit(1, "");
    }
    
    // Verify all children finished
    if (finished_children != num_children) {
      printf("Error: Expected %d children to finish, but %d finished\n", 
             num_children, finished_children);
      exit(1, "");
    }
    
    // Calculate the sum of all partial sums
    int total_sum = 0;
    for (int i = 0; i < finished_children; i++) {
      total_sum += statuses[i];
    }
    
    printf("Total sum: %d\n", total_sum);
    
    // The expected sum is 0+1+2+...+(2^16-1) = (2^16-1)*2^16/2 = 2147450880
    if (total_sum == 2147450880) {
      printf("Sum is correct!\n");
    } else {
      printf("Sum is incorrect! Expected 2147450880\n");
    }
    
    exit(0, "Calculation completed successfully");
  } else {
    // Child process
    int child_num = ret;
    int start_idx = (child_num - 1) * elements_per_child;
    int end_idx = (child_num == num_children) ? ARRAY_SIZE : child_num * elements_per_child;
    
    // Calculate partial sum for this child's portion of the array
    int partial_sum = 0;
    for (int i = start_idx; i < end_idx; i++) {
      partial_sum += array[i];
    }
    
    printf("Child %d (PID %d): Processing elements %d to %d, partial sum = %d\n", 
           child_num, getpid(), start_idx, end_idx - 1, partial_sum);
    
    // Return the partial sum as the exit status
    exit(partial_sum, "");
  }
}
