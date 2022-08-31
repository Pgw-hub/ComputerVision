#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(){
	int w = 150, h = 100;

	Mat image(h,w,CV_8UC1, Scalar(100,100,20));
	cout << "Size:" << image.size().height << "," << image.size().width << endl;
	imshow("image", image);

	waitKey(0);
	return 0;
}
