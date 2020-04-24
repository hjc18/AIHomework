//
//  Uct.cpp
//  Strategy
//
//  Created by 黄杰辰 on 2020/4/15.
//  Copyright © 2020 Yongfeng Zhang. All rights reserved.
//
#include <cstdlib>
#include <ctime>
//#include <iostream>
#include "Uct.hpp"
#include "Judge.h"
bool Uct::search(){
    // root 为当前状态，从该处开始寻找待扩展节点
    //print(curTop, curBoard);
//    if(root->child) for(int i = 0; i < N; i++){
//        if(root->child[i] && root->child[i]->already == -1){
//            root->bestKid = root->child[i];
//            return true;
//        }
//    }
    UctNode* cur = findNext();
    //print(curTop, curBoard);
    if(cur->already == 2){
        backward(cur, forward(cur));
    }
    update(cur->parent);
    return false;
}

void Uct::updateBest(UctNode* p){
    int try_cnt = 0;
    bool loser = true;
    bool s = true;
    if(!p->full){
        loser = false;
        s = false;
    }
    else for(int i = 0; i < N; i++){
        if(p->child[i]){
            if(p->child[i]->already != 1){
            loser = false;
            s = false;
            break;
            }
            else if(!p->child[i]->skip){
                s = false;
            }
        }
    }
    for(int i = 0; i < N; i++){
        if(p->child[i]){
            try_cnt += p->child[i]->n;
            if(p->child[i]->already == -1) {
                p->already = 1;
                if(p->child[i]->leaf){
                    p->skip = true;
                }
            }
        }
    }
    for(int i = 0; i < N; i++){
        if(p->child[i] && p->child[i]->confidence(try_cnt, 2) > p->bestKid->confidence(try_cnt, 2)){
            p->bestKid = p->child[i];
        }
    }
    if(loser){
        p->already = -1;
    }
    if(s){
        p->skipp = true;
    }
}
void Uct::clear(UctNode* cur, UctNode* safe){
    if(!cur->child) return;
    for (int i = 0; i < N; ++i) {
        if(cur->child[i] && cur->child[i] != safe) {
            clear(cur->child[i], nullptr);
            delete cur->child[i];
            cur->child[i] = nullptr;
        }
    }
    delete[] cur->child;
//    for(auto it = cur->kids.begin(); it != cur->kids.end(); it++){
//        if(nodes[it->second] != safe){
//            clear(nodes[it->second], nullptr);
//            delete nodes[it->second];
//            nodes[it->second] = nullptr;
//        }
//    }
//    cur->kids.clear();
}
void Uct::update(UctNode* cur){
    UctNode* p = cur;
    while(p){
        updateBest(p);
        p = p->parent;
    }
}

void Uct::rotate(UctNode* newroot) {
    depth = 0;
    clear(root, newroot);//new root 是下一层节点，需删除原根及无用子树
    delete root;
    root = newroot;
    root->parent = nullptr;
}
void Uct::move(int lastx, int lasty, int* _t, int** _b){
    curTop = _t;
    curBoard = _b;
    UctNode* newroot;
    if(root->has(lasty)) newroot = root->child[lasty];
    else {
        newroot = new UctNode(nullptr, lastx, lasty);
    }
    rotate(newroot);
}
void Uct::backward(UctNode* cur, int res){
    UctNode* p = cur;
    int count = 0;
    while(p){
        count++;
        //print(curTop, curBoard);
        p->n++;
        p->w += res;
        res *= -1;
        if(!p->parent) break;
        curTop[p->lasty]++;
        curBoard[p->lastx][p->lasty] = 0;
        if (p->lastx - 1 == nox && p->lasty == noy) {
            curTop[p->lasty]++;
        }
        p = p->parent;
    }
    if(count > depth) depth = count;
    //print(curTop, curBoard);
}
UctNode* Uct::findNext(){
    UctNode* p = root;
    while(p){
        if(!p->full) {
            for (int i = 0; i < N; i++) {
                if (i == noy && curTop[i] - 1 == nox) curTop[i]--;
                if (curTop[i] > 0 && !p->has(i)) {
                    curTop[i]--;
                    UctNode *newnode = new UctNode(p, curTop[i], i);// need to expand
                    p->add(newnode, N);
                    curBoard[curTop[i]][i] = p->type;
                    if (i == noy && curTop[i] - 1 == nox) curTop[i]--;
                    if (i == N) p->full = true;
                    return newnode;
                }
            }
            p->full = true;
        }
        srand(time(0));
        if(p->bestKid->leaf || p->bestKid->skip || p->bestKid->skipp){
            // 至此，下一步已经出结果了
            UctNode* q = p->bestKid;
            p->bestKid->n++;
            p->bestKid->w += q->already;
            backward(p, -1 * q->already);
            return q;
        }
        else {
            // 继续探
            curTop[p->bestKid->lasty]--;
            curBoard[p->bestKid->lastx][p->bestKid->lasty] = p->type;
            if(p->bestKid->lasty == noy && p->bestKid->lastx - 1 == nox) curTop[p->bestKid->lasty]--;
            p = p->bestKid;
        }
    }
    return nullptr;
}

