/**
 * Author: Thibault Raffaillac <traf@kth.se>
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static inline long min(long a, long b) { return (a < b) ? a : b; }
static inline long max(long a, long b) { return (a > b) ? a : b; }

void quicksort(int* keys, size_t num)
{
    enum { threshold = 16 };
    int* end = keys + num - 1;
    int* stack[2 * CHAR_BIT * sizeof(size_t)];
    stack[0] = keys;
    stack[1] = end;
    for (int** top = stack; top >= stack; top -= 2) {
        int* lo = top[0];
        int* hi = top[1];
        if (hi - lo >= threshold) {
            int* mid = lo + (hi - lo) / 2;
            int pivot = max(min(max(*lo, *mid), *hi), min(*lo, *mid));
            while (1) {
                while (*lo < pivot)
                    lo++;
                while (*hi > pivot)
                    hi--;
                if (lo > hi)
                    break;
                int key = *lo;
                *lo++ = *hi;
                *hi-- = key;
            }
            if (hi - top[0] > top[1] - lo)
                top[3] = top[1], top[2] = lo, top[1] = hi;
            else
                top[2] = top[0], top[3] = hi, top[0] = lo;
            top += 4;
        }
    }
    
    int* lo = keys;
    int* hi = (keys + threshold < end) ? keys + threshold : end;
    for (int* p = keys + 1; p <= hi; p++) {
        if (*p < *lo)
            lo = p;
    }
    int key = *keys;
    *keys = *lo;
    *lo = key;
    for (int* hi = keys + 2; hi <= end; hi++) {
        int key = *hi;
        int* p = hi;
        while (p[-1] > key)
            *p-- = p[-1];
        *p = key;
    }
}



int main(int argc, char* argv[])
{
    unsigned int n = atoi(argv[1]);
    int* array = malloc(n * sizeof(*array));
    for (unsigned int i = 0; i < n; i++)
        array[i] = rand() & 0xff;
    quicksort(array, n);
    free(array);
    return 0;
}
