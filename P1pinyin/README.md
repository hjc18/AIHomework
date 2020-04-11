# 拼音输入法实验报告

基于隐马尔可夫假设，实现了基于字频的二元、三元拼音输入法，并引入了双向模型提高准确率。基于新浪新闻、豆瓣社区大规模语料训练。在测试集上达到字准确率85-90%，句准确率50-60%。

[TOC]
### 0. 用法说明

#### 0.1. 参数
脚本 `pinyin.py` 默认使用命令行交互式输入输出，默认使用基于字频的单向二元模型，默认只输出一个最优结果。接受以下参数：

```bash
optional arguments:
  -h, --help            帮助信息
  --input, -i           输入文件路径（否则命令行交互式输入）
  --output, -o 					输出文件路径（否则命令行输出）
  --trigram, -t         使用基于字频的三元模型
  --bidir, -b           使用双向模型
  --topk           			输出topk位备选集（否则输出最优结果）
```

#### 0.2. 使用示例

```bash
  python pinyin.py # 默认模式
  python pinyin.py -b # 二元双向模型
  ./pinyin -t # 可执行程序 三元模型
  python pinyin.py -t --topk 3 # 三元模型 输出排名前3的结果
  python pinyin.py -t -b # 三元双向模型（效果最好）
  python pinyin.py -i ../data/input.txt -o ../data/output.txt -t # 使用三元模型，输入文件为input.txt，输出至output.txt
```
#### 0.3. 几点说明

1. 所有输入输出文件都需要用GBK编码。

2. bin目录下有macOS下可执行文件 `pinyin` ，与 `pinyin.py` 脚本使用方法相同，都必须在bin目录下运行。

3. 若使用三元模型，加载字频文件可能需要5-10s。

4. 采用标准输入时，用 `#` 作为退出符。

5. 程序成功加载后，会出现以下提示。

   ```Successfully loaded. Please input pinyin (use '#' to exit).```

6. 相关库

   >  python标准库 `argparse`和 `json` ，外部库 `pypinyin` （仅用于预处理语料）

7. 本地环境

   > macOS 10.13.6
   > python 3.7.5

