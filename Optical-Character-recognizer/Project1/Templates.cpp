#include "Templates.h"

BaseTemplate::BaseTemplate() {};

BaseTemplate::BaseTemplate(int num_bin, Mat img, bool showImg) : EHD(num_bin, img)
{
	Mat img_org;
	img.copyTo(img_org);
	this->rows = img.rows;
	this->cols = img.cols;
	cvtColor(img_org, img_org, CV_BGR2GRAY);
	//GaussianBlur(img_org, img_org, Size(7, 7), 1.5, 1.5);
	Mat binMat;
	threshold(img_org, binMat, 100, 255, cv::THRESH_BINARY);
	Mat img_grad = SobelGradient(img_org, showImg);
	im_grad = img_grad;

	float sector_size = _DegNum / bin_num;

	for(unsigned int i=0; i<img_grad.rows; i++){
		for(unsigned int j=0; j<img_grad.cols; j++){
			double pix_grad = img_grad.at<double>(i, j);
			//cout << pix_grad << " ";
			if(pix_grad != 0){
				int bin = pix_grad / sector_size;
				bins.at(bin).count++;
				pix_num++;
			}
		}
		//cout << endl;
	}
	for (unsigned int i=0; i<bins.size(); i++){
		bins.at(i).prob = (float) bins.at(i).count / pix_num;
	}

	binPix[0] = binDetect(1, 0, 0, 0, 0, 0, 0, 0, binMat); //top mid
	binPix[1] = binDetect(0, 1, 0, 0, 0, 0, 0, 0, binMat); //bot mid
	binPix[2] = binDetect(0, 0, 1, 0, 0, 0, 0, 0, binMat); //left mid
	binPix[3] = binDetect(0, 0, 0, 1, 0, 0, 0, 0, binMat); //right mid
	binPix[4] = binDetect(0, 0, 0, 0, 1, 0, 0, 0, binMat); //top left
	binPix[5] = binDetect(0, 0, 0, 0, 0, 1, 0, 0, binMat); //top right
	binPix[6] = binDetect(0, 0, 0, 0, 0, 0, 1, 0, binMat); //bot left
	binPix[7] = binDetect(0, 0, 0, 0, 0, 0, 0, 1, binMat); //bot right
}

int BaseTemplate::binDetect(bool horizontalUp, bool horizontalDown, bool verticalLeft, bool verticalRight, bool topLeft, bool topRight, bool botLeft, bool botRight, Mat img) {
	int midRow = img.rows / 2;
	int midCol = img.cols / 2;
	int Xdepth = img.cols / 5;
	int Ydepth = img.rows / 5;
	int leftBound = midCol - Xdepth / 2;
	int RightBound = midCol + Xdepth / 2;
	int UpperBound = midRow - Ydepth / 2;
	int LowerBound = midRow + Ydepth / 2;

	int cnt = 0;

	if (horizontalUp) {
		for (unsigned int i = 0; i<Ydepth; i++) {
			for (unsigned int j = leftBound; j < RightBound; j++) {
				if (img.at<uchar>(i, j) == 0) {
					cnt++;
				}
			}
		}

		return cnt;
	}

	else if (horizontalDown) {
		for (unsigned int i = img.rows - 1; i >= img.rows - Ydepth; i--) {
			for (unsigned int j = leftBound; j < RightBound; j++) {
				if (img.at<uchar>(i, j) == 0)
					cnt++;
			}
		}
	}

	else if (verticalLeft) {
		for (unsigned int i = UpperBound; i < LowerBound; i++) {
			for (unsigned int j = 0; j < Xdepth; j++) {
				if (img.at<uchar>(i, j) == 0) {
					cnt++;
				}
			}
		}
	}

	else if (verticalRight) {
		for (unsigned int i = UpperBound; i < LowerBound; i++) {
			for (unsigned int j = img.cols - Xdepth; j < img.cols; j++) {
				if (img.at<uchar>(i, j) == 0) {
					cnt++;
				}
			}
		}
	}
	else if(topLeft){
		for (unsigned int i=0; i < Ydepth; i++){
			for(unsigned int j=0; j < Xdepth; j++){
				if(img.at<uchar>(i, j) == 0){
					cnt++;
				}
			}
		}
	}

	else if(topRight){
		for(unsigned int i = 0; i < Ydepth; i++){
			for(unsigned int j = img.cols - Xdepth - 1; j < img.cols; j++){
				if(img.at<uchar>(i, j) == 0){
					cnt++;
				}
			}
		}
	}

	else if(botLeft){
		for(unsigned int i = img.rows - Ydepth - 1; i < img.rows; i++){
			for(unsigned int j = 0; j < Xdepth; j++){
				if(img.at<uchar>(i, j) == 0){
					cnt++;
				}
			}
		}
	}

	else if(botRight){
		for(unsigned int i = img.rows - Ydepth - 1; i <img.rows; i++){
			for(unsigned int j = img.cols - Xdepth - 1; j < img.cols; j++){
				if(img.at<uchar>(i, j) == 0){
					cnt++;
				}
			}
		}
	}


	return cnt;
}

