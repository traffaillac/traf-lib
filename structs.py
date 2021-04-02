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



# Balanced tree that is simple and fast in practice
from bisect import bisect, bisect_left
class BTree:
	def __init__(self, capacity=512):
		self.nodes = [[]]
		self.capacity = capacity
	def __len__(self):
		return sum(len(n) for n in self.nodes)
	def __iter__(self):
		for n in self.nodes:
			for k in n:
				yield k
	def __reversed__(self):
		for n in reversed(self.nodes):
			for k in reversed(n):
				yield k
	def __repr__(self):
		return f"[{', '.join(self)}]"
	
	def add(self, key, overwrite=None): # use None for a multiset
		N, C = self.nodes, self.capacity
		i, n = next(((i, n) for i, n in enumerate(N) if n and n[-1]>=key), (len(N)-1, N[-1]))
		j = bisect(n, key)
		# use tuples for keys and update this equality to turn the set into a map
		if j > 0 and n[j-1] == key and overwrite is not None:
			n[j] = key if overwrite else n[j]
			return overwrite
		if len(n) == C:
			N[i] = n[C//2:]
			N.insert(i, n[:C//2])
			n, j = (N[i], j) if j<=C//2 else (N[i+1], j-C//2)
		n.insert(j, key)
		return True
	def get_lower(self, key, default=None, delete=False):
		n = next((n for n in reversed(self.nodes) if n and n[0]<=key), [])
		j = bisect(n, key) - 1
		return default if j<0 else n.pop(j) if delete else n[j]
	def get_upper(self, key, default=None, delete=False):
		n = next((n for n in self.nodes if n and n[-1]>=key), [])
		j = bisect_left(n, key)
		return default if j==len(n) else n.pop(j) if delete else n[j]



# same as collections.deque, but with O(1) random access
class deque:
	def __init__(self, buf=None, start=0, end=0):
		self.buf = buf if buf else [None] * 1000001
		self.start = start
		self.end = end
	def __len__(self):
		b, s, e = self.buf, self.start, self.end
		return e - s if e >= s else e + len(b) - s
	def __getitem__(self, i):
		return self.buf[self._shift(i)]
	def __setitem__(self, i, v):
		self.buf[self._shift(i)] = v
	def __repr__(self):
		return "[" + ", ".join(map(repr, self)) + "]"
	
	def append(self, x):
		b, s, e = self.buf, self.start, self.end
		b[e] = x
		self.end = e = e+1 if e+1<len(b) else 0
		self._tail(b, s, e)
	def appendleft(self, x):
		b, s, e = self.buf, self.start, self.end
		self.start = s = s-1 if s>0 else len(b)-1
		b[s] = x
		self._tail(b, s, e)
	def pop(self):
		b, s, e = self.buf, self.start, self.end
		self.end = e = e-1 if e>0 else len(b)-1
		v, b[e] = b[e], None
		return v
	def popleft(self):
		b, s, e = self.buf, self.start, self.end
		v, b[s] = b[s], None
		self.start = s+1 if s+1<len(b) else 0
		return v
	
	def _shift(self, i):
		b, s, e = self.buf, self.start, self.end
		if not -len(self) <= i < len(self):
			raise IndexError
		if i >= 0:
			return s+i if s+i<len(b) else i-len(b)+s
		else:
			return e+i if e+i>=0 else i+e+len(b)
	def _tail(self, b, s, e):
		if s == e:
			self.buf = b[:s] + [None]*len(b) + b[s:]
			self.start = s + len(b)



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
