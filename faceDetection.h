#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

CascadeClassifier getFaceCascadeClassifier();
CascadeClassifier getEyesCascadeClassifier(void);
CascadeClassifier getMouthCascadeClassifier(void);
CascadeClassifier getNoseCascadeClassifier(void);
vector<Rect> detectFaces(CascadeClassifier face_classifier, Mat frame);
vector<Rect> detectEye(CascadeClassifier eye_classifier, Mat frame ) ;
vector<Rect> detectMouth(CascadeClassifier mouth_classifier, Mat frame ) ;
vector<Rect> detectNose(CascadeClassifier nose_classifier, Mat frame ) ;
int detectFacesWebcam();
void showFaces(string file);
void showAllFaces(void);
void showEyes(string filename);
void showAllEyes(void) ;
void showLeftRightEyes(string filename) ;
void showEyes(string filename);
