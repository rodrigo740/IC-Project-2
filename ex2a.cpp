#include"Golomb.h"
#include<vector>

int main(){



    Golomb g0(7);
    vector<int> x0 = g0.encode(-15);
    cout << "code: " << endl;
    for(int i : x0){
        cout << "" << i;
    }
    cout << endl;

    cout << "decode: " << g0.decode(x0) << endl;
/*
    vector<int> y0;
    y0.push_back(0);
    y0.push_back(1);
    y0.push_back(0);
    y0.push_back(1);
    y0.push_back(0);
    cout << "decode: " << g0.decode(y0) << endl;

    vector<int> x1 = g0.encode(8);
    cout << "code: " << endl;
    for(int i : x1){
        cout << "" << i << endl;
    }

    cout << "decode: " << g0.decode(x1) << endl;
*/
    /*
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
    */

    return 0;
}