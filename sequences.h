/**
 * Author: Thibault Raffaillac <traf@kth.se>
 */

#ifndef SEQUENCES_H
#define SEQUENCES_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>



/**
 * Find a longest increasing subsequence. O(n*log n)
 *
 * _ Begin with a Lis_item array with value initialised: Lis_item a[n], *p;
 * _ Compute and get the length of the lis: int length = lis(a, n);
 * _ Print the lis in decreasing order:
 *   for (p = a[l - 1].last; p != NULL; p = p->prev) printf("%d ", p->value);
 */
typedef struct Lis_item { struct Lis_item *last, *prev; int value; } Lis_item;
static int lis(Lis_item* a, int n) {
	assert(a!=NULL&&n!=0);
	Lis_item* cur;
	int i, l, range;
	for (a->last = a, a->prev = NULL, l = 0, i = 1; i < n; i++) {
		if (a[i].value > a[l].last->value) {
			a[i].prev = a[l].last, a[++l].last = a + i;
		} else {
			for (cur = a, range = l; range != 0; ) {
				if (cur[range / 2].last->value >= a[i].value)
					range /= 2;
				else
					cur += range / 2 + 1, range -= range / 2 + 1;
			}
			a[i].prev = (cur == a) ? NULL : (cur - 1)->last;
			if (a[i].value < cur->last->value)
				cur->last = a + i;
		}
	}
	return l + 1;
}



/**
 * Solver for the 0/1 knapsack problem.
 *
 * _ Start with an initialized list of items: KSitem it[n];
 * _ Sort it to improve performance: qsort(it, n, sizeof(*it), ks_comp);
 * _ Reduce the gcd of weights to 1 to improve performance further.
 * _ Find the items leading to the highest profit:
 *   KSsack ks[c + 1];
 *   KSnode bt[n * n];
 *   for (int b = ks[ks_solve(n, c)].first; b >= 0; b = bt[b].next)
 *      printf("%d\n", bt[b].id);
 */
static struct KSitem { int p_w, p, w, id; } *it;
static struct KSsack { int sum, first; } *ks;
static struct KSnode { int id, next; } *bt;
static inline int ks_comp(const void *a, const void *b) { return ((struct KSitem *)b)->p_w > ((struct KSitem *)a)->p_w; }
static inline int gcd(unsigned a, unsigned b) { while (b > 0) { int r = a % b; a = b; b = r; } return a; }
static int ks_solve(int n, int c) {
	int i, j, w, x, b = 0;
	memset(ks, -1, (c + 1) * sizeof(*ks));
	ks[0].sum = 0;
	for (i = 0; i < n; i++) {
		for (w = c - it[i].w; w >= 0; w--) {
			if (ks[w].sum >= 0 && ks[w].sum + it[i].p > ks[x = w + it[i].w].sum) {
				bt[b] = (struct KSnode){it[i].id, ks[w].first};
				ks[x] = (struct KSsack){ks[w].sum + it[i].p, b++};
			}
		}
	}
	for (w = 0, x = 1; x <= c; x++) {
		if (ks[x].sum >= ks[w].sum)
			w = x;
	}
	return w;
}



/**
 * Minimal interval coverage. O(n*log n)
 *
 * _ Begin with an array of Interval pointers: Interval *p[n];
 * _ Overwrite p with the pointers of the intervals covering [A, B], returning
 *   the number of intervals found: int num = Interval_cover(p, n, A, B);
 */
typedef double Interval_bound;
typedef struct { Interval_bound a, b; } Interval;
static int Interval_comp(const void* a, const void* b) { return ((*(Interval**)a)->a > (*(Interval**)b)->a); }
static int Interval_cover(Interval* base[], int n, Interval_bound A, Interval_bound B) {
	assert(base!=NULL&&A<=B);
	Interval *cur;
	int i = 0, num = 0;
	qsort(base, n, sizeof(*base), Interval_comp);
	do {
		for (cur = NULL; i < n && base[i]->a <= A; i++) {
			if (cur == NULL || base[i]->b > cur->b)
				cur = base[i];
		}
		if (cur == NULL)
			return 0;
		base[num++] = cur;
		A = cur->b;
	} while (A < B);
	return num;
}

#endif
