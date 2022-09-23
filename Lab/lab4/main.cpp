#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat drawHistogram(Mat src);
Mat HistText2Img(Mat src);

int main(){
	Mat image;
	Mat hist_equalized_image;
	Mat hist_graph;
	Mat hist_equalized_graph;
	Mat TextImage;
	Mat TextHistImage;
	
	image = imread("moon.png",0);
	if(!image.data) exit(1);

	//histrogram equalization
	equalizeHist(image, hist_equalized_image);

	//draw histogram
	hist_graph = drawHistogram(image);
	hist_equalized_graph = drawHistogram(hist_equalized_image);

	//put text to image
	TextImage = HistText2Img(image);	
	TextHistImage = HistText2Img(hist_equalized_image);

	//show
	imshow("h1",hist_graph);
	imshow("h2",hist_equalized_graph);
	imshow("before",TextImage);
	imshow("after",TextHistImage);
	waitKey(0);
}

Mat drawHistogram(Mat src){
	Mat hist, histImage;
	
	int i, hist_w, hist_h, bin_w, histSize;
	float range[] = {0,256};
	const float* histRange = {range};

	hist_w = 512;
	hist_h = 512;
	histSize = 16;
	bin_w = cvRound((double)hist_w / histSize);
	histImage = Mat(hist_h, hist_w, CV_8UC3, Scalar(255,255,255));

	//hist에다가 histrogram계산결과를 저장함.
	calcHist(&src, 1, 0, Mat(), hist, 1, &histSize, &histRange);

	//normalize
	normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	
	//make real graph
	for(int i = 0; i < histSize; ++i){
		rectangle(histImage, Point(bin_w * i, hist_h),Point(bin_w * i + hist_w/histSize, hist_h - cvRound(hist.at<float>(i))), Scalar(0,0,0), -1);
	}
	return histImage;
}

//image를 넣으면 거기에 hist를 구해주고,
//해당 hist값들을 이미지에 추가시켜주는 함수
Mat HistText2Img(Mat src){
	Mat hist, histImage;
	
	int i, histSize;
	float range[] = {0,256};//그냥 배열임
	const float* histRange = {range};//이것도 배열 2차원배열느낌, 배열을 저장한 배열

	//src에서 받아온 사진, hist값 계산하기
	histSize = 8;
	calcHist(&src, 1, 0, Mat(), hist, 1, &histSize, &histRange);
	int pixel_num = src.rows * src.cols;

	//normalize and putText
	Point myPoint(10,40);
	double myFontScale = 0.8;
	for(int i = 0; i < histSize; ++i){
		//round
		hist.at<float>(i) = round((hist.at<float>(i) / pixel_num) * 1000000) / 1000000;
		//putText
		putText(src,format("bin %d : %0.6lf",i,hist.at<float>(i)),
			myPoint,FONT_HERSHEY_SIMPLEX, myFontScale, Scalar(255,0,0),2
			);
		myPoint.y += 30;
	}

	return src;
}

