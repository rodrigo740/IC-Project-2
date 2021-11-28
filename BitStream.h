#include<iostream>
#include<fstream>
#include<vector>

using namespace std;

class BitStream {
    ifstream ifs;
    ofstream ofs;
    string fileName;
    int buffer[8] = {0,0,0,0,0,0,0,0};
    char buf = 0;        // the buffer of 8 bits
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
            ofs = ofstream(fileName);

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
        
        buffer[nbits] = (bit << (7-nbits));

        nbits++;
        if (nbits == 8) {
            char c = 0;
            for (int i = 0; i < 8; i++)
            {
                c |= buffer[i];
            }   
            ofs << c;
            nbits = 0;
        }    
    }

    void closeF(){
        ofs.close();
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
        /*//n -> number of bits to return
        int nbit = 0;
        if(nbits == 8){
            buf = ifs.get(); //get the next char(byte)
            nbits = 0;
        }
        nbit = (((1 << n) - 1) & (buf >> (nbits - 1)));
        nbits++;
        return nbit;*/
    }
    
    void writenbits(int n, int nbit){
        if(nbits == 8){
            ofs.put(buf);
            ofs.flush();
            nbits = 0;
            buf = 0;
        }
        buf =  nbit |= (1 << nbits);
        ofs << buf;
        nbits++;
    }

    int readstrings(){
        int bits = 0;
        for(char x; ifs >> x;){
            if(nbits == 8){
                buf = ifs.get(); //get the next char(byte)
                nbits = 0;
            }
            bits = (((1 << x) - 1) & (buf >> (nbits - 1)));
            nbits++;
        }
        return bits;
    }

    void writestrings(string s){
        for(int x=0; x<s.length() ;x++){
            if(nbits == 8){
                ofs.put(buf);
                ofs.flush();
                nbits = 0;
                buf = 0;
            }
            buf =  s[x] |= (1 << nbits);
            ofs << buf;
            nbits++;
        }
    }
};