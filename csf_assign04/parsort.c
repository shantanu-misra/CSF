#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int compare_i64(const void *left_, const void *right_) {
  int64_t left = *(int64_t *)left_;
  int64_t right = *(int64_t *)right_;
  if (left < right) return -1;
  if (left > right) return 1;
  return 0;
}

void seq_sort(int64_t *arr, size_t begin, size_t end) {
  size_t num_elements = end - begin;
  qsort(arr + begin, num_elements, sizeof(int64_t), compare_i64);
}

// Merge the elements in the sorted ranges [begin, mid) and [mid, end),
// copying the result into temparr.
void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {
  int64_t *endl = arr + mid;
  int64_t *endr = arr + end;
  int64_t *left = arr + begin, *right = arr + mid, *dst = temparr;

  for (;;) {
    int at_end_l = left >= endl;
    int at_end_r = right >= endr;

    if (at_end_l && at_end_r) break;

    if (at_end_l)
      *dst++ = *right++;
    else if (at_end_r)
      *dst++ = *left++;
    else {
      int cmp = compare_i64(left, right);
      if (cmp <= 0)
        *dst++ = *left++;
      else
        *dst++ = *right++;
    }
  }
}

void fatal(const char *msg) __attribute__ ((noreturn));

void fatal(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(1);
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  assert(end >= begin);
  size_t size = end - begin;

  if (size <= threshold) {
    seq_sort(arr, begin, end);
    return;
  }

  // recursively sort halves in parallel

  size_t mid = begin + size/2;

// TODO: parallelize the recursive sorting
  pid_t first = fork();
  if(first == 0){
    merge_sort(arr, begin, mid, threshold);
    exit(0);
  }
  else{
    exit(1);
  }
  pid_t second = fork();
  if(second == 0){
    merge_sort(arr, mid, end, threshold);
    exit(0);
  }
  else {
    exit(1);
  }

  int firstStatus;
  int secondStatus;

  pid_t firstStatuspid = waitpid(first, &firstStatus, 0);
  pid_t secondStatuspid = waitpid(second, &secondStatus, 0);

  if(firstStatuspid == -1 || secondStatuspid == -1) {
    fprintf(stderr, "%s", "Error: error while forking\n");
    exit(2);
  }
  
  if(!WIFEXITED(firstStatus) || !WIFEXITED(secondStatus)) {
    fprintf(stderr, "%s", "Error: error while forking\n");
  }

  if(WEXISTATUS(firstStatus) != 0 || WEXITSTATUS(secondStatus) != 0){
    fprintf(stderr, "%s", "Error: sorting caused error");
    munmap(arr, (end +1) * sizeof(int64_t));
    exit(2);
  }

  // allocate temp array now, so we can avoid unnecessary work
  // if the malloc fails
  int64_t *temp_arr = (int64_t *) malloc(size * sizeof(int64_t));
  if (temp_arr == NULL)
    fatal("malloc() failed");

  // child processes completed successfully, so in theory
  // we should be able to merge their results
  merge(arr, begin, mid, end, temp_arr);

  // copy data back to main array
  for (size_t i = 0; i < size; i++)
    arr[begin + i] = temp_arr[i];

  // now we can free the temp array
  free(temp_arr);

  // success!
}

int main(int argc, char **argv) {
  // check for correct number of command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <sequential threshold>\n", argv[0]);
    return 1;
  }

  // process command line arguments
  const char *filename = argv[1];
  char *end;
  size_t threshold = (size_t) strtoul(argv[2], &end, 10);

  // TODO: report an error (threshold value is invalid)
  if (end != argv[2] + strlen(argv[2]) && threshold <= 0) {
    fprint(stderr, "%s", "Error: threshold value is invalid\n"); 
    return 1;
  }

  // TODO: open the file
  int fileOpened = open(filename, O_RDWR);
  if (fileOpened < 0) {
    fprintf(stderr, "%s", "Error: file could not be opened\n");
    close(fileOpened);
    exit(1);
  }

  // TODO: use fstat to determine the size of the file
  struct stat statFile;
  int rc = fstat(fileOpened, &statFile);
  if(rc != 0){
    fprintf(stderr, "%s", "Error: error with fstat\n");
    close(fileOpened);
    exit(1);
  }
  size_t fileSize = statFile.st_size;

  // TODO: map the file into memory using mmap
  int64_t *curData = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileOpened, 0);
  close(fileOpened);
  if(curData == MAP_FAILED) {
    munmap(curData, fileSize);
    fprintf(stderr, "%s", "Error: error in opening map\n");
    exit(1);
  }

  // TODO: sort the data!
  merge_sort(curData, 0, fileSize/sizeof(int64_t) - 1, threshold);

  // TODO: unmap and close the file
  munmap(curData, fileSize);

  // TODO: exit with a 0 exit code if sort was successful
  return 0;
}
