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

    int startU = height*width;
    int startV = startU + startU/4;

    BitStream bs1(filename, 'r');
    

    vector<uchar> yuvFrame;
    vector<int> temp,resto;
    //v = bs1.readFile();

    string tmp = "";
    Golomb g(m);
    int bit = bs1.readbit();
    while(bit != -1){
        //cout << bit << endl;

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
                /*cout << "U: " << u << endl;
                cout << "resto: ";

                    for(int a: resto){
                        cout << a;
                    }
                    cout << endl;*/
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
                    f=true;
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

Mat reconstruct2(vector<uchar> yuvFrame, int width, int height, int n){
    cout << "Frame size: " << yuvFrame.size() << endl;
    cout << "Rec image" << endl;
    Mat y = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat u = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat v = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));

    Mat b = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat g = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat r = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));

    int pos = 0, col = 1, row = 0, yval, uval, vval;
    

    int startU = 4*n;

    y.at<uchar>(0,0) = (yuvFrame[pos]);

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












Mat reconstruct(vector<uchar> yuvFrame, int width, int height, int n){
    cout << "Frame size: " << yuvFrame.size() << endl;
    cout << "Rec image" << endl;
    Mat y = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat u = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat v = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));

    Mat b = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat g = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat r = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));

    int pos = 0, col = 1, row = 0, val, yval, uval, vval;
    

    int startU = 4*n;

    y.at<uchar>(0,0) = (yuvFrame[pos]);

    for (int i = 1; i < startU; i++)
    {
        y.at<uchar>(row,col) = y.at<uchar>(row,col-1) + (yuvFrame[i]);
        //cout << "Y val: " << (y.at<uchar>(row,col-1) + (yuvFrame[i])) << endl;

        col++;
        if(col == width){
            col = 0;
            row++;
        }
    }

    pos = 0;
    row = 0;
    col = 2;

    int startV = (4*n+n);



    u.at<uchar>(0,0) = (yuvFrame[startU]);
    u.at<uchar>(0,1) = (yuvFrame[startU]);
    u.at<uchar>(1,0) = (yuvFrame[startU]);
    u.at<uchar>(1,1) = (yuvFrame[startU]);

    v.at<uchar>(0,0) = (yuvFrame[startV]);
    v.at<uchar>(0,1) = (yuvFrame[startV]);
    v.at<uchar>(1,0) = (yuvFrame[startV]);
    v.at<uchar>(1,1) = (yuvFrame[startV]);


    cout << "Start pos of u: " << startU << endl;
    cout << "Total number os u: " << n << endl;
    cout << "Start pos of u: " << startU << endl;
    cout << "Start pos of v: " << startV << endl;
    cout << "Total number of u and v: " << n << endl;

    for (int i = 1; i < n; i++)
    {
        if(col == 0){
            u.at<uchar>(row,col) = u.at<uchar>(row-1,width-1) + (yuvFrame[startU+i]);
            u.at<uchar>(row,col+1) = u.at<uchar>(row-1,width-1) + (yuvFrame[startU+i]);
            u.at<uchar>(row+1,col) = u.at<uchar>(row-1,width-1) + (yuvFrame[startU+i]);
            u.at<uchar>(row+1,col+1) = u.at<uchar>(row-1,width-1) + (yuvFrame[startU+i]);

            v.at<uchar>(row,col) = v.at<uchar>(row-1,width-1) + (yuvFrame[startV+i]);
            v.at<uchar>(row,col+1) = v.at<uchar>(row-1,width-1) + (yuvFrame[startV+i]);
            v.at<uchar>(row+1,col) = v.at<uchar>(row-1,width-1) + (yuvFrame[startV+i]);
            v.at<uchar>(row+1,col+1) = v.at<uchar>(row-1,width-1) + (yuvFrame[startV+i]);
        }else{

            u.at<uchar>(row,col) = u.at<uchar>(row,col-1) + (yuvFrame[startU+i]);
            u.at<uchar>(row,col+1) = u.at<uchar>(row,col-1) + (yuvFrame[startU+i]);
            u.at<uchar>(row+1,col) = u.at<uchar>(row,col-1) + (yuvFrame[startU+i]);
            u.at<uchar>(row+1,col+1) = u.at<uchar>(row,col-1) + (yuvFrame[startU+i]);

            v.at<uchar>(row,col) = v.at<uchar>(row,col-1) + (yuvFrame[startV+i]);
            v.at<uchar>(row,col+1) = v.at<uchar>(row,col-1) + (yuvFrame[startV+i]);
            v.at<uchar>(row+1,col) = v.at<uchar>(row,col-1) + (yuvFrame[startV+i]);
            v.at<uchar>(row+1,col+1) = v.at<uchar>(row,col-1) + (yuvFrame[startV+i]);
        }

        col+=2;
        if(col == width){
            col = 0;
            row+=2;
        }
    }

    cout << "u and v mats done" << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
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

            //cout << "y col: " << j<< endl;
            //cout << "y row: " << i << endl;

        }
    }
    cout << "All done!" << endl;
    vector<Mat> bgr = {b, g, r};
    Mat img;
    merge(bgr, img);
    cout << "merged" << endl;
    namedWindow("y rec", WINDOW_NORMAL);
    imshow("y rec", y);
    waitKey(0);
    namedWindow("u rec", WINDOW_NORMAL);
    imshow("u rec", u);
    waitKey(0);
    namedWindow("v rec", WINDOW_NORMAL);
    imshow("v rec", v);
    waitKey(0);
    

    return img;
}

