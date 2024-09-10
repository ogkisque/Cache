from subprocess import run, Popen, PIPE
from sys import executable

num_test = 1
for i in range(1, 11):
	
	str_data =  "tests/"
	if i != 10:
		str_data += "0"
	str_data += str(i) + ".dat"
	file_in = open(str_data, "r")
	str_ans = "tests/"
	if i != 10:
		str_ans += "0"
	str_ans += str(i) + ".dat.ans"
	ans = ""
	for i in open(str_ans):
		ans = i
	result = run("./implementation/cache", capture_output = True, encoding='cp866', stdin=file_in)
	print("Test: " + str(num_test).strip())
	print("Result: " + str(result.stdout).strip())
	if result.stdout == ans:
		print("OK")
	else:
		print("ERROR, correct: " + str(ans).strip())
	print("-------------------------------------------------")
	num_test += 1
