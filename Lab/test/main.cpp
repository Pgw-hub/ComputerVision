#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat averageDraw(vector<Vec2f> lines, Mat frame, int dummy);

int main(){
	Mat frame;
	VideoCapture cap;

	if(cap.open("Road.mp4") == 0){
		cout << "no such file!!" << endl;
		waitKey(0);
	}

	double fps = cap.get(CAP_PROP_FPS);
	int delay = 1000/fps;

	Mat ROI_1, ROI_2, result;
	Rect rect_1(200,400,400,200);
	Rect rect_2(600,400,400,200);
	vector<Vec2f> lines, lines_2;

	while(1){

        double time_in_msec = cap.get(CAP_PROP_POS_MSEC);
		cap >> frame;
		if(frame.empty()){
			cout << "end of video" << endl;
			break;
		}

		//roi setting
		ROI_1 = frame(rect_1);
		cvtColor(ROI_1, ROI_1, CV_BGR2GRAY);
		ROI_2 = frame(rect_2);
		cvtColor(ROI_2, ROI_2, CV_BGR2GRAY);
		//blur
		blur(ROI_1, ROI_1, Size(5,5));
		blur(ROI_2, ROI_2, Size(5,5));	
		//Canny
		Canny(ROI_1,ROI_1,10,60);
		Canny(ROI_2,ROI_2,10,60);	
		//Hough lines
		HoughLines(ROI_1,lines, 3,CV_PI / 180, 150, 0, 0, (CV_PI/180) * 30,(CV_PI/180) * 60);
		HoughLines(ROI_2,lines_2,3,CV_PI / 180, 150, 0, 0, (CV_PI/180) * 120,(CV_PI/180) * 150);
		//draw lines using lines array
		frame = averageDraw(lines,frame,0);
		frame = averageDraw(lines_2,frame,1);
		//move window
		imshow("Left canny",ROI_1);
		moveWindow("Left canny",200,0);
		imshow("Right canny",ROI_2);
		moveWindow("Right canny",600,0);	
		imshow("Frame",frame);

        if(time_in_msec < 20000) waitKey(delay);
        else break;
	}

	return 0;
}

Mat averageDraw(vector<Vec2f> lines, Mat frame, int dummy){
	float rho, theta, a, b, x0, y0;
	Point p1, p2;

	//get average value
	for(int i = 0; i < lines.size(); ++i){
		rho += lines[i][0];
		theta += lines[i][1];
	}
	rho = rho / lines.size();
	theta = theta / lines.size();
	
	//get coordinate
	a = cos(theta);
	b = sin(theta);
	if(dummy == 0){
		x0 = a * rho + 200;
		y0 = b * rho + 400;
	}
	else {
		x0 = a * rho + 600;
		y0 = b * rho + 400;	
	}

	//draw
	p1 = Point(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * a));
	p2 = Point(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * a));
	line(frame,p1,p2,Scalar(0,0,255), 3, 8);

	return frame;
}


