/**
 * Author: Thibault Raffaillac <traf@kth.se>
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <assert.h>
#include <math.h>
#include <stdlib.h>



/**
 * Basic functions to handle points in the plane.
 * All of the algorithms can be adapted with minimal effort to run on integers
 * (with products of two coordinates unless specified), or 3D.
 */
typedef double Point_unit;
static Point_unit pmin(Point_unit a, Point_unit b) { return (a < b) ? a : b; }
static Point_unit pmax(Point_unit a, Point_unit b) { return (a > b) ? a : b; }
typedef struct { Point_unit x, y; } Point;
static Point Point_add(Point a, Point b) { Point p = {a.x + b.x, a.y + b.y}; return p; }
static Point Point_sub(Point a, Point b) { Point p = {a.x - b.x, a.y - b.y}; return p; }
static Point Point_mul(Point a, Point_unit d) { Point p = {a.x * d, a.y * d}; return p; }
static Point_unit Point_dot(Point a, Point b) { return a.x * b.x + a.y * b.y; }
static Point_unit Point_cross(Point a, Point b) { return a.x * b.y - b.x * a.y; }
static double Point_L1(Point a) { return fabs(a.x) + fabs(a.y); }
static double Point_L2(Point a) { return sqrt(a.x * a.x + a.y * a.y); }



/* Compute the area of a polygon (negative if points are sent clockwise). */
static double polygon_area(const Point* poly, int n) {
	assert(poly!=NULL);
	Point_unit sum = 0.0;
	int old, cur;
	for (old = n - 1, cur = 0; cur < n; old = cur++)
		sum += Point_cross(poly[old], poly[cur]);
	return 0.5 * sum;
}



/* Point in polygon query (-1=border, 0=out, 1=in). */
static int pip_mask(Point p, Point_unit z) { return ((p.x < -z) << 3) | ((p.x > z) << 2) | ((p.y < -z) << 1) | (p.y > z); }
static const int pip_inc[11][11] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 4, 0, 2, -1, 3, 0, 2, 1, 3}, {0, 4, 0, 0, 2, 3, 1, 0, 2, 3, -1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 2, 2, 0, 0, 1, -1, 0, 4, 3, 3},
	{0, 1, 3, 0, -1, 0, -2, 0, 3, 2, 4}, {0, 3, -1, 0, 1, 2, 0, 0, 3, 4, -2},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 2, 2, 0, 4, 3, 3, 0, 0, -1, 1},
	{0, -1, 3, 0, 3, -2, 4, 0, 1, 0, 2}, {0, 3, 1, 0, 3, 4, 2, 0, -1, -2, 0}};
static int point_in_polygon(const Point* p, const Point* poly, int n, Point_unit zero) {
	assert(p!=NULL&&poly!=NULL);
	Point cur, old = Point_sub(poly[n - 1], *p);
	int winding, i, mask_cur, mask_old = pip_mask(old, zero);
	for (i = winding = 0; i < n; old = cur, mask_old = mask_cur, i++) {
		cur = Point_sub(poly[i], *p);
		mask_cur = pip_mask(cur, zero);
		int inc = pip_inc[mask_old][mask_cur];
		if ((mask_old & mask_cur) == 0) {
			Point_unit cross = Point_cross(old, cur), limit = zero * Point_L1(Point_sub(cur, old));
			inc = (cross < -limit) ? -inc : (cross <= limit) ? 0 : inc;
			if (inc == 0)
				return -1;
		}
		winding += inc;
	}
	return (winding != 0);
}



/* Intersection of two segments. Returns the number of points found (0 to 2). */
static int segment_intersection(const Point* s1, const Point* e1, const Point* s2, const Point* e2, double i[2][2], Point_unit zero) {
	const Point v1 = Point_sub(*e1, *s1), v2 = Point_sub(*e2, *s2), w = Point_sub(*s2, *s1), *i1, *i2;
	Point_unit det = Point_cross(v1, v2), t1 = Point_cross(w, v1), t2 = Point_cross(w, v2), n = Point_L1(v1);
	if (n <= zero) {
		if (Point_L1(v2) <= zero) {
			i[0][0] = s1->x, i[0][1] = s1->y;
			return Point_L1(w) <= zero;
		} else return segment_intersection(s2, e2, s1, e1, i, zero);
	}
	if (det < 0)
		det = -det, t1 = -t1, t2 = -t2;
	if (det <= zero * n) { /* v1 and v2 are parallel, or v2 is a point. */
		Point_unit a = Point_dot(v1, *s1), b = Point_dot(v1, *e1), c = Point_dot(v1, *s2), d = Point_dot(v1, *e2);
		if (fabs(t1) > zero * n || pmax(a, pmin(c, d)) - pmin(b, pmax(c, d)) > zero * n)
			return 0;
		i1 = (pmin(c, d) < a) ? s1 : (c < d) ? s2 : e2;
		i2 = (pmax(c, d) > b) ? e1 : (c > d) ? s2 : e2;
		i[0][0] = i1->x, i[0][1] = i1->y, i[1][0] = i2->x, i[1][1] = i2->y;
		return 2 - (Point_L1(Point_sub(*i1, *i2)) <= zero);
	}
	i[0][0] = (double)v1.x * t2 / det + s1->x, i[0][1] = (double)v1.y * t2 / det + s1->y;
	return (t1 >= -zero * n && t1 - det <= zero * n && t2 >= -zero * n && t2 - det <= zero * n);
}



/* Distance between segment and point/segment. */
static double segment_point(const Point* s, const Point* e, const Point* p, Point_unit zero) {
	Point v = Point_sub(*e, *s), w = Point_sub(*p, *s);
	if (Point_L1(v) <= zero)
		return Point_L2(w);
	Point_unit t = Point_dot(v, w), n = Point_dot(v, v);
	return (t <= 0) ? Point_L2(w) : (t >= n) ? Point_L2(Point_sub(*p, *e)) : fabs(Point_cross(v, w) / sqrt(n));
}
static double segment_segment(const Point* s1, const Point* e1, const Point* s2, const Point* e2, Point_unit zero) {
	const Point v1 = Point_sub(*e1, *s1), v2 = Point_sub(*e2, *s2), w = Point_sub(*s2, *s1);
	Point_unit det = Point_cross(v1, v2), t1 = Point_cross(w, v1), t2 = Point_cross(w, v2), d = Point_dot(v1, v1);
	double n = sqrt(d);
	if (n <= zero)
		return segment_point(s2, e2, s1, zero);
	if (det < 0)
		det = -det, t1 = -t1, t2 = -t2;
	if (det <= zero * n)
		det = d, t2 = Point_dot(w, v1), t1 = -t2;
	double d1 = (t1 < 0) ? segment_point(s1, e1, s2, zero) : (t1 > det) ? segment_point(s1, e1, e2, zero) : HUGE_VAL,
		d2 = (t2 < 0) ? segment_point(s2, e2, s1, zero) : (t2 > det) ? segment_point(s2, e2, e1, zero) : (d1 == HUGE_VAL) ? 0.0 : HUGE_VAL;
	return (d1 < d2) ? d1 : d2;
}

#endif
