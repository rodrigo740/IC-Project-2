#include"BitStream.h"
int main(){
    /*
    BitStream bs("a.bit",'r');
    //ab -> 0x61 0x62 0110 0001 0110 0010
    for(int i=0; i<4; i++){
         int f0 = bs.readbit();
        cout << f0 << endl;
    }
    cout << "READ N BITS" << endl;
    vector<int> f1 = bs.readnbits(4);
    for (int i : f1){
        cout << f1[i] << endl;
    }
    /*cout << "READ STRINGS" << endl;
    vector<int> f2 = bs.readstrings();
    for (int i : f2){
        cout << f2[i] << endl;
    }*/
    BitStream bs2("b.bit",'w');

    bs2.writebit(0);
    bs2.writebit(1);
    bs2.writebit(1);
    bs2.writebit(0);

    bs2.writebit(0);
    bs2.writebit(0);
    bs2.writebit(0);
    bs2.writebit(1);
    
    bs2.writebit(0);
    bs2.writebit(1);
    bs2.writebit(1);
    bs2.writebit(0);

    bs2.writebit(0);
    bs2.writebit(0);
    bs2.writebit(1);
    bs2.writebit(0);

    bs2.writebit(0);
    bs2.writenbits(1,2);
    bs2.writebit(0);

    bs2.writenbits(0,3);
    bs2.writebit(1);

    bs2.writebit(0);
    bs2.writenbits(1,2);
    bs2.writebit(0);

    bs2.writenbits(0,2);
    bs2.writebit(1);
    bs2.writebit(0);

    bs2.writestrings("01100001");
    bs2.writestrings("01100010");

    bs2.writebit(0);
    bs2.writebit(1);
    bs2.writebit(0);
    bs2.writebit(0);

    bs2.writebit(1);


    bs2.completeByte();



    bs2.closeF();



    
    BitStream bs("b.bit",'r');
    //ab -> 0x61 0x62 0110 0001 0110 0010
    for(int i=0; i<32; i++){
        int f0 = bs.readbit();
        cout << f0 << endl;
    }
    

    return 0;
}
