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
        int r = n%m;
        // unary code
        for(int i = 0; i < q; i++){
            code.push_back(0);
        }
        code.push_back(1);
        //binary code

        double rt = log2(m);
        double resto;
        modf(rt, &resto);
        if (resto == 0)
        {
            for(int j : decToBinary(r,m)){
                code.push_back(j);
            }

        }else{
            int f = floor(rt);
            int c = ceil(rt);
            int x = r, k = 0, t = m;

            while (t > 1)
            {
                k++;
                t >>=1;
            }

            int u = (1 << k+1) - m;
            string res;
            if (x < u) {
                res = binary(x, k);
            }else{
                res = binary(x + u, k + 1);
            }

            cout << res << endl;

            for (int i = 0; i < res.length(); i++)
            {
                cout << "res[i]: " <<  res[i] << endl;
                int j = res[i];
                code.push_back(j);
            }
        }  

        return code;
    }

    string binary (int x, int len){
        string s = "";
        while (x!=0)
        {
            if (even(x))
            {
                s = '0' + s;
            }else{
                s = '1' + s;
            }
            x >>= 1;
        }

        while (s.length() < len)
        {
            s = '0' + s;
            return s;
        }
    }

    bool even(int x){
        return x%2 == 0;
    }

    int decode(vector<int> code){
        int n = 0;
        int q = 0;
        int r = 0;
        int nbits = log2(m);
        vector<int> res = code;

        for(int i : res){
            if(i!=1){
                q++;
                code.erase(code.begin());
            }
            else{
                code.erase(code.begin());
                break;
            }
        }

        r = binaryToDecimal(code);
        n=q*m+r;
        
        return n;
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