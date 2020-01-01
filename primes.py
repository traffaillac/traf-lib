def sieve(n:int):
	primes = [False, True, False, False, False, True]*(n//6+1)
	primes[1] = False
	primes[2] = primes[3] = True
	for i in range(4, int(n**.5)+1):
		if primes[i]:
			for j in range(i*i, n+1, i*2):
				primes[j] = False
	return primes

# Based on http://miller-rabin.appspot.com/
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
