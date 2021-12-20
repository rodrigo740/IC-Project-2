#include<iostream>
#include<fstream>
#include<vector>
#include<math.h>
using namespace std;

class Golomb { ///Test da descrição brief!
    int m;
    /**
    * Desciraaoaooa
    */

    public: 
        Golomb(int param){
            m = param;
        }

    vector<int> encode(int n){
        n = folding(n);
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
        double resto = rt - (int) rt;

        if (resto == 0){
            for(int j : decToBinary(r,m)){
                code.push_back(j);
            }
        //truncated binary encoding
        }else{
                int x = r, k = 0, t = m;
                while (t > 1){
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

                for (int i = 0; i < res.length(); i++){
                    int j = res[i] - 48;
                    code.push_back(j);
                }
        }  

        return code;
    }

    string binary (int x, int len){
        string s = "";
        if(x == 0){
            s += '0';
        }
        while (x!=0){
            if (even(x))
            {
                s = '0' + s;
            }else{
                s = '1' + s;
            }
            x >>= 1;
        }
        while (s.length() < len){
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
        double rt = log2(m);
        double resto = rt - (int) rt;
        vector<int> res = code;
        cout << "code before: ";
        for (int c : code) {
            cout << c;
        }
        cout << endl;
        
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

        cout << "code after: ";
        for (int c : code) {
            cout << c;
        }
        cout << endl;

        if(resto == 0){
            r = binaryToDecimal(code);
        }
        //truncated binary decoding
        else{
            string tmp = "";
            int u = (1 << nbits+1) - m;
            // read the first nbits
            for(int i = 0; i < nbits; i++){
                tmp += to_string(code[i]);
            }
            long long result = stoll(tmp);
            
            //if they encode a value less than u
            if(result < u){
                r = binaryToDecimalINT(result);
            }
            //read an additional bit and subtract u from result
            else{
                tmp += to_string(code[nbits]);
                result = stoll(tmp);
                result = binaryToDecimalINT(result) - u;
                result = decimalToBinary(result);
                r = binaryToDecimalINT(result);
            }
        }
        n=q*m+r;
        return unfolding(n);
    }

    int decimalToBinary(int N)
    {
    
        // To store the binary number
        int B_Number = 0;
        int cnt = 0;
        while (N != 0) {
            int rem = N % 2;
            int c = pow(10, cnt);
            B_Number += rem * c;
            N /= 2;
    
            // Count used to store exponent value
            cnt++;
        }
    
        return B_Number;
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
        int dif = nbits - i;
        while(dif > 0){
            bin.push_back(0);
            //bin.insert(bin.begin(), 0);
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
        long long num = stoll(s);
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

    int binaryToDecimalINT(int n)
    {
        int num = n;
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

    int folding(int n){
        int x;

        if(n >= 0){
            x = 2*n;
        }else{
            x = 2*abs(n)-1;
        }
        
        return x;
    }

    int unfolding(int n){
        int x;

        if(n%2==0){
            x = n/2;
        }else{
            x = -(n+1)/2;
        }

        return x;
    }

};