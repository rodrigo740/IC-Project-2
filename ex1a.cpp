#include"BitStream.h"
int main(){
    BitStream bs("a.bit",'r');
    //ab -> 0x61 0x62 0110 0001 0110 0010
    for(int i=0; i<32; i++){
         int f0 = bs.readbit();
        cout << f0 << endl;
    }
    cout << "READ N BITS" << endl;
    vector<int> f1 = bs.readnbits(4);
    for (int i : f1){
        cout << f1[i] << endl;
    }

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

    bs2.writenbits(16,65535);

    return 0;
}
