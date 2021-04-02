from bisect import bisect_left

def lis(A):
	M = [] # index and value of last element for each length of LIS
	P = [-1] * len(A) # index of predecessor for LIS ending here
	for i, a in enumerate(A):
		j = bisect_left(M, a << 32)
		if j == len(M):
			M.append(a << 32 | i)
		else:
			M[j] = a << 32 | i
		if j > 0:
			P[i] = M[j - 1] & 0xffffffff
	i = M[-1] & 0xffffffff
	for j in range(len(M) - 1, -1, -1):
		M[j] = i
		i = P[i]
	return M

# solves Kattis longincsubseq
for l in stdin:
	N = int(l)
	L = lis([int(i) for i in stdin.readline().split()])
	print(len(L))
	print(' '.join(map(str, L)))
