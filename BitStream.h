#include<iostream>
#include<fstream>
#include<vector>

using namespace std;

class BitStream { 
    /**
    * \class BitStream  BitStream.h 
    * This class includes methods to read/write bits from/to a file. 
    */
    ifstream ifs;
    ofstream ofs;
    string fileName;
    int buffer[8] = {0,0,0,0,0,0,0,0};
    char buf = 0;    // the buffer of 8 bits
    int nbits=0;     // the bit buffer index
    int pos = 0;
    vector<char> chars;

    public: 
    /**
    * This class is composed of a constructor that takes as  parameters  a  string,  which  will  correspond  to  the  name  of  the  file,  and  a character (’r’ or ’w’) to decide which operation will be executed, read or write.
    */
        BitStream(string f, char ch){
            fileName=f;
            setFile(f, ch);
        }

    void setFile(string f, char ch){
        /**
        * \memberof(BitStream)
        * Function to set the input or output file as file "f" according to the char "ch".
        */
        if (ch == 'w'){ //write
            ofs = ofstream(fileName);

        }
        if (ch == 'r'){ //read
            ifstream ifs(fileName);
            char c;
            while(ifs.get(c)){
                chars.push_back(c);
            }
            buf=chars[0];
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
        /**
        * \memberof(BitStream)
        * Function to read one bit from a file.
        */
        int bit = 0;
        if(nbits == 8){
            pos++;
            // if pos == chars.size() no more chars left
            if(pos == chars.size()){
                return -1;
            }
            buf=chars.at(pos);
            nbits = 0;
        }
        bit = (buf >> (7-nbits)) & 1;
        nbits++;
        return bit;
    }


    vector<int> readFile(){
        /**
        * \memberof(BitStream)
        * Function to read a file bit by bit. 
        */
        vector<int> bits;
        int b = (buf >> 7) & 1;
        bits.push_back(b);
        //cout << "Chars size: " << chars.size() << endl;
        while (pos < chars.size())
        {
            b = readbit();
            //cout << "bit: " << b << endl;
            bits.push_back(b);
            /*if((bits.size()-1)%8==0){
                cout << "-----------" << endl;
            }*/
        }
        //cout << "Final pos: " << pos << endl;
        return bits;   
    }


    /*
    char readbit(){
        if (pos != chars.size())
        {
            char c = chars[pos];
            pos++;
            return c;
        }else{
            return -1;
        }   
    }
    */

    void writebit(int bit){
        /**
        * \memberof(BitStream)
        * Function to write one bit to a file.
        */
        
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

    void completeByte(){
        /**
        * \memberof(BitStream)
        * Function to complete/fill one byte (8 bits).
        */
        while(nbits != 0){
            writebit(0);
        }
    }

    void closeF(){
        /**
        * \memberof(BitStream)
        * Function to close the input/output file.
        */
        ofs.close();
    }

    vector<int> readnbits(int n){
        /**
        * \memberof(BitStream)
        * Function to read "n" bits from a file. 
        */
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
        /**
        * \memberof(BitStream)
        * Function to write "n" bits.
        */
        for (int i = 0; i < nbit; i++)
        {
            writebit(n);
        }
    }

    vector<int> readstrings(){
        /**
        * \memberof(BitStream)
        * Function to read strings bit by bit from a file.
        */
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
        /**
        * \memberof(BitStream)
        * Function to write a string bit by bit to a file.
        */
        for(int i=0; i<s.length() ;i++){
            writebit(s[i]-48);      // 48 is the value of 0
        }
    }
};