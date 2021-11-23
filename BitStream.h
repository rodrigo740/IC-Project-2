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
            buf = ifs.get();
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
        nbits++;
    }

    void readnbits(int n){

    }
    void writenbits(int n, int* bit){

    }
};