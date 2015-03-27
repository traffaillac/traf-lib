/**
 * Author: Thibault Raffaillac <traf@kth.se>
 */

#ifndef MISC_H
#define MISC_H

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define SWAP(a, b) do { typeof(a) t = a; a = b; b = t; } while(0)
static inline long long min(long long a, long long b) { return (a < b) ? a : b; }
static inline long long max(long long a, long long b) { return (a > b) ? a : b; }
static inline unsigned long long umin(unsigned long long a, unsigned long long b) { return (a < b) ? a : b; }
static inline unsigned long long umax(unsigned long long a, unsigned long long b) { return (a > b) ? a : b; }



/* Custom bitset implementation (replace 100 below with the desired size). */
enum { BITS_CHUNKS = (100 + LONG_BIT - 1) / LONG_BIT };
typedef struct { unsigned long d[BITS_CHUNKS]; } Bits;
static void Bits_set(Bits* b, unsigned int i) { b->d[i / LONG_BIT] |= 1 << i % LONG_BIT; }
static void Bits_reset(Bits* b, unsigned int i) { b->d[i / LONG_BIT] &= ~(1 << i % LONG_BIT); }
static void Bits_flip(Bits* b, unsigned int i) { b->d[i / LONG_BIT] ^= 1 << i % LONG_BIT; }
static int  Bits_test(Bits* b, unsigned int i) { return (b->d[i / LONG_BIT] >> i % LONG_BIT) & 1; }
static void Bits_or(Bits* dst, const Bits* src) { int c; for (c = 0; c < BITS_CHUNKS; c++) dst->d[c] |= src->d[c]; }
static void Bits_and(Bits* dst, const Bits* src) { int c; for (c = 0; c < BITS_CHUNKS; c++) dst->d[c] &= src->d[c]; }
static int  Bits_count(const Bits* b) { int c, v; for (c = v = 0; c < BITS_CHUNKS; c++) v += __builtin_popcountl(b->d[c]); return v; }



/* integer <-> string conversions. */
static inline long asciitol(uint8_t **str_p) {
    uint8_t *str = *str_p;
    while (*str - 1 < ' ')
        str++;
    long negate = (*str == '-');
    str += negate;
    long res = 0;
    while (*str - '0' <= 9)
        res = res * 10 + *str++ - '0';
    *str_p = str;
    return (res ^ -negate) + negate;
}

static char* itostr(char* str, int value) {
	char buf[20], *p = buf;
	unsigned int div, mod, orig = abs(value);
	for (*str = '-', str += (value < 0); orig >= 10; orig = div)
		div = orig / 10, mod = orig % 10, *p++ = '0' + mod;
	for (*p = '0' + orig; p >= buf; *str++ = *p--);
	return str;
}



/* stdlib's qsort is slow, use this to compete with C++ sort. */
static void quicksort(int *first, int *last, int *values)
{
    while (last - first >= 1024) {
        typeof(first) lo = first, hi = last, mid = first + (size_t)(last - first) / 2;
        typeof(*first) pivot = max(min(max(*first, *last), *mid), min(*first, *last));
        while (1) {
            while (*lo < pivot)
                lo++;
            while (*hi > pivot)
                hi--;
            if (lo > hi)
                break;
            SWAP(*lo, *hi);
            SWAP(values[lo - first], values[hi - first]);
            lo++;
            hi--;
        }
        quicksort(lo, last, values + (hi - first));
        last = hi;
    }
}
void sort(int *keys, int *values, size_t num)
{
    quicksort(keys, keys + num, values);
    typeof(keys) p, lo = keys, hi = keys + (num < 1024 ? num : 1024);
    for (p = keys + 1; p < hi; p++)
        lo = (*p < *lo) ? p : lo;
    SWAP(*keys, *lo);
    SWAP(*values, values[lo - keys]);
    for (hi = keys + 2; hi < keys + num; hi++) {
        __typeof(*keys) key = *hi;
        __typeof(*values) value = values[hi - keys];
        for (p = hi; p[-1] > key; p--)
            *p = p[-1], values[p - keys] = values[p - keys - 1];
        *p = key;
        values[p - keys] = value;
    }
}



/**
 * Base functions to manage a d-ary heap. O(log n)
 *
 * _ Begin with a custom typed and sized array: double array[n];
 * _ Create the heap, its size and a -1 initialised pos: int heap[n], pos[n], s=0;
 * _ Update the value of array[2], then its position in the heap:
 *   array[2] = 1.6, s = Heap_ins(heap, pos, array, sizeof(*array), comp, s, 2);
 * _ Remove and return the first index in the heap:
 *   int i = Heap_get(heap, pos, array, sizeof(*array), comp, --s);
 * _ comp is meant as in qsort: the first element is the leftmost
 */
static int Heap_ins(int* heap, int* pos, void* array, size_t s, int (*comp)(const void*, const void*), int n, int index) {
	assert(heap!=NULL&&pos!=NULL&&array!=NULL&&comp!=NULL&&n>=0&&index>=0);
	unsigned int parent, i = (pos[index] < 0) ? n++ : pos[index], d = 4;
	for (; i > 0 && comp(array + heap[parent = (i - 1) / d] * s, array + index * s) > 0; i = parent)
		pos[heap[i] = heap[parent]] = i;
	pos[heap[i] = index] = i;
	return n;
}
static int Heap_get(int* heap, int* pos, void* array, size_t s, int (*comp)(const void*, const void*), int n) {
	assert(heap!=NULL&&pos!=NULL&&array!=NULL&&comp!=NULL&&n>=0);
	int index, lim, c, res = heap[0], parent = -n, i = 0, d = 4;
	while (i > parent) {
		for (parent = i, index = heap[n], c = d * i + 1, lim = (n < c + d) ? n : c + d; c < lim; c++) {
			if (comp(array + index * s, array + heap[c] * s) > 0)
				index = heap[c], i = c;
		}
		if (i > parent)
			pos[heap[parent] = heap[i]] = parent;
	}
	pos[heap[parent] = heap[n]] = parent;
	pos[res] = -1;
	return res;
}



/* Manage elements in sets (begin with a -1 initialised array). */
static int  Set_find(int* set, int i) { return (set[i] < 0) ? i : (set[i] = Set_find(set, set[i])); }
static int  Set_test(int* set, int i, int j) { return (Set_find(set, i) == Set_find(set, j)); }
static int  Set_card(int* set, int i) { return -set[Set_find(set, i)]; }
static void Set_union(int* set, int i, int j) {
	int a = Set_find(set, i), b = Set_find(set, j), c;
	if (a != b) {
		if (set[a] > set[b])
			c = a, a = b, b = c;
		set[a] += set[b], set[b] = a;
	}
}



/* Fenwick tree, to keep prefix sums (begin with a zero-initialised array). */
static void FTree_add(int* base, int n, int i, int diff) { for (; i < n; i |= i + 1) base[i] += diff; }
static int  FTree_sum(int* base, int i) {
	int res = 0;
	for (; i >= 0; i = (i & i + 1) - 1)
		res += base[i];
	return res;
}

#endif
