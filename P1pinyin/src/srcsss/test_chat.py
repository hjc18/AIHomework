import os
import re
pat = re.compile(r'[\u4e00-\u9fa5]+')
root = '../corpus/chinese'
filepath = []
wordlist = []
for a,b, files in os.walk(root):
    for fpath in files:
        filepath.append(os.path.join(root, fpath))

for e in filepath:
    f = open(e, "r")
    for line in f:
        res = pat.findall(line)
        for w in res:
            if len(w) > 1:
                wordlist.append(w+'\n')
    f.close()

print(len(wordlist))
f = open("../data/input_chatbot_set.txt", "w", encoding="gbk")
f.writelines(wordlist)