int main(int argc, char **argv){

    auto start = high_resolution_clock::now();
    
    /*
    if(argc != 3){
        cerr << "Usage: ./ex1c " << endl;
        return -1;
    }*/
    
    Mat image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    cout << "img type: " << image.type() << endl;
    Mat y = Mat::zeros(image.size(), CV_8UC1);
    Mat u = Mat::zeros(image.size(), CV_8UC1);
    Mat v = Mat::zeros(image.size(), CV_8UC1);

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
    int val;

    vector<uchar> uv, vv;

    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            b = bgr_planes[0].at<uchar>(i,j);
            g = bgr_planes[1].at<uchar>(i,j);
            r = bgr_planes[2].at<uchar>(i,j);


            // Y
            val = 0.299 * r + 0.587 * g + 0.114 * b + 16;
            if(val < 16) val = 16;
            if(val > 235) val = 235;
            y.at<uchar>(i,j) = val;

            // U
            val = (-0.147 * r - 0.289 * g + 0.436 * b) + 128;
            
            if(val < 16) val = 16;
            if(val > 240) val = 240;
            uv.push_back(val);
            u.at<uchar>(i,j) = val;
            // V
            val = (0.615 * r - 0.515 * g - 0.100 * b) + 128;
            
            if(val < 16) val = 16;
            if(val > 240) val = 240;
            vv.push_back(val);
            v.at<uchar>(i,j) = val;
        }
    }

    Mat u2 = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat v2 = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));

    int row=0, col=0;

    vector<uchar> u2v,v2v;
    
    
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

    namedWindow("u2", WINDOW_NORMAL);
    imshow("u2", u2);
    waitKey(0);


    int x = (height*2)*(width*2);
    int n = x/4;

    
    
    Mat yuvFrame = Mat::zeros(6, n, CV_8UC1);

    MatIterator_<uchar> itYUV, itY = y.begin<uchar>(), itU = u2.begin<uchar>(), itV = v2.begin<uchar>();

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

    int s = yuvF.size();
    cout << "Amount of y: " << s << endl;
    
    for (int i = 0; i < n; i++)
    {
        yuvFrame.at<uchar>(4, i) = u2v[i];
        yuvF.push_back(u2v[i]);
        
    }

    int s2 = yuvF.size() - s;
    cout << "Amount of u: " << u2v.size() << endl;


    for (int i = 0; i < n; i++)
    {
        yuvFrame.at<uchar>(5, i) = v2v[i];
        yuvF.push_back(v2v[i]);
        
    }

    int s3 = yuvF.size() - (s2+s);
    cout << "Amount of v: " << s3 << endl;
    
    double mean = 0;

    for(int val: yuvF){
        mean += val;
    }
    
    mean=mean/x;
    double alpha = mean/(mean+1.0);
    int m = (int) ceil(-1/log2(alpha));
    cout << "M ideal: " << m << endl;
    Golomb gol(m);
    BitStream bs("encoded.bit", 'w');

    vector<int> vec, res;

    int residual;

    row = 0; 
    col = 0;
    
    res = gol.encode(yuvF[0]);

    for (int bit: res)
    {
        bs.writebit(bit);
    }
    /*
    residual = yuvF[0];
    res = gol.encode(residual);

    for (int bit: res)
    {
        bs.writebit(bit);
    }
    
    residual = yuvF[0];
    res = gol.encode(residual);

    for (int bit: res)
    {
        bs.writebit(bit);
    }
    */
    for (int i = 0; i < yuvF.size(); i++)
    {
        residual = yuvF[i+1] - yuvF[i];
        res = gol.encode(residual);

        for (int bit: res)
        {
            bs.writebit(bit);
        }
    }
    

    bs.closeF();
    

    vector<uchar> decoded = decoder(m, 6, n, "encoded.bit");
    
    //vector<int> decoded = decoder(15000, 6, 65000, "encoded.bit");

    
    
    //vector<uchar> decoded = yuvF;
    //Mat rc_img = reconstruct2(yuvF, width*2, height*2, n);
    Mat rc_img = reconstruct(decoded, width*2, height*2, n);
    //rc_img.convertTo(rc_img, 16);
    cout << "done!" << endl;
    namedWindow("Rec image", WINDOW_NORMAL);
    imshow("Rec image", rc_img);
    waitKey(0);

    namedWindow("y orig", WINDOW_NORMAL);
    imshow("y orig", y);
    waitKey(0);
    
    namedWindow("u orig", WINDOW_NORMAL);
    imshow("u orig", u);
    waitKey(0);


    namedWindow("v orig", WINDOW_NORMAL);
    imshow("v orig", v);
    waitKey(0);











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