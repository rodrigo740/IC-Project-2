#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <chrono>
#include "Golomb.h"

using namespace std;
using namespace cv;
using namespace std::chrono;


vector<int> decoder(int m, int height, int width, string filename){

    const int n = log2(m);
    int nbits = log2(m);
    cout << "nbits: " << nbits << endl;

    bool plus1 = false, f = false;

    int startU = height*width;
    int startV = startU + startU/4;

    BitStream bs1(filename, 'r');
    

    vector<int> yuvFrame, v, temp, resto;
    //v = bs1.readFile();

    string tmp = "";
    Golomb g(m);
    int bit = bs1.readbit();
    while(bit != -1){
        if (f){
            nbits--;
            tmp+=to_string(bit);
            resto.push_back(bit);
        }
        temp.push_back(bit);
        if (nbits == 0){
            f = false;
            if(plus1){
                /*cout << "to decode1: ";

                for(int a: temp){
                    cout << a;
                }
                cout << endl;*/
                int r_dec = g.decode(temp);
                yuvFrame.push_back(r_dec);
                temp.clear();
                resto.clear();

                tmp="";
                nbits = n;
                plus1=false;
            }
            else{
                int u = (1 << n+1) - m;
                int result = g.binaryToDecimal(resto);
                
                
                if(result < u){
                    /*cout << "to decode2: ";

                    for(int a: temp){
                        cout << a;
                    }
                    cout << endl;*/
                    int r_dec = g.decode(temp);
                    yuvFrame.push_back(r_dec);
                    temp.clear();
                    resto.clear();

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
        bit = bs1.readbit();
    }
    /*
    for(int b: yuvFrame){
        cout << b << " ";
    }

    cout << endl;
    */


    return yuvFrame;
}

int main(int argc, char **argv){

    auto start = high_resolution_clock::now();
    
    /*
    if(argc != 3){
        cerr << "Usage: ./ex1c " << endl;
        return -1;
    }*/
    
    Mat image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    Mat y = Mat::zeros(image.size(), CV_32SC1);
    Mat u = Mat::zeros(image.size(), CV_32SC1);
    Mat v = Mat::zeros(image.size(), CV_32SC1);

    int height = image.rows/2;
    int width = image.cols/2;
    int n_dim = 2;
    int n_ch = 1;

    int sizes[n_dim] = {width, height};
    

    vector<Mat> bgr_planes;
    split(image, bgr_planes);

    bgr_planes[0].convertTo(bgr_planes[0], CV_8UC1);
    bgr_planes[1].convertTo(bgr_planes[1], CV_8UC1);
    bgr_planes[2].convertTo(bgr_planes[2], CV_8UC1);
    
    uchar b,g,r;
    const int c = 1000;

    vector<int> yValues;

    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            b = bgr_planes[0].at<uchar>(i,j);
            g = bgr_planes[1].at<uchar>(i,j);
            r = bgr_planes[2].at<uchar>(i,j);

            // Y
            y.at<int>(i,j) = (0.257 * r + 0.504 * g + 0.098 * b +  16) * c;

            // U
            u.at<int>(i,j) = (-0.148 * r - 0.291 * g + 0.439 * b + 128) * c;

            // V
            v.at<int>(i,j) = (0.439 * r - 0.368 * g - 0.071 * b + 128) * c;
        }
    }

    Mat u2 = Mat(Size(width,height), CV_32SC1, Scalar(0, 0, 0));
    Mat v2 = Mat(Size(width,height), CV_32SC1, Scalar(0, 0, 0));

    for (int i = 0; i < height-1; i+=2)
    {
        for (int j = 0; j < width-1; j+=2)
        {
            u2.at<int>(i,j) = (u.at<int>(i,j) + u.at<int>(i+1,j) + u.at<int>(i+1,j) + u.at<int>(i+1,j+1))/4;
            v2.at<int>(i,j) = (v.at<int>(i,j) + v.at<int>(i+1,j) + v.at<int>(i+1,j) + v.at<int>(i+1,j+1))/4;
        }
    }


    int x = height*width;
    int n = x/4;

    
    
    Mat yuvFrame = Mat::zeros(6, n, CV_32SC1);

    MatIterator_<int> itYUV, itY = y.begin<int>(), itU = u.begin<int>(), itV = v.begin<int>();

    vector<int> yuvF;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < n; j++)
        {
            yuvFrame.at<int>(i,j) = *itY;
            yuvF.push_back(*itY);
            itY++;
        }
    }
    
    for (int i = 0; i < n; i++)
    {
        yuvFrame.at<int>(4, i) = *itU;
        yuvF.push_back(*itU);
        itU++;
    }

    for (int i = 0; i < n; i++)
    {
        yuvFrame.at<int>(5, i) = *itV;
        yuvF.push_back(*itV);
        itV++;
    }

    double mean = 0;

    for (int i = 0; i < yuvFrame.rows; i++)
    {
        for (int j = 0; j < yuvFrame.cols; j++)
        {
            mean += yuvFrame.at<int>(i,j);
        }
    }

    mean=mean/x;
    double alpha = mean/(mean+1.0);
    int m = (int) ceil(-1/log2(alpha));
    Golomb gol(m);
    BitStream bs("encoded.bit", 'w');

    vector<int> vec, res;

    int residual;

    int row, col = 0;

    res = gol.encode(yuvF[0]);

    for (int bit: res)
    {
        bs.writebit(bit);
    }
    

    for (int i = 0; i < yuvF.size()-1; i++)
    {
        residual = yuvF[i+1] - yuvF[i];
        res = gol.encode(residual);

        for (int bit: res)
        {
            bs.writebit(bit);
        }
    }
    

    bs.closeF();
    

    vector<int> decoded = decoder(m, 6, n, "encoded.bit");
    //vector<int> decoded = decoder(15000, 6, 65000, "encoded.bit");
    






































    /*
    by: 235045
    bu: 128000
    bv: 128000
    */

    /*
    vector<Mat> ch;
    ch.push_back(gray);
    ch.push_back(u);
    ch.push_back(v);
    Mat img;
    Mat half(gray.size(), gray.type(), 127);
    Mat h = Mat::zeros(image.size(), CV_8UC1);
    namedWindow("Grayscale before", WINDOW_NORMAL);
    imshow("Grayscale before", gray);
    waitKey(0);
    ch = {gray, u, gray};
    merge(ch, img);
    namedWindow("u", WINDOW_NORMAL);
    imshow("u", img);
    waitKey(0);
    ch = {gray, gray, v};
    merge(ch, img);
    namedWindow("v", WINDOW_NORMAL);
    imshow("v", img);
    waitKey(0);
    */
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Processing Time: " << duration.count() << " μs" << endl;
    
    return 0;
}