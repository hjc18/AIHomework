from corpus import *
import json
import time
# for sinanews and char-based bi-gram L -> 0.84


def load_files():
    start = time.time()
    corpus = Corpus()
    corpus.tri_gram = json.load(open("../json/simplesimple_trigram.json", "r", encoding='gbk'))
    corpus.bi_gram = json.load(open("../json/tri_bigram.json", "r", encoding='gbk'))
    corpus.pro2char = json.load(open("../json/pro2char.json", "r", encoding='gbk'))
    corpus.single_char = json.load(open("../json/tri_single.json", "r", encoding='gbk'))
    corpus.char2pro = json.load(open("../json/char2pro.json", "r", encoding='gbk'))
    with open("../json/multi.txt", "r", encoding='gbk') as f:
        for c in f.read().strip():
            corpus.multi.add(c)
    print(time.time()-start)
    return corpus


# initiate
corpus = load_files()
total_single = sum(corpus.single_char.values())

# bigram model
def cal(_pre, _e, L):
    xishu = 0.0 if _pre not in corpus.bi_gram[_e] else corpus.bi_gram[_e][_pre]
    xishu2 = 1e10 if corpus.single_char[_pre] == 0 else corpus.single_char[_pre]
    return L * xishu / xishu2 + (1-L) * corpus.single_char[_e] / total_single


def cal_re(_e, _next, L):
    xishu = 0.0 if _e not in corpus.bi_gram[_next] else corpus.bi_gram[_next][_e]
    xishu2 = 1e10 if corpus.single_char[_next] == 0 else corpus.single_char[_next]
    return L * xishu / xishu2 + (1-L) * corpus.single_char[_e] / total_single


def bi_direction(pylist, bidir, trigram, L):
    pre_cnt = 3
    final = {}
    for loop in range(2):
        # 1 loop left to right
        # 2 loop right to left
        last1 = ''
        last2 = ''
        pbb = []
        if loop == 1:
            pylist = reversed(pylist)
        for index, py in enumerate(pylist):
            if py not in corpus.pro2char:
                return "invalid input \"{}\"".format(py)
            curlist = []
            pbb.append(curlist)
            for e in corpus.pro2char[py]:
                curdic = {}
                # dic := {pre string : (pre two word, pre two word cnt, total pbb) }
                if last1 == '':
                    curdic[e[0]] = (0, 0, corpus.single_char[e])
                elif last2 == '':
                    for prei, pre in enumerate(corpus.pro2char[last1]):
                        if loop == 0 and (pre in corpus.bi_gram[e] or len(curdic) == 0):
                            for ppre, ppre_val in pbb[index - 1][prei].items():
                                word = ppre + e[0]
                                xishu2 = 1e10 if pre not in corpus.bi_gram[e] else corpus.bi_gram[e][pre]
                                curdic[word] = (pre + e, xishu2, ppre_val[2] * cal(pre, e, 0.89))
                        if loop == 1 and (e in corpus.bi_gram[pre] or len(curdic) == 0):
                            for nn, nn_val in pbb[index - 1][prei].items():
                                word = e[0] + nn
                                xishu2 = 1e10 if e not in corpus.bi_gram[pre] else corpus.bi_gram[pre][e]
                                curdic[word] = (pre, xishu2, nn_val[2] * cal_re(e, pre, 0.89))
                else:
                    for prei, pre in enumerate(corpus.pro2char[last1]):
                        if loop == 0 and (pre in corpus.bi_gram[e] or len(curdic) == 0):
                            for ppre, ppre_set in pbb[index - 1][prei].items():
                                word = ppre + e[0]
                                pretwo = ppre_set[0]
                                xishu = 0.0 if pretwo not in corpus.tri_gram[e] else corpus.tri_gram[e][pretwo]
                                xishu2 = 1e10 if pre not in corpus.bi_gram[e] else corpus.bi_gram[e][pre]
                                curdic[word] = (pre + e, xishu2, ppre_set[2] * (
                                            L * (xishu + 1) / (ppre_set[1] + 7497) + (1-L) * cal(pre, e, 0.89)))
                        if loop == 1 and (e in corpus.bi_gram[pre] or len(curdic) == 0):
                            for nn, nn_set in pbb[index - 1][prei].items():
                                word = e[0] + nn
                                two = e + pre
                                xishu = 0.0 if two not in corpus.tri_gram[nn_set[0]] else corpus.tri_gram[nn_set[0]][two]
                                xishu2 = 1e10 if e not in corpus.bi_gram[pre] else corpus.bi_gram[pre][e]
                                curdic[word] = (pre, xishu2, nn_set[2] * (
                                            L * (xishu + 1) / (nn_set[1] + 7497) + (1-L) * cal_re(e, pre, 0.89)))
                if last1 != '':
                    sorted_list = sorted(curdic.items(), key=lambda x: x[1][2], reverse=True)[0:pre_cnt]
                    curdic = dict(sorted_list)
                pbb[index].append(curdic)
            if trigram:
                last2 = last1
            last1 = py
        for i, e in enumerate(pbb[-1]):
            for k, v in e.items():
                if loop == 1 and k in final:
                    final[k] += v[2]
                else:
                    final[k] = v[2]
        if len(pbb) == 1 or not bidir:
            break
    return sorted(final.items(), key=lambda x: x[1], reverse=True)[0][0]


