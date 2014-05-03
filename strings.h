/**
 * Author: Thibault Raffaillac <traf@kth.se>
 */

#ifndef STRINGS_H
#define STRINGS_H

#include <assert.h>
#include <stdio.h>
#include <sys/types.h>



/**
 * Find occurences of one substring in many strings. O(n)
 *
 * _ Begin with a KMP_item substring with value initialized: KMP_item sub[n];
 * _ Preprocess it: KMP_init(sub);
 * _ Print the occurences found in str: KMP_find(sub, str);
 */
typedef struct KMP_item { struct KMP_item* back; int value; } KMP_item;
static void KMP_init(KMP_item* sub) {
	assert(sub!=NULL);
	KMP_item *i, *pos;
	for (sub->back = NULL, i = pos = sub; i->value != '\0';) {
		if ((++i)->value == pos->value)
			i->back = pos->back, pos++;
		else
			i->back = pos, pos = sub;
	}
}
static void KMP_find(const KMP_item* sub, const char* str) {
	assert(sub!=NULL&&str!=NULL);
	const KMP_item* i;
	int s;
	for (i = sub, s = 0; str[s] != '\0'; s++) {
		for (; i != NULL && i->value != str[s]; i = i->back);
		i = (i == NULL) ? sub : i + 1;
		if (i->value == '\0') {
			printf("%d\n", s);
			i = i->back; /* sub here to disable overlapping detection */
		}
	}
}



/**
 * Find occurences of a set of substrings in many strings. O(n)
 *
 * _ Begin with a string of linked GKMP_item lists, setting end = string_index
 *   for each last item: GKMP_item storage[s]; GKMP_item* subs[n+1];
 * _ For small alphabets, sort subs: qsort(subs, n, sizeof(*subs), GKMP_comp);
 * _ Preprocess it: GKMP_end ends[n]; GKMP_item* start = GKMP_init(subs, ends);
 * _ Print the occurences found in str: GKMP_find(start, ends, str);
 */
typedef struct GKMP_end { ssize_t size, next; } GKMP_end;
typedef struct GKMP_item { struct GKMP_item *back, *next; ssize_t end; int value; } GKMP_item;
int GKMP_comp(const void* a, const void* b) {
	const GKMP_item *i = *(const GKMP_item**)a, *j = *(const GKMP_item**)b;
	int d = 0, l = 8;
	while (d == 0 && l != 0 && i != NULL && j != NULL)
		d = i->value - j->value, l--, i = i->next, j = j->next;
	return d;
}
GKMP_item* GKMP_init(GKMP_item** subs, GKMP_end* ends) {
	GKMP_item *s, *i, *b, *n, **r, **w;
	ssize_t e;
	for (s = NULL, r = subs; (i = *r) != NULL; s = i, r++)
		i->back = s;
	do {
		for (r = w = subs; (i = *r) != NULL; r++) {
			b = i->back, n = i->next;
			if (b != NULL && i->value == b->value) i->back = b->back;
			else for (; b != NULL && i->value != b->value; b = b->back);
			if (b == NULL) e = -1, b = s;
			else e = b->end, b = b->next;
			if (n != NULL) i->end = e, *w++ = n, n->back = b;
			else assert(i->end>=0), ends[i->end].next = e, i->next = b;
		}
	} while (*w = NULL, *subs != NULL);
	return s;
}
void GKMP_find(const GKMP_item* start, const GKMP_end* ends, const char* str) {
	const GKMP_item* i;
	ssize_t s, e;
	for (i = start, s = 0; str[s] != '\0'; s++) {
		for (; i != NULL && i->value != str[s]; i = i->back);
		if (i == NULL)
			i = start;
		else for (e = i->end, i = i->next; e >= 0; e = ends[e].next)
			printf("%ld, %ld\n", s, e);
	}
}

#endif
