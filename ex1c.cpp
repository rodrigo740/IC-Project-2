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
    /*
    for (int i = 0; i < y.rows; i++)
    {
        for (int j = 0; j < y.cols; j++)
        {
            y.at<int>(i,j) = ((yuvFrame[pos]-16)/c) + y.at<int>(i,j); // asdmk
            if(i == 0 && j == 0){
                y.at<int>(i,j) = ((yuvFrame[pos]-16)/c);
            }
            

            pos++;
        }
    }
    */

    int startU = 4*n;

    y.at<uchar>(0,0) = (yuvFrame[pos]);

    for (int i = 1; i < startU; i++)
    {
        y.at<uchar>(row,col) = (yuvFrame[i]);
        //cout << "Y val: " << (yuvFrame[i]) << endl;

        col++;
        if(col == width){
            col = 0;
            row++;
        }
    }

    cout << "width: " << width << endl;
    cout << "height: " << height << endl;
    
    cout << "y mat done" << endl;
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
    cout << "Start pos of v: " << startV << endl;
    cout << "Total number of u and v: " << n << endl;

    for (int i = 1; i < n; i++)
    {
        //cout << "Row: " << row << endl;
        //cout << "Col: " << col << endl;

        u.at<uchar>(row,col) = (yuvFrame[startU+i]);
        u.at<uchar>(row,col+1) = (yuvFrame[startU+i]);
        u.at<uchar>(row+1,col) = (yuvFrame[startU+i]);
        u.at<uchar>(row+1,col+1) = (yuvFrame[startU+i]);

        v.at<uchar>(row,col) = (yuvFrame[startV+i]);
        v.at<uchar>(row,col+1) = (yuvFrame[startV+i]);
        v.at<uchar>(row+1,col) = (yuvFrame[startV+i]);
        v.at<uchar>(row+1,col+1) = (yuvFrame[startV+i]);

        //cout << "U val: " << ((yuvFrame[startU+pos])) << endl;
        //cout << i << endl;
        col+=2;
        if(col == width){
            col = 0;
            row+=2;
            //cout << "Row: " << row << endl;
            //cout << "Col: " << col << endl;
        }
    }

    cout << "u and v mats done" << endl;
    int c,d,e;
    long val;

    for (int i = 0; i < y.rows; i++)
    {
        for (int j = 0; j < y.cols; j++)
        {
            yval = y.at<uchar>(i,j)-16;
            vval = v.at<uchar>(i,j)-128;
            uval = u.at<uchar>(i,j)-128;

            // Y
            //y.at<uchar>(i,j) = 0.299 * r + 0.587 * g + 0.114 * b; // will never be negative

            // U
            //u.at<uchar>(i,j) = (-0.147 * r - 0.289 * g + 0.436 * b) + 128; //

            // V
            //v.at<uchar>(i,j) = (0.615 * r - 0.515 * g - 0.100 * b) + 128;
            val = 1.164*yval + 1.596*vval;
            if (val > 255){
                val = 255;
            }else if(val < 0){
                val = 0;
            }
            r.at<uchar>(i,j) = (uchar) val;

           cout << "R: " << val << endl;
            
            //cout << "R: " << r.at<uchar>(i,j) << endl;
            val = 1*yval - 0.813*uval - 0.391*vval;
            if (val > 255){
                val = 255;
            }else if(val < 0){
                val = 0;
            }
            g.at<uchar>(i,j) = (uchar) val;

            cout << "G: " << val << endl;



            //cout << "G: " << g.at<uchar>(i,j) << endl;
            val = 1.164 * yval + 2.018*uval;
            if (val > 255){
                val = 255;
            } else if(val < 0){
                val = 0;
            }
                
            b.at<uchar>(i,j) = (uchar) val;

            cout << "B: " << val << endl;

            //cout << "b val: " << (uchar) val << endl;

            //cout << "B: " << b.at<uchar>(i,j) << endl;

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












Mat reconstruct(vector<int> yuvFrame, int width, int height, int n, int c){
    cout << "Frame size: " << yuvFrame.size() << endl;
    cout << "Rec image" << endl;
    Mat y = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat u = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat v = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));

    Mat b = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat g = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));
    Mat r = Mat(Size(width,height), CV_8UC1, Scalar(0, 0, 0));

    int pos = 0, col = 1, row = 0, val, yval, uval, vval;
    /*
    for (int i = 0; i < y.rows; i++)
    {
        for (int j = 0; j < y.cols; j++)
        {
            y.at<int>(i,j) = ((yuvFrame[pos]-16)/c) + y.at<int>(i,j); // asdmk
            if(i == 0 && j == 0){
                y.at<int>(i,j) = ((yuvFrame[pos]-16)/c);
            }
            

            pos++;
        }
    }
    */

    int startU = (4*n+pos)-1;

    y.at<uchar>(0,0) = (yuvFrame[pos]);

    for (int i = 1; i < startU; i++)
    {
        y.at<uchar>(row,col) = y.at<uchar>(row,col-1) + (yuvFrame[i]);
        cout << "Y val: " << (y.at<uchar>(row,col-1) + (yuvFrame[i])) << endl;

        col++;
        if(col == width){
            col = 0;
            row++;
        }
    }

    cout << "width: " << width << endl;
    cout << "height: " << height << endl;
    
    cout << "y mat done" << endl;
    pos = 0;
    row = 0;
    col = 2;


    u.at<int>(0,0) = (yuvFrame[4*n+pos])/c-128;
    u.at<int>(0,1) = (yuvFrame[4*n+pos])/c-128;
    u.at<int>(1,0) = (yuvFrame[4*n+pos])/c-128;
    u.at<int>(1,1) = (yuvFrame[4*n+pos])/c-128;

    v.at<int>(0,0) = (yuvFrame[4*n+n+pos])/c-128;
    v.at<int>(0,1) = (yuvFrame[4*n+n+pos])/c-128;
    v.at<int>(1,0) = (yuvFrame[4*n+n+pos])/c-128;
    v.at<int>(1,1) = (yuvFrame[4*n+n+pos])/c-128;

    pos++;

    cout << "Start pos of u: " << (4*n+pos)-1 << endl;
    cout << "Total number os u: " << n << endl;
    int startV = (4*n+n+pos)-1;
    cout << "Start pos of u: " << startU << endl;
    cout << "Start pos of v: " << startV << endl;
    cout << "Total number of u and v: " << n << endl;

    for (int i = 0; i < n; i++)
    {
        //cout << "Row: " << row << endl;
        //cout << "Col: " << col << endl;


        if(col == 0){
            u.at<int>(row,col) = u.at<int>(row-1,width-1) + (yuvFrame[startU+pos])/c-128;
            u.at<int>(row,col+1) = u.at<int>(row-1,width-1) + (yuvFrame[startU+pos])/c-128;
            u.at<int>(row+1,col) = u.at<int>(row-1,width-1) + (yuvFrame[startU+pos])/c-128;
            u.at<int>(row+1,col+1) = u.at<int>(row-1,width-1) + (yuvFrame[startU+pos])/c-128;

            v.at<int>(row,col) = v.at<int>(row-1,width-1) + (yuvFrame[startV+pos])/c-128;
            v.at<int>(row,col+1) = v.at<int>(row-1,width-1) + (yuvFrame[startV+pos])/c-128;
            v.at<int>(row+1,col) = v.at<int>(row-1,width-1) + (yuvFrame[startV+pos])/c-128;
            v.at<int>(row+1,col+1) = v.at<int>(row-1,width-1) + (yuvFrame[startV+pos])/c-128;
        }else{
            //cout << "U val: " << u.at<int>(row,col-1) + (yuvFrame[startU+pos])/c-128 << endl;

            u.at<int>(row,col) = u.at<int>(row,col-1) + (yuvFrame[startU+pos])/c-128;
            u.at<int>(row,col+1) = u.at<int>(row,col-1) + (yuvFrame[startU+pos])/c-128;
            u.at<int>(row+1,col) = u.at<int>(row,col-1) + (yuvFrame[startU+pos])/c-128;
            u.at<int>(row+1,col+1) = u.at<int>(row,col-1) + (yuvFrame[startU+pos])/c-128;

            v.at<int>(row,col) = v.at<int>(row,col-1) + (yuvFrame[startV+pos])/c-128;
            v.at<int>(row,col+1) = v.at<int>(row,col-1) + (yuvFrame[startV+pos])/c-128;
            v.at<int>(row+1,col) = v.at<int>(row,col-1) + (yuvFrame[startV+pos])/c-128;
            v.at<int>(row+1,col+1) = v.at<int>(row,col-1) + (yuvFrame[startV+pos])/c-128;
        }

        col+=2;
        pos++;
        if(col == width){
            col = 0;
            //cout << "Row: " << row << endl;
            //cout << "Col: " << col << endl;
        }
    }

    /*
    for (int i = 0; i < height-1; i+=2)
    {
        for (int j = 0; j < width-1; j+=2)
        {
            val = (yuvFrame[4*n+pos])/c-128; //
            u.at<int>(i,j) = val;
            u.at<int>(i,j+1) = val;
            u.at<int>(i+1,j) = val;
            u.at<int>(i+1,j+1) = val;

            val = (yuvFrame[4*n+n+pos])/c-128;
            v.at<int>(i,j) = val;
            v.at<int>(i,j+1) = val;
            v.at<int>(i+1,j) = val;
            v.at<int>(i+1,j+1) = val;

            pos++;
        }
    }
    */
    cout << "u and v mats done" << endl;


    for (int i = 0; i < u.rows; i++)
    {
        for (int j = 0; j < u.cols; j++)
        {
            //cout << "U: " << u.at<int>(i,j) << endl;
            //cout << "U: " << v.at<int>(i,j) << endl;
            
        }
        
    }
    


    for (int i = 0; i < y.rows; i++)
    {
        for (int j = 0; j < y.cols; j++)
        {
            yval = y.at<int>(i,j);
            vval = v.at<int>(i,j);
            uval = u.at<int>(i,j);

            r.at<int>(i,j) = (yval + 1.13983*vval);
            //cout << "R: " << r.at<int>(i,j) << endl;
            
            g.at<int>(i,j) = (yval - 0.39465*uval - 0.58060*vval);
            //cout << "G: " << g.at<int>(i,j) << endl;

            b.at<int>(i,j) = (yval + 2.03211*uval);
            //cout << "B: " << b.at<int>(i,j) << endl;

        }
    }
    
    vector<Mat> bgr = {b, g, r};
    Mat img;
    merge(bgr, img);

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
            cout << "orig R: " << (int)r << endl;
            cout << "orig G: " << (int)g << endl;
            cout << "orig B: " << (int)b << endl;
            cout << "####################################" << endl;


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
                cout << "row: " << row << endl;
                cout << "row: " << u2.rows << endl;

            }
        }
    }
    /*
    for (int i = 0; i < uv.size(); i+=4)
    {
        val = (uv[i] + uv[i+1] + uv[i+2] + uv[i+3])/4;
        u2v.push_back(val);

        val = (vv[i] + vv[i+1] + vv[i+2] + vv[i+3])/4;
        v2v.push_back(val);
    }
    */

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
    /*
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
    /*
    res = gol.encode(yuvF[0]);

    for (int bit: res)
    {
        bs.writebit(bit);
    }
    
    residual = yuvF[0];
    res = gol.encode(residual);

    for (int bit: res)
    {
        bs.writebit(bit);
    }*/
    /*
    residual = yuvF[0];
    res = gol.encode(residual);

    for (int bit: res)
    {
        bs.writebit(bit);
    }
    for (int i = 1; i < yuvF.size()-1; i++)
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
    */
    //vector<int> decoded = decoder(15000, 6, 65000, "encoded.bit");

    
    
    //vector<uchar> decoded = yuvF;
    Mat rc_img = reconstruct2(yuvF, width*2, height*2, n);
    //rc_img.convertTo(rc_img, 16);
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