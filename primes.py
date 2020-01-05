from math import gcd

# Eratosthene's sieve
def sieve(n:int):
	primes = [True]*n
	primes[0] = primes[1] = False
	for i in range(2, int(n**.5)+1):
		if primes[i]:
			for j in range(i*i, n, i*2):
				primes[j] = False
	return primes

# Deterministic Miller-Rabin for 64bit (http://miller-rabin.appspot.com/)
def isprime(n:int):
	if n<2: return False
	r = ((n-1)&(1-n)).bit_length()-1
	d = n>>r
	for a in (2, 325, 9375, 28178, 450775, 9780504, 1795265022):
		if a%n == 0: return True
		x = pow(a, d, n)
		if x != 1 and x != n-1:
			for s in range(r-1):
				x = x*x%n
				if x == n-1: break
			else: return False
	return True

# Pollard's rho extraction of unordered prime factors
def factors(n:int):
	def f(x): return (x*x+1)%n
	if n == 1: return ()
	if isprime(n): return (n,)
	for i in range(2, n):
		x, y, d = i, i, 1
		while d == 1:
			x = f(x)
			y = f(f(y))
			d = gcd(abs(x-y), n)
		if d != n: break
	return factors(d) + factors(n//d)
