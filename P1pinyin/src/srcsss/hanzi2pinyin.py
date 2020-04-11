from pypinyin import lazy_pinyin
inlist = ['../data/input_chatbot_set.txt', '../data/input_weibo_response_set.txt']
outlist = ['../data/input_py_chatbot_set.txt', '../data/input_py_weibo_response_set.txt']
for id, p in enumerate(inlist):
    outline = []
    with open(p, 'r', encoding='gbk') as f:
        for line in f:
            lazy = lazy_pinyin(line)
            for i in range(len(lazy)):
                if lazy[i] == "n":
                    lazy[i] = "en"
            sts = ' '.join(lazy)
            sts = sts.replace("nve", "nue")
            sts = sts.replace("lve", "lue")
            outline.append(sts)
    f = open(outlist[id], 'w')
    f.writelines(outline)