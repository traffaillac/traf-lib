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
 * _ Begin with a Knapsack_item with p, w and p_w initialized: Knapsack_item k[n];
 * _ Optionally sort it: qsort(k, n, sizeof(*k), Knapsack_comp);
 * _ The performance can also improve by reducing the gcd of all weights to 1.
 * _ Find the items leading to the highest profit:
 *   Knapsack_node sto[n * n], *cur;
 *   Knapsack_best b[c + 1];
 *   for (cur = b[Knapsack_bfill(k, n, b, c, sto)].first; cur != NULL; cur = cur->next)
 *     printf("%u %i\n", k[cur->item].w, k[cur->item].p);
 */
static int gcd(int a, int b) { int r; while (b > 0) r = a % b, a = b, b = r; return a; }
typedef struct { double p_w; unsigned int w; int p; } Knapsack_item;
typedef struct Knapsack_node { struct Knapsack_node* next; int item; } Knapsack_node;
typedef struct { Knapsack_node* first; int p; } Knapsack_best;
static int Knapsack_comp(const void* a, const void* b) { return (((Knapsack_item*)a)->p_w < ((Knapsack_item*)b)->p_w); }
static int Knapsack_bfill(const Knapsack_item* k, int n, Knapsack_best* b, int c, Knapsack_node* sto) {
	assert(k!=NULL&&b!=NULL&&c>=0&&sto!=NULL);
	int i, j, w;
	memset(b, 0, (c + 1) * sizeof(*b));
	for (i = 0; i < n; i++) {
		for (w = c - k[i].w; w >= 0; w--) {
			if ((w == 0 || b[w].first != NULL) && b[w].p + k[i].p > b[j = w + k[i].w].p)
				sto->next = b[w].first, sto->item = i, b[j].first = sto++, b[j].p = b[w].p + k[i].p;
		}
	}
	for (w = 0, j = 1; j <= c; j++) {
		if (b[j].p > b[w].p)
			w = j;
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
