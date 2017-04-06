#pragma once
#include "EHD.h"
#include <Header.h>
#include <Utility.h>

class BaseTemplate : EHD
{
private:
	Mat im_grad;
	int binDetect(bool horizontalUp, bool horizontalDown, bool verticalLeft, bool verticalRight, bool topLeft, bool topRight, bool botLeft, bool botRight, Mat img);
public:
	int rows, cols;
	BaseTemplate();
	BaseTemplate(int num_bin, Mat img, bool showImg = false);
	~BaseTemplate(void);
	void imhist();
	void imshow_grad();
	int size();
	float GetProb(int bin_num);
	int binPix[8];
};

class TemplateSet {
public:
	unsigned long long avgRows;
	unsigned long long avgCols;
	TemplateSet(string t);
	~TemplateSet();
	void InsertTemplate(string t, int NumberOfBins);
	void UpdateProb();
	string character;
	vector <float> bin_meanGrad;
	vector <BaseTemplate> Templates;
	Mat standardTemplate;
	void LoadStandardTemplate(string t);
	void imhist();
	float binProb[8];
};


void TemplateA(TemplateSet& T);
void TemplateB(TemplateSet& T);
void TemplateC(TemplateSet& T);
void TemplateD(TemplateSet& T);
void TemplateE(TemplateSet& T);
void TemplateF(TemplateSet& T);
void TemplateG(TemplateSet& T);
void TemplateH(TemplateSet& T);
void TemplateI(TemplateSet& T);
void TemplateJ(TemplateSet& T);
void TemplateK(TemplateSet& T);
void TemplateL(TemplateSet& T);
void TemplateM(TemplateSet& T);
void TemplateN(TemplateSet& T);
void TemplateO(TemplateSet& T);
void TemplateP(TemplateSet& T);
void TemplateQ(TemplateSet& T);
void TemplateR(TemplateSet& T);
void TemplateS(TemplateSet& T);
void TemplateT(TemplateSet& T);
void TemplateU(TemplateSet& T);
void TemplateV(TemplateSet& T);
void TemplateW(TemplateSet& T);
void TemplateX(TemplateSet& T);
void TemplateY(TemplateSet& T);
void TemplateZ(TemplateSet& T);
bool TemplateCompare(BaseTemplate InputTemplate, TemplateSet TS, int NumberOfBins);