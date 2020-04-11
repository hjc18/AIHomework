import os
f = open("../data/input_set.txt", "r", encoding="gbk")
lines = f.readlines()
i = 0
words = []
pys = []
while i + 1 < len(lines):
    pys.append(lines[i])
    words.append(lines[i+1])
    i += 2
f.close()
f = open("../data/input1.txt", "w", encoding="gbk")
f.writelines(pys)
f.close()
f = open("../data/ans1.txt", "w", encoding="gbk")
f.writelines(words)
f.close()
words = []
pys = []
i = 0
f = open("../data/input_lastyear.txt", "r", encoding="gbk")
lines = f.readlines()
while i + 1 < len(lines):
    pys.append(lines[i])
    words.append(lines[i+1])
    i += 2
f.close()
f = open("../data/input2.txt", "w", encoding="gbk")
f.writelines(pys)
f.close()
f = open("../data/ans2.txt", "w", encoding="gbk")
f.writelines(words)
f.close()

