#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat find_green_video(Mat frame);
void find_text();
void find_blue();
void find_green_img();

int main() {
	VideoCapture cap("road_video.mp4");

	if (!cap.isOpened()) {
		cerr << "Video open faild!" << endl;
		return 0;
	}

	Mat frame;
	while (true) {
		cap >> frame;
		if (frame.empty())
			break;
		Mat dst = find_green_video(frame);

		imshow("result", dst);
		if (waitKey(30) == 27)
			//destroyAllWindows();
			break;
	}
	
	//find_blue();
	//find_text();
	return 0;
}

void find_green_img() {
	Mat kernel1 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	Mat kernel2 = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));

	Mat src;
	src = imread("D:/University/Junior/snd_semester/Open_CV/team_project/team_project/road_sign03.jpg", IMREAD_COLOR);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	Mat dst;
	resize(src, dst, Size(800, 800));//���� (800,800)���� ũ�� ��ȯ
	Mat gray_src;
	cvtColor(dst, gray_src, COLOR_BGR2GRAY);


	Mat img_hsv;
	cvtColor(dst, img_hsv, COLOR_BGR2HSV);

	Mat green_mask, green_img;

	//�׸��� �����̵带 �̿��Ͽ� �뷫���� ��� ǥ���� h�� ã�� (70~90)
	Scalar lower_green = Scalar(75, 120, 80); //(h,s,v)->(����, ä��, ����)
	Scalar upper_green = Scalar(90, 255, 150);

	//�ʷϻ� ���� ����
	inRange(img_hsv, lower_green, upper_green, green_mask);
	//morphologyEx(green_mask, green_mask, MORPH_OPEN, kernel1);
	//morphologyEx(green_mask, green_mask, MORPH_CLOSE, kernel1);

	//�����̹����� ������ ���� �ʷϻ� ����ũ �ռ�
	bitwise_and(dst, dst, green_img, green_mask);

	//imshow("green_mask", green_mask);
	//imshow("green_img", green_img);

	Mat img_canny;
	Canny(green_img, img_canny, 100, 150);//canny �������� ���� ����
	//imshow("canny", img_canny);

	Mat morpho;
	morphologyEx(img_canny, morpho, MORPH_CLOSE, kernel2);//�������� ����
	//imshow("morpho", morpho);

	vector<vector<Point>> contours;//�ܰ��� ������ ����
	vector<Vec4i>hierarchy;//�ܰ����� ���� ���� ��� ����
	findContours(morpho, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//�ܰ��� ����

	//���������� �ܰ��� ���
	Mat dst2;
	dst.copyTo(dst2);
	drawContours(dst2, contours, -1, Scalar(0, 0, 255), 2);
	//imshow("dst2", dst2);

	for (vector<Point>& pts : contours) {
		if (contourArea(pts) < 40)
			continue;
		vector<Point> approx;
		approxPolyDP(pts, approx, arcLength(pts, true) * 0.035, true);

		int vtc = (int)approx.size();

		//������ ������ 4���̰ų� 6���� ��� �ٿ�� �ڽ��� �׸�
		if (vtc == 4 || vtc == 6) {
			Rect rc = boundingRect(pts);
			rectangle(dst, rc, Scalar(0, 0, 255), 2);
		}
	}
	imshow("result", dst);
	waitKey();
}

