#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(){
    Mat query, image, descriptor1, descriptor2, bestquery;
    Ptr<ORB> orbF = ORB::create(1000);
    vector<KeyPoint> keypoints1, keypoints2;
    vector<KeyPoint> best_keypoints1, best_keypoints2;
    vector<vector < DMatch > > matches;
    vector<DMatch> goodMatches;
    vector<DMatch> bestMatches;
    BFMatcher matcher(NORM_HAMMING);
    Mat imgMatches;


    //compared target list(현재 10번부터 받긴 하지만 다 받긴함.)
    string compare_path = "assignment7/DBs/Handong*_1.jpg";
    vector<String> str;
    glob(compare_path, str, false);


    //input file()
    string filename;
    cout << "Type in file name : " << endl;
    cin >> filename;
    string input_path = "assignment7/query_image/" + filename;

    for(int j = 0; j < str.size(); ++j){
        int i, k;
        float nndr;
        query = imread(str[j]);
        image = imread(input_path);
        if(query.empty() || image.empty()){
            cout << "No file!" << endl;
            return -1;
        }

        resize(query, query, Size(640,480));
        resize(image, image, Size(640,480));
        

        //computing ORB features
        orbF->detectAndCompute(query, noArray(), keypoints1, descriptor1);
        orbF->detectAndCompute(image,noArray(),keypoints2, descriptor2);

        //실제 매칭이 됨.
        k = 2;
        matcher.knnMatch(descriptor1,descriptor2, matches,k);//여기에서 matching이 되고 이 안에 매치되는 인원들이 들어간다.

        nndr = 0.6f;
        for(int i = 0; i < matches.size(); ++i){
            if(matches.at(i).size() == 2 && matches.at(i).at(0).distance <= nndr * matches.at(i).at(1).distance){
                goodMatches.push_back(matches[i][0]);
            } 
        }
        cout << str[j] << " : " << filename << " Matching = " << goodMatches.size() << endl; 

        if(goodMatches.size() > bestMatches.size()){
            bestMatches = goodMatches;
            bestquery = query;
            best_keypoints1 = keypoints1;
            best_keypoints2 = keypoints2;
            cout << "              bestquery is " << str[j] << endl;
        }

        goodMatches.clear();
        matches.clear();
    }

    drawMatches(bestquery, best_keypoints1, image, best_keypoints2, bestMatches,imgMatches,Scalar::all(-1), Scalar(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    if(bestMatches.size() < 4){
        cout << "Matchingfailed" << endl;
        return 0;
    }

    imshow("Query", image);
    imshow("Best_matching", imgMatches);
    waitKey(0);
    return 0;
}