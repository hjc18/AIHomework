import re
pylist = []
wordlist = []
with open("../corpus/stc_weibo_train_response.txt", "r") as f:
    pat = re.compile(r'[\u4e00-\u9fa5]+')
    listcount = 0
    for line in f:
        linelist = line.strip().split(' ')
        cnt = 0
        for w in linelist:
            if len(w) > 1:
                cnt += 1
        if cnt * 2 < len(linelist):
            continue
        txt = ''.join(linelist)
        res = pat.findall(txt)
        for w in res:
            listcount += 1
            if listcount % 1000 == 0 and len(w) > 1:
                wordlist.append(w+'\n')
f.close()
print(len(wordlist))
# f = open("../data/input_py_set.txt", "w", encoding="gbk")
# f.writelines(pylist)
# f.close()
f = open("../data/input_weibo_response_set.txt", "w", encoding="gbk")
f.writelines(wordlist)