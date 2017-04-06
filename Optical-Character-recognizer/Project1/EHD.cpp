#include "EHD.h"


EHD::EHD(int num_bin, Mat img)
{
	bin_num = num_bin;
	float sector_size = _DegNum / bin_num;
	float cnt = 0;
	pix_num = 0;
	for(unsigned int i=0; i<bin_num; i++){
		sectors s;
		s.count = 0;
		s.rl = cnt;
		cnt += sector_size;
		s.rh = cnt;
		bins.push_back(s);
	}
}

EHD::EHD() {};

EHD::~EHD(void)
{
}
