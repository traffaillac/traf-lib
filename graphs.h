/**
 * Author: Thibault Raffaillac <traf@kth.se>
 */

#ifndef GRAPHS_H
#define GRAPHS_H

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"



/**
 * Find the shortest paths from a single node in a directed graph. O(e*log n)
 *
 * _ Begin with a zero-initialised array with the edges set: Graph_node dij[n];
 * _ Compute the shortest paths from start to end, returning the last node
 *   processed (i.e. end, or the farthest if end equals -1 or is unreachable):
 *   int last = Graph_Dijkstra(dij, n, start, end);
 * _ For each node, prev stores the predecessor (or NULL if it is unreachable).
 */
typedef struct Graph_edge { struct Graph_edge* next; int dst, cost; } Graph_edge;
typedef struct Graph_node { Graph_edge* first; struct Graph_node* prev; int total; } Graph_node;
static int Graph_ncomp(const void* a, const void* b) { return (((Graph_node*)a)->total > ((Graph_node*)b)->total); }
static int Graph_Dijkstra(Graph_node* base, int n, int start, int end) {
	assert(base!=NULL&&n>=0&&(unsigned int)start<n&&(unsigned int)end<n);
	int heap[n], pos[n], i, s, t;
	Graph_edge* e;
	memset(pos, -1, sizeof(pos));
	base[start].prev = base + start;
	s = Heap_ins(heap, pos, base, sizeof(*base), Graph_ncomp, 0, start);
	while (s > 0 && (i = Heap_get(heap, pos, base, sizeof(*base), Graph_ncomp, --s)) != end) {
		for (e = base[i].first, t = base[i].total; e != NULL; e = e->next) {
			if (base[e->dst].prev == NULL || base[e->dst].total > t + e->cost) {
				base[e->dst].total = t + e->cost;
				base[e->dst].prev = base + i;
				s = Heap_ins(heap, pos, base, sizeof(*base), Graph_ncomp, s, e->dst);
			}
		}
	}
	return i;
}



/**
 * Find the shortest paths from a single node in a directed graph with possibly
 * negative edge weights. O(n*e)
 *
 * _ Begin with a zero-initialised array with the edges set: Graph_node bf[n];
 * _ Compute the shortest paths from start, returning 0 if no negative-weight
 *   cycle was found: int cyclic = Graph_Bellman_Ford(bf, n, start);
 * _ For each node, total equals INT_MIN if it crosses a negative-weight cycle.
 */
static int Graph_Bellman_Ford(Graph_node* base, int n, int start) {
	assert(base!=NULL&&n>=0&&(unsigned int)start<n);
	int stack[2][n], pos[n], *r = stack[0], *w = stack[1], *p, c = 0, i, j, k, l = 1, t;
	Graph_edge* e;
	for (base[*r = start].prev = &base[start]; l > 0 && c < n; p = r, r = w, w = p, c++, l = k) {
		for (memset(pos, -1, sizeof(pos)), j = k = 0; j < l; j++) {
			for (i = r[j], t = base[i].total, e = base[i].first; e != NULL; e = e->next) {
				if (base[e->dst].prev == NULL || base[e->dst].total > t + e->cost) {
					base[e->dst].total = t + e->cost;
					base[e->dst].prev = base + i;
					if (pos[e->dst] < 0)
						pos[e->dst] = k, w[k++] = e->dst;
				}
			}
		}
	}
	for (j = 0; j < l; j++)
		base[r[j]].total = INT_MIN;
	return l;
}
static int Graph_inf_path(Graph_node* base, int start, int end) {
	assert(base!=NULL&&start>=0&&end>=0);
	do {
		if (base[end].total == INT_MIN)
			return (1);
		end = base[end].prev - base;
	} while (start != end);
	return 0;
}



/**
 * Find the shortest paths between all pairs of nodes in a directed graph with
 * possibly negative edge weights. O(n³)
 *
 * _ Begin with a square array of edges: Graph_dense fw[n*n];
 * _ Initialize it before setting the costs: Graph_init_dense(fw, n);
 * _ Compute the shortest paths: Graph_Floyd_Warshall(fw, n);
 * _ If a path goes through an intermediary node, it is stored in next.
 */
typedef struct Graph_dense { int cost, next; } Graph_dense;
static void Graph_init_dense(Graph_dense* base, int n) {
	assert(base!=NULL);
	int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++)
			base[i*n+j].cost = (i == j) ? 0 : INT_MAX;
	}
}
static void Graph_Floyd_Warshall(Graph_dense* base, int n) {
	assert(base!=NULL);
	int i, j, k, c;
	for (k = 0; k < n; k++) {
		for (i = 0; i < n; i++) {
			for (c = base[i*n+k].cost, j = 0; c != INT_MAX && j < n; j++) {
				if (base[k*n+j].cost != INT_MAX && c + base[k*n+j].cost < base[i*n+j].cost)
					base[i*n+j].cost = c + base[k*n+j].cost, base[i*n+j].next = k;
			}
		}
	}
}
static int Graph_inf_dense_path(Graph_dense* base, int n, int i, int j) {
	int k, inf;
	for (inf = k = 0; inf == 0 && k < n; k++)
		inf = (base[k*n+k].cost < 0 && base[i*n+k].cost != INT_MAX && base[k*n+j].cost != INT_MAX);
	return inf;
}



/**
 * Find a minimum spanning tree in an undirected graph. O(e*log n)
 *
 * _ Begin with an array of edges: Graph_biedge mst[e];
 * _ Compute a mst for the graph, returning its cost and a union-find forest:
 *   int forest[n];
 *   int cost = Graph_Kruskal(mst, e, forest, n);
 */
typedef struct { int cost, node1, node2; } Graph_biedge;
static int Graph_bcomp(const void* a, const void* b) { return ((Graph_biedge*)a)->cost - ((Graph_biedge*)b)->cost; }
static int Graph_Kruskal(Graph_biedge* base, int e, int* forest, int n) {
	assert(base!=NULL&&e>=0&&forest!=NULL&&n>=0);
	int i, j, c;
	qsort(base, e, sizeof(*base), Graph_bcomp);
	memset(forest, -1, sizeof(*forest) * n);
	for (i = c = j = 0; i < e; i++) {
		if (Set_test(forest, base[i].node1, base[i].node2) == 0)
			Set_union(forest, base[i].node1, base[i].node2), c += base[i].cost, base[j++] = base[i];
	}
	return c;
}

#endif
