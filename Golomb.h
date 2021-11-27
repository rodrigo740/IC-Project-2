#include<iostream>
#include<fstream>
#include<vector>
#include<math.h>
using namespace std;

class Golomb {
    int m;

    public: 
        Golomb(int param){
            m = param;
        }

    vector<int> encode(int n){
        vector<int> code;
        int q = n/m;
        cout << "q: " << q << endl;
        int r = n%m;
        cout << "r: " << r << endl;
        // unary code
        for(int i = 0; i < q; i++){
            code.push_back(0);
        }
        code.push_back(1);
        //binary code
        for(int j : decToBinary(r,m)){
            code.push_back(j);
        }
        return code;
    }

    void decode(){

    }

    vector<int> decToBinary(int n, int m){
        int nbits = log2(m);
        vector<int> binaryNum;
        vector<int> bin;
        int i = 0;
        while (n > 0) {
            // storing remainder in binary array
            binaryNum.push_back(n % 2);
            n = n / 2;
            i++;
        }
        int dif = nbits - binaryNum.size();
        while(dif > 0){
            bin.push_back(0);
            dif=dif-1;
        }
        for (int i : binaryNum){
            bin.push_back(i);
        }
        binaryNum = bin;
        return binaryNum;
    }

};