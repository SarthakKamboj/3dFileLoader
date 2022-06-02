def solve(nums):
	nums.sort(cmp = lambda x, y: func(x,y))
	print(nums)
	res = "".join(nums[::-1])
	return res

def func(x, y):
	if x == y: return True
	xStr = str(x)
	yStr = str(y)
	for i in range(len(xStr)):
		if i >= len(yStr): return False
		if int(xStr[i]) > int(yStr[i]): return True
		if int(xStr[i]) < int(yStr[i]): return False
	return True

solve([45, 14, 70, 67, 95])
	