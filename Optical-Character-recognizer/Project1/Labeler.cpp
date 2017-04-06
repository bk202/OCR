#include <Labeler.h>

Labeler::Labeler(Mat img, int d_threshold, Mat img_org){
	distance_threshold = d_threshold;
	img_org.copyTo(Original_Image);
	for (unsigned int i = 0; i < img.rows; i++) {
		for (unsigned int j = 0; j < img.cols; j++) {
			int pix = img.at<uchar>(i, j);
			if (pix != 0) {
				bool found = false;
				for (unsigned int k = 0; k < blob_vector.size(); k++) {
					if (blob_vector.at(k).isNear(j, i, (float)distance_threshold)) {
						found = true;
						break;
					}
				}
				if (!found) {
					Blob b(j, i);
					blob_vector.push_back(b);
				}
			}
		}
	}

	this->BlobReduction();

	for (unsigned int i = 0; i < blob_vector.size(); i++) {
		Point2f p1(blob_vector.at(i).min_x, blob_vector.at(i).min_y);
		Point2f p2(blob_vector.at(i).max_x, blob_vector.at(i).max_y);
		rectangle(img_org, p1, p2, (255, 255, 255));
	}

	namedWindow("Display window", CV_WINDOW_AUTOSIZE);
	//imshow("Display window", Original_Image);
	imshow("Display window", img_org);
	waitKey(0);

	//for(unsigned int i=0; i<blob_img.size(); i++){
	//	namedWindow("Display window", WINDOW_FREERATIO);
	//	imshow("Display window", blob_img.at(i));
	//	waitKey(0);
	//}
}

Labeler::~Labeler() {};

void Labeler::Match(vector <TemplateSet> alphabetSet){
	for(unsigned int i=0; i<blob_img.size(); i++){
		float min_dist[3];
		int min_pos[3];
		min_dist[0] = min_dist[1] = min_dist[2] = INFINITY;
		min_pos[0] = min_pos[1] = min_pos[2] = -1;
		BaseTemplate input_img;
		for(unsigned int j=0; j<alphabetSet.size(); j++){
			Size size(alphabetSet.at(j).avgCols, alphabetSet.at(j).avgRows);
			Mat tmp_img;
			blob_img.at(i).copyTo(tmp_img);
			resize(tmp_img, tmp_img, size);
			input_img = BaseTemplate(_NumberofBins, tmp_img);
			
			float dist = DisCalc(alphabetSet.at(j), input_img);
			if(min_dist[0] > dist){
				if (min_dist[0] < min_dist[1]) {
					if (min_dist[1] < min_dist[2]) {
						min_dist[2] = min_dist[1];
						min_pos[2] = min_pos[1];
					}
					min_dist[1] = min_dist[0];
					min_pos[1] = min_pos[0];
				}

				min_dist[0] = dist;
				min_pos[0] = j;
			}
			else if (min_dist[1] > dist) {
				if (min_dist[1] < min_dist[2]) {
					min_dist[2] = min_dist[1];
					min_pos[2] = min_pos[1];
				}

				min_dist[1] = dist;
				min_pos[1] = j;
			}
			else if (min_dist[2] > dist) {
				min_dist[2] = dist;
				min_pos[2] = j;
			}
			//cout<<alphabetSet.at(j).character<<" : ";
			//cout<<dist<<endl;
		}
		//cout<<endl;
		if(min_pos[0] != -1){
			float weightEHD, weightSL;
			
			if (present(min_pos, alphabetSet, "Q", "O") || present(min_pos, alphabetSet, "J", "T")  || present(min_pos, alphabetSet, "S", "E")){
				weightEHD = 0.9;
				weightSL = 0.1;
			}
			else if (present(min_pos, alphabetSet, "B", "E") || present(min_pos, alphabetSet, "P", "D") || present(min_pos, alphabetSet, "I", "J") || present(min_pos, alphabetSet, "Y", "V")) {
				weightEHD = 0.3;
				weightSL = 0.7;
			}
			else {
				weightEHD = 0.7;
				weightSL = 0.3;
			}

			//cout << alphabetSet.at(min_pos[0]).character << " " << alphabetSet.at(min_pos[1]).character << " " << alphabetSet.at(min_pos[2]).character << endl;
			int min_position = scanLine(alphabetSet.at(min_pos[0]).standardTemplate, alphabetSet.at(min_pos[1]).standardTemplate, alphabetSet.at(min_pos[2]).standardTemplate, blob_img.at(i), min_dist, weightEHD, weightSL);
			blob_vector.at(i).character = alphabetSet.at(min_pos[min_position]).character;
			//cout<<alphabetSet.at(min_pos[min_position]).character<<endl;
			//input_img.imhist();
			//cout<<endl;
			//disp.imhist();
			//namedWindow("Display window", WINDOW_FREERATIO);
			//imshow("Display window", blob_img.at(i));
			//waitKey(0);
			//input_img.imshow_grad();
			//alphabetSet.at(3).Templates.at(15).imhist();
			//alphabetSet.at(3).Templates.at(15).imshow_grad();
			//input_img.imshow_grad();
			//int w = 0;
			//cin >> w;
			//alphabetSet.at(w).Templates.at(0).imshow_grad();
		}
	}
}

