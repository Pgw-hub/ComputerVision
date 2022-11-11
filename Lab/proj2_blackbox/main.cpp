
#include <iostream>
#include <opencv2/opencv.hpp> 

using namespace std;
using namespace cv;
#define FILT_SIZE 9
#define MOR_SIZE 5

Mat bgSub(Mat background, Mat src);

int main(){

    Mat frame, background, image, result;
    VideoCapture cap;

    if(cap.open("Project2.mp4") == 0){
        cout << "no such file!!" << endl;
        exit(-1);
    }

    double fps = cap.get(CAP_PROP_FPS);
    int delay = 1000 / fps;

    Mat tmp;
    cap.read(tmp);
    background = tmp.clone();
    cvtColor(background, background, CV_BGR2GRAY);
    
    int count = 0;
    int f_num;
    int moving_detect = 0;
    Mat live;

    while(1){
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        cap >> frame;
        if(frame.empty()){
            cout << "end of video" << endl;
            break;
        }
        live = frame.clone();
        

        f_num = cap.get(CAP_PROP_POS_FRAMES);
        if(f_num % 10 == 0){
            background = frame.clone();
            medianBlur(background,background,FILT_SIZE);
            cvtColor(background, background, CV_BGR2GRAY);
        }

        //backgroundsubtraction
        Mat result = bgSub(background, frame);

        //findcontours
        findContours(result, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        vector<Rect> boundRect(contours.size());
        for(int i = 0; i < contours.size(); ++i){
            boundRect[i] = boundingRect(Mat(contours[i]));
            if(boundRect[i].area() > 2000){
                moving_detect = 3;
            }
        }

        moving_detect--;
        cout << moving_detect << endl;
 
        if(moving_detect > 0){
            putText(live,format("Alert! Moving object!"),Point(100,100),FONT_HERSHEY_SIMPLEX,2,Scalar(255,255,255),4);
        }

        imshow("Project2",live);
        waitKey(delay);
    }

    return 0;
}

//background, 현재프레임 받아와서 bgSub처리.
Mat bgSub(Mat background, Mat src){

    Mat diff;
    medianBlur(src,src,FILT_SIZE);
    cvtColor(src,src,CV_RGB2GRAY);
    absdiff(src, background, diff);

    threshold(diff, diff, 50,255, CV_THRESH_BINARY);
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(MOR_SIZE,MOR_SIZE));
    morphologyEx(diff,diff,CV_MOP_OPEN, element);

    return diff;

}