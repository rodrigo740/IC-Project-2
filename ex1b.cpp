#include"AudioFile/AudioFile.h"
#include"Golomb.h"
#include"BitStream.h"
#include<vector>
#include<fstream>
#include<math.h>
#include <limits>
#include<map>
#include <chrono>
using namespace std::chrono;
using namespace std;

int s = 0;
int c = 0;

//comparar diferentes preditores, entropias com a entropia do audio original

int predictor(float prevSample, float currSample){
    int p = (int)(0.5*prevSample + 0.5*currSample);
    return p;
}

vector<int> encode(int m, string file){

    AudioFile<float> audioFile;
    audioFile.load (file);
    int numSamples = audioFile.getNumSamplesPerChannel();
    s = numSamples;
    int numChannels = audioFile.getNumChannels();
    c = numChannels;
    vector<int> r_enc,g_res;
    int po =pow(2,8);
    int r;

    ofstream ofs("histogram_ex1b.txt");

    map<int,int> map;
        
    BitStream bs("out.bit", 'w');

    char* s;

    for (int i = 1; i < numSamples; i++){
        for (int channel = 0; channel < numChannels; channel++){
            r = (int) (audioFile.samples[channel][i])*po - predictor(audioFile.samples[channel][i-1]*po, audioFile.samples[channel][i]*po);
            map[r]++;
            Golomb g(m);
            r_enc.push_back(r);
            //cout << "to encode: " << r << endl;
            g_res = g.encode(r);
            for (int j: g_res){
                bs.writebit(j);
                s += j;
            }
        }
    }

    /*histogram and entropy*/
    double h =0;
    for(pair<int,int> i : map){
        double p = (static_cast<double>(i.second)/static_cast<double>(numChannels*numSamples));
        h = h-p*log2(p);
        ofs << "" << i.first << " -> " << i.second << endl;
    }
    cout << "Entropy: " << h << endl;

    ofs.close();
    bs.closeF();
    
    return r_enc;
}

vector<int> decode(int m, string file, string audiofile){

    AudioFile<float> copy;
    int numChannels = 2;
    
    const int n = log2(m);
    int nbits = log2(m);
    int c = 0;
    bool f,plus1 = false;
    int bit;
    vector<int> v, temp, res;
    vector<double> samples;

    BitStream bs(file,'r');

    int b = bs.readbit();
    while(b != 1){
        v.push_back(bs.readbit());
    }
    /*for(int i = 0; i < 16; i++){
        v.push_back(bs.readbit());
    }*/

    v = bs.readFile();

    Golomb g(m);

    int pos = 0;
    //001011
    string tmp = "";
    for(int bit: v){ 

        /*if (f){
            nbits--;
        }
        temp.push_back(bit);
        if (nbits == 0){
            nbits = n;
            f = false;
            int r_dec = g.decode(temp);
            double sample = r_dec/pow(2,8);
            samples.push_back(sample);
            res.push_back(r_dec);
            temp.clear();
        }else if (bit == 1){
            f = true;
        }*/

        if (f){
            nbits--;
            tmp+=to_string(bit);
        }
        temp.push_back(bit);
        if (nbits == 0){
            f = false;
            if(plus1){
                /*cout << "to decode: ";
                for(int d:temp){
                    cout << d;
                }
                cout << endl;*/ 
                int r_dec = g.decode(temp);
                double sample = r_dec/pow(2,8) ;
                samples.push_back(sample);
                res.push_back(r_dec);
                temp.clear();
                tmp="";
                nbits = n;
                plus1=false;
            }
            else{
                
                int u = (1 << n+1) - m;
                long result = stol(tmp);
                if(result < u){
                    /*cout << "to decode: ";
                    for(int d:temp){
                        cout << d;
                    }
                    cout << endl;*/
                    int r_dec = g.decode(temp);
                    double sample = r_dec/pow(2,8);
                    samples.push_back(sample);
                    res.push_back(r_dec);
                    temp.clear();
                    tmp="";
                    nbits = n;
                }
                else{
                    nbits++;
                    plus1=true;
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
    auto start = high_resolution_clock::now();

    //calculating the ideal m for golomb
    AudioFile<float> audioFile;
    audioFile.load (argv[1]);
    int numSamples = audioFile.getNumSamplesPerChannel();
    int numChannels = audioFile.getNumChannels();
    double sum = 0;
    for (int i = 1; i < numSamples; i++){
        for (int channel = 0; channel < numChannels; channel++){
                sum += - audioFile.samples[channel][i]*pow(2,20);
        }
    }
    double mean = sum/(numSamples*numChannels);
    double alpha = mean/(mean+1.0);
    int m = (int) ceil(-1/log2(alpha));
    cout << "ideal m: " << m << endl;
    m=8;

    vector<int> v = encode(m, argv[1]);
    vector<int> result = decode(m, "out.bit","out.wav");
    
    for (int i = 0; i < result.size(); i++){
        if (result[i] != v[i]){
            cout << "Diff at pos " << i << ": " << v[i] << " != " << result[i] << endl;
        }   
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Processing Time: " << duration.count()*pow(10,-6) << " seconds" << endl;
    return 0;
}
