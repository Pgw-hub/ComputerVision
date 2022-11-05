#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<Point> FaceDetect(Mat frame, int min, int max);
Mat GrabCut(Mat frame, vector<Point> point);

int status = 0;
int curr_status;
bool tracking_mode = false;
bool detecting_mode = false;

int main(){
	Mat frame;
	VideoCapture cap;
    vector<Point> point;

	if(cap.open("Faces.mp4") == 0){
		cout << "no such file!!" << endl;
		waitKey(0); 
	}

	double fps = cap.get(CAP_PROP_FPS);
	int delay = 1000/fps;

	int min = 0;
	int max = 0;

	while(1){
		cap >> frame;
		if(frame.empty()){
			cout << "end of video" << endl;
			break;
		}

		curr_status = waitKey(delay);
		//status
		if(curr_status == 'f' || curr_status == 'n' || curr_status == 'm'){
			detecting_mode = true;
			status = curr_status;
		}
		else if(curr_status == 't'){
			if(detecting_mode == false){
				putText(frame,format("Detect before Tracking"),Point(30,30),FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,255),4);
			}
			else{
				tracking_mode = true;
			}
		}
		else if(curr_status == 'r'){
			status = 0;
			tracking_mode = false;
			detecting_mode = false;
			destroyWindow("Tracking");
		}

		if(status != 0){
			switch(status){
				case 'f' : 
					cout << 'f' << tracking_mode << endl;
					min = 30;
					max = 35;
					break;
				case 'm' : 
					cout << 'm' << tracking_mode << endl;
					min = 50;
					max = 55;
					break;
				case 'n' : 
					cout << 'n' << tracking_mode << endl;
					min = 80;
					max = 85;
					break;
			}        
        	//frame이랑 min, max만 위에서 얻고, point를 계산 받아서 오기.
            point = FaceDetect(frame, min, max);
            if(tracking_mode == true){
                Mat foreground;
                foreground = GrabCut(frame,point);
                imshow("Tracking",foreground);
            }
            rectangle(frame, point[0], point[1], Scalar(0,255,0), 3, 4, 0);
        }

        imshow("Faces", frame);
	}	

	return 0;
}

vector<Point> FaceDetect(Mat frame,int min, int max){
	CascadeClassifier face_classifier;
	vector<Rect> faces;
    vector<Point> tmp;
	Mat grayframe;

	face_classifier.load("haarcascade_frontalface_alt.xml");
	cvtColor(frame,grayframe,COLOR_BGR2GRAY);
	face_classifier.detectMultiScale(
		grayframe,
		faces,
		1.1,
		3,
		0,
		Size(min,min),
		Size(max,max)
	);
	Point lb(faces[0].x + faces[0].width, faces[0].y + faces[0].height);
	Point tr(faces[0].x, faces[0].y);
    tmp.push_back(lb);
    tmp.push_back(tr);
    
	return tmp;
}

Mat GrabCut(Mat frame, vector<Point> point){

    Mat result, bgdModel, fgdModel, foreground;
    Rect rect(point[0],point[1]);
    grabCut(frame, result, rect, bgdModel, fgdModel, 10, GC_INIT_WITH_RECT);
    compare(result, GC_PR_FGD, result, CMP_EQ);
    foreground = Mat(frame.size(), CV_8UC3, Scalar(255,0,0));
    frame.copyTo(foreground,result);

    return foreground;
}
