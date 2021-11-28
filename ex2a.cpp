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

    Golomb g3(4);
    int y1 = g3.decode(x1);
    cout << "value: " << y1 << endl;

    int y2 = g3.decode(x2);
    cout << "value: " << y2 << endl;

    Golomb g4(5);
    int y3 = g4.decode(x3);
    cout << "value: " << y3 << endl;

    int y4 = g4.decode(x4);
    cout << "value: " << y4 << endl;

    return 0;
}