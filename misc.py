# find the first point for which formula holds in [a,b], or None if not even on b
def dichotomy(formula, a, b):
	while a != b:
		mid = (a + b + (a > b)) // 2 # round to a
		if formula(mid):
			b = mid
		else:
			a = mid + (1 if a < b else -1)
	return a if formula(a) else None
