#include"Golomb.h"
#include<vector>
#include <chrono>
using namespace std::chrono;
using namespace std;

int main(){
    bool flag = false;
    vector<int> m_errors;

    cout << "Testing Golomb class\nm = [2,1500], encoded value = [-2000,2000]" << endl;

    auto start = high_resolution_clock::now();

    for (int j = 2; j <= 1500; j++)
    {
        Golomb g0(j);
        for(int i = -2000; i <= 2000; i++){
            vector<int> x0 = g0.encode(i);
            if (i!= g0.decode(x0)){
                cout << "Error with m = " << j << endl;
                cout << "real encoded value: " << i << " != " << g0.decode(x0) << endl;
                flag = true;
                m_errors.push_back(j);
            }
        }
        if (flag)
        {
            cout << "m = " << j << " coded with errors!" << endl;
            flag = false;
        }else{
            cout << "m = " << j << " coded without errors!" << endl;
        }
        
    }
    
    cout << "\nTest is over" << endl;
    for(int m: m_errors){
        cout << "Errors found in m = " << m << endl;
    }
    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    cout << "Processing Time: " << duration.count() << "s" << endl;

    return 0;
}