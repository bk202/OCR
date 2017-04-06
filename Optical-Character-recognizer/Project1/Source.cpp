#include <Header.h>
#include <Labeler.h>
#include <Utility.h>
#include <Templates.h>

Labeler BlobDetect(string t1){
	Mat img = imread(t1, CV_LOAD_IMAGE_COLOR);
	//Size size(500, 500);
	//resize(img, img, size);
	Mat img_org = img;
	cvtColor(img, img, CV_BGR2GRAY);
	GaussianBlur(img, img, Size(7, 7), 1.5, 1.5);
	Canny(img, img, 60, 200);

	//namedWindow("Display window", WINDOW_AUTOSIZE);
	//imshow("Display window", img_org);
	//waitKey(0);

	int distance_threshold = _Distance_TH_H;

	Labeler L(img, distance_threshold, img_org);
	return L;
}

void LoadTemplates(vector <TemplateSet>& alphabetSet) {
	TemplateSet Template_A("A");
	TemplateA(Template_A);
	TemplateSet Template_B("B");
	TemplateB(Template_B);
	TemplateSet Template_C("C");
	TemplateC(Template_C);
	TemplateSet Template_D("D");
	TemplateD(Template_D);
	TemplateSet Template_E("E");
	TemplateE(Template_E);
	TemplateSet Template_F("F");
	TemplateF(Template_F);
	TemplateSet Template_G("G");
	TemplateG(Template_G);
	TemplateSet Template_H("H");
	TemplateH(Template_H);
	TemplateSet Template_I("I");
	TemplateI(Template_I);
	TemplateSet Template_J("J");
	TemplateJ(Template_J);
	TemplateSet Template_K("K");
	TemplateK(Template_K);
	TemplateSet Template_L("L");
	TemplateL(Template_L);
	TemplateSet Template_M("M");
	TemplateM(Template_M);
	TemplateSet Template_N("N");
	TemplateN(Template_N);
	TemplateSet Template_O("O");
	TemplateO(Template_O);
	TemplateSet Template_P("P");
	TemplateP(Template_P);
	TemplateSet Template_Q("Q");
	TemplateQ(Template_Q);
	TemplateSet Template_R("R");
	TemplateR(Template_R);
	TemplateSet Template_S("S");
	TemplateS(Template_S);
	TemplateSet Template_T("T");
	TemplateT(Template_T);
	TemplateSet Template_U("U");
	TemplateU(Template_U);
	TemplateSet Template_V("V");
	TemplateV(Template_V);
	TemplateSet Template_W("W");
	TemplateW(Template_W);
	TemplateSet Template_X("X");
	TemplateX(Template_X);
	TemplateSet Template_Y("Y");
	TemplateY(Template_Y);
	TemplateSet Template_Z("Z");
	TemplateZ(Template_Z);

	alphabetSet.push_back(Template_A);
	alphabetSet.push_back(Template_B);
	alphabetSet.push_back(Template_C);
	alphabetSet.push_back(Template_D);
	alphabetSet.push_back(Template_E);
	alphabetSet.push_back(Template_F);
	alphabetSet.push_back(Template_G);
	alphabetSet.push_back(Template_H);
	alphabetSet.push_back(Template_I);
	alphabetSet.push_back(Template_J);
	alphabetSet.push_back(Template_K);
	alphabetSet.push_back(Template_L);
	alphabetSet.push_back(Template_M);
	alphabetSet.push_back(Template_N);
	alphabetSet.push_back(Template_O);
	alphabetSet.push_back(Template_P);
	alphabetSet.push_back(Template_Q);
	alphabetSet.push_back(Template_R);
	alphabetSet.push_back(Template_S);
	alphabetSet.push_back(Template_T);
	alphabetSet.push_back(Template_U);
	alphabetSet.push_back(Template_V);
	alphabetSet.push_back(Template_W);
	alphabetSet.push_back(Template_X);
	alphabetSet.push_back(Template_Y);
	alphabetSet.push_back(Template_Z);


}

int main() {
	vector <TemplateSet> alphabetSet;

	LoadTemplates(alphabetSet);

	/*Labeler L = BlobDetect("../Test_image.jpg");*/
	Labeler L = BlobDetect("../Test_image5.jpg");

	L.Match(alphabetSet);
	L.sort();
	L.print();
	return 0;
}