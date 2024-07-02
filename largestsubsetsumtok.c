#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

// gcc largestsubsetsumtok.c -o largestsubsetsumtok.bin -O3 -march=native -Wall 

void printarray(int64_t *arr, int64_t arrsize) {  
  int64_t i, sum=0;
  for (i=0; i<arrsize; i++) {
    sum += arr[i];
    printf("%li ", arr[i]);
  }
  printf("\nSum = %li\n", sum);
  return;
}

int16_t **maxsubsetsumsize_init(int16_t arrsize, int64_t max_target_sum) {
  /* Create zeroed array results[k][j] representing the minimum subset size of the first k elements which sums to j  
     results[k][j] = 0 means target sum not possible
     results[k][j] > arrsize means value not yet set
  */
  int16_t **results = malloc(((int32_t)arrsize+1)*sizeof(int16_t*));
  results[0] = malloc((max_target_sum+1) * sizeof(int16_t));
  results[0][0] = 0;
  for (int64_t i=1; i<=max_target_sum; i++) results[0][i] = arrsize+1;
  for (int i=1; i<=arrsize; i++) {
    results[i] = malloc((max_target_sum+1) * sizeof(int16_t));
    memcpy(results[i], results[0], (max_target_sum+1) * sizeof(int16_t));
  }  
  return results;  
}

void maxsubsetsumsize_free(int16_t **results, int16_t arrsize) {
  for (int i=0; i<=arrsize; i++) free(results[i]);
  free(results);
}

int16_t maxsubsetsumsize(int16_t **results, int64_t *arr, int16_t arrsize, int64_t target_sum, int16_t k) {
  return results[k][target_sum];
}

void printresultsarray(int16_t **results, int64_t arrsize, int64_t max_target_sum) {  
  int64_t i,j;
  for (j=1; j<=max_target_sum; j++) {
    printf("%03li ", j);
  }
  printf("\n");
  for (i=1; i<=arrsize; i++) {
    for (j=1; j<=max_target_sum; j++) {
      printf("%03i ", results[i][j]);
    }
    printf("\n");
  }
  printf("\n");
  return;
}

void popresultsarray(int16_t **results, int64_t *arr, int16_t arrsize, int64_t max_target_sum) {
  int64_t j, arrk;
  int16_t k;
  int16_t result, result1, result2;
  for (j=1; j<=max_target_sum; j++) {
    results[1][j] = 0;
  }
  if (arr[0] <= max_target_sum) results[1][arr[0]] = 1;
  for (k=2; k<=arrsize; k++) {
    arrk = arr[k-1];
    for (j=1; j<=max_target_sum; j++) {
      result1 = results[k-1][j];  // largest subset size with sum j from first k-1 elements
      if (j > arrk) {
        result = results[k-1][j - arrk];
        result2 = (result ? 1+result : 0);
      } else {
        if (j == arrk) {
          result2 = 1;
        } else {
          result2 = 0;
        }
      }
      results[k][j] = (result1 < result2 ? result2 : result1);
    }
  }
}

int64_t *findsubset(int16_t **results, int64_t *arr, int16_t arrsize, int64_t target_sum) {
// Returns subset of maximal size with element sum equal to target_sum or NULL if target_sum not acheivable.
  int16_t result = results[arrsize][target_sum];
  if (result == 0) return NULL;
  int16_t k, result1=result;
  int64_t *subset = calloc(result, sizeof(int64_t));
  k = arrsize-1;
  while (result) {
    if (k > 0) result1 = results[k][target_sum];
    if ((k == 0) || (result1 < result)) {
      target_sum -= arr[k];
      result--;
      subset[result] = arr[k];
    }
    k--;
  }
  return subset;
};

