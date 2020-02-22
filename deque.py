# same as collections.deque, but with O(1) random access
class deque:
	def __init__(self, buf=[None]*8, start=0, end=0):
		self.buf = buf
		self.start = start
		self.end = end
	def __len__(self):
		b, s, e = self.buf, self.start, self.end
		return e - s if e >= s else e + len(b) - s
	def __getitem__(self, i):
		return self.buf[self._shift(i)]
	def __setitem__(self, i, v):
		self.buf[self._shift(i)] = v
	
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



if __name__ == "__main__":
	print("__init__ tests: ", end="")
	d = deque(["C", "D", None, None, "A", "B"], 4, 2)
	e = deque(["C", "D", None, None, "A", "B"], 4, 2)
	f = deque(["A", "B", "C", "D", None], 0, 4) # TODO
	empty = deque()
	print("\x1b[32mOK\x1b[0m")
	print("__len__ tests: ", end="")
	assert len(d) == 4
	assert len(empty) == 0
	print("\x1b[32mOK\x1b[0m")
	print("__getitem__ tests: ", end="")
	for i in range(4):
		assert d[i] == ["A", "B", "C", "D"][i]
	for i in range(-1, -5, -1):
		assert d[i] == ["A", "B", "C", "D"][i]
	# TODO: vérifier que d[4], d[-5], empty[0] et empty[-1] plantent
	print("\x1b[32mOK\x1b[0m")
	print("__setitem__ tests: ", end="")
	for i in range(-1, -5, -1):
		d[i] = ["D", "C", "B", "A"][i]
	for i in range(4):
		assert d[i] == ["D", "C", "B", "A"][i]
	print("\x1b[32mOK\x1b[0m")
	print("append tests: ", end="")
	for v in ("E", "F", "G", "H"):
		e.append(v)
	for i in range(8):
		assert e[i] == ("A", "B", "C", "D", "E", "F", "G", "H")[i]
	print("\x1b[32mOK\x1b[0m")
	print("appendleft tests: ", end="")
	for v in ("E", "F", "G", "H"):
		d.appendleft(v)
	for i in range(8):
		assert d[-1-i] == ("A", "B", "C", "D", "E", "F", "G", "H")[i]
	print("\x1b[32mOK\x1b[0m")
