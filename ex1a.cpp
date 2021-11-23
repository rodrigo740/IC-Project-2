#include"BitStream.h"
int main(){
    BitStream bs("a.bit",'r');
    int f0 = bs.readbit();
    cout << f0 << endl;
    int f1 = bs.readbit();
    cout << f1 << endl;
    int f2 = bs.readbit();
    cout << f2 << endl;
    int f3 = bs.readbit();
    cout << f3 << endl;
    int f4 = bs.readbit();
    cout << f4 << endl;
    int f5 = bs.readbit();
    cout << f5 << endl;
    int f6 = bs.readbit();
    cout << f6 << endl;
    int f7 = bs.readbit();
    cout << f7 << endl;
    int f8 = bs.readbit();
    cout << f8 << endl;
    int f9 = bs.readbit();
    cout << f9 << endl;
    int f10 = bs.readbit();
    cout << f10 << endl;
    int f11 = bs.readbit();
    cout << f11 << endl;

    BitStream bs2("b.bit",'w');
    bs2.writebit(0);
    bs2.writebit(0);
    bs2.writebit(0);
    bs2.writebit(0);
    bs2.writebit(0);
    bs2.writebit(0);
    bs2.writebit(0);
    bs2.writebit(0);
    bs2.writebit(3);
    bs2.writebit(4);
    bs2.writebit(0);
    bs2.writebit(1);
    bs2.writebit(2);
    bs2.writebit(3);
    bs2.writebit(4);
    bs2.writebit(0);
    bs2.writebit(1);
    bs2.writebit(2);
    bs2.writebit(3);
    bs2.writebit(4);

    return 0;
}
