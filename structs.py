# Union-Find (on array initialized with -1)
def set_find(s, i):
	while s[i] >= 0:
		i = s[i]
	return i
def set_union(s, i, j):
	i = set_find(s, i)
	j = set_find(s, j)
	if i != j:
		if -s[i] < -s[j]:
			i, j = j, i
		s[i] += s[j]
		s[j] = i



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
			return True, self.root
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
