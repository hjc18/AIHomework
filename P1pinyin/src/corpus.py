import os
import json
from pypinyin import lazy_pinyin


class Corpus(object):
    def __init__(self):
        self.pro2char = {}
        self.char2pro = {}
        self.single_char = {}
        self.bi_gram = {}
        self.multi = set()

    def get_pro(self, path):
        assert os.path.exists(path)
        with open(path, 'r', encoding='gbk', errors="ignore") as f:
            for line in f:
                charlist = line.strip().split(" ")
                self.pro2char[charlist[0]] = charlist[1:]
                for e in charlist[1:]:
                    self.char2pro[e].append(charlist[0])
        for k, v in self.char2pro.items():
            if len(v) > 1:
                self.multi.add(k)
                for p in v:
                    i = self.pro2char[p].index(k)
                    self.pro2char[p][i] = k + p
        for v in self.pro2char.values():
            for c in v:
                self.single_char[c] = 0
                self.bi_gram[c] = {}
        f.close()

    def get_dictionary(self, path):
        assert os.path.exists(path)
        with open(path, 'r', encoding='gbk', errors="ignore") as f:
            chars = f.read()
            # original characters (ignore multi-pinyin)
            for c in chars:
                self.char2pro[c] = []
        f.close()

    def tokenize(self, path):
        assert os.path.exists(path)
        with open(path, 'r', encoding='gbk', errors="ignore") as f:
            count = 0
            for line in f:
                count += 1
                news = json.loads(line)
                str = news['title'] + " " + news["html"]
                # if str[0] in self.single_char:
                #     self.single_char[str[0]] += 1
                for i, c in enumerate(str):
                    if c in self.char2pro:
                        read = ""
                        if c in self.multi:
                            buf = str[i-1] + c if i > 0 else " " + c
                            buf = buf + str[i+1] if i < len(str)-1 else buf + " "
                            read = lazy_pinyin(buf)[1]
                            if c+read not in self.single_char:
                                read = self.char2pro[c][0]
                        char = c + read
                        self.single_char[char] += 1
                        # if the character before is also in the dict, count the bi-frequency c(wi, wj)
                        if i > 0 and str[i - 1] in self.char2pro:
                            read = ""
                            if str[i-1] in self.multi:
                                buf = str[i-2] + str[i-1] if i > 1 else " " + str[i-1]
                                buf = buf + c
                                read = lazy_pinyin(buf)[1]
                                if str[i-1]+read not in self.single_char:
                                    read = self.char2pro[str[i-1]][0]
                            beforechar = str[i-1] + read
                            if beforechar in self.bi_gram[char]:
                                self.bi_gram[char][beforechar] += 1
                            else:
                                # new
                                self.bi_gram[char][beforechar] = 1
                if count % 10000 == 0:
                    print("| {} pieces processed |".format(count))
        f.close()







