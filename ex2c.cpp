#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <chrono>
#include "Golomb.h"
#include "BitStream.h"

using namespace std;
using namespace cv;
using namespace std::chrono;


vector<uchar> decoder(int m, int height, int width, string filename){

    const int n = log2(m);
    int nbits = log2(m);
    cout << "nbits: " << nbits << endl;

    bool plus1 = false, f = false;

    BitStream bs1(filename, 'r');
    

    vector<uchar> yuvFrame;
    vector<int> temp,resto;

    Golomb g(m);
    int bit = bs1.readbit();
    while(bit != -1){

        if (f){
            nbits--;
            resto.push_back(bit);
        }
        temp.push_back(bit);
        if (nbits == 0){
            f = false;
            if(plus1){
                int r_dec = g.decode(temp);
                yuvFrame.push_back(r_dec);
                temp.clear();
                resto.clear();

                nbits = n;
                plus1=false;
            }
            else{
                int u = (1 << n+1) - m;
                int result = g.binaryToDecimal(resto);
                
                if(result < u){

                    int r_dec = g.decode(temp);
                    yuvFrame.push_back(r_dec);
                    temp.clear();
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
        bit = bs1.readbit();
    }


    return yuvFrame;
}

Mat reconstruct(vector<uchar> yuvFrame, int width, int height, int n){
    cout << "Frame size: " << yuvFrame.size() << endl;
    cout << "Rec image" << endl;
    Mat y = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat u = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat v = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));

    Mat b = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat g = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat r = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));

    int col = 1, row = 0, yval, uval, vval;
    
    int startU = 4*n;

    y.at<uchar>(0,0) = (yuvFrame[0]);

    for (int i = 1; i < startU; i++)
    {
        y.at<uchar>(row,col) = (yuvFrame[i]);

        col++;
        if(col == width){
            col = 0;
            row++;
        }
    }

    row = 0;
    col = 2;

    int startV = (4*n+n);

    for (int i = 0; i < n; i++)
    {

        u.at<uchar>(row,col) = (yuvFrame[startU+i]);
        u.at<uchar>(row,col+1) = (yuvFrame[startU+i]);
        u.at<uchar>(row+1,col) = (yuvFrame[startU+i]);
        u.at<uchar>(row+1,col+1) = (yuvFrame[startU+i]);

        v.at<uchar>(row,col) = (yuvFrame[startV+i]);
        v.at<uchar>(row,col+1) = (yuvFrame[startV+i]);
        v.at<uchar>(row+1,col) = (yuvFrame[startV+i]);
        v.at<uchar>(row+1,col+1) = (yuvFrame[startV+i]);

        
        col+=2;
        if(col == width){
            col = 0;
            row+=2;
        }
    }

    int val;

    for (int i = 0; i < y.rows; i++)
    {
        for (int j = 0; j < y.cols; j++)
        {
            yval = y.at<uchar>(i,j)-16;
            vval = v.at<uchar>(i,j)-128;
            uval = u.at<uchar>(i,j)-128;

            val = 1.164*yval + 1.596*vval;
            if (val > 255) val = 255;
            if (val < 0)   val = 0;
            
            r.at<uchar>(i,j) = (uchar) val;

            val = 1*yval - 0.813*uval - 0.391*vval;
            if (val > 255)  val = 255;
            if (val < 0)    val = 0;
            g.at<uchar>(i,j) = (uchar) val;

            val = 1.164 * yval + 2.018*uval;
            if (val > 255) val = 255;
            if (val < 0)   val = 0;
                
            b.at<uchar>(i,j) = (uchar) val;

        }
    }
    namedWindow("y rec", WINDOW_NORMAL);
    imshow("y rec", y);
    waitKey(0);
    namedWindow("u rec", WINDOW_NORMAL);
    imshow("u rec", u);
    waitKey(0);
    namedWindow("v rec", WINDOW_NORMAL);
    imshow("v rec", v);
    waitKey(0);
    

    vector<Mat> bgr = {b, g, r};
    Mat img;
    merge(bgr, img);

    return img;

}



vector<uchar> recFrame(vector<uchar> decFrame){

    vector<uchar> yuvFrame;

    // real value = residual + last value
    yuvFrame.push_back(decFrame[0]);

    for (int i = 1; i < decFrame.size(); i++)
    {
        yuvFrame.push_back(decFrame[i] + yuvFrame[i-1]);
    }
    

    return yuvFrame;
}

