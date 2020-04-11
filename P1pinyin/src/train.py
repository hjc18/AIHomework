from corpus import *
import json
import time

def save_files():
    print("saving")
    f = open("../json/multi.txt", "w", encoding='gbk')
    f.writelines(cp.multi)
    f.close()
    f = open("../json/pro2char.json", "w", encoding='gbk')
    json.dump(cp.pro2char, f, indent=2, ensure_ascii=False)
    f.close()
    f = open("../json/tri_single.json", "w", encoding='gbk')
    json.dump(cp.single_char, f, indent=2, ensure_ascii=False)
    f.close()
    f = open("../json/simple_trigram.json", "w", encoding='gbk')
    json.dump(cp.tri_gram, f, indent=2, ensure_ascii=False)
    f = open("../json/tri_bigram.json", "w", encoding='gbk')
    json.dump(cp.bi_gram, f, indent=2, ensure_ascii=False)
    f.close()
    f = open("../json/char2pro.json", "w", encoding='gbk')
    json.dump(cp.char2pro, f, indent=2, ensure_ascii=False)
    f.close()


def load_files():
    corpus = Corpus()
    corpus.tri_gram = json.load(open("../json/simple_trigram.json", 'r', encoding='gbk'))
    corpus.bi_gram = json.load(open("../json/bigram.json", "r", encoding='gbk'))
    corpus.pro2char = json.load(open("../json/pro2char.json", "r", encoding='gbk'))
    corpus.single_char = json.load(open("../json/single.json", "r", encoding='gbk'))
    corpus.char2pro = json.load(open("../json/char2pro.json", "r", encoding='gbk'))
    with open("../json/multi.txt", "r", encoding='gbk') as f:
        for c in f.read().strip():
            corpus.multi.add(c)
    return corpus


cp = load_files()
# for word, prewordd in cp.tri_gram.items():
#     newdic = {}
#     for k, v in prewordd.items():
#         if v > 10:
#             newdic[k] = v
#     cp.tri_gram[word] = newdic
file_list = ["2016-02.txt", "2016-04.txt", "2016-05.txt",
             "2016-06.txt", "2016-07.txt", "2016-08.txt",
             "2016-09.txt", "2016-10.txt", "2016-11.txt",
             'raw_dev.txt', 'raw_test.txt', 'raw_train.txt']
start = time.time()
print("-" * 80)
cp = Corpus()
cp.get_dictionary("../corpus/一二级汉字表.txt")
cp.get_pro("../corpus/拼音汉字表.txt")
print("| dictionary loaded... <run time {}s> |".format(time.time() - start))
print("-" * 80)
print("-" * 80)
for file in file_list:
    start = time.time()
    cp.tokenize_tri("../corpus/r_"+file)
    print("| {} tokenized... <run time {:5.2f}s> |".format(file, time.time() - start))
print("-" * 80)
save_files()




