#include"AudioFile/AudioFile.h"
#include"Golomb.h"
#include"BitStream.h"
#include<vector>
#include<fstream>
#include<math.h>

using namespace std;

int s = 0;
int c = 0;

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

    BitStream bs("out.bit", 'w');

    char* s;

    for (int i = 1; i < numSamples; i++){
        for (int channel = 0; channel < numChannels; channel++){
            r = (int) (audioFile.samples[channel][i])*po - predictor(audioFile.samples[channel][i-1]*po, audioFile.samples[channel][i]*po);
            Golomb g(m);
            r_enc.push_back(r);
            g_res = g.encode(r);
            for (int j: g_res){
                bs.writebit(j);
                s += j;
            }
        }
    }
    bs.closeF();
    
    return r_enc;
}

vector<int> decode(int m, string file, string audiofile){

    AudioFile<float> copy;
    int numChannels = 2;
    
    
    const int n = log2(m);
    int nbits = log2(m);
    int c = 0;
    bool f = false;
    int bit;
    vector<int> v, temp, res;
    vector<double> samples;

    BitStream bs(file,'r');

    v = bs.readFile();

    Golomb g(m);

    int pos = 0;

    for(int bit: v){ 
        if (f){
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

    vector<int> v = encode(5, argv[1]);
    vector<int> result = decode(5, "out.bit","out.wav");
    
    for (int i = 0; i < result.size(); i++){
        if (result[i] != v[i]){
            cout << "Diff at pos " << i << ": " << v[i] << " != " << result[i] << endl;
        }   
    }
    
    return 0;
}