void find_blue() {
	Mat kernel1 = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
	Mat kernel2 = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));

	Mat src;
	src = imread("D:/University/Junior/snd_semester/Open_CV/team_project/team_project/road_sign09.png", IMREAD_COLOR);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	Mat dst;
	resize(src, dst, Size(800, 800));//���� (800,800)���� ũ�� ��ȯ
	Mat gray_src;
	cvtColor(dst, gray_src, COLOR_BGR2GRAY);


	Mat img_hsv;
	cvtColor(dst, img_hsv, COLOR_BGR2HSV);

	Mat blue_mask, blue_img;

	//�׸��� �����̵带 �̿��Ͽ� �뷫���� û�� ǥ���� h�� ã�� (70~90)
	Scalar lower_blue = Scalar(105, 165, 95); //(h,s,v)->(����, ä��, ����)
	Scalar upper_blue = Scalar(125, 255, 250);

	//û�� ���� ����
	inRange(img_hsv, lower_blue, upper_blue, blue_mask);
	//morphologyEx(blue_mask, blue_mask, MORPH_OPEN, kernel1);
	//morphologyEx(blue_mask, blue_mask, MORPH_CLOSE, kernel1);

	//�����̹����� ������ ���� û�� ����ũ �ռ�
	bitwise_and(dst, dst, blue_img, blue_mask);

	imshow("blue_mask", blue_mask);
	imshow("blue_img", blue_img);

	Mat img_canny;
	Canny(blue_img, img_canny, 100, 150);//canny �������� ���� ����
	imshow("canny", img_canny);

	Mat morpho;
	morphologyEx(img_canny, morpho, MORPH_CLOSE, kernel2);//�������� ����
	imshow("morpho", morpho);

	vector<vector<Point>> contours;//�ܰ��� ������ ����
	vector<Vec4i>hierarchy;//�ܰ����� ���� ���� ��� ����
	findContours(morpho, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//�ܰ��� ����

	//���������� �ܰ��� ���
	Mat dst2;
	dst.copyTo(dst2);
	drawContours(dst2, contours, -1, Scalar(0, 0, 255), 2);
	imshow("dst2", dst2);

	for (vector<Point>& pts : contours) {
		if (contourArea(pts) < 40)
			continue;
		vector<Point> approx;
		approxPolyDP(pts, approx, arcLength(pts, true) * 0.035, true);

		int vtc = (int)approx.size();

		//������ ������ 4���̰ų� 6���� ��� �ٿ�� �ڽ��� �׸�
		if (vtc == 4 || vtc == 6) {
			Rect rc = boundingRect(pts);
			rectangle(dst, rc, Scalar(0, 0, 255), 2);
		}
	}
	imshow("result", dst);
	waitKey();
}

//https://www.youtube.com/watch?v=kOqUw7t5bYk&t=479s
//https://stackoverflow.com/questions/46044853/opencv-draw-rectangles-around-only-large-contours
//https://poorman.tistory.com/184
Mat find_green_video(Mat frame) {
	Mat kernel1 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	Mat kernel2 = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));

	Mat dst;
	resize(frame, dst, Size(800, 800));//���� (800,800)���� ũ�� ��ȯ
	Mat gray_src;
	cvtColor(dst, gray_src, COLOR_BGR2GRAY);
	

	Mat img_hsv;
	cvtColor(dst, img_hsv, COLOR_BGR2HSV);
	
	Mat green_mask, green_img;

	//�׸��� �����̵带 �̿��Ͽ� �뷫���� ��� ǥ���� h�� ã�� (70~90)
	Scalar lower_green = Scalar(75, 120, 80); //(h,s,v)->(����, ä��, ����)
	Scalar upper_green = Scalar(95, 255, 150);

	//�ʷϻ� ���� ����
	inRange(img_hsv, lower_green, upper_green, green_mask);
	//morphologyEx(green_mask, green_mask, MORPH_OPEN, kernel1);
	//morphologyEx(green_mask, green_mask, MORPH_CLOSE, kernel1);

	//�����̹����� ������ ���� �ʷϻ� ����ũ �ռ�
	bitwise_and(dst, dst, green_img, green_mask);

	imshow("green_mask", green_mask);
	imshow("green_img", green_img);

	Mat img_canny;
	Canny(green_img, img_canny, 100, 150);//canny �������� ���� ����
	//imshow("canny", img_canny);

	Mat morpho;
	morphologyEx(img_canny, morpho, MORPH_CLOSE, kernel2);//�������� ����
	//imshow("morpho", morpho);
	
	vector<vector<Point>> contours;//�ܰ��� ������ ����
	vector<Vec4i>hierarchy;//�ܰ����� ���� ���� ��� ����
	findContours(morpho, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//�ܰ��� ����

	//���������� �ܰ��� ���
	Mat dst2;
	dst.copyTo(dst2);
	drawContours(dst2, contours, -1, Scalar(0,0,255),2);
	//imshow("dst2", dst2);

	for (vector<Point>& pts : contours) {
		if (contourArea(pts) < 40)
			continue;
		vector<Point> approx;
		approxPolyDP(pts, approx, arcLength(pts, true) * 0.035, true);

		int vtc = (int)approx.size();
		
		//������ ������ 4���̰ų� 6���� ��� �ٿ�� �ڽ��� �׸�
		if (vtc == 4 ||vtc == 6) {
			Rect rc = boundingRect(pts);
			rectangle(dst, rc, Scalar(0, 0, 255), 2);
		}
	}
	return dst;
	//imshow("result", dst);
	//waitKey();
}


