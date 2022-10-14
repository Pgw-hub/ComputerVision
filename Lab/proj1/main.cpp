#include <iostream>
#include <opencv2/opencv.hpp> 

using namespace std;
using namespace cv;

Mat NegaTrans(Mat frame);
Mat HistEqual(Mat frame);
Mat GammaTrans(Mat frame);
Mat ColorSlicing(Mat frame);
Mat ColorConversion(Mat frame);
Mat AvgFiltering(Mat frame);
Mat WhiteBalancing(Mat frame);
void white_balancing(Mat img);

int main(){

    Mat frame, image; 
    VideoCapture cap;

    if(cap.open("Road.mp4") == 0){
        cout << "no such file!!" << endl;
        exit(-1);
    }


    double fps = cap.get(CAP_PROP_FPS);
    int delay = 1000 / fps;

    int input;
    int status = 0;
    int curr_status;
    while(1){
        cap >> frame;

        if(frame.empty()){
            cout << "end of video" << endl;
            break;
        }
        curr_status = waitKey(delay);
        if(curr_status == 'n' || curr_status =='g' || curr_status == 'h' || curr_status == 's' || curr_status == 'c' || curr_status == 'a' || curr_status == 'w' || curr_status == 'r' || curr_status == 27){
            status = curr_status;
        }


        switch(status){
            case 'n' : //ok
                frame = NegaTrans(frame);
                break;
            case 'g' : //ok
                frame = GammaTrans(frame);
                break;
            case 'h' : //ok 
                frame = HistEqual(frame);
                break;
            case 's' : //ok
                frame = ColorSlicing(frame);
                break; 
            case 'c' : //ok
                frame = ColorConversion(frame);
                break;  
            case 'a' : //ok
                frame = AvgFiltering(frame);
                break; 
            case 'w' : //ok
                frame = WhiteBalancing(frame);
                break; 
            case 'r' : //ok
                break; 
            case 27 :
                exit(-1);
                break;
                }
        
        imshow("video", frame);
    }

    return 0;
}

Mat NegaTrans(Mat frame){
    Mat HSV, negative_img;
    vector<Mat> nt(3);

    int rows = frame.rows;
    int cols = frame.cols;
    uchar* h;
    uchar* s;
    uchar* v;

    cvtColor(frame, HSV, CV_BGR2HSV);
    split(HSV,nt);

    for(int j = 0; j < rows; ++j){
        v = nt[2].ptr<uchar>(j);
        for(int i = 0; i < cols; ++i){
            v[i] = 255 - v[i];
        }
    }
    merge(nt,negative_img);
    cvtColor(negative_img, negative_img, CV_HSV2BGR);

    return negative_img;
}

Mat HistEqual(Mat frame){
    Mat HSV, intensity_change;
    vector<Mat> ic(3);

    int rows = frame.rows;
    int cols = frame.cols;
    uchar* h;
    uchar* s;
    uchar* v;

    cvtColor(frame, HSV, CV_BGR2HSV);
    split(HSV,ic);

    equalizeHist(ic[2],ic[2]);
    merge(ic,intensity_change);
    cvtColor(intensity_change, intensity_change, CV_HSV2BGR);

    return intensity_change;
}

Mat GammaTrans(Mat frame){

    Mat HSV, gamma_img;
    unsigned char pix[256];
    vector<Mat> gt(3);

    float gamma = 2.5;
    int rows = frame.rows;
    int cols = frame.cols;
    uchar* h;
    uchar* s;
    uchar* v;

    cvtColor(frame, HSV, CV_BGR2HSV);
    split(HSV,gt);
    
    for(int i = 0; i < 256; ++i){
        pix[i] = saturate_cast<uchar>(pow((float)(i / 255.0), gamma) * 255.0f);
    }

    for(int j = 0; j < rows; ++j){
        v = gt[2].ptr<uchar>(j);
        for(int i = 0; i < cols; ++i){
            v[i] = pix[v[i]];
        }
    }
    merge(gt,gamma_img);
    cvtColor(gamma_img, gamma_img, CV_HSV2BGR);

    return gamma_img;
}

//범위에 들어가지 않는 값들의 채도를 모두 0으로.
Mat ColorSlicing(Mat frame){
    Mat HSV, color_slicing;
    vector<Mat> mo(3);

    int rows = frame.rows;
    int cols = frame.cols;
    uchar* h;
    uchar* s;
    uchar* v;

    cvtColor(frame, HSV, CV_BGR2HSV);
    split(HSV,mo);
    
    for(int j = 0; j < rows; ++j){
        h = mo[0].ptr<uchar>(j);//pointer 타입이라서 이걸 바꾸어주어도, 이후에 mo의 값이 바뀌는 것임.
        s = mo[1].ptr<uchar>(j);
        for(int i = 0; i < cols; ++i){
            if(h[i] > 9 && h[i] < 23) s[i] = s[i];
            else s[i] = 0;
        }
    }

    merge(mo,color_slicing);
    cvtColor(color_slicing,color_slicing, CV_HSV2BGR);

    return color_slicing;
}

Mat ColorConversion(Mat frame){

    Mat HSV, color_conversion;
    vector<Mat> cc(3);

    int rows = frame.rows;
    int cols = frame.cols;
    uchar* h;
    uchar* s;
    uchar* v;

    cvtColor(frame, HSV, CV_BGR2HSV);
    split(HSV,cc);

    for(int j = 0; j < rows; ++j){
        h = cc[0].ptr<uchar>(j);
        s = cc[1].ptr<uchar>(j);
        for(int i = 0; i < cols; ++i){
            if(h[i] + 50 > 179) h[i] + 50 - 179;
            else h[i] += 50;
        }
    }

    merge(cc,color_conversion);
    cvtColor(color_conversion,color_conversion, CV_HSV2BGR);

    return color_conversion;
}

Mat AvgFiltering(Mat frame){

    Mat HSV, filtered_img;
    vector<Mat> af(3);

    int rows = frame.rows;
    int cols = frame.cols;
    uchar* v;

    cvtColor(frame, HSV, CV_BGR2HSV);
    split(HSV,af);

    blur(af[2],af[2],Size(9,9));
    merge(af,filtered_img);
    cvtColor(filtered_img, filtered_img, CV_HSV2BGR);

    return filtered_img;
}

Mat WhiteBalancing(Mat frame){

    white_balancing(frame);

    return frame;
}

void white_balancing(Mat img){
    Mat bgr_channels[3];
    split(img, bgr_channels);

    double avg;
    int sum, temp, i, j, c;

    for(int c = 0; c < img.channels(); c++){
        sum = 0;
        avg = 0.0f;
        for(i = 0; i < img.rows; ++i){
            for(j = 0; j < img.cols; ++j){
                sum += bgr_channels[c].at<uchar>(i,j);
            }
        }
        avg = sum / (img.rows * img.cols);
        for(int i = 0; i < img.rows; ++i){
            for(int j = 0; j < img.cols; ++j){
                temp = (128 / avg) * bgr_channels[c].at<uchar>(i,j);
                if(temp > 255) bgr_channels[c].at<uchar>(i,j) = 255;
                else bgr_channels[c].at<uchar>(i,j) = temp;
            }
        }
    }
    merge(bgr_channels,3,img);
}