#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <fstream>

using namespace std;
using namespace cv;
using namespace dnn;

#define WORK_RATIO 5
#define FILT_SIZE 9
#define MOR_SIZE 5

void LineDetection(Mat& frame);
void Yolo(Mat& detectionMat, Mat& frame, vector<String>& classNamesVec);
Mat bgSub(Mat background, Mat src);
void averageDraw(vector<Vec2f> lines, Mat& frame, int dummy);

//flag
int car_detected;
int human_detected;
int me_moving;
bool front_clear;
bool front_car = false;
int moving_text = 0; 
bool st_flag = false;

//lane_departure
int line_text;
bool left_line = false;
bool mid_line = false;
bool right_line = false;

int main(){

    Mat frame, nearby_background, front_background, nearby_frame, front_frame;
    String modelConfiguration = "deep/yolov2-tiny.cfg";
    String modelBinary = "deep/yolov2-tiny.weights"; 

    Net net = readNetFromDarknet(modelConfiguration, modelBinary);

    VideoCapture cap("Project3_1.mp4");
    // VideoCapture cap(0); //detecting person

    vector<String> classNamesVec;
    ifstream classNamesFile("deep/coco.names");

    if(classNamesFile.is_open()){
        string className = "";
        while(std::getline(classNamesFile, className)) classNamesVec.push_back(className);
    }

    double fps = cap.get(CAP_PROP_FPS);
    int delay = 1000 / fps;

    int working = 0;

    while(1){
        cap >> frame;
        Mat frame_ld = frame.clone();
        int n = cap.get(CAP_PROP_POS_FRAMES);
        if(frame.empty()){
            cout << "end of video" << endl;
            break;
        }

        if(frame.channels() == 4) cvtColor(frame,frame, COLOR_BGRA2BGR); 
        //roi for backsub
        Rect nearby_rect(150,frame.rows - 40, 410, 40);
        Mat nearby_frame = frame(nearby_rect);
        Rect front_rect(330, frame.rows - 200, 10, 100);
        Mat front_frame = frame(front_rect);
        // imshow("near frame", nearby_frame);
        // imshow("front frame",front_frame);
 

        //Working
        if(working % WORK_RATIO == 0){
            //YOLO
            Mat inputBlob = blobFromImage(frame, 1 / 255.F, Size(416, 416), Scalar(), true, false);
            net.setInput(inputBlob, "data");
            Mat detectionMat = net.forward("detection_out");
            Yolo(detectionMat, frame, classNamesVec);
            //renew backgrounds
                //backgrounds roi하기.
            nearby_background = nearby_frame.clone();
            front_background = front_frame.clone();
                //blur, cvtcolor하기
            medianBlur(nearby_background, nearby_background, FILT_SIZE);
            cvtColor(nearby_background, nearby_background, CV_BGR2GRAY);
            medianBlur(front_background, front_background, FILT_SIZE);
            cvtColor(front_background, front_background, CV_BGR2GRAY);
        }
        //background sub
        Mat nearby_result = bgSub(nearby_background, nearby_frame);
        Mat front_result = bgSub(front_background, front_frame); 
        // imshow("nearby",nearby_result);
        // imshow("front",front_result);

        //detect moving or not
        for(int i = 0; i < nearby_result.cols; ++i){
            for(int j = 0; j < nearby_result.rows; ++j){
                int nearby_value = nearby_result.at<uchar>(j,i);
                if(nearby_value == 255){
                    me_moving = 20;  
                    // cout << "Me moving!!" << endl;
                }
            }
        }
        for(int i = 0; i < front_result.cols; ++i){
            for(int j = 0; j < front_result.rows; ++j){
                int front_value = front_result.at<uchar>(j,i);
                if(front_value == 255){
                    front_clear = true;  
                    // cout << "front moving!!" << endl;
                }
            }
        }

        if(front_clear == true && me_moving < 0 && n < 268) {
            moving_text = 60;
        }
        if(left_line == false && mid_line == true && right_line == false){
            line_text = 10;
        }

        //line detection
        LineDetection(frame);

        //post labeling
        if(car_detected > 0){
            putText(frame,"Car Detected nearby!",Point(90,150), FONT_HERSHEY_DUPLEX, 1.5, Scalar(0,0,255),2);
        }
        if(human_detected > 0){
            putText(frame,"Human Detected nearby!",Point(65,300), FONT_HERSHEY_DUPLEX, 1.5, Scalar(0,255,0),2);
        }
        if(moving_text > 0){
            putText(frame,"Start Moving!",Point(180,200), FONT_HERSHEY_DUPLEX, 1.5, Scalar(255,0,0),2);
        }
        if(line_text > 0){
            putText(frame,"Lane departure!",Point(150,250), FONT_HERSHEY_DUPLEX, 1.5, Scalar(0,0,255),2);
        }

        // cout << "[front_clear] : " << front_clear << " [me_moving] : " << me_moving << " [front_car] : " << front_car << endl;
        // cout << "[Car_detected] : " << car_detected << " [moving_text] : " << moving_text << endl;
        //adjusting flags
        car_detected--;
        moving_text--;
        working++;
        front_clear = false;
        me_moving--;
        line_text--;
        human_detected--;

        imshow("Project3",frame);
        if(waitKey(delay) >= 0) break;
        }

        return 0;
    }


Mat bgSub(Mat background, Mat src){

    Mat diff;
    // medianBlur(src,src,FILT_SIZE);
    cvtColor(src,src,CV_RGB2GRAY);
    absdiff(src, background, diff);

    threshold(diff, diff, 50,255, CV_THRESH_BINARY);
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(MOR_SIZE,MOR_SIZE));
    morphologyEx(diff,diff,CV_MOP_OPEN, element);

    return diff;

}

