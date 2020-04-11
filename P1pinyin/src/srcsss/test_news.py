import json

hanzi_set = set()
with open("../corpus/一二级汉字表.txt", "r", encoding='gbk') as f:
    for c in f.read().strip():
        hanzi_set.add(c)
file_list = ["2016-02.txt", "2016-04.txt", "2016-05.txt",
             "2016-06.txt", "2016-07.txt", "2016-08.txt",
             "2016-09.txt", "2016-10.txt", "2016-11.txt"]
savepath = ['raw_dev.txt', 'raw_test.txt', 'raw_train.txt']

for e in savepath:
    f = open('../corpus/douban/'+e, "r")
    wordlist = []
    for line in f:
        rr = ''
        for char in line:
            if char not in hanzi_set:
                rr += ' '
            else:
                rr += char
        rwline = rr.split(' ')
        newline = ''
        for token in rwline:
            token = token.strip()
            if len(token) > 2:
                newline += token + ' '
        wordlist.append(newline + "\n")
    f = open("../corpus/r_" + e, 'w', encoding='gbk')
    f.writelines(wordlist)
    print("{} finished".format(e))