void BaseTemplate::imshow_grad() {
	namedWindow("Display window", WINDOW_FREERATIO);
	imshow("Display window", im_grad);
	waitKey(0);
}


BaseTemplate::~BaseTemplate(void)
{
}

void BaseTemplate::imhist() {
	float sector_size = _DegNum / bin_num;
	for (unsigned int i = 0; i < bins.size(); i++) {
		cout << i * sector_size + sector_size / 2 << " : " << bins.at(i).prob << endl;
	}
	cout << pix_num << endl;
}

int BaseTemplate::size() {
	return this->bin_num;
}

float BaseTemplate::GetProb(int bin_num) {
	return this->bins.at(bin_num).prob;
}

TemplateSet::TemplateSet(string t) : character(t) {};
TemplateSet::~TemplateSet() {};

void TemplateSet::InsertTemplate(string t, int NumberOfBins) {
	string t1 = t;
	Mat test = imread(t1, CV_LOAD_IMAGE_COLOR);
	Size size(500, 500);
	resize(test, test, size);
	Mat img_org = test;
	cvtColor(test, test, CV_BGR2GRAY);
	GaussianBlur(test, test, Size(7, 7), 1.5, 1.5);
	threshold(test, test, 100, 255, cv::THRESH_BINARY);

	int min_x, min_y, max_x, max_y;
	min_x = 0; max_x = 10;
	min_y = 0; max_y = 10;
	for (int i = 0; i<img_org.rows; i++) {
		for (int j = 0; j<img_org.cols; j++) {
			if ((int)test.at<uchar>(i, j) == 0) {
				min_y = i;
				i = img_org.rows;
				break;
			}
		}
	}

	for (int i = img_org.rows - 1; i >= 0; i--) {
		for (int j = 0; j<img_org.rows; j++) {
			if ((int)test.at<uchar>(i, j) == 0) {
				max_y = i;
				i = -1;
				break;
			}
		}
	}

	for (int j = 0; j<img_org.cols; j++) {
		for (int i = 0; i<img_org.rows; i++) {
			if ((int)test.at<uchar>(i, j) == 0) {
				min_x = j;
				j = img_org.cols;
				break;
			}
		}
	}

	for (int j = img_org.cols - 1; j >= 0; j--) {
		for (int i = 0; i<img_org.rows; i++) {
			if ((int)test.at<uchar>(i, j) == 0) {
				max_x = j;
				j = -1;
				break;
			}
		}
	}

	int width = max_x - min_x;
	int height = max_y - min_y;

	img_org = img_org(Rect(min_x, min_y, width, height));

	BaseTemplate A = BaseTemplate(NumberOfBins, img_org);
	Templates.push_back(A);
}

