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

void Yolo(Mat& detectionMat, Mat& frame, vector<String>& classNamesVec);
Mat bgSub(Mat background, Mat src);

//flag
int car_detected;
int human_detected;

int me_moving;
bool front_clear;
bool front_car = false;
int moving_text = 0; 
bool st_flag = false;

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
        int curr_frame = cap.get(CAP_PROP_POS_FRAMES);
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
        imshow("near frame", nearby_frame);
        imshow("front frame",front_frame);
 

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
        imshow("nearby",nearby_result);
        imshow("front",front_result);

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

        if(front_clear == true && me_moving < 0) {
            moving_text = 40;
            cout << "Now! ";
        }


        //post labeling
        if(car_detected > 0){
            putText(frame,"Car Detected nearby!",Point(50,50), FONT_HERSHEY_TRIPLEX, 1, Scalar(0,0,255));
        }
        if(moving_text > 0 && curr_frame < 350){
            putText(frame,"Start Moving!",Point(50,100), FONT_HERSHEY_TRIPLEX, 1, Scalar(255,0,0));
        }

        cout << "[front_clear] : " << front_clear << " [me_moving] : " << me_moving << " [front_car] : " << front_car << endl;
        // cout << "[Car_detected] : " << car_detected << " [moving_text] : " << moving_text << endl;
        //adjusting flags
        car_detected--;
        moving_text--;
        working++;
        front_clear = false;
        me_moving--;

        imshow("YOLO: Detections",frame);
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

                //flag handling
                if(className == "car"){
                    rectangle(frame, object, Scalar(0,0,255));
                    if(object.area() > 30000){
                        car_detected = 5;
                    }
                }
                else if(className == "human"){
                    human_detected = 5;
                    rectangle(frame, object, Scalar(255,0,0));
                }
                else{
                    rectangle(frame, object, object_roi_color);
                }
            }
        }
}