void Yolo(Mat& detectionMat, Mat& frame, vector<String>& classNamesVec){
        float confidenceThreshold = 0.24;
        front_car = false;

        for(int i = 0; i < detectionMat.rows; ++i){
            const int probability_index = 5;
            const int probability_size = detectionMat.cols - probability_index;
            float *prob_array_ptr = &detectionMat.at<float>(i,probability_index);
            size_t objectClass = max_element(prob_array_ptr,prob_array_ptr + probability_size) - prob_array_ptr;
            //prediction probability of each class
            float confidence = detectionMat.at<float>(i,(int)objectClass + probability_index);
            //for drawing labels with name and confidence
            if(confidence > confidenceThreshold){
                    float x_center = detectionMat.at<float>(i,0) * frame.cols;
                    float y_center = detectionMat.at<float>(i,1) * frame.rows;
                    float width = detectionMat.at<float>(i,2) * frame.cols;
                    float height = detectionMat.at<float>(i,3) * frame.rows;

                Point p1(cvRound(x_center - width / 2), cvRound(y_center - height / 2));
                Point p2(cvRound(x_center + width / 2), cvRound(y_center + height / 2));
                Rect object(p1,p2);
                // cout << "object size = " << object.area() << endl;
                Scalar object_roi_color(0,255,0);

                String className = objectClass < classNamesVec.size() ? classNamesVec[objectClass] : cv::format("unknown(%d)",objectClass);
                String label = format("%s: %.2f",className.c_str(), confidence);
                int baseLine = 0;
                // cout << className << endl;

                //flag handling
                if(className == "car"){
                    rectangle(frame, object, Scalar(0,0,255));
                    if(object.area() > 30000){
                        car_detected = 10;
                    }
                }
                else if(className == "person"){
                    rectangle(frame, object, Scalar(0,255,0));
                    if(object.area() > 5000){
                        human_detected = 10;
                    }
                }
                else{
                    rectangle(frame, object, object_roi_color);
                }
            }
        }
}

void LineDetection(Mat& frame){

   Mat ROI_left, ROI_right, ROI_mid, result;
   vector<Vec2f> lines_left, lines_mid, lines_right;
   int width = frame.cols;
   int height = frame.rows;

   Rect rect_left(50,height - 200, 250, 200);
   Rect rect_mid(300,height - 230, 100, 230);
   Rect rect_right(400, height - 200, 250, 200);

   ROI_left = frame(rect_left);
   ROI_mid = frame(rect_mid);
   ROI_right = frame(rect_right);
   
   cvtColor(ROI_left, ROI_left, CV_BGR2GRAY);
   cvtColor(ROI_mid, ROI_mid, CV_BGR2GRAY);
   cvtColor(ROI_right, ROI_right, CV_BGR2GRAY);

   blur(ROI_left,ROI_left,Size(5,5));
   blur(ROI_mid,ROI_mid,Size(5,5));
   blur(ROI_right,ROI_right,Size(5,5));

   Canny(ROI_left,ROI_left, 10,60);
   Canny(ROI_mid,ROI_mid, 10,60);
   Canny(ROI_right,ROI_right, 10,60);

   HoughLines(ROI_left,lines_left, 3, CV_PI / 180, 150, 0, 0, (CV_PI/180) * 30, (CV_PI/180) * 50);
   HoughLines(ROI_mid,lines_mid, 3, CV_PI / 180, 150, 0, 0, (CV_PI/180) * -10, (CV_PI/180) * 10);
   HoughLines(ROI_right,lines_right, 3, CV_PI / 180, 150, 0, 0, (CV_PI/180) * 120, (CV_PI/180) * 130);

   averageDraw(lines_left, frame, 0);
   averageDraw(lines_mid, frame, 1);
   averageDraw(lines_right, frame, 2);

//    imshow("left",ROI_left);
//    moveWindow("left",0,400);
//    imshow("mid",ROI_mid);
//    moveWindow("mid",250,400);
//    imshow("right",ROI_right);
//    moveWindow("right",500,400);
}

void averageDraw(vector<Vec2f> lines, Mat& frame, int dummy){
	float rho = 0;
    float theta = 0;
    float a = 0;
    float b = 0; 
    float x0 = 0;
    float y0 = 0;
	Point p1, p2;
    int width = frame.cols;
    int height = frame.rows;

	//get average value
	for(int i = 0; i < lines.size(); ++i){
		rho += lines[i][0];
		theta += lines[i][1];
	}
	rho = rho / lines.size();
	theta = theta / lines.size();
    if(dummy == 0){// left
        if(lines.size() != 0) left_line = true;
        else left_line = false;

        // cout << "[left] rho : " << rho << "theta : " << theta << "line size : " << lines.size() << endl;
    }
    else if(dummy == 1){//mid
        if(lines.size() != 0) mid_line = true;
        else mid_line = false;
        
        // cout << "[mid] rho : " << rho << "theta : " << theta << "line size : " << lines.size() << endl;
    }
    else if(dummy == 2){//right
        if(lines.size() != 0) right_line = true;
        else right_line = false;

        // cout << "[right] rho : " << rho << "theta : " << theta << "line size : " << lines.size() << endl;
    }

	
	//get coordinate
	a = cos(theta);
	b = sin(theta);
	if(dummy == 0){
		x0 = a * rho + 50;
		y0 = b * rho + (height - 200);
	}
    else if(dummy == 1){
		x0 = a * rho + 300;
		y0 = b * rho + (height - 230);
    }
	else if(dummy == 2){
		x0 = a * rho + 400;
		y0 = b * rho + (height - 200);	
	}

	//draw
	p1 = Point(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * a));
	p2 = Point(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * a));
	// line(frame,p1,p2,Scalar(0,0,255), 3, 8);
}