void TemplateSet::UpdateProb() {
	for(int i=0; i<_NumberofBins; i++){
		float grad = 0;
		for(unsigned int j=0; j<Templates.size(); j++){
			grad += Templates.at(j).GetProb(i);
		}
		grad /= Templates.size();
		this->bin_meanGrad.push_back(grad);
	}

	this->avgRows = 0;
	this->avgCols = 0;
	this->binProb[0] = this->binProb[1] = this->binProb[2] = this->binProb[3] = 0;
	this->binProb[4] = this->binProb[5] = this->binProb[6] = this->binProb[7] = 0;

	for (unsigned int i = 0; i < Templates.size(); i++) {
		avgRows += Templates.at(i).rows;
		avgCols += Templates.at(i).cols;

		this->binProb[0] += Templates.at(i).binPix[0];
		this->binProb[1] += Templates.at(i).binPix[1];
		this->binProb[2] += Templates.at(i).binPix[2];
		this->binProb[3] += Templates.at(i).binPix[3];
		this->binProb[4] += Templates.at(i).binPix[4];
		this->binProb[5] += Templates.at(i).binPix[5];
		this->binProb[6] += Templates.at(i).binPix[6];
		this->binProb[7] += Templates.at(i).binPix[7];
	}

	avgRows /= Templates.size();
	avgCols /= Templates.size();

	this->binProb[0] /= Templates.size();
	this->binProb[1] /= Templates.size();
	this->binProb[2] /= Templates.size();
	this->binProb[3] /= Templates.size();
	this->binProb[4] /= Templates.size();
	this->binProb[5] /= Templates.size();
	this->binProb[6] /= Templates.size();
	this->binProb[7] /= Templates.size();
}

void TemplateSet::imhist() {
	float sector_size = _DegNum / _NumberofBins;
	for (unsigned int i = 0; i < bin_meanGrad.size(); i++) {
		cout << i * sector_size + sector_size / 2 << " : " << bin_meanGrad.at(i) << endl;
	}
}

/*TEMPLATE INFO FILE FORMAT
UPDATE STATUS
bin_meanGrad
binProb
avgRows
*/

