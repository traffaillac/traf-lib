# graph is a list/dict of adjacency lists, depth and low are None-initialized arrays
def articulations_bridges(graph, depth, low, i, d=0, parent=None):
	depth[i] = low[i] = d
	children = 0
	is_articulation = False
	for j in graph[i]:
		if depth[j] == None:
			articulations_bridges(graph, depth, low, j, d+1, i, res)
			children += 1
			if low[j] >= d:
				is_articulation = True
			if low[j] > d:
				print("bridge between", i, "and", j)
			low[i] = min(low[i], low[j])
		elif j != parent:
			low[i] = min(low[i], depth[j])
	if parent != None and is_articulation or parent == None and children > 1:
		print("articulation at", i)



# graph is a list/dict of adjacency lists, index and low are None-initialized arrays
def tarjan_scc(graph, index, low, i, uuid=0, stack=[]):
	index[i] = low[i] = uuid
	stack.append(i) # condition for i being on stack is low[i]!=None
	for j in graph[i]:
		if index[j] == None: # node was never visited before
			uuid = tarjan_scc(graph, index, low, j, uuid+1, stack)
		if low[j] != None: # node was not visited by another SCC
			low[i] = min(low[i], low[j])
	if index[i] == low[i]:
		while True:
			j = stack.pop()
			low[j] = None
			print(j, end=' ')
			if j == i:
				break
		print()
	return uuid
