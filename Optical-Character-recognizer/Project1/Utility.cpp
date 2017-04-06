#include "Utility.h"
#define PI 3.14159265
#define RAD2DEG(rad) (180/PI) * rad

Mat SobelGradient(Mat src, bool showImg) {
	Mat grad_x, grad_y;
	Mat img_grad = Mat(src.rows, src.cols, CV_64F);
	int ddepth = CV_64F;
	grad_x = Mat(src.rows, src.cols, CV_64F);
	grad_y = Mat(src.rows, src.cols, CV_64F);
	//GaussianBlur(src, src, Size(3, 3), 1.5, 1.5);
	//Canny(src, src, 5, 20); //threshold for hand-written characters
	//blur(src, src, Size(3, 3));
	Canny(src, src, 5, 50);

	if (showImg) {
		namedWindow("Display window", WINDOW_FREERATIO);
		imshow("Display window", src);
		waitKey(0);
	}

	Sobel(src, grad_x, ddepth, 1, 0, 13);
	Sobel(src, grad_y, ddepth, 0, 1, 13);

	//namedWindow("Display window", WINDOW_FREERATIO);
	//imshow("Display window", grad_x);
	//waitKey(0);


	for (unsigned int i = 0; i < img_grad.rows; i++) {
		for (unsigned int j = 0; j < img_grad.cols; j++) {
			double x = grad_x.at<double>(i, j);
			double y = grad_y.at<double>(i, j);
			double grad = RAD2DEG(atan2(y, x));
			grad = grad + (grad < 0) * _DegNum;
			grad = fmod(grad, _DegNum);
			img_grad.at<double>(i, j) = grad;
			//cout<<img_grad.at<double>(i, j)<< " ";
		}
		//cout<<endl;
	}

	grad_x = ~Mat();
	grad_y = ~Mat();

	return img_grad;
}