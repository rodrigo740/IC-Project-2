#include"Golomb.h"
#include<vector>

int main(){

    Golomb g1(4);
    vector<int> x1 = g1.encode(11);
    cout << "code: " << endl;
    for(int i : x1){
        cout << "" << i << endl;
    }

    vector<int> x2 = g1.encode(17);
    cout << "code: " << endl;
    for(int i : x2){
        cout << "" << i << endl;
    }

    Golomb g2(5);
    vector<int> x3 = g2.encode(11);
    cout << "code: " << endl;
    for(int i : x3){
        cout << "" << i << endl;
    }

    vector<int> x4 = g2.encode(17);
    cout << "code: " << endl;
    for(int i : x4){
        cout << "" << i << endl;
    }
    return 0;
}