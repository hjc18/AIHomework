from corpus import *
import time


def save_files():
    f = open("../json/multi.txt", "w", encoding='gbk')
    f.writelines(cp.multi)
    f.close()
    f = open("../json/pro2char.json", "w", encoding='gbk')
    json.dump(cp.pro2char, f, indent=2, ensure_ascii=False)
    f.close()
    f = open("../json/single.json", "w", encoding='gbk')
    json.dump(cp.single_char, f, indent=2, ensure_ascii=False)
    f.close()
    f = open("../json/bigram.json", "w", encoding='gbk')
    json.dump(cp.bi_gram, f, indent=2, ensure_ascii=False)
    f.close()
    f = open("../json/char2pro.json", "w", encoding='gbk')
    json.dump(cp.char2pro, f, indent=2, ensure_ascii=False)
    f.close()


file_list = ["2016-02.txt", "2016-04.txt", "2016-05.txt",
             "2016-06.txt", "2016-07.txt", "2016-08.txt",
             "2016-09.txt", "2016-10.txt", "2016-11.txt"]
cp = Corpus()
start = time.time()
print("-" * 80)
cp.get_dictionary("../corpus/一二级汉字表.txt")
cp.get_pro("../corpus/拼音汉字表.txt")
print("| dictionary loaded... <run time {}s> |".format(time.time() - start))
print("-" * 80)
print("-" * 80)
for file in file_list:
    start = time.time()
    cp.tokenize("../corpus/"+file)
    print("| {} tokenized... <run time {:5.2f}s> |".format(file, time.time() - start))
print("-" * 80)
save_files()




