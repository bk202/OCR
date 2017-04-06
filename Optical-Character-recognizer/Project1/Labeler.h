#pragma once
#include <Blob.h>
#include <Templates.h>

class Labeler {
private:
	Mat Original_Image;
	vector <Blob> blob_vector;
	int distance_threshold;
	vector <Mat> blob_img;
	//vector <vector <Blob> > sorted_blobs;
	string sorted_blobs[_NumberofRows][_NumberofCols];
	void SortAndRemove();
	void BlobReduction();
	bool within(Blob inner_blob, Blob outter_blob);
	float DisCalc(TemplateSet characterTemplate, BaseTemplate input_img);
	int scanLine(Mat ST1, Mat ST2, Mat ST3, Mat blob_img, float EHD_dist[], float weightEHD, float weightSL);
	bool present(int min_pos[], vector <TemplateSet> T, string c1, string c2);

public:
	Labeler(Mat img, int distance_threshold, Mat img_org);
	void Match(vector <TemplateSet> alphabetSet);
	~Labeler();
	void sort();
	void print();
};

bool isNoise(Mat img);