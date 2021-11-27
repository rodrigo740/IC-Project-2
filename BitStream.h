#include<iostream>
#include<fstream>
#include<vector>

using namespace std;

class BitStream {
    ifstream ifs;
    ofstream ofs;
    string fileName;
    char buf;        // the buffer of 8 bits
    int nbits=0;     // the bit buffer index
    int pos = 0;
    vector<char> chars;

    public: 
        BitStream(string f, char ch){
            fileName=f;
            setFile(f, ch);
        }

    void setFile(string f, char ch){
        if (ch == 'w'){ //write
            ofstream ofs(fileName);
        }
        if (ch == 'r'){ //read
            ifstream ifs(fileName);
            buf=ifs.get();
            char c;
            while(ifs.get(c)){
                chars.push_back(c);
            }
            if (!ifs.is_open()){
                cerr << "Could not open input file: '" << fileName << "'" << endl;
                exit;
            }
        }
        if (ch == 'x'){ //read and write
            ifstream ifs(fileName);
            if (!ifs.is_open()){
                cerr << "Could not open input file: '" << fileName << "'" << endl;
                exit;
            }
            ofstream ofs(fileName);
        }
    }

    /*
    Access bit N

    Get: (INPUT >> N) & 1;

    Set: INPUT |= 1 << N;

    Unset: INPUT &= ~(1 << N);

    Toggle: INPUT ^= 1 << N;
    */

    int readbit(){
        int bit = 0;
        if(nbits == 8){
            buf=chars.at(pos);
            pos++;
            nbits = 0;
        }
        bit = (buf >> (7-nbits)) & 1;
        nbits++;
        return bit;
    }

    void writebit(int bit){
        if(nbits == 8){
            ofs.put(buf);
            ofs.flush();
            nbits = 0;
            buf = 0;
        }
        buf =  bit |= 1 << (7-nbits);
        ofs << buf;
        nbits++;
    }

    vector<int> readnbits(int n){
        vector<int> bit;
        for (int i=0; i < n; i++){
            if(nbits == 8){
                buf=chars.at(pos);
                pos++;
                nbits = 0;
            }
            bit.push_back((buf >> (7-nbits)) & 1);
            nbits++;
        }
        return bit;
    }

    void writenbits(int n, int nbit){
        if(nbits == 8){
            ofs.put(buf);
            ofs.flush();
            nbits = 0;
            buf = 0;
        }
        buf =  nbit |= (1 << (7-nbits));
        ofs << buf;
        nbits++;
    }

    vector<int> readstrings(){
        vector<int> bit;
        string str;
        while(getline(ifs,str)){
            for(char x : str){
                while(nbits != 8){
                    bit.push_back((x >> (7-nbits)) & 1);
                    nbits++;
                }
                nbits=0;
            }
        }
        return bit;
    }

    void writestrings(string s){
        for(int x=0; x<s.length() ;x++){
            if(nbits == 8){
                ofs.put(buf);
                ofs.flush();
                nbits = 0;
                buf = 0;
            }
            buf =  s[x] |= (1 << (7-nbits));
            ofs << buf;
            nbits++;
        }
    }
};