void Uct::print(const int* t, int* const *board){
    using namespace std;
    cout << "\t";
    for(int j = 0; j < N; j++){
        cout << j << "\t";
    }
    cout << endl;
    for(int i = 0; i < M; i++){//y
        cout << i << "\t";
        for(int j = 0; j < N; j++){//x
            if(i == nox && j == noy){
                cout << "X" << "\t";
            }
            else if (!board[i][j]){
                cout << "." << "\t";
            }
            else cout << board[i][j] << "\t";
        }
        cout << endl;
    }
    cout << '\t';
    for(int i = 0; i < N; i++){
        cout << t[i] << '\t';
    }
    cout << endl;
}
int Uct::forward(UctNode* const cur){
    UctNode* p = cur;

//    res = globalJudge(curBoard, curTop, type, nextx, nexty);
//    p->already = res;
//    if(res == 1){
//        p->nextx = nextx;
//        p->nexty = nexty;
//        return res;
//    }
//    else if(res == -1){
//        //拓展所有子节点
//        for(int i = 0; i < N; i++){
//            if(curTop[i] > 0) {
//                curTop[i]--;
//                UctNode *newnode = new UctNode(p, curTop[i], i);// need to expand
//                p->add(newnode, N);
//                curBoard[curTop[i]][i] = p->type;
//                if (i == noy && curTop[i] - 1 == nox) curTop[i]--;
//                forward(newnode);
//                curTop[i]++;
//                if (i == noy && curTop[i] == nox) curTop[i]++;
//            }
//        }
//        p->nextx = nextx;
//        p->nexty = nexty;
//        p->full = true;
//        return res;
//    }
//    else if(res == 0){
//        return 0;
//    }
//    else if(nextx != -1){
//        p->nextx = nextx;
//        p->nexty = nexty;
//    }
    //可认为top是顶 nox noy已处理
    if(p->type == 2){// 上一步是1 用户已经赢了
        if(userWin(p->lastx, p->lasty, M, N, curBoard)){
            p->already = -1;
            p->leaf = true;
            return -1;
        }
        else if(isTie(N, curTop)){
            p->already = 0;
            p->leaf = true;
            return 0;
        }
    }
    else {
        if(machineWin(p->lastx, p->lasty, M, N, curBoard)){
            p->already = -1;
            p->leaf = true;
            return -1;
        }
        else if(isTie(N, curTop)){
            p->already = 0;
            p->leaf = true;
            return 0;
        }
    }
    int type = p->type;
    int res = 0; int col, row;
    int nextx = -1, nexty = -1;
    res = globalJudge(curBoard, curTop, type, nextx, nexty);
    if(res != 2){
        return res;
    }
    srand((unsigned int)time(0));
    //copy
    int** _board = new int*[M];
    for(int i = 0; i < M; i++){
        _board[i] = new int[N];
        for(int j = 0; j < N; j++){
            _board[i][j] = curBoard[i][j];
        }
    }
    int* _top = new int[N];
    for(int i = 0; i < N; i++){
        _top[i] = curTop[i];
    }
    while(1){
//        col = rand() % N;
//        while(_top[col] < 1) col = rand() % N;
//        _top[col]--;
//        _board[_top[col]][col] = type;
//        //print(_top, _board);
//        if(type == 1 && userWin(_top[col], col, M, N, _board)){//上一步是1
//            res = p->type == 1 ? 1 : -1;
//            break;
//        }
//        else if(type == 2 && machineWin(_top[col], col, M, N, _board)){//上一步是2
//            res = p->type == 2 ? 1 : -1;
//            break;
//        }
//        if(col == noy && _top[col] - 1 == nox) _top[col]--;
//        if(_top[col] < 1 && isTie(N, _top)){
//            res = 0;
//            break;
//        }
//        type = 3 - type;
        if(nextx == -1 && nexty == -1) {
            col = rand() % N;
            while (_top[col] < 1) col = rand() % N;
        }
        else col = nexty;
        _top[col]--;
        _board[_top[col]][col] = type;
        row = _top[col];
        //print(_top, _board);
        if(col == noy && _top[col] - 1 == nox) _top[col]--;
        res = quickJudge(row, col, _board, _top, type, nextx, nexty);
        if(res == 2){//continue
            type = 3 - type;
            continue;
        }
        //else
        if(type != p->type) res *= -1;
        break;
    }
    for(int i = 0; i < M; i++){
        delete[] _board[i];
    }
    delete[] _board;
    delete[] _top;
    return res;
}
int Uct::globalJudge(int *const *board, const int *top, int curType, int &nextx, int &nexty) {
    //测试当前全局有无急点
    int judge[2] = {curType, 3-curType};
    int urgent_cnt = 0;
    int count = 0;
    bool full = true;
    nextx = -1;
    nexty = -1;
    for(int t = 0; t < 2; t++){
        for (int y = 0; y < N; y++) {
            int nx = top[y] - 1;
            if (nx < 0) continue;
            full = false;
            int i, j;
            count = 0;
            for (i = nx + 1; i < M; i++)
                if (board[i][y] != judge[t]) break;
            count += (i - nx);
            if (count >= 4) {
                nextx = nx;
                nexty = y;
                if (t == 0) return 1;
                else {
                    urgent_cnt++;
                    if(urgent_cnt > 1) return -1;
                    continue;
                }
            }
            //左右
            count = 0;
            for (i = y - 1; i >= 0; i--)
                if (board[nx][i] != judge[t]) break;
            count += (y - i);
            for (i = y + 1; i < N; i++)
                if (board[nx][i] != judge[t]) break;
            count += (i - y - 1);
            if (count >= 4) {
                nextx = nx;
                nexty = y;
                if (t == 0) return 1;
                else{
                    urgent_cnt++;
                    if(urgent_cnt > 1) return -1;
                    continue;
                }
            }
            //左下-右上
            count = 0;
            for (i = nx + 1, j = y - 1; i < M && j >= 0; i++, j--)
                if (board[i][j] != judge[t]) break;
            count += (y - j);
            for (i = nx - 1, j = y + 1; i >= 0 && j < N; i--, j++)
                if (board[i][j] != judge[t]) break;
            count += (j - y - 1);
            if (count >= 4) {
                nextx = nx;
                nexty = y;
                if (t == 0) return 1;
                else{
                    urgent_cnt++;
                    if(urgent_cnt > 1) return -1;
                    continue;
                }
            }
            //左上-右下
            count = 0;
            for (i = nx - 1, j = y - 1; i >= 0 && j >= 0; i--, j--)
                if (board[i][j] != judge[t]) break;
            count += (y - j);
            for (i = nx + 1, j = y + 1; i < M && j < N; i++, j++)
                if (board[i][j] != judge[t]) break;
            count += (j - y - 1);
            if (count >= 4) {
                nextx = nx;
                nexty = y;
                if (t == 0) return 1;
                else{
                    urgent_cnt++;
                    if(urgent_cnt > 1) return -1;
                    continue;
                }
            }
        }
    }
    if(full) return 0;
    return 2;
}
int Uct::quickJudge(int x, int y, int * const* board, const int *top, int curType, int& nextx, int& nexty) {
    //大前提，我下这步前，盘面没有急点
    int urgent_cnt = 0;//己方急点数 若多于一个 直接判胜
    int nx = top[y]-1;
    nextx = -1; nexty = -1;
    if(nx < 0){//可能平了
        int i;
        for(i = 0; i < N; i++){
            if(top[i] > 0) break;
        }
        if(i == N) return 0;
    }
    else {//上面新生成了可落子点
        //对方新急点只可能在 x y 上面
        int enemy = 3 - curType;
        int count = 0;
        while (1) {
            //第一轮测试对方急点
            //第二轮测试是否己方急点
            int i, j;
            if (enemy == curType) {//向下
                count = 0;
                for (i = nx + 1; i < M; i++)
                    if (board[i][y] != enemy) break;
                count += (i - nx);
                if(count >= 5){
                    return 1;
                }
                if (count >= 4) {
                    urgent_cnt++;
                    nextx = nx; nexty = y;
                    break;
                }
            }
            //左右
            count = 0;
            for (i = y - 1; i >= 0; i--)
                if (board[nx][i] != enemy) break;
            count += (y - i);
            for (i = y + 1; i < N; i++)
                if (board[nx][i] != enemy) break;
            count += (i - y - 1);
            if (count >= 4) {
                if(enemy == curType){
                    urgent_cnt++;
                    nextx = nx; nexty = y;
                    break;
                }
                else return -1;
            }
            //左下-右上
            count = 0;
            for (i = nx + 1, j = y - 1; i < M && j >= 0; i++, j--)
                if (board[i][j] != enemy) break;
            count += (y - j);
            for (i = nx - 1, j = y + 1; i >= 0 && j < N; i--, j++)
                if (board[i][j] != enemy) break;
            count += (j - y - 1);
            if (count >= 4) {
                if(enemy == curType){
                    urgent_cnt++;
                    nextx = nx; nexty = y;
                    break;
                }
                else return -1;
            }
            //左上-右下
            count = 0;
            for (i = nx - 1, j = y - 1; i >= 0 && j >= 0; i--, j--)
                if (board[i][j] != enemy) break;
            count += (y - j);
            for (i = nx + 1, j = y + 1; i < M && j < N; i++, j++)
                if (board[i][j] != enemy) break;
            count += (j - y - 1);
            if (count >= 4) {
                if(enemy == curType){
                    urgent_cnt++;
                    nextx = nx; nexty = y;
                    break;
                }
                else return -1;
            }
            if (enemy == curType) break;
            enemy = curType;
        }
    }
    //对方不可能有其他急点 测试己方其他急点
    //若测试到己方有超过一个急点 判胜
    //若己方一个急点 传回nextx nexty
    //左右
    int il, ir, l, r;
    for(r = y+1; r < N; r++){
        if(board[x][r] != curType) break;
    }
    for(l = y-1; l >= 0; l--){
        if(board[x][l] != curType) break;
    }
    if(r-l-1 >= 4){
        return 1;
    }
    if(r - l - 1 >= 3){
        if(l >= 0 && top[l]-1 == x) {// l 可落子
            urgent_cnt++;
            if(urgent_cnt > 1) return 1;
            nextx = x; nexty = l;
        }
        if(r < N && top[r]-1 == x) {// l 可落子
            urgent_cnt++;
            if(urgent_cnt > 1) return 1;
            nextx = x; nexty = r;
        }
    }
    //左下右上
    for(il = x+1, l = y-1; il < M && l >= 0; il++, l--){
        if(board[il][l] != curType) break;
    }
    ir = x-1; r = y+1;
    if(x > 0) for( ; ir >= 0 && r < N; ir--, r++){
        if(board[ir][r] != curType) break;
    }
    if(r - l - 1 >= 4) return 1;
    if(r - l - 1 >= 3){
        if(l >= 0 && top[l]-1 == il) {// l 可落子
            urgent_cnt++;
            if(urgent_cnt > 1) return 1;
            nextx = il; nexty = l;
        }
        if(r < N && ir >= 0 && top[r]-1 == ir) {// l 可落子
            urgent_cnt++;
            if(urgent_cnt > 1) return 1;
            nextx = ir; nexty = r;
        }
    }
    //左上右下
    il = x-1; l = y-1;
    if(x > 0) for( ; il >= 0 && l >= 0; il--, l--){
        if(board[il][l] != curType) break;
    }
    for(ir = x+1, r = y+1; ir < M && r < N; ir++, r++){
        if(board[ir][r] != curType) break;
    }
    if(r - l - 1 >= 4) return 1;
    if(r - l - 1 >= 3){
        if(l >= 0 && il >= 0 && top[l]-1 == il) {// l 可落子
            urgent_cnt++;
            if(urgent_cnt > 1) return 1;
            nextx = il; nexty = l;
        }
        if(r < N && top[r]-1 == ir) {// l 可落子
            urgent_cnt++;
            if(urgent_cnt > 1) return 1;
            nextx = ir; nexty = r;
        }
    }
    return 2;
}