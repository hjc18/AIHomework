#include <iostream>
#include "Judge.h"
#include "Strategy.h"
#include <cmath>
using namespace std;
int M, N, nox, noy;
int** board;
int* board_r;
int* top;
void print(){
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
        cout << top[i] << '\t';
    }
    cout << endl;
}
int main() {
    cin >> M >> N >> nox >> noy;
    board = new int*[M];
    for (int i = 0; i < M; ++i) {
        board[i] = new int[N];
        memset(board[i], 0, N*sizeof(int));
    }
    board_r = new int[M*N];
    memset(board_r, 0, M*N*sizeof(int));
    top = new int[N];
    for (int j = 0; j < N; ++j) {
        if(noy == j && nox == M-1) top[j] = M-1;
        else top[j] = M;
    }
    Point* move;
    int lastx = -1, lasty = -1;
    print();
    while(1){

        cin >> lasty;
        top[lasty]--;
        lastx = top[lasty];
        board[lastx][lasty] = 1;
        board_r[lastx*N+lasty] = 1;
        cout << "you" << "(" << lastx << ',' << lasty << ")" << endl;
        print();
        if(userWin(lastx, lasty, M, N, board)){
            cout << "you win" << endl;
            break;
        }
        if(lastx - 1 == nox && lasty == noy) top[lasty]--;
        if(isTie(N, top)){
            cout << "tie" << endl;
            break;
        }
        move = getPoint(M, N, top, board_r, lastx, lasty, nox, noy);
        top[move->y]--;
        board[move->x][move->y] = 2;
        board_r[move->x * N + move->y] = 2;
        cout << "ai" << "(" << move->x << ',' << move->y << ")" << endl;
        print();
        if(machineWin(move->x, move->y, M, N, board)){
            cout << "ai win" << endl;
            break;
        }
        if(move->x - 1 == nox && move->y == noy) top[move->y]--;
        if(isTie(N, top)){
            cout << "tie" << endl;
            break;
        }
        clearPoint(move);

    }
    return 0;
}