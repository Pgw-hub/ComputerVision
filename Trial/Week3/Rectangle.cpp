#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(){
        Mat image = imread("../../Lab/data/lena.png");
        Rect rect = Rect(10, 10, 100, 100); // LT position, width, height
        rectangle(image, rect, Scalar(255, 0, 0), 4, 8, 0);
        imshow("image",image);
        waitKey(0);
        return 0;
    }
