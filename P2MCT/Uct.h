//
//  Uct.hpp
//  Strategy
//
//  Created by 黄杰辰 on 2020/4/15.
//  Copyright © 2020 Yongfeng Zhang. All rights reserved.
//

#ifndef Uct_h
#define Uct_h
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <cmath>
#include <ctime>
#include <cstdlib>
class UctNode{
public:
    UctNode(UctNode* prt, int lx, int ly): parent(prt), lastx(lx), lasty(ly){
        // prt 父状态，t 当前落子方，lx 上一步
        type = prt ? 3 - prt->type : 2;
        bestKid = nullptr;
        child = nullptr;
        w = 0;
        n = selfn = 0;
        tag = 2;
        full = false;
        leaf = false;
        skip = 0;
    }
    inline bool has(int y) {
        // 测试子状态(x, y)是否扩展
        return child && child[y];
    }
    void add(UctNode* p, int N) {
        // 添加子状态
        if(!child) {
            child = new UctNode *[N];
            bestKid = p;
            for(int i = 0; i < N; i++) child[i] = nullptr;
        }
        child[p->lasty] = p;
    }
    inline double confidence(){
        return -1 * (double)w / n + 0.8 * sqrt(2*log(parent->n - parent->selfn) / (double)n);
    }
    friend class Uct;
private:
    UctNode* parent;
    UctNode** child;
    int type; // 1 对方 2 己方
    int w;
    int n, selfn;
    int lastx, lasty;
    int tag; // 1输了 2赢了 3平
    bool full;
    bool leaf;
    int skip;
    UctNode* bestKid;
};

class Uct{
public:
    Uct() = delete;
    Uct(int _m, int _n, int _nox, int _noy, int* top, int** board): M(_m), N(_n), nox(_nox), noy(_noy) {
        root = new UctNode(nullptr, -1, -1); // default: my first step
        curTop = top;
        curBoard = board;
    }
    ~Uct(){
        clear(root, nullptr);
    }
    inline int getX(){
        return root->bestKid->lastx;
    }
    inline int getY(){
        return root->bestKid->lasty;
    }
    inline UctNode* getBest(){
        return root->bestKid;
    }
    bool endCheck();
    void finalCheck();
    void debug(){
        std::cout << "-----------------------------------------------------------" << std::endl;
        std::cout << root->n << std::endl;
        if(root->child)
        for(int j = 0; j < N; j++){
            std::cout << j;
            if(root->child[j]){
                std::cout << "<" << std::setprecision(4) << (double)(root->child[j]->n - root->child[j]->w) / (double)(2 * root->child[j]->n)
                          << "\t" << -root->child[j]->w
                          << "\t" << root->child[j]->n
                          << "\t" << root->child[j]->tag << ">";
                if(root->child[j]->leaf){
                    std::cout << "leaf";
                }
                else std::cout << root->child[j]->skip;
            }
            else{
                std::cout << "NULL";
            }
            std::cout << std::endl;
        }
        std::cout << "-----------------------------------------------------------" << std::endl;
    }
    void updateTag(UctNode*p);
    void updateBest(UctNode*);
    void clear(UctNode*, UctNode*);
    void rotate(UctNode* newroot);
    void move(int lastx, int lasty, int* _t, int** _b);
    void search();
    UctNode* findNext();
    void update(UctNode*);
    int forward(UctNode* const p);
    void backward(UctNode* p, int res);
    void print(const int* t, int* const * b, int x, int y);
    int quickJudge(int lastx, int lasty, int*board, const int*top, int curType, int& nextx, int& nexty);
    int globalJudge(int* const* board, const int* top, int curType, int&nextx, int&nexty);
    bool JudgeWin(const int x, const int y, int** board, int type);
private:
    UctNode* root;
    const int M, N;
    const int nox, noy;
    int** curBoard;
    int* curTop;
    
};

#endif /* Uct_h */
