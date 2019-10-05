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
 * Base functions to manage a priority queue. O(log n)
 */
static void Heap_push(const void* key, void* base, size_t* num, size_t size, int (*comp)(const void*, const void*)) {
    int lo = (*num)++;
    int hi;
    while ((hi = (lo - 1) >> 1) >= 0 && comp(key, base + hi * size) < 0) {
        memcpy(base + lo * size, base + hi * size, size);
        lo = hi;
    }
    memcpy(base + lo * size, key, size);
}
static void Heap_pop(void* base, size_t* num, size_t size, int (*comp)(const void *, const void *)) {
    (*num)--;
    int hi = 0;
    int lo;
    while ((lo = hi * 2 + 1) < *num) {
        lo += lo + 1 < *num && comp(base + (lo + 1) * size, base + lo * size) < 0;
        if (comp(base + *num * size, base + lo * size) < 0)
            break;
        memcpy(base + hi * size, base + lo * size, size);
        hi = lo;
    }
    memcpy(base + hi * size, base + *num * size, size);
}



/* Manage elements in sets (begin with a -1 initialised array). */
static int  Set_find(int* set, int i) { return (set[i] < 0) ? i : (set[i] = Set_find(set, set[i])); }
static int  Set_test(int* set, int i, int j) { return (Set_find(set, i) == Set_find(set, j)); }
static int  Set_card(int* set, int i) { return -set[Set_find(set, i)]; }
static void Set_union(int* set, int i, int j) {
	int a = Set_find(set, i), b = Set_find(set, j), c;
	if (a != b) {
		if (set[a] < set[b])
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
