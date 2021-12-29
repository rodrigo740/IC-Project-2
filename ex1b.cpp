#include"AudioFile/AudioFile.h"
#include"Golomb.h"
#include"BitStream.h"
#include<vector>
#include<fstream>
#include<math.h>
#include <limits>
#include<map>
#include <chrono>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
using namespace std::chrono;
using namespace std;

int s = 0;
int c = 0;

int binaryToDecimalINT(int n)
    /** \memberof ex1b
     *  Function to convert from binary to decimal.
     */
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

//comparar diferentes preditores, entropias com a entropia do audio original

int predictor(float prevSample, float currSample){
    /** \memberof ex1b
     *  Function to calculate the predictor.
     */
    int p = (int)(0.5*prevSample + 0.5*currSample);
    return p;
}

int predictor2(float prevSample){
    /** \memberof ex1b
     *  Function to calculate the integer value of the predictor.
     */
    return (int)prevSample;
}

vector<int> encode(int m, string file){
    /** \memberof ex1b
     *  Function to encode the code.
     */
    AudioFile<float> audioFile;
    audioFile.load (file);
    int numSamples = audioFile.getNumSamplesPerChannel();
    s = numSamples;
    int numChannels = audioFile.getNumChannels();
    c = numChannels;
    vector<int> r_enc,g_res;
    int po =pow(2,log2(m));
    int r;
    ofstream ofs("histogram_ex1b_residual_lossless.txt");
    map<int,int> map;
    BitStream bs("out.bit", 'w');
    Golomb g(m);
    char* s;
    r = (int)(audioFile.samples[0][0]*po + audioFile.samples[1][0]*po)/2;
    r_enc.push_back(r);
    g_res = g.encode(r);

    for (int j: g_res){
        bs.writebit(j);
        s += j;
    }
    for (int i = 1; i < numSamples; i++){
            float media = (audioFile.samples[0][i]*po + audioFile.samples[1][i]*po)/2;
            float media2 = (audioFile.samples[0][i-1]*po + audioFile.samples[1][i-1]*po)/2;
            r = (int) media - predictor(media2, media);
            //r = (int) media - predictor2(media2);
            map[r]++;
            r_enc.push_back(r);
            g_res = g.encode(r);
            for (int j: g_res){
                bs.writebit(j);
                s += j;
            }
    }

    /*histogram and entropy*/
    double h =0;
    for(pair<int,int> i : map){
        double p = (static_cast<double>(i.second)/static_cast<double>(numChannels*numSamples));
        h = h-p*log2(p);
        ofs << "" << i.first << " -> " << i.second << endl;
    }
    cout << "Entropy of residual values lossless: " << h << endl;

    ofs.close();
    bs.closeF();
    
    return r_enc;
}

vector<int> decode(int m, string file, string audiofile){
    /** \memberof ex1b
     *  Function to decode the code.
     */
    AudioFile<float> copy;
    int numChannels = 2;
    
    const int n = log2(m);
    int nbits = log2(m);
    int c = 0;
    bool f,plus1 = false;
    int bit;
    vector<int> v, temp, res, resto;
    vector<double> samples;

    BitStream bs(file,'r');

    int b=0;
    int co = 0;
    while(b!=-1){
        b = bs.readbit();
        v.push_back(b);
        co++;
    }
    
    Golomb g(m);
    int pos = 0;
    string tmp = "";
    for(int bit: v){ 
        if (f){
            nbits--;           
            tmp+=to_string(bit);  
            resto.push_back(bit);
        }
        temp.push_back(bit); 
        if (nbits == 0){
            f = false;
            if(plus1){
                int r_dec = g.decode(temp);
                double sample = r_dec/pow(2,log2(m)) ;
                samples.push_back(sample);
                res.push_back(r_dec);
                temp.clear();
                tmp="";
                resto.clear();
                nbits = n;
                plus1=false;
            }
            else{
                int u = (1 << n+1) - m;
                int result = g.binaryToDecimal(resto);
                if(result < u){
                    int r_dec = g.decode(temp);
                    double sample = r_dec/pow(2,log2(m));
                    samples.push_back(sample);
                    res.push_back(r_dec);
                    temp.clear();
                    tmp="";
                    resto.clear();
                    nbits = n;
                }
                else{
                    nbits++;
                    plus1=true;
                    f=true;
                }
            }
        }else if (bit == 1){
            f = true;
        }
    }
        
    copy.setNumSamplesPerChannel(s);
    copy.setNumChannels(2);

    for (int i = 0; i < s; i++){
        for(int ch = 0; ch < 2; ch++){
            copy.samples[c][i] = samples[i];
        }
    }
    copy.save(audiofile, AudioFileFormat::Wave);
    return res;
}



