#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(){

    Mat finger_img = imread("finger_print.png",0);
    Mat adaptive_img = imread("adaptive.png",0);
    Mat adaptive_1_img = imread("adaptive_1.jpg",0);

    Mat finger_threshed_img;
    Mat adaptive_threshed_img;
    Mat adaptive_1_threshed_img;

    threshold(finger_img,finger_threshed_img,150,255,THRESH_BINARY);
    adaptiveThreshold(adaptive_img,adaptive_threshed_img,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,3,4);
    adaptiveThreshold(adaptive_1_img,adaptive_1_threshed_img,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,101,15);

    imshow("finger_print",finger_threshed_img);
    imshow("adaptive",adaptive_threshed_img);
    imshow("adaptive_1",adaptive_1_threshed_img);
    
    waitKey(0);

    return 0;
}