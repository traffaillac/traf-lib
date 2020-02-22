# Union-Find (on array initialized with -1)
def set_find(s:list, i:int):
	while s[i] >= 0:
		i = s[i]
	return i
def set_union(s:list, i:int, j:int):
	i = set_find(s, i)
	j = set_find(s, j)
	if i != j:
		if -s[i] < -s[j]:
			i, j = j, i
		s[i] += s[j]
		s[j] = i



# Fenwick tree (on array initialized with 0)
def fenwick_add(a:list, i:int, diff:int):
	while i < len(a):
		a[i] += diff
		i |= i + 1
def fenwick_sum(a:list, i:int):
	s = 0
	while i >= 0:
		s += a[i]
		i = (i & (i + 1)) - 1
	return s



# 2D Fenwick tree (on array of 0-initialized arrays)
def fenwick2_add(a:list, i:int, j:int, diff:int):
	while i < len(a):
		b, k = a[i], j
		while k < len(b):
			b[k] += diff
			k |= k + 1
		i |= i + 1
def fenwick2_sum(a:list, i:int, j:int):
	s = 0
	while i >= 0:
		b, k = a[i], j
		while k >= 0:
			s += b[k]
			k = (k & (k + 1)) - 1
		i = (i & (i + 1)) - 1
	return s



# Balanced and order statistic tree that is simple and fast in practice
class BTree:
	def __init__(self, capacity=512):
		self.nodes = [[]]
		self.capacity = capacity
	def __len__(self):
		return sum(len(n) for n in self.nodes)
	def __iter__(self):
		for n in self.nodes:
			for p in n:
				yield p # tuple (key, value)
	
	def get(self, key, default=None):
		n = next((n for n in self.nodes if n[-1][0]>=key), [])
		return next((p[1] for p in n if p[0]==key), default)
	
	def insert(self, key, value, overwrite=True):
		N, C = self.nodes, self.capacity
		i, n = next(((i, n) for i, n in enumerate(N) if n and n[-1][0]>=key), (len(N)-1, N[-1]))
		j, k = next(((j, p[0]) for j, p in enumerate(n) if p[0]>=key), (len(n), None))
		if k == key:
			n[j] = (key, value) if overwrite else n[j]
			return overwrite, n[j][1]
		if len(n) == C:
			N[i] = n[C//2:]
			N.insert(i, n[:C//2])
			n, j = (N[i], j) if j<=C//2 else (N[i+1], j-C//2)
		n.insert(j, (key, value))
		return True, value



# Binary tree with red-black balancing (https://en.wikipedia.org/wiki/Red-black_tree)
# Nodes are (key, value, parent, left, right, color)
class Tree:
	def __init__(self):
		self.root = None
		self.len = 0
	def __len__(self):
		return self.len
	
	def __iter__(self):
		stack = []
		node = self.root
		while stack or node:
			while node:
				stack.append(node)
				node = node[3]
			node = stack.pop()
			yield (node[0], node[1])
			node = node[4]
	
	def get(self, key, default=None):
		node = self.root
		while node:
			if node[0] == key: return node[1]
			node = node[3 if key < node[0] else 4]
		return default
	
	def insert(self, key, value, overwrite=True):
		N = self.root
		if not N:
			self.len += 1
			self.root = [key, value, None, None, None, 'black']
			return True, value
		while N:
			P = N
			if key == P[0]:
				P[1] = value if overwrite else P[1]
				return overwrite, P[1]
			N = P[3 if key < P[0] else 4]
		self.len += 1
		N = P[3 if key < P[0] else 4] = [key, value, P, None, None, 'red']
		
		while P and P[5] == 'red':
			G = P[2]
			gu = 4 if P is G[3] else 3
			U = G[gu]
			if not U or U[5] == 'black':
				if N is P[gu]:
					N[7-gu], N[7-gu][2], P[gu], N[2], G[7-gu], P[2] = P, P, N[7-gu], G, N, N
				if G[2]: G[2][3 if G is G[2][3] else 4] = P
				else: self.root = P
				P[gu], P[gu][2], G[7-gu], P[2], G[2] = G, G, P[gu], G[2], P
				P[5] = 'black'
				G[5] = 'red'
				break
			P[5] = U[5] = 'black'
			N, P = G, G[2]
			if P: N[5] = 'red'
		return True, value
