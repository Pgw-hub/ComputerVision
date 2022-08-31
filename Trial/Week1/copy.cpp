#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {

    Mat image = imread("lena.png", 0); 
    Mat copied_img = image.clone();
    
    imshow("this",copied_img);
    imshow("origin",image);
    waitKey(0);
}