vector<int> encode_lossy(int m,int div, string file){
    /** \memberof ex1b
     *  Function to encode the code in a lossy way.
     */
    AudioFile<float> audioFile;
    audioFile.load (file);
    int numSamples = audioFile.getNumSamplesPerChannel();
    s = numSamples;
    int numChannels = audioFile.getNumChannels();
    c = numChannels;
    vector<int> r_enc,g_res;
    int po =pow(2,log2(m));
    int r;
    ofstream ofs("histogram_ex1b_residual_lossy.txt");
    map<int,int> map;
    BitStream bs("out.bit", 'w');
    Golomb g(m);
    char* s;
    r = (int)(audioFile.samples[0][0]*po + audioFile.samples[1][0]*po)/2;
    
    r = r >> div;
    r = r << div;

    r_enc.push_back(r);
    g_res = g.encode(r);

    for (int j: g_res){
        bs.writebit(j);
        s += j;
    }
    for (int i = 1; i < numSamples; i++){
            float media = (audioFile.samples[0][i]*po + audioFile.samples[1][i]*po)/2;
            float media2 = (audioFile.samples[0][i-1]*po + audioFile.samples[1][i-1]*po)/2;
            r = (int) media - predictor(media2, media);
            r = r >> div;
            r = r << div;
            //r = (int) media - predictor2(media2);
            map[r]++;
            r_enc.push_back(r);
            g_res = g.encode(r);
            for (int j: g_res){
                bs.writebit(j);
                s += j;
            }
    }

    /*histogram and entropy*/
    double h =0;
    for(pair<int,int> i : map){
        double p = (static_cast<double>(i.second)/static_cast<double>(numChannels*numSamples));
        h = h-p*log2(p);
        ofs << "" << i.first << " -> " << i.second << endl;
    }
    cout << "Entropy of residual values lossy: " << h << endl;

    ofs.close();
    bs.closeF();
    
    return r_enc;
}

vector<int> decode_lossy(int m,int div, string file, string audiofile){
    /** \memberof ex1b
     *  Function to decode the code in a lossy way.
     */

    AudioFile<float> copy;
    int numChannels = 2;
    
    const int n = log2(m);
    int nbits = log2(m);
    int c = 0;
    bool f,plus1 = false;
    int bit;
    vector<int> v, temp, res, resto;
    vector<double> samples;

    BitStream bs(file,'r');

    int b=0;
    int co = 0;
    while(b!=-1){
        b = bs.readbit();
        v.push_back(b);
        co++;
    }
    
    Golomb g(m);
    int pos = 0;
    string tmp = "";
    for(int bit: v){ 
        if (f){
            nbits--; 
            tmp+=to_string(bit);  
            resto.push_back(bit);
        }
        temp.push_back(bit); 
        if (nbits == 0){
            f = false;
            if(plus1){
                int r_dec = g.decode(temp);
                r_dec = r_dec << div;
                r_dec = r_dec >> div;
                double sample = r_dec/pow(2,log2(m)) ;
                samples.push_back(sample);
                res.push_back(r_dec);
                temp.clear();
                tmp="";
                resto.clear();
                nbits = n;
                plus1=false;
            }
            else{
                int u = (1 << n+1) - m;
                int result = g.binaryToDecimal(resto);
                if(result < u){
                    int r_dec = g.decode(temp);
                    r_dec = r_dec << div;
                    r_dec = r_dec >> div;
                    double sample = r_dec/pow(2,log2(m));
                    samples.push_back(sample);
                    res.push_back(r_dec);
                    temp.clear();
                    tmp="";
                    resto.clear();
                    nbits = n;
                }
                else{
                    nbits++;
                    plus1=true;
                    f=true;
                }
            }
        }else if (bit == 1){
            f = true;
        }

    }
        
    copy.setNumSamplesPerChannel(s);
    copy.setNumChannels(2);

    for (int i = 0; i < s; i++){
        for(int ch = 0; ch < 2; ch++){
            copy.samples[c][i] = samples[i];
        }
    }
    copy.save(audiofile, AudioFileFormat::Wave);
    return res;
}



