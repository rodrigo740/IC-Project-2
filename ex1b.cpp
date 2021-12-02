#include"AudioFile/AudioFile.h"
#include"Golomb.h"
#include<vector>
#include<fstream>
#include<math.h>

using namespace std;

int predictor(float prevSample, float currSample){
    int p = (int)(0.5*prevSample + 0.5*currSample);
    return p;
}

int main(int argc, char **argv){
    ofstream ofs("out.txt");
    AudioFile<float> audioFile;
    audioFile.load (argv[1]);
    int m = 4;
    int numSamples = audioFile.getNumSamplesPerChannel();
    int numChannels = audioFile.getNumChannels();
    vector<int> r_enc;
    int po =pow(2,8);
    for (int i = 1; i < numSamples; i++){
        for (int channel = 0; channel < numChannels; channel++){
            //cout << audioFile.samples[channel][i] << endl;
            int r = audioFile.samples[channel][i]*po - predictor(audioFile.samples[channel][i-1]*po, audioFile.samples[channel][i]*po);
            //cout << r << endl;
            Golomb g(m);
            for (int i: g.encode(r)){
                r_enc.push_back(i);
                ofs << i;
            }
        }
    }
    ofs.close();
    return 0;
}

