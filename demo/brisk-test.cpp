/*
 * File: brisk-test.cpp
 * Date: November 2017
 * Author: Andrew Kramer
 * Description: tests function of FBrisk
 */

#include <iostream>
#include <vector>

#include "DBoW2.h"
#include "FBrisk.h"

#include <DUtils/DUtils.h>
#include <DVision/DVision.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

using namespace DBoW2;
using namespace DUtils;
using namespace std;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void testMean(vector<cv::Mat> &features);
void testDistance(vector<cv::Mat> &features, int f1, int f2);
void makeFeatures(vector<cv::Mat> &features, int numFeatures);
void testPrint(vector<cv::Mat> &features, int v1);
void loadFeatures(vector<cv::Mat> &features);
void changeStructure(const cv::Mat &plain, vector<cv::Mat> &out);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main()
{
	vector<cv::Mat> features;
	makeFeatures(features, 2);
	testDistance(features, 0, 1);
	makeFeatures(features, 2);
	makeFeatures(features, 1);
	for (int i = 0; i < features.size(); i++)
		testPrint(features, i);
	testMean(features);
}

 // loads features from 1 image
void loadFeatures(vector<cv::Mat> &features)
{
	features.clear();
	cv::Ptr<cv::BRISK> brisk = cv::BRISK::create();
	cout << "Extracting features from image" << endl;
	cout << endl;

	cv::Mat image = cv::imread("images/image0.png", 0);
	cv::Mat mask;
	vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;

	brisk->detectAndCompute(image, mask, keypoints, descriptors);

	changeStructure(descriptors, features);
}

// change structure of a descriptor to a vector of descriptors
void changeStructure(const cv::Mat &plain, vector<cv::Mat> &out)
{
  out.resize(plain.rows);

  for(int i = 0; i < plain.rows; ++i)
  {
    out[i] = plain.row(i);
  }
}

void testPrint(vector<cv::Mat> &features, int v)
{
	if (features.size() < v + 1)
	{
		cout << "no features in given vector" << endl;
	}
	else 
	{
		cout << "testing print function" << endl;
		cout << FBrisk::toString(features[v]) << endl;
		cout << endl;
	}
}

void testMean(vector<cv::Mat> &features)
{
	vector<FBrisk::pDescriptor> descriptors;
	descriptors.reserve(features.size());
	for(int i = 0; i < features.size(); i++)
		descriptors.push_back(&features[i]);
	cv::Mat mean;
	FBrisk::meanValue(descriptors, mean);
	cout << "Mean of the given features:" << endl;
	cout << FBrisk::toString(mean) << endl;
	cout << endl;
}

void testDistance(vector<cv::Mat> &features, int f1, int f2)
{
	cout << "testing distance function:" << endl;
	double distance = FBrisk::distance(features[f1], features[f2]);
	cout << "feature 1:" << endl;
	cout << FBrisk::toString(features[f1]) << endl;
	cout << endl;
	cout << "feature 2:" << endl;
	cout << FBrisk::toString(features[f2]) << endl;
	cout << endl;
	cout << "Hamming distance between features: " << distance << endl;
	cout << endl;
}

void makeFeatures(vector<cv::Mat> &features, int numFeatures)
{
	stringstream s;
	for (int i = 0; i < numFeatures; i++)
	{
		for (int j = 0; j < FBrisk::L; j++)
		{
			if (j == i)
				s << "255" << " ";
			else
				s << "0" << " ";
		}
		cv::Mat f1;
		FBrisk::fromString(f1, s.str());
		features.push_back(f1);
		s.str("");
	}
}