int main(int argc, char **argv){
    /** \class ex1b
     *  Main class of a lossless predictive audio codec followedby Golomb encoding.
     */

    if(argc != 3){
        cerr << "Usage: ./ex1b <input_audio> <output_audio> \nExample: ./ex1c audio/sample01.wav audio/output_sample01.wav" << endl;
        return -1;
    }

    int op;
    cout << "0 - lossless" << endl;
    cout << "1 - lossy" << endl;
    cout << "Option: ";
    cin >> op;

    auto start = high_resolution_clock::now();


    //lossless
    if(op == 0){
        //calculating the ideal m for golomb
        AudioFile<float> audioFile;
        audioFile.load (argv[1]);
        int numSamples = audioFile.getNumSamplesPerChannel();
        int numChannels = audioFile.getNumChannels();
        double sum = 0;
        map<int,int> map;
        
        for (int i = 1; i < numSamples; i++){
            for (int channel = 0; channel < numChannels; channel++){
                    sum += - audioFile.samples[channel][i]*pow(2,20);
                    map[audioFile.samples[channel][i]*pow(2,20)]++;
            }
        }

        double h =0;
        ofstream ofs("histogram_ex1b_original.txt");
    
        for(pair<int,int> i : map){
            double p = (static_cast<double>(i.second)/static_cast<double>(numChannels*numSamples));
            h = h-p*log2(p);
            ofs << "" << i.first << " -> " << i.second << endl;
        }
        cout << "Entropy of original values: " << h << endl;

        double mean = sum/(numSamples*numChannels);
        double alpha = mean/(mean+1.0);
        int m = (int) ceil(-1/log2(alpha));
        cout << "ideal m: " << m << endl;

        vector<int> v = encode(m, argv[1]);
        vector<int> result = decode(m, "out.bit",argv[2]);
        
        for (int i = 0; i < result.size(); i++){
            if (result[i] != v[i]){
                cout << "Diff at pos " << i << ": " << v[i] << " != " << result[i] << endl;
            }   
        }


        cout << "Original File size: " <<  fs::file_size(argv[1]) << " bits" << endl;
        cout << "Compressed File size: " << fs::file_size("out.bit") << " bits" << endl;

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Processing Time: " << duration.count()*pow(10,-6) << " seconds" << endl;
        return 0;
    }

    //lossy
    if(op == 1){
        //calculating the ideal m for golomb
        AudioFile<float> audioFile;
        audioFile.load (argv[1]);
        int numSamples = audioFile.getNumSamplesPerChannel();
        int numChannels = audioFile.getNumChannels();
        double sum = 0;
        map<int,int> map;

        for (int i = 1; i < numSamples; i++){
            for (int channel = 0; channel < numChannels; channel++){
                    sum += - audioFile.samples[channel][i]*pow(2,20);
                    map[audioFile.samples[channel][i]*pow(2,20)]++;
            }
        }

        double h =0;
        ofstream ofs("histogram_ex1b_original.txt");
    
        for(pair<int,int> i : map){
            double p = (static_cast<double>(i.second)/static_cast<double>(numChannels*numSamples));
            h = h-p*log2(p);
            ofs << "" << i.first << " -> " << i.second << endl;
        }
        cout << "Entropy of original values: " << h << endl;

        double mean = sum/(numSamples*numChannels);
        double alpha = mean/(mean+1.0);
        int m = (int) ceil(-1/log2(alpha));
        cout << "ideal m: " << m << endl;

        //quantization

        int niveis;
        cout << "Quantization value: ";
        cin >> niveis;
        int div = (32/niveis)/2;   //nºde bits a dividir por quantos niveis quero
                                   //a dividir por 2 porque dar shift num bit divide por dois, 2 bits por 4, etc...
        
        vector<int> v = encode_lossy(m,div, argv[1]);
        vector<int> result = decode_lossy(m,div, "out.bit",argv[2]);
        
        for (int i = 0; i < result.size(); i++){
            if (result[i] != v[i]){
                cout << "Diff at pos " << i << ": " << v[i] << " != " << result[i] << endl;
            }   
        }

        cout << "Original File size: " <<  fs::file_size(argv[1]) << " bits" << endl;
        cout << "Compressed File size: " << fs::file_size("out.bit") << " bits" << endl;

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Processing Time: " << duration.count()*pow(10,-6) << " seconds" << endl;
        return 0;
    }
    
}


