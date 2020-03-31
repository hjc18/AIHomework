from corpus import *
import time


def load_files():
    corpus = Corpus()
    corpus.bi_gram = json.load(open("../json/bigram.json", "r", encoding='gbk'))
    corpus.pro2char = json.load(open("../json/pro2char.json", "r", encoding='gbk'))
    corpus.single_char = json.load(open("../json/single.json", "r", encoding='gbk'))
    corpus.char2pro = json.load(open("../json/char2pro.json", "r", encoding='gbk'))
    with open("../json/multi.txt", "r", encoding='gbk') as f:
        for c in f.read().strip():
            corpus.multi.add(c)
    return corpus


# initiate
corpus = load_files()
while True:
    inp = input()
    if inp == '#':
        break
    pylist = inp.strip().split(' ')
    last = ''
    prev = []
    pbb = []
    for index, py in enumerate(pylist):
        if py not in corpus.pro2char:
            print("invalid input \"{}\"".format(py))
            break
        py2char_num = len(corpus.pro2char[py])
        prev.append([0] * py2char_num)
        pbb.append([0] * py2char_num)
        if last == '':
            last = py
            for i, e in enumerate(corpus.pro2char[py]):
                pbb[0][i] = corpus.single_char[e]
        else:
            for i, e in enumerate(corpus.pro2char[py]):
                max = 0
                for prei, pre in enumerate(corpus.pro2char[last]):
                    if pre in corpus.bi_gram[e] \
                    and pbb[index-1][prei] * corpus.bi_gram[e][pre] / corpus.single_char[pre]> max:
                        max = pbb[index-1][prei] * corpus.bi_gram[e][pre] / corpus.single_char[pre]
                        prev[index][i] = prei
                pbb[index][i] = max
            last = py
        if index == len(pylist)-1:
            max = 0
            res = 0
            for i, e in enumerate(pbb[index]):
                if e > max:
                    max = e
                    res = i
            back = index
            words = []
            while back >= 0:
                word = corpus.pro2char[pylist[back]][res]
                if len(word) > 1:
                    word = word[0:1]
                words.append(word)
                res = prev[back][res]
                back -= 1
            print(''.join(reversed(words)))




