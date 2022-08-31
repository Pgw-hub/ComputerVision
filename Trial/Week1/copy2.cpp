#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    Mat image = imread("lena.png");
    Mat mask = Mat::zeros(image.size(), image.type());
    Mat copied;
    Rect rect = Rect(10, 10, 100, 100); // LT position, width, height
    rectangle(mask, rect, Scalar(255,  0, 0), -1, 8, 0);
    image.copyTo(copied, mask);
    imshow("original", image);
    imshow("copied", copied);
    waitKey(0);
    return 0;
    }