int main(int argc, char **argv){
    
    if(argc != 5){
        cerr << "Usage: ./ex2c <input_img> <number_of_levels_Y_component> <number_of_levels_U_component> <number_of_levels_V_component>\nExample: ./ex2c images/lena.ppm 8 8 8" << endl;
        return -1;
    }

    auto start = high_resolution_clock::now();

    int nlvl_r = stoi(argv[2]);
    int nlvl_g = stoi(argv[3]);
    int nlvl_b = stoi(argv[4]);

    const int d_r = 256/nlvl_r;
    const int d_g = 256/nlvl_g;
    const int d_b = 256/nlvl_b;

    double valueByLevel_r[nlvl_r];
    double valueByLevel_g[nlvl_g];
    double valueByLevel_b[nlvl_b];

    for (int i = 0; i < nlvl_r; i++)
    {
        valueByLevel_r[i] = (i+1)*d_r/2 + (i*d_r)/2;
    }
    for (int i = 0; i < nlvl_g; i++)
    {
        valueByLevel_g[i] = (i+1)*d_g/2 + (i*d_g)/2;
    }
    for (int i = 0; i < nlvl_b; i++)
    {
        valueByLevel_b[i] = (i+1)*d_b/2 + (i*d_b)/2;
    }
    
    Mat image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    Mat y = Mat::zeros(image.size(), CV_8UC1);
    Mat u = Mat::zeros(image.size(), CV_8UC1);
    Mat v = Mat::zeros(image.size(), CV_8UC1);

    int height = image.rows/2;
    int width = image.cols/2;
    

    vector<Mat> bgr_planes;
    split(image, bgr_planes);

    bgr_planes[0].convertTo(bgr_planes[0], CV_8UC1);
    bgr_planes[1].convertTo(bgr_planes[1], CV_8UC1);
    bgr_planes[2].convertTo(bgr_planes[2], CV_8UC1);

    
    
    uchar b,g,r;
    int val;
    int tmp;
    int level;

    vector<uchar> uv, vv;

    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            b = bgr_planes[0].at<uchar>(i,j);
            g = bgr_planes[1].at<uchar>(i,j);
            r = bgr_planes[2].at<uchar>(i,j);

            
        /*
            tmp = trunc(b/d_b);
            b = valueByLevel_b[tmp];

            tmp = trunc(g/d_g);
            g = valueByLevel_g[tmp];

            tmp = trunc(r/d_r);
            r = valueByLevel_r[tmp];*/


            // Y
            val = 0.299 * r + 0.587 * g + 0.114 * b + 16;
            if(val < 16) val = 16;
            if(val > 235) val = 235;

            tmp = trunc(val/d_r);
            val = valueByLevel_r[tmp];


            y.at<uchar>(i,j) = val;

            // U
            val = (-0.147 * r - 0.289 * g + 0.436 * b) + 128;
            if(val < 16) val = 16;
            if(val > 240) val = 240;

            tmp = trunc(val/d_g);
            val = valueByLevel_g[tmp];

            uv.push_back(val);
            u.at<uchar>(i,j) = val;

            // V
            val = (0.615 * r - 0.515 * g - 0.100 * b) + 128;
            if(val < 16) val = 16;
            if(val > 240) val = 240;

            tmp = trunc(val/d_b);
            val = valueByLevel_b[tmp];


            vv.push_back(val);
            v.at<uchar>(i,j) = val;
        }
    }

    Mat u2 = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat v2 = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));

    int row=0, col=0;

    vector<uchar> u2v,v2v;
    
    // each pixel in U2 and V2 Mats correspond to 4 pixels in U and V Mats
    for (int i = 0; i < u.rows; i+=2)
    {
        for (int j = 0; j < u.cols; j+=2)
        {
            val = (u.at<uchar>(i,j) + u.at<uchar>(i,j+1) + u.at<uchar>(i+1,j) + u.at<uchar>(i+1,j+1))/4;
            u2v.push_back(val);
            u2.at<uchar>(row,col) = val;

            val = (v.at<uchar>(i,j) + v.at<uchar>(i,j+1) + v.at<uchar>(i+1,j) + v.at<uchar>(i+1,j+1))/4;
            v2v.push_back(val);
            v2.at<uchar>(row,col) = val;
            
            col++;
            if(col == u.cols){
                col=0;
                row++;
            }
        }
    }


    int x = (height*2)*(width*2);
    int n = x/4;
    
    // The YUV Frame consists in the sequence of all Y values, then the U values and finally the V values
    // The U and V each occupy a single row
    // This frame will be encoded using golomb codes
    Mat yuvFrame = Mat::zeros(6, n, CV_8UC1);

    MatIterator_<uchar> itY = y.begin<uchar>();

    vector<uchar> yuvF;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < n; j++)
        {
            yuvFrame.at<uchar>(i,j) = *itY;
            yuvF.push_back(*itY);
            itY++;
        }
    }
    
    for (int i = 0; i < n; i++)
    {
        yuvFrame.at<uchar>(4, i) = u2v[i];
        yuvF.push_back(u2v[i]);
        
    }


    for (int i = 0; i < n; i++)
    {
        yuvFrame.at<uchar>(5, i) = v2v[i];
        yuvF.push_back(v2v[i]);
    }

    // mean value calculation    
    double mean = 0;

    for(int val: yuvF){
        mean += val;
    }
    
    // ideal m calculation
    mean=mean/x;
    double alpha = mean/(mean+1.0);
    int m = (int) ceil(-1/log2(alpha));
    cout << "M ideal: " << m << endl;
    Golomb gol(m);
    BitStream bs("encoded.bit", 'w');

    vector<int> res;

    int residual;

    res = gol.encode(yuvF[0]);

    for (int bit: res)
    {
        bs.writebit(bit);
    }
    
    for (int i = 1; i < yuvF.size(); i++)
    {
        residual = yuvF[i] - yuvF[i-1];
        res = gol.encode(residual);

        for (int bit: res)
        {
            bs.writebit(bit);
        }
    }
    

    bs.closeF();
    

    vector<uchar> decoded = decoder(m, 6, n, "encoded.bit");

    vector<uchar> temp = recFrame(decoded);

    // difference check between original YUVFrame and reconstructed YUVFrame
    for (int i = 0; i < yuvF.size(); i++)
    {
        if(yuvF[i] != temp[i]){
            cout << "diff at pos " << i << ": " << (int)yuvF[i] << " != " << (int)temp[i] << endl;
        }
    }
    

    Mat rc_img = reconstruct(temp, width*2, height*2, n);
    
    namedWindow("Rec image", WINDOW_NORMAL);
    imshow("Rec image", rc_img);
    waitKey(0);

    /*
    namedWindow("y orig", WINDOW_NORMAL);
    imshow("y orig", y);
    waitKey(0);
    
    namedWindow("u orig", WINDOW_NORMAL);
    imshow("u orig", u);
    waitKey(0);


    namedWindow("v orig", WINDOW_NORMAL);
    imshow("v orig", v);
    waitKey(0);
    */




    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Processing Time: " << duration.count() << " μs" << endl;
    
    return 0;
}