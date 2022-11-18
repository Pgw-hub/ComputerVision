#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

struct MouseParams{
    Mat img;
    Mat prof;
    vector<Point2f> in, out;
};

static void onMouse(int event, int x, int y, int, void* param);
bool transform_mode = false;

int main(){

    Mat timesq, prof, result;
    VideoCapture cap, cap2;
    MouseParams mp;

    if(cap.open("Timesquare.mp4") == 0 || cap2.open("contest.mp4") == 0){
        cout << "no such file!!" << endl;
        exit(-1);
    }

    double fps = cap.get(CAP_PROP_FPS);
    int delay = 1000 / fps;

    while(1){
        cap >> mp.img;
        cap2 >> mp.prof;

        if(mp.img.empty()){
            cout << "end of video" << endl;
            break;
        }
        //mp initialize(mp img갱신과 거기에 찍힌 점들 반영)
        if(transform_mode == false){
            for(size_t i = 0; i < mp.in.size(); i++){
                circle(mp.img, mp.in[i],3, Scalar(0,0,255), 5);
            }
        }
        if(transform_mode == true){//mp.in이 내가 찍은 좌표, mp.out이 교수님 동영상좌표. 교수님 동영상 -> 내가찍은 좌표.
            Mat homo_mat = getPerspectiveTransform(mp.out, mp.in); 
            warpPerspective(mp.prof, result, homo_mat, Size(mp.img.cols,mp.img.rows));

            //add polygon
            Point trapezoid[1][4];
            for(int i = 0; i < 4; i++){
                trapezoid[0][i] = Point(mp.in[i].x,mp.in[i].y);
            }
            const Point* ppt[1] = {trapezoid[0]};
            int npt[] = {4};
            fillPoly(mp.img,ppt,npt,1,Scalar(0,0,0),8);
            add(mp.img, result, mp.img);
        }

        imshow("background",mp.img);
        imshow("input",mp.prof);
        setMouseCallback("background", onMouse, (void*)&mp);//mouseevent를 처리할 윈도우 지정
        waitKey(delay);
    }
}

static void onMouse(int event, int x, int y, int, void* param){
    MouseParams* mp = (MouseParams*)param;
    Mat img = mp -> img;
    //점찍기.
    if(event == EVENT_LBUTTONDOWN){
        mp -> in.push_back(Point2f(x,y));
        //찍힌 점이 4군데이면, transformation하게
        if(mp -> in.size()== 4){
            int x = mp -> prof.cols;
            int y = mp -> prof.rows;
            mp -> out.push_back(Point2f(0,0));
            mp -> out.push_back(Point2f(x,0));
            mp -> out.push_back(Point2f(x,y));
            mp -> out.push_back(Point2f(0,y));
            transform_mode = true;
        }
    }

    //reset position
    if(event == EVENT_RBUTTONDOWN){
        transform_mode = false;
        mp -> in.clear();
        mp -> out.clear();
    }
}