void find_text() {
	Mat color_img, contrast_img, canny_img, morph_open, morph_close;
	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 5));
	vector<vector<Point>>contour;//�ܰ��� ���� ��� ����
	vector<Vec4i>hierarchy;//�ܰ����� ���� ���� ��� ����

	Mat img = imread("D:/University/Junior/snd_semester/Open_CV/team_project/team_project/templ05.png", IMREAD_GRAYSCALE);//�׷��̽����Ϸ� ���� �о���̱�

	if (img.empty()) {
		cerr << "Image load faild!" << endl;
		return;
	}
	
	resize(img, img, Size(500, 500));//���� (500,500)���� ũ�� ��ȯ
	float alpha = 3.f; //��ϵ� ���İ�

	contrast_img = img + (img - 128) * alpha;//��ϵ� ����
	imshow("img", img);
	imshow("Contrast_img", contrast_img);
	//adaptiveThreshold(contrast_img, canny_img, 255, ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV ,5,3);

	Canny(contrast_img, canny_img, 100, 300, 3);//canny �������� ���� ����
	imshow("canny_img", canny_img);

	morphologyEx(canny_img, morph_close, MORPH_CLOSE, kernel);//�������� ���� ����
	imshow("morph_close", morph_close);

	findContours(morph_close, contour, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);// �ܰ��� ����
	
	cvtColor(img, color_img, COLOR_GRAY2BGR); //�׷��̽����Ͽ��� BGR�� ��ȯ

	drawContours(color_img, contour, -1, Scalar(0, 0, 255), 2);//�������� �ܰ��� ��Ÿ����
	imshow("contour_img", color_img);

	vector<vector<Point>> contours_poly(contour.size());
	vector<Rect>boundBox1(contour.size());
	vector<Rect>boundBox2(contour.size());
	double ratio, refinery_count = 0;

	for (int i = 0; i < contour.size(); i++){
		approxPolyDP(Mat(contour[i]), contours_poly[i], 1, true);
		boundBox1[i] = boundingRect(Mat(contours_poly[i]));
	}
	Mat bound_img = Mat::zeros(img.size(),CV_8UC3);

	for (int i = 0; i < contour.size(); i++)
	{
		ratio = (double)boundBox1[i].height / boundBox1[i].width;
		if ((ratio <= 5) && (ratio >= 0.5) && (boundBox1[i].area() <= 2000) && (boundBox1[i].area() >= 100)) {
			drawContours(bound_img, contour, i, Scalar(0, 255, 255), 1, 8, hierarchy, 0, Point());
			rectangle(bound_img, boundBox1[i].tl(), boundBox1[i].br(), Scalar(255, 0), 1, 8, 0);
			boundBox2[refinery_count] = boundBox1[i];
			refinery_count += 1;
		}
	}
	boundBox2.resize(refinery_count);
	imshow("contour", bound_img);
	waitKey();
}
