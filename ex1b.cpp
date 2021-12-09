#include"AudioFile/AudioFile.h"
#include"Golomb.h"
#include"BitStream.h"
#include<vector>
#include<fstream>
#include<math.h>

using namespace std;

int predictor(float prevSample, float currSample){
    int p = (int)(0.5*prevSample + 0.5*currSample);
    return p;
}

vector<int> encode(int m, string file){

    AudioFile<float> audioFile;
    audioFile.load (file);

    int numSamples = audioFile.getNumSamplesPerChannel();
    int numChannels = audioFile.getNumChannels();
    vector<int> r_enc,g_res;
    int po =pow(2,8);
    int r;
    AudioFile<float> copiaFile;
    copiaFile.setNumSamplesPerChannel(numSamples);
    copiaFile.setNumChannels(numChannels);

    BitStream bs("out.bit", 'w');

    char* s;

    for (int i = 1; i < numSamples; i++){
        for (int channel = 0; channel < numChannels; channel++){
            //cout << audioFile.samples[channel][i] << endl;
            r = (int) (audioFile.samples[channel][i])*po - predictor(audioFile.samples[channel][i-1]*po, audioFile.samples[channel][i]*po);
            //cout << r << endl;
            Golomb g(m);
            //cout << "Valor a codificar: " << r << endl;
            r_enc.push_back(r);
            g_res = g.encode(r);
            for (int j: g_res){
                cout << j;
                bs.writebit(j);
                s += j;
            }


            cout << atoi(s) << endl;
            copiaFile.samples[channel][i]= atoi(s);
            s = "";
            cout << "asnd" << endl;
            
            

            //cout << endl;
        }
    }
    bs.closeF();
    copiaFile.setBitDepth (audioFile.getBitDepth());
    copiaFile.setSampleRate (audioFile.getSampleRate());
    copiaFile.save("out.wav", AudioFileFormat::Wave);
    return r_enc;
}

vector<int> decode(int m, string file){
    const int n = log2(m);
    int nbits = log2(m);
    int c = 0;
    bool f = false;
    int bit;
    vector<int> v, temp, res;

    cout << "nbits: " << nbits << endl;

    BitStream bs(file,'r');

    v = bs.readFile();

    Golomb g(m);

    int pos = 0;

    for(int bit: v){
        
        if (f)
        {
            nbits--;
        }

        temp.push_back(bit);

        if (nbits == 0){
            nbits = n;
            //cout << "Segment done!" << endl;
            f = false;
            res.push_back(g.decode(temp));
            /*cout << "Valor descodificado: " << res[pos] << endl;
            for(int asd: temp){
                cout << asd;
            }
            cout << endl;
            pos++;*/
            temp.clear();
        }else if (bit == 1)
        {
            f = true;
        }
    }

    /*
    while (nbits>0)
    {
        
        if (f)
        {
            nbits--;
            cout << nbits << endl;
        }

        bit = bs.readbit();

        
        v.push_back(bit);
        cout << "bit: " << bit << endl;

        if (bit == 1)
        {
            f = true;
        }        
    }
    */
   return res;
}


int main(int argc, char **argv){
    
    vector<int> v = encode(4, argv[1]);
    vector<int> result = decode(4, "out.bit");
    
    for (int i = 0; i < result.size(); i++)
    {
        if (result[i] != v[i])
        {
            cout << "Diff at pos " << i << ": " << v[i] << " != " << result[i] << endl;
            
        }
        
    }
    
    return 0;
}
