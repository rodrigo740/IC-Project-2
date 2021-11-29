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

    int decode(vector<int> code){
        int n = 0;
        int q = 0;
        int r = 0;
        int nbits = log2(m);
        vector<int> res = code;

        cout << "Code: " << endl;

        




        for(int i : res){
            if(i!=1){
                q++;
                code.erase(code.begin());
            }
            else{
                code.erase(code.begin());

                for (int i = 0; i < code.size(); i++)
                {
                    cout << code[i] << endl;
                }

                r = binaryToDecimal(code);
                cout << "r -> " << r << endl;
                n=q*m+r;
                return n;
            }
        }

        

        
        

        
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
        for (int i= binaryNum.size()-1; i>=0; i--){
            bin.push_back(binaryNum[i]);
        }
        binaryNum = bin;
        return binaryNum;
    }

    int binaryToDecimal(vector<int> n){
        string s;
        for(int i : n){
            string s1 = to_string(i);
            s = s + s1;
        }
        int num = stoi(s);
        cout << "num: " << num << endl;
        int dec_value = 0;
        // Initializing base value to 1, i.e 2^0
        int base = 1;
        int temp = num;
        while (temp) {
            int last_digit = temp % 10;
            temp = temp / 10;
    
            dec_value += last_digit * base;
    
            base = base * 2;
        }
        return dec_value;
    }

};