void TemplateA(TemplateSet& T) {
	ifstream file("../Templates/TemplateA/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 50; i++) {
			string t = "../Templates/TemplateA/TemplateA_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateA/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}

	T.LoadStandardTemplate("../Templates/TemplateA/TemplateA_2.jpg");
}

void TemplateB(TemplateSet& T) {
	ifstream file("../Templates/TemplateB/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 65; i++) {
			string t = "../Templates/TemplateB/TemplateB_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateB/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}


	//for (int i = 1; i <= 65; i++) {
	//	string t = "../Templates/TemplateB/TemplateB_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateB/TemplateB_2.jpg");
	//T.UpdateProb();
}

void TemplateC(TemplateSet& T) {
	ifstream file("../Templates/TemplateC/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 50; i++) {
			string t = "../Templates/TemplateC/TemplateC_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateC/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 50; i++) {
	//	string t = "../Templates/TemplateC/TemplateC_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateC/TemplateC_2.jpg");
	//T.UpdateProb();
}

void TemplateD(TemplateSet& T) {
	ifstream file("../Templates/TemplateD/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 90; i++) {
			string t = "../Templates/TemplateD/TemplateD_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateD/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 90; i++) {
	//	string t = "../Templates/TemplateD/TemplateD_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateD/TemplateD_2.jpg");
	//T.UpdateProb();
}


void TemplateE(TemplateSet& T) {
	ifstream file("../Templates/TemplateE/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 130; i++) {
			string t = "../Templates/TemplateE/TemplateE_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateE/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 130; i++) {
	//	string t = "../Templates/TemplateE/TemplateE_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateE/TemplateE_2.jpg");
	//T.UpdateProb();
}

void TemplateF(TemplateSet& T) {
	ifstream file("../Templates/TemplateF/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 140; i++) {
			string t = "../Templates/TemplateF/TemplateF_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateF/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 140; i++) {
	//	string t = "../Templates/TemplateF/TemplateF_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateF/TemplateF_2.jpg");
	//T.UpdateProb();
}

void TemplateG(TemplateSet& T) {
	ifstream file("../Templates/TemplateG/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 20; i++) {
			string t = "../Templates/TemplateG/TemplateG_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateG/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 20; i++) {
	//	string t = "../Templates/TemplateG/TemplateG_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateG/TemplateG_2.jpg");
	//T.UpdateProb();
}

void TemplateH(TemplateSet& T) {
	ifstream file("../Templates/TemplateH/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 135; i++) {
			string t = "../Templates/TemplateH/TemplateH_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateH/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 135; i++) {
	//	string t = "../Templates/TemplateH/TemplateH_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateH/TemplateH_2.jpg");
	//T.UpdateProb();
}

void TemplateI(TemplateSet& T) {
	ifstream file("../Templates/TemplateI/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 50; i++) {
			string t = "../Templates/TemplateI/TemplateI_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateI/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 50; i++) {
	//	string t = "../Templates/TemplateI/TemplateI_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateI/TemplateI_2.jpg");
	//T.UpdateProb();
}

void TemplateJ(TemplateSet& T) {
	ifstream file("../Templates/TemplateJ/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 14; i++) {
			string t = "../Templates/TemplateJ/TemplateJ_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateJ/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 14; i++) {
	//	string t = "../Templates/TemplateJ/TemplateJ_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateJ/TemplateJ_2.jpg");
	//T.UpdateProb();
}

void TemplateK(TemplateSet& T) {
	ifstream file("../Templates/TemplateK/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 30; i++) {
			string t = "../Templates/TemplateK/TemplateK_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateK/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 30; i++) {
	//	string t = "../Templates/Templatek/Templatek_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateK/TemplateK_2.jpg");
	//T.UpdateProb();
}

void TemplateL(TemplateSet& T) {
	ifstream file("../Templates/TemplateL/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 45; i++) {
			string t = "../Templates/TemplateL/TemplateL_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateL/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 45; i++) {
	//	string t = "../Templates/Templatel/Templatel_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateL/TemplateL_2.jpg");
	//T.UpdateProb();
}

void TemplateM(TemplateSet& T) {
	ifstream file("../Templates/TemplateM/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 13; i++) {
			string t = "../Templates/TemplateM/TemplateM_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateM/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 13; i++) {
	//	string t = "../Templates/TemplateM/TemplateM_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateM/TemplateM_2.jpg");
	//T.UpdateProb();
}

void TemplateN(TemplateSet& T) {
	ifstream file("../Templates/TemplateN/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 60; i++) {
			string t = "../Templates/TemplateN/TemplateN_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateN/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 60; i++) {
	//	string t = "../Templates/TemplateN/TemplateN_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateN/TemplateN_2.jpg");
	//T.UpdateProb();
}

void TemplateO(TemplateSet& T) {
	ifstream file("../Templates/TemplateO/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 50; i++) {
			string t = "../Templates/TemplateO/TemplateO_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateO/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 50; i++) {
	//	string t = "../Templates/TemplateO/TemplateO_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateO/TemplateO_2.jpg");
	//T.UpdateProb();
}

void TemplateP(TemplateSet& T) {
	ifstream file("../Templates/TemplateP/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 6; i++) {
			string t = "../Templates/TemplateP/TemplateP_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateP/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 6; i++) {
	//	string t = "../Templates/TemplateP/TemplateP_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateP/TemplateP_2.jpg");
	//T.UpdateProb();
}

void TemplateQ(TemplateSet& T) {
	ifstream file("../Templates/TemplateQ/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 37; i++) {
			string t = "../Templates/TemplateQ/TemplateQ_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateQ/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 37; i++) {
	//	string t = "../Templates/TemplateQ/TemplateQ_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateQ/TemplateQ_2.jpg");
	//T.UpdateProb();
}

void TemplateR(TemplateSet& T) {
	ifstream file("../Templates/TemplateR/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 30; i++) {
			string t = "../Templates/TemplateR/TemplateR_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateR/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 30; i++) {
	//	string t = "../Templates/TemplateR/TemplateR_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateR/TemplateR_2.jpg");
	//T.UpdateProb();
}

void TemplateS(TemplateSet& T) {
	ifstream file("../Templates/TemplateS/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 55; i++) {
			string t = "../Templates/TemplateS/TemplateS_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateS/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 55; i++) {
	//	string t = "../Templates/TemplateS/TemplateS_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateS/TemplateS_2.jpg");
	//T.UpdateProb();
}

void TemplateT(TemplateSet& T) {
	ifstream file("../Templates/TemplateT/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 17; i++) {
			string t = "../Templates/TemplateT/TemplateT_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateT/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 17; i++) {
	//	string t = "../Templates/TemplateT/TemplateT_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateT/TemplateT_2.jpg");
	//T.UpdateProb();
}

void TemplateU(TemplateSet& T) {
	ifstream file("../Templates/TemplateU/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 55; i++) {
			string t = "../Templates/TemplateU/TemplateU_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateU/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 55; i++) {
	//	string t = "../Templates/TemplateU/TemplateU_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateU/TemplateU_25.jpg");
	//T.UpdateProb();
}

void TemplateV(TemplateSet& T) {
	ifstream file("../Templates/TemplateV/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 10; i++) {
			string t = "../Templates/TemplateV/TemplateV_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateV/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 10; i++) {
	//	string t = "../Templates/TemplateV/TemplateV_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateV/TemplateV_2.jpg");
	//T.UpdateProb();
}

void TemplateW(TemplateSet& T) {
	ifstream file("../Templates/TemplateW/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 20; i++) {
			string t = "../Templates/TemplateW/TemplateW_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateW/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 20; i++) {
	//	string t = "../Templates/TemplateW/TemplateW_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	//T.LoadStandardTemplate("../Templates/TemplateW/TemplateW_15.jpg");
	T.LoadStandardTemplate("../Templates/TemplateW/TemplateW_2.jpg");
	//T.UpdateProb();
}

void TemplateX(TemplateSet& T) {
	ifstream file("../Templates/TemplateX/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 34; i++) {
			string t = "../Templates/TemplateX/TemplateX_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateX/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 34; i++) {
	//	string t = "../Templates/TemplateX/TemplateX_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateX/TemplateX_2.jpg");
	//T.UpdateProb();
}

void TemplateY(TemplateSet& T) {
	ifstream file("../Templates/TemplateY/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 35; i++) {
			string t = "../Templates/TemplateY/TemplateY_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateY/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 35; i++) {
	//	string t = "../Templates/TemplateY/TemplateY_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateY/TemplateY_2.jpg");
	//T.UpdateProb();
}

void TemplateZ(TemplateSet& T) {
	ifstream file("../Templates/TemplateZ/TemplateInfo.txt");
	string status;
	getline(file, status);
	if (status == "Updated") {
		string word;
		for (int i = 0; i < _NumberofBins; i++) {
			file >> word;
			float bin_meanGrad = stof(word);
			T.bin_meanGrad.push_back(bin_meanGrad);
		}

		for (int i = 0; i < 8; i++) {
			file >> word;
			T.binProb[i] = stof(word);
		}
		file >> word;
		T.avgRows = stof(word, 0);
		file >> word;
		T.avgCols = stof(word, 0);
		file.close();
	}

	else {
		for (int i = 1; i <= 5; i++) {
			string t = "../Templates/TemplateZ/TemplateZ_" + to_string(i) + ".jpg";
			T.InsertTemplate(t, _NumberofBins);
		}
		T.UpdateProb();
		ofstream file;
		file.open("../Templates/TemplateZ/TemplateInfo.txt");
		file << "Updated\n";
		for (int i = 0; i < _NumberofBins; i++) {
			file << T.bin_meanGrad.at(i) << " ";
		}
		file << "\n";
		for (int i = 0; i < 8; i++) {
			file << T.binProb[i] << " ";
		}
		file << "\n";
		file << T.avgRows;
		file << "\n";
		file << T.avgCols;
		file << "\n";
		file.close();
	}
	//for (int i = 1; i <= 5; i++) {
	//	string t = "../Templates/TemplateZ/TemplateZ_" + to_string(i) + ".jpg";
	//	T.InsertTemplate(t, _NumberofBins);
	//}

	T.LoadStandardTemplate("../Templates/TemplateZ/TemplateZ_2.jpg");
	//T.UpdateProb();
}

void TemplateSet::LoadStandardTemplate(string t) {
	standardTemplate = imread(t, CV_LOAD_IMAGE_COLOR);
	cvtColor(standardTemplate, standardTemplate, CV_BGR2GRAY);
	threshold(standardTemplate, standardTemplate, 100, 255, cv::THRESH_BINARY);

	//namedWindow("Display window", WINDOW_FREERATIO);
	//imshow("Display window", standardTemplate);
	//waitKey(0);
}