//
//  Uct.hpp
//  Strategy
//
//  Created by 黄杰辰 on 2020/4/15.
//  Copyright © 2020 Yongfeng Zhang. All rights reserved.
//

#ifndef Uct_hpp
#define Uct_hpp
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
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
        //kids.clear();
        w = n = 0;
        already = 2;
        nextx = -1;
        nexty = -1;
        full = false;
        leaf = false;
        skip = false;
        skipp = false;
    }
    bool has(int y) {
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
//        if(kids.empty()) bestKid = p;
//        kids[p->lasty] = p->id;
    }
    double confidence(int N, double L){
        return -1 * (double)w / n + sqrt(L*log(N) / (double)n);
    }

    friend class Uct;
private:
    UctNode* parent;
    //std::unordered_map<int, int> kids;// 状态转移 y
    UctNode** child;
    int type; // 1 对方 2 己方
    int w, n;
    int lastx, lasty;
    int nextx, nexty;
    int already; // 1输了 2赢了 3平
    bool full;
    bool leaf;
    bool skip;
    bool skipp;
    UctNode* bestKid;
};

class Uct{
public:
    Uct() = delete;
    Uct(int _m, int _n, int _nox, int _noy, int* top, int** board): M(_m), N(_n), nox(_nox), noy(_noy) {
        root = new UctNode(nullptr, -1, -1); // default: my first step
        curTop = top;
        curBoard = board;
        depth =0 ;
    }
    ~Uct(){
        clear(root, nullptr);
//        for(auto e : nodes){
//            if(e) {
//                delete e;
//                e = nullptr;
//            }
//        }
//        nodes.clear();
    }
    int getX(){
        return root->bestKid->lastx;
    }
    int getY(){
        return root->bestKid->lasty;
    }
    UctNode* getBest(){
        return root->bestKid;
    }
    bool precheck(){
        return false;
        if(root->already == -1 || root->already == 1){
            int nextx = root->nextx;
            int nexty = root->nexty;
            UctNode* newroot;
            if(root->has(nexty)) newroot = root->child[nexty];
            else {
                newroot = new UctNode(nullptr, nextx, nexty);
            }
            root->bestKid = newroot;
            return true;
        }
        return false;
    }
    bool endcheck(){
        for(int i = 0; i < N; i++){
            if(root->child[i] && root->child[i]->leaf){
                root->bestKid = root->child[i];
                return true;
            }
        }
        for(int i = 0; i < N; i++){
            if(root->child[i] && root->child[i]->already == -1){
                root->bestKid = root->child[i];
                return true;
            }
        }
        for(int i = 0; i < N; i++){
            if(root->child[i] && root->child[i]->n > root->bestKid->n){
                return false;
            }
        }
        return true;
    }
    void debug(){
        std::cout << "-----------------------------------------------------------" << std::endl;
        std::cout << depth << " " << root->n << std::endl;
        if(root->child)
        for(int j = 0; j < N; j++){
            std::cout << j;
            if(root->child[j]){
                std::cout << "<" << (double)(root->child[j]->n - root->child[j]->w) / (double)(2 * root->child[j]->n) << "," << root->child[j]->n << " " <<root->child[j]->already<<">";
                //std::cout << "<" <<  root->child[j]->w  << "," << root->child[j]->n << ",total:" << root->n << ">";
            }
            else{
                std::cout << "NULL";
            }
            std::cout << std::endl;
        }
        std::cout << "-----------------------------------------------------------" << std::endl;
    }
    void updateBest(UctNode*);
    void clear(UctNode*, UctNode*);
    void rotate(UctNode* newroot);
    void move(int lastx, int lasty, int* _t, int** _b);
    bool search();
    UctNode* findNext();
    void update(UctNode*);
    int forward(UctNode* const p);
    void backward(UctNode* p, int res);
    void print(const int* t, int* const * b);
    int quickJudge(int lastx, int lasty, int* const*board, const int*top, int curType, int& nextx, int& nexty);
    int globalJudge(int* const* board, const int* top, int curType, int&nextx, int&nexty);
private:
    UctNode* root;
    int depth;
    //std::vector<UctNode*> nodes;
    const int M, N;
    const int nox, noy;
    int** curBoard;
    int* curTop;
    
};

#endif /* Uct_hpp */
