#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <fstream>

using namespace std;
using namespace cv;
using namespace dnn;

#define WORK_RATIO 5

void Yolo(Mat& detectionMat, Mat& frame, vector<String>& classNamesVec);

//flag
int car_detected;
int human_detected;
bool detect_red = false;
int front_clear;

int main(){

    Mat frame;
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
        if(frame.empty()){
            cout << "end of video" << endl;
            break;
        }

        if(frame.channels() == 4) cvtColor(frame,frame, COLOR_BGRA2BGR); 

        //Working(Yolo)
        if(working % WORK_RATIO == 0){
            //Convert Mat to batch of images
            Mat inputBlob = blobFromImage(frame, 1 / 255.F, Size(416, 416), Scalar(), true, false);
            net.setInput(inputBlob, "data");
            Mat detectionMat = net.forward("detection_out");
            Yolo(detectionMat, frame, classNamesVec);
        }

        //make nearby area

        //post labeling
        if(car_detected > 0){
            putText(frame,"Car Detected nearby!",Point(50,50), FONT_HERSHEY_TRIPLEX, 1, Scalar(0,0,255));
        }
        if(front_clear > 0){
            putText(frame,"Start Moving!",Point(50,100), FONT_HERSHEY_TRIPLEX, 1, Scalar(255,0,0));
        }

        cout << "[Car_detected] : " << car_detected << " [front_clear] : " << front_clear << endl;
        car_detected--;
        working++;
        front_clear--;

        imshow("YOLO: Detections",frame);
        if(waitKey(delay) >= 0) break;
        }
        return 0;
    }








void Yolo(Mat& detectionMat, Mat& frame, vector<String>& classNamesVec){
        float confidenceThreshold = 0.24;
        detect_red = false;

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


                Rect nearby_area(250, frame.rows - 100 ,200,100);
                Mat nearby = frame(nearby_area);
                for(int i = 0; i < nearby.cols; ++i){
                    for(int j = 0; j < nearby.rows; ++j){
                        int value_R = nearby.at<Vec3b>(i,j)[2];
                        int value_G = nearby.at<Vec3b>(i,j)[1];
                        int value_B = nearby.at<Vec3b>(i,j)[0];
                        // cout << " [value R] : " << value_R << endl;
                        if(value_R == 255 && value_G == 0 && value_B == 0){
                            detect_red = true;  
                            cout << "Detect RED!!" << endl;
                        }
                    }
                } 

                if(detect_red == true){
                    front_clear = 0;
                }
                else{
                    front_clear = 10;
                }
                imshow("roi",nearby);
                // Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
                // rectangle(frame, Rect(p1,Size(labelSize.width, labelSize.height + baseLine)), object_roi_color, FILLED);
                // putText(frame, label, p1 + Point(0,labelSize.height), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,0));
            }
        }
}