#include "bofSift.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <stdio.h>


using namespace std;
using namespace cv;

void buildSiftDictionary(void){
	//Step 1 - Obtain the set of bags of features.
	initModule_nonfree() ;
	//to store the input file names
	char * filename = new char[100];       
	//to store the current input image
	Mat input;    

	//To store the keypoints that will be extracted by SIFT
	vector<KeyPoint> keypoints;
	//To store the SIFT descriptor of current image
	Mat descriptor;
	//To store all the descriptors that are extracted from all the images.
	Mat featuresUnclustered;
	//The SIFT feature extractor and descriptor
	Ptr<FeatureDetector> detector = FeatureDetector::create("SIFT");
	Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create("SIFT");


	//Images to extract feature descriptors and build the vocabulary
	for(int f=0;f<2;f++){        
		//create the file name of an image
		sprintf(filename,"../dictionary/%i.jpg",f);
		cout << filename << endl ;

		//open the file
		input = imread(filename, CV_LOAD_IMAGE_GRAYSCALE); //Load as grayscale   
		cout << input.cols << " " << input.rows << endl;
		imshow("I",input);
		waitKey() ;
		//detect feature points
		detector->detect(input, keypoints);
		cout << keypoints.size() << endl ;

		//compute the descriptors for each keypoint
		extractor->compute(input, keypoints,descriptor); 
		//put the all feature descriptors in a single Mat object 
		featuresUnclustered.push_back(descriptor);        
		//print the percentage
		//cout << f/10 << " percent done\n" << endl ;
		printf("%i percent done\n",f/10);
	}    

	cout << "features Unclustered " << featuresUnclustered.size() << endl ;
	
	//Construct BOWKMeansTrainer
	//the number of bags
	int dictionarySize=2;
	//define Term Criteria
	TermCriteria tc(CV_TERMCRIT_ITER,100,0.001);
	//retries number
	int retries=1;
	//necessary flags
	int flags=KMEANS_PP_CENTERS;
	//Create the BoW (or BoF) trainer
	BOWKMeansTrainer bowTrainer(dictionarySize,tc,retries,flags);
	//cluster the feature vectors
	//Mat feature ;
	//featuresUnclustered.convertTo(feature,CV_32FC1);
	Mat dictionary=bowTrainer.cluster(featuresUnclustered) ;
	cout << "Dico cree" << endl ;
	//store the vocabulary
	FileStorage fs("../dictionary.yml", FileStorage::WRITE);
	fs << "vocabulary" << dictionary;
	fs.release();

	cout << " Dictionnaire OK" << endl ;
}

void getSiftDescriptor(void) {

	//Step 2 - Obtain the BoF descriptor for given image/video frame. 

    //prepare BOW descriptor extractor from the dictionary
	//Mat udictionary ;
	//dictionary.convertTo(udictionary,CV_8UC1);
    Mat dictionary; 
    FileStorage fs("../dictionary.yml", FileStorage::READ);
    fs["vocabulary"] >> dictionary;
    fs.release();    
    cout << "dictionary loaded" << endl ;

    //create a nearest neighbor matcher
	Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher) ;
		//= DescriptorMatcher::create("BruteForce");
	//The SIFT feature extractor and descriptor
	Ptr<FeatureDetector> detector2 = FeatureDetector::create("SIFT") ; //("Dense")
	Ptr<DescriptorExtractor> extractor2 = DescriptorExtractor::create("SIFT") ;  //("ORB");

	cout << "init ok" << endl ;
    //create BoF (or BoW) descriptor extractor
    BOWImgDescriptorExtractor bowDE(extractor2,matcher);
    //Set the dictionary with the vocabulary we created in the first step
    bowDE.setVocabulary(dictionary);
	cout << "Set voc ok" << endl ;
    //To store the image file name
    char * filename2 = new char[100];
    //To store the image tag name - only for save the descriptor in a file
    char * imageTag = new char[10];
 
    //open the file to write the resultant descriptor
    FileStorage fs1("../descriptor.yml", FileStorage::WRITE);    
    
    //the image file with the location
    sprintf(filename2,"../testimages/1.jpg");        
    //read the image
    Mat img=imread(filename2,CV_LOAD_IMAGE_GRAYSCALE);    
	cout << img.cols << " x " << img.rows << endl ;
	imshow("I2",img);

    //To store the keypoints that will be extracted by SIFT
    vector<KeyPoint> keypoints2;        
    //Detect SIFT keypoints (or feature points)
    detector2->detect(img,keypoints2);
    //To store the BoW (or BoF) representation of the image
    Mat bowDescriptor;        
    //extract BoW (or BoF) descriptor from given image
    bowDE.compute(img,keypoints2,bowDescriptor);
 
    //prepare the yml (some what similar to xml) file
    sprintf(imageTag,"img1");            
    //write the new BoF descriptor to the file
    fs1 << imageTag << bowDescriptor;        
             
    //release the file storage
    fs1.release();

	cout << "C'est fini" << endl ;
}