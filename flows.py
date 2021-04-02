# Edmonds-Karp O(E²V) - iteratively fills shortest paths with max capacity
def maxflow(C, s, t):
	flow = 0 # maximum flow
	N = len(C) # number of nodes
	F = [[0] * N for _ in range(N)] # edge flows
	P = [-1] * N # index of previous node
	Q = [] # BFS queue
	
	while True:
		M = [0] * N # maximum flow going through node
		M[s] = 1_000_000_000
		Q.append(s)
		while Q[0] != t:
			u = Q.pop(0)
			for v in range(N):
				f = min(C[u][v] - F[u][v], M[u])
				if M[v] < f:
					if M[v] == 0:
						Q.append(v)
					M[v] = f
					P[v] = u
			if not Q:
				return flow, F
			M[u] = 1_000_000_000 # make this node "unvisitable"
		Q.clear()
		flow += M[t]
		u, v = P[t], t
		while v != s:
			F[u][v] += M[t]
			F[v][u] -= M[t]
			u, v = P[u], u

# should solve Kattis maxflow
n, m, s, t = map(int, input().split())
C = [[0] * n for _ in range(n)]
for i in range(m):
	u, v, c = map(int, input().split())
	C[u][v] = c
f, F = maxflow(C, s, t)
print(n, f, sum(F[u][v] > 0 for u in range(n) for v in range(n)))
for u in range(n):
	for v in range(n):
		if F[u][v] > 0:
			print(u, v, F[u][v])
