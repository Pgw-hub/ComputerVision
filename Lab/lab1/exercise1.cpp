#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    Mat frame;
    VideoCapture cap;

    // check if file exists. if none program ends
    if (cap.open("background.mp4") == 0) {
        cout << "no such file!" << endl;
        waitKey(0);
    }

    double fps = cap.get(CAP_PROP_FPS);
    int total_frames = cap.get(CAP_PROP_FRAME_COUNT);
    int delay = 1000/fps; 

    while(1){
        double time_in_msec = cap.get(CAP_PROP_POS_MSEC);
        int curr_frame = cap.get(CAP_PROP_POS_FRAMES);

        cap >> frame;

        if(frame.empty()){
            cout << "end of video" << endl;
            break;
        }
        imshow("video",frame);
        if(time_in_msec < 3000) waitKey(delay);
        else waitKey(0);

        cout << "frames: " << curr_frame << " / " << total_frames << endl;
    }
}