float Labeler::DisCalc(TemplateSet characterTemplate, BaseTemplate input_img){
	float Euc_Dis_from_template = 0;
	for(unsigned int j=0; j<_NumberofBins; j++){
		float tmp = input_img.GetProb(j) - characterTemplate.bin_meanGrad.at(j);
		Euc_Dis_from_template += pow(tmp, 2);
	}

	float F2_dist = 0;
	for (unsigned int i = 0; i < 8; i++) {
		float tmp = input_img.binPix[i] - characterTemplate.binProb[i];
		tmp = pow(tmp, 2);
		F2_dist += tmp;
		//cout << input_img.binPix[i] << " " << characterTemplate.binProb[i] << endl;
	}

	F2_dist = sqrt(F2_dist);
	//cout << F2_dist << endl;
	Euc_Dis_from_template = sqrt(Euc_Dis_from_template) + (F2_dist / 10000);
	return Euc_Dis_from_template;
}

void Labeler::SortAndRemove(){
	for(unsigned int i=0; i<blob_vector.size(); i++){
		int min_x, max_x, min_y, max_y;
		min_x = blob_vector.at(i).min_x;
		min_y = blob_vector.at(i).min_y;
		max_x = blob_vector.at(i).max_x;
		max_y = blob_vector.at(i).max_y;
		int width = max_x - min_x;
		width = width + (width < 1) * 1;
		int height = max_y - min_y;
		height = height + (height < 1) * 1;
		Mat tmp_img = Original_Image(Rect(min_x, min_y, width, height));
		cout << i << endl;
		namedWindow("Display window", WINDOW_AUTOSIZE);
		imshow("Display window", tmp_img);
		waitKey(0);
		if (!isNoise(tmp_img)) {
			blob_img.push_back(tmp_img);
			//namedWindow("Display window", WINDOW_AUTOSIZE);
			//imshow("Display window", tmp_img);
			//waitKey(0);
		}
	}
}

bool isNoise(Mat img) {
	Mat tmp;
	img.copyTo(tmp);
	Size size(100, 100);
	resize(tmp, tmp, size);
	cvtColor(tmp, tmp, CV_BGR2GRAY);
	GaussianBlur(tmp, tmp, Size(7, 7), 1.5, 1.5);
	Canny(tmp, tmp, 20, 50);
	float threshold = 0.5 * img.cols * img.rows;
	int cnt = 0;
	for (unsigned int i = 0; i < tmp.rows; i++) {
		for (unsigned int j = 0; j < tmp.cols; j++) {
			if (tmp.at<uchar>(i, j) != 0) cnt++;
		}
	}

	if (cnt < threshold) return true;
	return false;
}

void Labeler::BlobReduction() {
	int threshold = _Distance_TH_L;
	for (unsigned int i = 0; i < blob_vector.size(); i++) {
		for (unsigned int j = 0; j < blob_vector.size(); j++) {
			if (j == i) continue;

			//right blobs
			int distX = abs(blob_vector.at(i).max_x - blob_vector.at(j).min_x);
			int distY = abs(blob_vector.at(i).max_y - blob_vector.at(j).max_y);
			int Height = blob_vector.at(i).max_y - blob_vector.at(i).min_y;
			
			if (distX <= threshold && distY < Height) {
				blob_vector.at(i).min_x = min(blob_vector.at(j).min_x, blob_vector.at(i).min_x);
				blob_vector.at(i).max_x = max(blob_vector.at(j).max_x, blob_vector.at(i).max_x);
				blob_vector.at(i).min_y = min(blob_vector.at(j).min_y, blob_vector.at(i).min_y);
				blob_vector.at(i).max_y = max(blob_vector.at(j).max_y, blob_vector.at(i).max_y);
				continue;
			}

			//left blobs
			distX = abs(blob_vector.at(i).min_x - blob_vector.at(j).max_x);
			if (distX <= threshold && distY <= Height) {
				blob_vector.at(i).min_x = min(blob_vector.at(j).min_x, blob_vector.at(i).min_x);
				blob_vector.at(i).max_x = max(blob_vector.at(j).max_x, blob_vector.at(i).max_x);
				blob_vector.at(i).min_y = min(blob_vector.at(j).min_y, blob_vector.at(i).min_y);
				blob_vector.at(i).max_y = max(blob_vector.at(j).max_y, blob_vector.at(i).max_y);
				continue;
			}
		}
	}

	vector <Blob> new_blobVector;
	for (unsigned int i = 0; i < blob_vector.size(); i++) {
		bool isWithin = false;
		for (unsigned int j = 0; j < blob_vector.size(); j++) {
			if (j == i) continue;
			if (this->within(blob_vector.at(i), blob_vector.at(j))) {
				isWithin = true;
				break;
			}
		}
		if (!isWithin) {
			new_blobVector.push_back(blob_vector.at(i));
			int min_x, max_x, min_y, max_y;
			min_x = blob_vector.at(i).min_x;
			min_y = blob_vector.at(i).min_y;
			max_x = blob_vector.at(i).max_x;
			max_y = blob_vector.at(i).max_y;
			int width = max_x - min_x;
			width = width + (width < 1) * 1;
			int height = max_y - min_y;
			height = height + (height < 1) * 1;
			Mat tmp_img = Original_Image(Rect(min_x, min_y, width, height));
			//cout << i << endl;
			//namedWindow("Display window", WINDOW_FREERATIO);
			//imshow("Display window", tmp_img);
			//waitKey(0);
			blob_img.push_back(tmp_img);
		}
	}

	this->blob_vector = new_blobVector;
}

