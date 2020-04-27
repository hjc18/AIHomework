//
//  Uct.cpp
//  Strategy
//
//  Created by 黄杰辰 on 2020/4/15.
//  Copyright © 2020 Yongfeng Zhang. All rights reserved.
//
#include <cstdlib>
#include <ctime>
#include "Uct.h"
#include "Judge.h"
int _board[144]={0};
int _top[12]={0};
bool Uct::endCheck(){
    for(int i = 0; i < N; i++){
        if(root->child[i] && root->child[i]->n > root->bestKid->n){
            return false;
        }
    }
    return true;
}
void Uct::finalCheck(){
    for(int i = 0; i < N; i++){
        if(root->child[i] && root->child[i]->leaf){
            root->bestKid = root->child[i];
            return;
        }
    }
    for(int i = 0; i < N; i++){
        if(root->child[i] && root->child[i]->tag == -1){
            root->bestKid = root->child[i];
            return;
        }
    }
    if(root->bestKid->tag == 1) {
        UctNode *sec = nullptr;
        for (int j = 0; j < N; j++) {
            if (root->child[j] && root->child[j]->tag != 1) {
                sec = root->child[j];
            }
        }
        if(sec){
            for(int j = 0; j < N; j++){
                if(root->child[j] && root->child[j]->tag != 1 && root->child[j]->confidence() > sec->confidence())
                    sec = root->child[j];
            }
            root->bestKid = sec;
        }
    }
}
void Uct::search(){
    // root 为当前状态，从该处开始寻找待扩展节点
    UctNode* cur = findNext();
    if(!cur->leaf && !cur->skip) backward(cur, forward(cur));// 需要模拟
    update(cur->parent); // 更新树
}
void Uct::updateTag(UctNode * p) {
    int min = 100;
    int max = 0;
    for(int i = 0; i < N; i++){
        if(p->child[i]){
            if(p->child[i]->tag != 1){
                return;
            }
            else {
                if (p->child[i]->skip < min) {
                    min = p->child[i]->skip;
                }
                if (p->child[i]->skip > max){
                    max = p->child[i]->skip;
                }
            }
        }
    }
    p->tag = -1;
    p->parent->tag = 1;
    if(min >= 1 && max <= 3){
        p->skip = max + 1;
        p->parent->skip = max + 2;
    }
}
void Uct::updateBest(UctNode* p){

    for(int i = 0; i < N; i++){
        if(p->child[i] && p->child[i]->confidence() > p->bestKid->confidence()){
            p->bestKid = p->child[i];
        }
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
}
void Uct::update(UctNode* cur){
    UctNode* p = cur;
    while(p){
        updateBest(p);
        if(p != root && p->tag == 2 && p->full) updateTag(p);
        p = p->parent;
    }
}

void Uct::rotate(UctNode* newroot) {
    clear(root, newroot);//new root 是下一层节点，需删除原根及无用子树
    delete root;
    root = newroot;
    root->parent = nullptr;
}
void Uct::move(int lastx, int lasty, int* _t, int** _b){
    curTop = _t;
    curBoard = _b;
    if(root->has(lasty)) rotate(root->child[lasty]);
    else {
        UctNode* newroot = new UctNode(nullptr, lastx, lasty);
        rotate(newroot);
    }
}
void Uct::backward(UctNode* cur, int res){
    UctNode* p = cur;
    while(true){
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
        if(p->bestKid->leaf || p->bestKid->skip){
            // 至此，下一步已经出结果了
            UctNode* q = p->bestKid;
            p->bestKid->n ++;
            p->bestKid->selfn ++;
            p->bestKid->w +=  q->tag;
            backward(p, -1 * q->tag);
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

void Uct::print(const int* t, int* const *board, int x, int y){
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
            else{
                if(x == i && y == j){
                    cout << "|";
                }
                cout << board[i][j];
                if(x == i && y == j){
                    cout << "|";
                }
                cout << "\t";
            }
        }
        cout << endl;
    }
    cout << '\t';
    for(int i = 0; i < N; i++){
        cout << t[i] << '\t';
    }
    cout << endl;
}
int Uct::forward(UctNode* const p){
    p->selfn ++;
    //可认为top是顶 nox noy已处理
    if(JudgeWin(p->lastx, p->lasty, curBoard, 3-p->type)){
        p->tag = -1;
        p->leaf = true;
        p->parent->tag = 1;
        p->parent->skip = 1;
        return -1;
    }
    else if(isTie(N, curTop)){
        p->tag = 0;
        p->leaf = true;
        return 0;
    }
    int type = p->type;
    int col, row;
    int nextx = -1, nexty = -1;
    int res = globalJudge(curBoard, curTop, type, nextx, nexty);
    if(res != 2){
        return res;
    }
    srand((unsigned int)time(0));
    for(int i = 0; i < M; i++){
        for(int j = 0; j < N; j++){
            _board[ i * N + j] = curBoard[i][j];
        }
    }
    for(int i = 0; i < N; i++){
        _top[i] = curTop[i];
    }
    while(1){
        if(nextx == -1 && nexty == -1) {
            col = rand() % N;
            while (_top[col] < 1) col = rand() % N;
        }
        else col = nexty;
        _top[col]--;
        _board[_top[col]*N+col] = type;
        row = _top[col];
        if(col == noy && _top[col] - 1 == nox) _top[col]--;
        res = quickJudge(row, col, _board, _top, type, nextx, nexty);
        if(res != 2){
            return type == p->type ? res : res*-1;
        }
        type = 3-type;

    }
}
bool Uct::JudgeWin(const int x, const int y, int** board, int type){
    //横向检测
    int i, j;
    int count = 0;
    for (i = y; i >= 0; i--)
        if (board[x][i] != type) break;
    count += (y - i);
    for (i = y; i < N; i++)
        if (board[x][i] != type) break;
    count += (i - y - 1);
    if (count >= 4) return true;

    //左下-右上
    count = 0;
    for (i = x, j = y; i < M && j >= 0; i++, j--)
        if (board[i][j] != type) break;
    count += (y - j);
    for (i = x, j = y; i >= 0 && j < N; i--, j++)
        if (board[i][j] != type) break;
    count += (j - y - 1);
    if (count >= 4) return true;

    //左上-右下
    count = 0;
    for (i = x, j = y; i >= 0 && j >= 0; i--, j--)
        if (board[i][j] != type) break;
    count += (y - j);
    for (i = x, j = y; i < M && j < N; i++, j++)
        if (board[i][j] != type) break;
    count += (j - y - 1);
    if (count >= 4) return true;

    //纵向检测
    count = 0;
    for (i = x; i < M; i++)
        if (board[i][y] != type) break;
    count += (i - x);
    if (count >= 4) return true;

    return false;
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
int Uct::quickJudge(int x, int y, int * board, const int *top, int curType, int& nextx, int& nexty) {
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
                    if (board[i*N+y] != enemy) break;
                count += (i - nx);
                if(count >= 5) return 1;
                if (count >= 4) {
                    urgent_cnt++;
                    nextx = nx; nexty = y;
                    break;
                }
            }
            //左右
            count = 0;
            for (i = y - 1; i >= 0; i--)
                if (board[nx*N+i] != enemy) break;
            count += (y - i);
            for (i = y + 1; i < N; i++)
                if (board[nx*N+i] != enemy) break;
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
                if (board[i*N+j] != enemy) break;
            count += (y - j);
            for (i = nx - 1, j = y + 1; i >= 0 && j < N; i--, j++)
                if (board[i*N+j] != enemy) break;
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
                if (board[i*N+j] != enemy) break;
            count += (y - j);
            for (i = nx + 1, j = y + 1; i < M && j < N; i++, j++)
                if (board[i*N+j] != enemy) break;
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
    int ill, irr, ll, rr;
    for(r = y+1; r < N; r++){
        if(board[x*N+r] != curType) break;
    }
    for(l = y-1; l >= 0; l--){
        if(board[x*N+l] != curType) break;
    }
    if(r - l - 1 >= 4){
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
    else {
        if (r < N - 1 && top[r] - 1 == x) {
            for (rr = r + 1; rr < N; rr++) {
                if (board[x*N+rr] != curType) break;
            }
            if (rr - l - 1 >= 4){
                urgent_cnt++;
                if(urgent_cnt > 1) return 1;
                nextx = x; nexty = r;
            }
        }
        if (l > 0 && top[l] - 1 == x) {
            for (ll = l - 1; ll >= 0; ll--) {
                if (board[x*N+ll] != curType) break;
            }
            if (r - ll - 1 >= 4){
                urgent_cnt++;
                if(urgent_cnt > 1) return 1;
                nextx = x; nexty = l;
            }
        }
    }
    //左下右上
    for(il = x+1, l = y-1; il < M && l >= 0; il++, l--){
        if(board[il*N+l] != curType) break;
    }
    ir = x-1; r = y+1;
    if(x > 0) for( ; ir >= 0 && r < N; ir--, r++){
        if(board[ir*N+r] != curType) break;
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
    else {
        if (r < N - 1 && ir > 0 && top[r] - 1 == ir) {
            for (rr = r + 1, irr = ir - 1; irr >= 0 && rr < N; irr--, rr++) {
                if (board[irr*N+rr] != curType) break;
            }
            if (rr - l - 1 >= 4){
                urgent_cnt++;
                if(urgent_cnt > 1) return 1;
                nextx = x; nexty = r;
            }
        }
        if (l > 0 && il < M-1 && top[l] - 1 == il) {
            for (ll = l - 1, ill = il + 1; ll >= 0 && ill < M; ll--, ill++) {
                if (board[ill*N+ll] != curType) break;
            }
            if (r - ll - 1 >= 4){
                urgent_cnt++;
                if(urgent_cnt > 1) return 1;
                nextx = x; nexty = l;
            }
        }
    }
    //左上右下
    il = x-1; l = y-1;
    if(x > 0) for( ; il >= 0 && l >= 0; il--, l--){
        if(board[il*N+l] != curType) break;
    }
    for(ir = x+1, r = y+1; ir < M && r < N; ir++, r++){
        if(board[ir*N+r] != curType) break;
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
    else {
        if (r < N - 1 && ir < M - 1 && top[r] - 1 == ir) {
            for (rr = r + 1, irr = ir + 1; irr < M && rr < N; irr++, rr++) {
                if (board[irr*N+rr] != curType) break;
            }
            if (rr - l - 1 >= 4){
                urgent_cnt++;
                if(urgent_cnt > 1) return 1;
                nextx = x; nexty = r;
            }
        }
        if (l > 0 && il > 0 && top[l] - 1 == il) {
            for (ll = l - 1, ill = il - 1; ll >= 0 && ill >= 0; ll--, ill--) {
                if (board[ill*N+ll] != curType) break;
            }
            if (r - ll - 1 >= 4){
                urgent_cnt++;
                if(urgent_cnt > 1) return 1;
                nextx = x; nexty = l;
            }
        }
    }
    return 2;
}
