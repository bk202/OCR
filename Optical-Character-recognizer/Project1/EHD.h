#pragma once
#include <Header.h>


class EHD
{
protected:
	int bin_num;
	struct sectors{
		float rl, rh;
		int count;
		float prob;
	};
	vector <sectors> bins;
	int pix_num;

public:
	EHD();
	EHD(int num_bin, Mat img);
	~EHD(void);
};

