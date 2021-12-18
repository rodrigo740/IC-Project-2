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

    cout << "height: " << height << endl;
    cout << "width: " << width << endl;

    cout << "rows: " << x/n + 2 << endl;
    cout << "cols: " << n << endl;

    MatIterator_<int> itYUV, itY = y.begin<int>(), itU = u.begin<int>(), itV = v.begin<int>();

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < n; j++)
        {
            yuvFrame.at<int>(i,j) = *itY;
            itY++;
        }
    }
    
    for (int i = 0; i < n; i++)
    {
        yuvFrame.at<int>(4, i) = *itU;
        yuvFrame.at<int>(5, i) = *itV;
        itU++;
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

    cout << "Mean: " << mean << endl;
    cout << "M: " << m << endl;

    for (int i = 0; i < yuvFrame.rows; i++)
    {
        cout << "row: " << i << endl;
        for (int j = 0; j < yuvFrame.cols; j++)
        {
            res = gol.encode(yuvFrame.at<int>(i,j));
            vec.push_back(yuvFrame.at<int>(i,j));
            for(int bit: res){
                bs.writebit(bit);
            }
            
        }
    }
    
    
    bs.closeF();











    /*
    by: 235045
    bu: 128000
    bv: 128000
    */


    /*
    for (int i = 0; i < 255; i++)
    {
        b = r = 255;
        g = 255;

        cout << "by: " << (0.257 * r + 0.504 * g + 0.098 * b + 16) *c<< endl;
        cout << "bu: " << (-0.148 * r - 0.291 * g + 0.439 * b + 128)*c << endl;
        cout << "bv: " << (0.439 * r - 0.368 * g - 0.071 * b + 128)*c << endl;
    }
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