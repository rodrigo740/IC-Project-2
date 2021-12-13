#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <chrono>

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
    Mat gray = Mat::zeros(image.size(), CV_8UC1);
    Mat u = Mat::zeros(image.size(), CV_8UC1);
    Mat v = Mat::zeros(image.size(), CV_8UC1);
    

    vector<Mat> bgr_planes;
    split(image, bgr_planes);

    bgr_planes[0].convertTo(bgr_planes[0], CV_8UC1);
    bgr_planes[1].convertTo(bgr_planes[1], CV_8UC1);
    bgr_planes[2].convertTo(bgr_planes[2], CV_8UC1);
    

    uchar b,g,r;


    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            b = bgr_planes[0].at<uchar>(i,j);
            g = bgr_planes[1].at<uchar>(i,j);
            r = bgr_planes[2].at<uchar>(i,j);

            // Grayscale
            gray.at<uchar>(i,j) = 0.299 * r + 0.587 * g + 0.114 * b;

            // U
            u.at<uchar>(i,j) = -0.147 * r - 0.289 * g + 0.436 * b;

            // V
            v.at<uchar>(i,j) = 0.615 * r - 0.515 * g - 0.100 * b;
        }
    }

    

    namedWindow("Grayscale before", WINDOW_AUTOSIZE);
    imshow("Grayscale before", gray);
    waitKey(0);

    namedWindow("u", WINDOW_AUTOSIZE);
    imshow("u", u);
    waitKey(0);

    namedWindow("v", WINDOW_AUTOSIZE);
    imshow("v", v);
    waitKey(0);







    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Processing Time: " << duration.count() << " μs" << endl;
    
    return 0;
}