bool Labeler::within(Blob inner_blob, Blob outter_blob) {
	int cx = (inner_blob.min_x + inner_blob.max_x) / 2;
	int cy = (inner_blob.min_y + inner_blob.max_y) / 2;

	bool xWithin = (cx >= outter_blob.min_x) && (cx <= outter_blob.max_x);
	bool yWithin = (cy >= outter_blob.min_y) && (cy <= outter_blob.max_y);

	return (xWithin && yWithin);
}

int Labeler::scanLine(Mat ST1, Mat ST2, Mat ST3, Mat blob_img, float EHD_dist[], float weightEHD, float weightSL) {

	int dist_Y[3];
	int dist_X[3];
	dist_Y[2] = dist_Y[1] = dist_Y[0] = INFINITY;
	dist_X[2] = dist_X[1] = dist_X[0] = INFINITY;

	//==============Scan line comparison with standard template 1 =======================================================
	Size size1(ST1.cols, ST1.rows);
	Mat tmp_img;
	blob_img.copyTo(tmp_img);
	resize(tmp_img, tmp_img, size1);

	int mid_col = tmp_img.cols / 2;
	int left_bound = mid_col - 5;
	int right_bound = mid_col + 5;

	int mid_row = tmp_img.rows / 2;
	int upper_bound = mid_row - 5;
	int lower_bound = mid_row + 5;

	cvtColor(tmp_img, tmp_img, CV_BGR2GRAY);
	Mat binaryMat(tmp_img.size(), tmp_img.type());
	threshold(tmp_img, binaryMat, 100, 255, cv::THRESH_BINARY);

	int accumEdges = 0;
	int ST_edges = 0;

	for (unsigned int i = 0; i < binaryMat.rows; i++) {
		for (unsigned int j = left_bound; j <= right_bound; j++) {
			int input_img_pix = (int)binaryMat.at<uchar>(i, j);
			if (input_img_pix == 0)
				accumEdges++;
			int ST_pix = (int)ST1.at<uchar>(i, j);
			if (ST_pix == 0)
				ST_edges++;
		}
	}

	dist_Y[0] = accumEdges - ST_edges;

	accumEdges = 0;
	ST_edges = 0;

	for (unsigned int i = upper_bound; i <= lower_bound; i++) {
		for (unsigned int j = 0; j < binaryMat.cols; j++) {
			int pix = (int)binaryMat.at<uchar>(i, j);
			if (pix == 0)
				accumEdges++;
			int ST_pix = (int)ST1.at<uchar>(i, j);
			if (ST_pix == 0)
				ST_edges++;
		}
	}
	
	dist_X[0] = accumEdges - ST_edges;

	//=================================Scan line comparison with standard template 2 =======================================
	Size size2(ST2.cols, ST2.rows);
	blob_img.copyTo(tmp_img);
	resize(tmp_img, tmp_img, size2);

	cvtColor(tmp_img, tmp_img, CV_BGR2GRAY);
	binaryMat = Mat(tmp_img.size(), tmp_img.type());
	threshold(tmp_img, binaryMat, 100, 255, cv::THRESH_BINARY);

	mid_col = tmp_img.cols / 2;
	left_bound = mid_col - 5;
	right_bound = mid_col + 5;

	mid_row = tmp_img.rows / 2;
	upper_bound = mid_row - 5;
	lower_bound = mid_row + 5;

	accumEdges = 0;
	ST_edges = 0;

	for (unsigned int i = 0; i < binaryMat.rows; i++) {
		for (unsigned int j = left_bound; j <= right_bound; j++) {
			int pix = (int)binaryMat.at<uchar>(i, j);
			if (pix == 0)
				accumEdges++;
			int ST_pix = ST2.at<uchar>(i, j);
			if (ST_pix == 0)
				ST_edges++;
		}
	}

	dist_Y[1] = accumEdges - ST_edges;

	accumEdges = 0;
	ST_edges = 0;

	for (unsigned int i = upper_bound; i <= lower_bound; i++) {
		for (unsigned int j = 0; j < binaryMat.cols; j++) {
			int pix = (int)binaryMat.at<uchar>(i, j);
			if (pix == 255)
				accumEdges++;
			int ST_pix = (int)ST2.at<uchar>(i, j);
			if (ST_pix == 255)
				ST_edges++;
		}
	}

	dist_X[1] = accumEdges - ST_edges;

	
	//==============================Scan line comparison with standard template 3 =============================================================
	Size size3(ST3.cols, ST3.rows);
	blob_img.copyTo(tmp_img);
	resize(tmp_img, tmp_img, size3);

	cvtColor(tmp_img, tmp_img, CV_BGR2GRAY);
	binaryMat = Mat(tmp_img.size(), tmp_img.type());
	threshold(tmp_img, binaryMat, 100, 255, cv::THRESH_BINARY);

	mid_col = tmp_img.cols / 2;
	left_bound = mid_col - 5;
	right_bound = mid_col + 5;

	mid_row = tmp_img.rows / 2;
	upper_bound = mid_row - 5;
	lower_bound = mid_row + 5;

	accumEdges = 0;
	ST_edges = 0;

	for (unsigned int i = 0; i < binaryMat.rows; i++) {
		for (unsigned int j = left_bound; j <= right_bound; j++) {
			int pix = (int)binaryMat.at<uchar>(i, j);
			if (pix == 0)
				accumEdges++;
			int ST_pix = ST3.at<uchar>(i, j);
			if (ST_pix == 0)
				ST_edges++;
		}
	}

	dist_Y[2] = abs(accumEdges - ST_edges);

	accumEdges = 0;
	ST_edges = 0;

	for (unsigned int i = upper_bound; i <= lower_bound; i++) {
		for (unsigned int j = 0; j < binaryMat.cols; j++) {
			int pix = (int)binaryMat.at<uchar>(i, j);
			if (pix == 0)
				accumEdges++;
			int ST_pix = (int)ST3.at<uchar>(i, j);
			if (ST_pix == 0)
				ST_edges++;
		}
	}

	dist_X[2] = accumEdges - ST_edges;


	//namedWindow("Display window", WINDOW_FREERATIO);
	//imshow("Display window", binaryMat);
	//waitKey(0);

	float dist[3];
	dist[0] = dist[1] = dist[2] = INFINITY;
	int min = 0;
	for (int i = 0; i < 3; i++) {
		int tmpX = pow(dist_X[i], 2);
		int tmpY = pow(dist_Y[i], 2);
		dist[i] = (sqrt(tmpX + tmpY) * weightSL) + (EHD_dist[i]*1000 * weightEHD);
		if (dist[i] < dist[min]) {
			min = i;
		}
		//cout << dist[i] << " ";
	}
	//cout << endl;

	return min;
}