int main(int argc, char*argv[]) {
  if (argc <= 4) {
    printf("This program finds a maximally sized subset of a set of n positive integers 0 < a_i < 2^63 which has a target element sum k using dynamic programming.\n");
    printf("Space and time complexity is approx. O(nk).\n");
    printf("Author: Simon Goater Mar 2024.\n");
    printf("Usage: %s maxbytes n k a1 ... an\n", argv[0]);
    printf("maxbytes is the maximum size of the internal matrix generated by the calculation in RAM.\n2*(n+1)*(k+1) <= maxbytes < 2^63\n");
    printf("1 <= k < 2^63\n");
    printf("1 <= n <= 32000\n");
    exit(0);
  }
  int64_t maxresultsbytes = atol(argv[1]);
  if (maxresultsbytes <= 0) {
    fprintf(stderr, "%s - Invalid maxbytes!!\n", argv[0]);
    exit(1);
  }
  int32_t arrsize = atoi(argv[2]);
  if (((4+arrsize) != argc) || (arrsize > 32000)) {
    fprintf(stderr, "%s - Invalid array size!!\n", argv[0]);
    exit(1);
  }
  int64_t target_sum = atol(argv[3]);
  if (target_sum <= 0) {
    fprintf(stderr, "%s - Invalid target sum!!\n", argv[0]);
    exit(1);
  }
  int64_t *arr = malloc(arrsize*sizeof(int64_t));
  __int128 arrsum = 0;
  int32_t j = 0;
  for (int32_t i=0; i<arrsize; i++) {
    arr[j] = atol(argv[4+i]);
    if (arr[j] <= 0) {
      free(arr);
      fprintf(stderr, "%s - Invalid array element!!\n", argv[0]);
      exit(1);      
    } else {
      if (arr[j] < target_sum) {
        arrsum += arr[j];
        j++;
      }
    }
  }
  arrsize = j; // Re-adjust arrsize after removing elements greater than target_sum.
  if (arrsum < target_sum) {
    free(arr);
    printf("No solution.\n");
    exit(0);    
  }
  if (arrsum == target_sum) {
    printf("Max. subset size which sums to %li is %i\n", target_sum, arrsize);
    printarray(arr, arrsize);
    free(arr);
    exit(0);      
  }
  __int128 resultsbytes = ((__int128)(arrsize+1)*(target_sum+1))*sizeof(int16_t);
  if (resultsbytes > maxresultsbytes) {
    free(arr);
    fprintf(stderr, "%s - Matrix Too Big!! %li required, %li allowed.\n", argv[0], (int64_t)resultsbytes, maxresultsbytes);
    exit(1);
  }
  int16_t **results = maxsubsetsumsize_init(arrsize, target_sum);
  printf("Using array of %li bytes\n", (int64_t)resultsbytes);
  popresultsarray(results, arr, arrsize, target_sum);
  int16_t result = maxsubsetsumsize(results, arr, arrsize, target_sum, arrsize);
  //printresultsarray(results, arrsize, target_sum);
  if (result > 0) {
    printf("Max. subset size which sums to %li is %i\n", target_sum, result);
    int64_t *subset = findsubset(results, arr, arrsize, target_sum);
    printarray(subset, result);
    free(subset);
  } else {
    printf("No solution.\n");
  }
  free(arr);
  maxsubsetsumsize_free(results, arrsize);
}
/*
time ./largestsubsetsumtok.bin 100000000 40 40000 1432 3343 4433 6444 955 5535 223 7675 666 3111 4001 3232 788 2212 888 449 6543 211 6996 3197 3751 831 3258 2444 855 761 888 4132 974 312 777 999 4621 676 3194 502 799 479 3062 2421
Using array of 3280082 bytes
Max. subset size which sums to 40000 is 28
1432 955 223 666 4001 3232 788 2212 888 449 211 831 3258 2444 855 761 888 4132 974 312 777 999 676 3194 502 799 479 3062 
Sum = 40000

real	0m0.007s
user	0m0.001s
sys	0m0.006s

From the set of the first 1000 positive squares, find a maximally sized subset which sums to 1,234,567, if one exists.
time ./largestsubsetsumtok.bin 2500000000 1000 1234567 ...
Using array of 2471605136 bytes
Max. subset size which sums to 1234567 is 153
1 4 9 16 25 36 49 64 81 100 121 144 169 196 225 256 289 324 361 400 441 484 529 576 625 676 729 784 841 900 961 1024 1089 1156 1225 1296 1369 1444 1521 1600 1681 1764 1849 1936 2025 2116 2209 2304 2401 2500 2601 2704 2809 2916 3025 3136 3249 3364 3481 3600 3721 3844 3969 4096 4225 4356 4489 4624 4761 4900 5041 5184 5329 5476 5625 5776 5929 6084 6241 6400 6561 6724 6889 7056 7225 7396 7569 7744 7921 8100 8281 8464 8836 9025 9216 9409 9604 9801 10000 10201 10404 10609 10816 11025 11236 11449 11664 11881 12100 12321 12544 12769 12996 13225 13456 13689 13924 14161 14400 14641 14884 15376 15625 15876 16129 16384 16641 16900 17161 17424 17689 17956 18225 18496 18769 19044 19600 19881 20164 20449 20736 21025 21316 21609 21904 22201 22500 22801 23104 23409 23716 24025 24336 
Sum = 1234567

real	0m4.012s
user	0m2.631s
sys	0m1.080s

*/
