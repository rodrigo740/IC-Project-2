#include<iostream>
#include<fstream>

using namespace std;

class BitStream {
    ifstream ifs;
    ofstream ofs;
    string fileName;
    char buf;      // the buffer of bits
    int nbits;     // the bit buffer index

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
        }
        if (ch == 'x'){ //read and write
            ifstream ifs(fileName);
            ofstream ofs(fileName);
        }
    }

    int readbit(){
        int bit = 0;
        if(nbits == 8){
            buf = ifs.get(); //get the next char(byte)
            nbits = 0;
        }
        bit = (1 & buf >> (7-nbits));
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
        buf =  buf | (bit << (7-nbits));
        ofs << buf;
        nbits++;
    }

    int readnbits(int n){
        //n -> number of bits to return
        int nbit = 0;
        if(nbits == 8){
            buf = ifs.get(); //get the next char(byte)
            nbits = 0;
        }
        nbit = (((1 << n) - 1) & (buf >> (nbits - 1)));
        nbits++;
        return nbit;
    }
    void writenbits(int n, int nbit){
        if(nbits == 8){
            ofs.put(buf);
            ofs.flush();
            nbits = 0;
            buf = 0;
        }
        buf =  buf | (nbit << (nbits - 1));
        ofs << buf;
        nbits++;
    }
};