#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// 2次元vectorの中身のセット
void setVec(vector<vector<int>>& v){
    vector<int> tmp;

    tmp.push_back(5);tmp.push_back(2);tmp.push_back(3);
    v.push_back(tmp);
    tmp.clear();

    tmp.push_back(1);tmp.push_back(4);tmp.push_back(5);
    v.push_back(tmp);
    tmp.clear();

    tmp.push_back(3);tmp.push_back(3);tmp.push_back(2);
    v.push_back(tmp);
    tmp.clear();
}

//2次元ベクトルを表示
void printVec(const vector<vector<int>> v){
    for(int i=0;i<v.size();i++){
        for(int j=0;j<v[i].size();j++){
            cout << v[i][j] << " ";
        }
        cout << endl;
    }
}

int main(void){
    vector<vector<int>> myVec;
    setVec(myVec);

    cout << " -- 初期状態 -- " << endl;
    printVec(myVec);


    cout << " -- 要素0でソート -- " << endl;
    sort(myVec.begin(),myVec.end(),[](const vector<int> &alpha,const vector<int> &beta){return alpha[0] > beta[0];});
    printVec(myVec);

    cout << " -- 要素1でソート -- " << endl;
    sort(myVec.begin(),myVec.end(),[](const vector<int> &alpha,const vector<int> &beta){return alpha[1] < beta[1];});
    printVec(myVec);

    cout << " -- 要素2でソート -- " << endl;
    sort(myVec.begin(),myVec.end(),[](const vector<int> &alpha,const vector<int> &beta){return alpha[2] < beta[2];});    
    printVec(myVec);
}