bool Labeler :: present(int min_pos[], vector <TemplateSet> T, string c1, string c2) {
	bool c1present, c2present;
	c1present = c2present = false;
	for (int i = 0; i < 3; i++) {
		string character = T.at(min_pos[i]).character;
		if (character == c1) {
			c1present = true;
		}
		else if (character == c2) {
			c2present = true;
		}
	}

	return (c1present && c2present);
}

void Labeler::sort() {
	for (int i = 0; i < _NumberofRows; i++) {
		for (int j = 0; j < _NumberofCols; j++) {
			sorted_blobs[i][j] = " ";
		}
	}

	int rowSize = Original_Image.rows / _NumberofRows;
	int colSize = Original_Image.cols / _NumberofCols;

	for (unsigned int i = 0; i < blob_vector.size(); i++) {
		string c = blob_vector.at(i).character;
		int cx = (blob_vector.at(i).max_x + blob_vector.at(i).min_x) / 2;
		int cy = (blob_vector.at(i).max_y + blob_vector.at(i).min_y) / 2;
		int rowPos = cy / rowSize;
		int colPos = cx / colSize;
		sorted_blobs[rowPos][colPos] = blob_vector.at(i).character;
	}
}

void Labeler::print() {
	ofstream file("../Output.txt");
	for (int i = 0; i < _NumberofRows; i++) {
		for (int j = 0; j < _NumberofCols; j++) {
			cout << sorted_blobs[i][j];
			file << sorted_blobs[i][j];
		}
		cout << endl;
		file << "\n";
	}
}