def viterbi(pylist, is3, L):
    last1 = ''
    last2 = ''
    _prev = []
    _pbb = []
    for index, py in enumerate(pylist):
        if py not in corpus.pro2char:
            return [], py
        py2char_num = len(corpus.pro2char[py])
        _prev.append([0] * py2char_num)
        _pbb.append([0] * py2char_num)
        if last1 == '':
            for i, e in enumerate(corpus.pro2char[py]):
                _pbb[0][i] = corpus.single_char[e]
        elif last2 == '':
            for i, e in enumerate(corpus.pro2char[py]):
                max = 0.0
                for prei, pre in enumerate(corpus.pro2char[last1]):
                    if pre in corpus.bi_gram[e] and _pbb[index - 1][prei] * cal(pre, e, 0.89) > max:
                        max = _pbb[index-1][prei] * cal(pre, e, 0.89)
                        _prev[index][i] = prei
                _pbb[index][i] = max
        else:
            for i, e in enumerate(corpus.pro2char[py]):
                max = 0.0
                for prei, pre in enumerate(corpus.pro2char[last1]):
                    ppre = corpus.pro2char[last2][_prev[index - 1][prei]]
                    preword = ppre + pre
                    if pre not in corpus.bi_gram[e]:
                        continue
                    xishu = 0.0 if preword not in corpus.tri_gram[e] else corpus.tri_gram[e][preword]
                    xishu2 = 1e6 if ppre not in corpus.bi_gram[pre] else corpus.bi_gram[pre][ppre]
                    if _pbb[index - 1][prei] * (
                            L * (xishu + 1) / (xishu2 + 7497) + (1-L) * cal(pre, e, 0.89)) > max:
                        max = _pbb[index - 1][prei] * (
                                L * (xishu + 1) / (xishu2 + 7497) + (1-L) * cal(pre, e, 0.89))
                        _prev[index][i] = prei
                _pbb[index][i] = max
        if is3:
            last2 = last1
        last1 = py
    return _prev, _pbb


def output(_pylist, _prev, _pbb):
    max = 0
    res = 0
    for i, e in enumerate(_pbb[-1]):
        if e > max:
            max = e
            res = i
    back = len(_pylist) - 1
    words = []
    while back >= 0:
        word = corpus.pro2char[_pylist[back]][res]
        if len(word) > 1:
            word = word[0:1]
        words.append(word)
        res = _prev[back][res]
        back -= 1
    return ''.join(reversed(words))


def convert(com, bidir, tirgram, L):
    if bidir:
        return bi_direction(com, True, tirgram, 0.8)
    prev, pbb = viterbi(com, tirgram, 0.8)
    if len(prev) == 0:
        return "invalid input \"{}\"".format(pbb)
    return output(com, prev, pbb)

def score(word, ans):
    error = 0
    for i, c in enumerate(word):
        if c != ans[i]:
            error += 1
    return error


inlist = ['../data/input1.txt',
          '../data/input2.txt',
          '../data/input_py_chatbot_set.txt',
          '../data/input_py_weibo_response_set.txt']
outlist = ['../data/ans1.txt',
           '../data/ans2.txt',
           '../data/input_chatbot_set.txt',
           '../data/input_weibo_response_set.txt']
# main
s = [(False, False), (True, False), (False, True), (True, True)]
for lam in range(4):
    print("*" * 90)
    print("| lambda :{} ".format(lam/10))
    totalcnt = 0
    errorcnt = 0
    lltotal = 0
    error = 0
    for id, p in enumerate(inlist):
        f = open(outlist[id], 'r', encoding='gbk')
        result = f.readlines()
        with open(p, 'r', encoding='gbk') as f:
            start = time.time()
            for i, line in enumerate(f):
                # bires, bis = pinyin2hanzi(line.strip().lower().split(' '), 0.95)
                trires = convert(line.strip().lower().split(' '), s[lam][0], s[lam][1], lam/10)
                errorcnt += score(trires, result[i])
                totalcnt += len(result[i].strip())
                lltotal += 1
                if trires != result[i].strip():
                    error += 1
        print("| char: {:.2%} / {} sentence: {:.2%} / {} <{}s> for \'{}\' ".format(1-errorcnt/totalcnt, totalcnt, 1-error/lltotal, lltotal, time.time()-start, p))
        # print("| {} : {}".format(getcnt2, getcnt3))


