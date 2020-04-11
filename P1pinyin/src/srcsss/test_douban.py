import os
import re
pat = re.compile(r'[\u4e00-\u9fa5]+')
filepath = ['dev.txt', 'test.txt', 'train.txt']
savepath = ['raw_dev.txt', 'raw_test.txt', 'raw_train.txt']

for i, e in enumerate(filepath):
    totalcnt = 0
    wordlist = []
    f = open('../corpus/douban/'+ e , "r")
    for line in f:
        rwline = ''.join(line.split(' '))
        wset = pat.findall(rwline)
        for w in wset:
            totalcnt += len(w)
        wordlist.append(' '.join(wset) + "\n")
    print(totalcnt)
    f = open("../corpus/douban/" + savepath[i], 'w')
    f.writelines(wordlist)
