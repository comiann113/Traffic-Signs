#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void find_green_video(Mat frame);
void find_blue_video(Mat frame);
void find_blue_img(Mat img);
void find_green_img(Mat img);

int main() {
	int select_num = 0;
	string img_path;
	string video_path;

	
	cout << "\n이미지 검출, 동영상 검출 중 선택해주세요!\n" << endl;
	cout << "1. 이미지 검출\n" << "2. 동영상 검출" << endl;
	cin >> select_num;
	tryagin:
		if (select_num == 1) {
			cout << "사진의 경로를 입력해주세요: " << endl;
			cin >> img_path;
			Mat img = imread(img_path);
			
			if (img.empty()) {
				cerr << "Image load failed!" << endl;
				return 0;
			}

			resize(img, img, Size(800, 800));

			find_green_img(img);
			find_blue_img(img);
			waitKey();

		}

		else if (select_num == 2) {
			cout << "동영상의 경로를 입력해주세요: " << endl;
			cin >> video_path;
			VideoCapture cap(video_path);

			if (!cap.isOpened()) {
				cerr << "Video open faild!" << endl;
				return 0;
			}

			Mat frame;
			while (true) {
				cap >> frame;

				if (frame.empty())
				break;

				resize(frame, frame, Size(800, 800));

				find_green_video(frame);
				find_blue_video(frame);
				if (waitKey(30) == 27)
					break;
			}
		}

		else {
			cout << "잘못된 입력입니다. 1번 그림 검출과 2번 동영상 검출 중 선택해주세요!" << endl;
			cin >> select_num;
			goto tryagin;
		}

	return 0;
}

void find_green_img(Mat img) {
	Mat kernel2 = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));

	Mat dsts = img;
	Mat shifted;
	pyrMeanShiftFiltering(img, shifted, 21, 51);
	//imshow("shifted", shifted);

	Mat gray_img;
	cvtColor(shifted, gray_img, COLOR_BGR2GRAY);
	//imshow("gray_img", gray_img);

	Mat bin_img;
	threshold(gray_img, bin_img, 0, 255, THRESH_BINARY | THRESH_OTSU);
	//imshow("bin_img", bin_img);

	Mat sure_bg;
	dilate(bin_img, sure_bg, Mat::ones(3, 3, CV_8U));
	//imshow("sure Background", sure_bg);

	
	Mat inv_bg, bit;
	threshold(sure_bg, inv_bg, 0, 255, THRESH_BINARY_INV);
	bitwise_and(dsts, dsts, bit, inv_bg);
	//imshow("bitwise_and", bit);

	Mat img_hsv;
	cvtColor(bit, img_hsv, COLOR_BGR2HSV);

	Mat green_mask, green_img;

	//그림판 스포이드를 이용하여 대략적인 녹색 표지판 hsv값 찾음
	Scalar lower_green = Scalar(75, 120, 80); //(h,s,v)->(색상, 채도, 선명도)
	Scalar upper_green = Scalar(90, 255, 150);

	//초록색 영역 검출
	inRange(img_hsv, lower_green, upper_green, green_mask);

	//원본이미지에 위에서 구한 초록색 마스크 합성
	bitwise_and(bit, bit, green_img, green_mask);
	//imshow("color_mask", green_mask);
	//imshow("color_img", green_img);

	Mat img_canny;
	Canny(green_img, img_canny, 100, 150);//canny 연산으로 에지 검출
	//imshow("canny_img", img_canny);

	Mat morpho;
	morphologyEx(img_canny, morpho, MORPH_CLOSE, kernel2);//모폴로지 닫힘
	//imshow("morpho", morpho);

	vector<vector<Point>> contours;//외곽선 저장할 변수
	vector<Vec4i>hierarchy;//외곽선의 계층 정보 담는 변수
	findContours(morpho, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//외곽선 검출

	//빨간색으로 외곽선 출력
	Mat img2;
	img.copyTo(img2);
	drawContours(img2, contours, -1, Scalar(0, 0, 255), 2);

	vector<Rect> boundRect1(contours.size());
	int i = 0;

	for (vector<Point>& pts : contours) {
		if (contourArea(pts) < 40)
			continue;
		vector<Point> approx;
		approxPolyDP(pts, approx, arcLength(pts, true) * 0.035, true);

		int vtc = (int)approx.size();

		//꼭지점 개수가 4개이거나 6개인 경우 바운딩 박스를 그림
		if (vtc == 4 || vtc == 6) {
			boundRect1[i] = boundingRect(pts);
			i++;
		}
	}
	//width와 height의 길이가 50 이상인 것만 박스 그리기
	for (int j = 0; j < boundRect1.size(); j++) {
		if (boundRect1[j].width >= 50 && boundRect1[j].height >= 50)
			rectangle(img, boundRect1[j], Scalar(0, 0, 255), 2);
	}
	imshow("result", img);
}

void find_blue_img(Mat img) {
	Mat kernel1 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	Mat kernel2 = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));

	Mat shifted;
	pyrMeanShiftFiltering(img, shifted, 21, 51);
	//imshow("shifted", shifted);

	Mat gray_img;
	cvtColor(shifted, gray_img, COLOR_BGR2GRAY);
	//imshow("gray_img", gray_img);

	Mat bin_img;
	threshold(gray_img, bin_img, 0, 255, THRESH_BINARY | THRESH_OTSU);
	//imshow("bin_img", bin_img);

	Mat sure_bg;
	dilate(bin_img, sure_bg, Mat::ones(3, 3, CV_8U));
	//imshow("sure Background", sure_bg);

	Mat inv_bg, bit;
	threshold(sure_bg, inv_bg, 0, 255, THRESH_BINARY_INV);
	bitwise_and(img, img, bit, inv_bg);
	//imshow("bitwise_or", bit);

	Mat img_hsv;
	cvtColor(bit, img_hsv, COLOR_BGR2HSV);

	Mat blue_mask, blue_img;

	//그림판 스포이드를 이용하여 대략적인 청색 표지판 hsv값 찾음
	Scalar lower_blue = Scalar(100, 150, 95); //(h,s,v)->(색상, 채도, 선명도)
	Scalar upper_blue = Scalar(125, 255, 255);

	//청색 영역 검출
	inRange(img_hsv, lower_blue, upper_blue, blue_mask);

	//원본이미지에 위에서 구한 청색 마스크 합성
	bitwise_and(bit, bit, blue_img, blue_mask);

	//imshow("blue_mask", blue_mask);
	//imshow("blue_img", blue_img);

	Mat img_canny;
	Canny(blue_img, img_canny, 100, 150);//canny 연산으로 에지 검출
	//imshow("canny", img_canny);

	Mat morpho;
	morphologyEx(img_canny, morpho, MORPH_CLOSE, kernel2);//모폴로지 닫힘
	//imshow("morpho", morpho);

	vector<vector<Point>> contours;//외곽선 저장할 변수
	vector<Vec4i>hierarchy;//외곽선의 계층 정보 담는 변수
	findContours(morpho, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//외곽선 검출

	//빨간색으로 외곽선 출력
	Mat img2;
	img.copyTo(img2);
	drawContours(img2, contours, -1, Scalar(0, 0, 255), 2);
	//imshow("img2", img2);

	//박스의 가로와 세로가 50 이상인 것만 넣음
	vector<Rect> boundRect1(contours.size());

	int i = 0;
	for (vector<Point>& pts : contours) {
		if (contourArea(pts) < 40)
			continue;
		vector<Point> approx;
		approxPolyDP(pts, approx, arcLength(pts, true) * 0.04, true);

		int vtc = (int)approx.size();

		//꼭지점 개수가 4개이거나 6개인 경우 바운딩 박스를 그림
		if (vtc == 4 || vtc == 6) {
			boundRect1[i] = boundingRect(pts);
			i++;
		}
	}
	//width와 height의 길이가 50 이상인 것만 박스 그리기
	for (int j = 0; j < boundRect1.size(); j++) {
		if (boundRect1[j].width >= 50 && boundRect1[j].height >= 50)
			rectangle(img, boundRect1[j], Scalar(255, 0, 255), 2);
	}
	imshow("result", img);
}

//https://www.youtube.com/watch?v=kOqUw7t5bYk&t=479s
//https://stackoverflow.com/questions/46044853/opencv-draw-rectangles-around-only-large-contours
//https://poorman.tistory.com/184

void find_green_video(Mat frame) {
	Mat kernel1 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	Mat kernel2 = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));

	Mat dst = frame;

	Mat img_hsv;
	cvtColor(dst, img_hsv, COLOR_BGR2HSV);

	Mat green_mask, green_img;
	Mat blue_mask, blue_img;

	//그림판 스포이드를 이용하여 대략적인 녹색 표지판 hsv값 찾음 (70~90)
	Scalar lower_green = Scalar(75, 100, 60); //(h,s,v)->(색상, 채도, 선명도)
	Scalar upper_green = Scalar(95, 255, 230);

	//초록색 영역 검출
	inRange(img_hsv, lower_green, upper_green, green_mask);
	morphologyEx(green_mask, green_mask, MORPH_CLOSE, kernel2);
	morphologyEx(green_mask, green_mask, MORPH_OPEN, Mat());

	//원본이미지에 위에서 구한 초록색 마스크 합성
	bitwise_and(dst, dst, green_img, green_mask);

	//imshow("green_mask", green_mask);
	//imshow("green_img", green_img);

	Mat img_canny;
	Canny(green_img, img_canny, 100, 150);//canny 연산으로 에지 검출
	//imshow("canny", img_canny);

	Mat morpho;
	morphologyEx(img_canny, morpho, MORPH_CLOSE, kernel2);//모폴로지 닫힘
	//imshow("morpho", morpho);
	
	vector<vector<Point>> contours;//외곽선 저장할 변수
	vector<Vec4i>hierarchy;//외곽선의 계층 정보 담는 변수

	findContours(morpho, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//외곽선 검출

	//빨간색으로 외곽선 출력
	Mat dst2;
	dst.copyTo(dst2);
	drawContours(dst2, contours, -1, Scalar(0, 0, 255), 2);
	//imshow("dst2", dst2);

	vector<Rect>boundbox(contours.size());
	int i = 0;

	for (vector<Point>& pts : contours) {
		if (contourArea(pts) < 40)
			continue;
		vector<Point> approx;
		approxPolyDP(pts, approx, arcLength(pts, true) * 0.035, true);

		int vtc = (int)approx.size();

		//꼭지점 개수가 4개이거나 6개인 경우 바운딩 박스를 그림
		if (vtc == 4 || vtc == 6) 
			Rect rc = boundingRect(pts);
		
		for (int i = 0; i< boundbox.size();i++) {
			boundbox[i] = boundingRect(pts);
			i++;
		}
		//width와 height의 길이가 25 이상인 것만 박스 그리기
		for (int j = 0; j < boundbox.size(); j++) {
			if (boundbox[j].width >= 25 && boundbox[j].height >= 25)
				rectangle(dst, boundbox[j], Scalar(0, 0, 255), 2);
		}
	}
	
	imshow("result", dst);
}
//https://stackoverflow.com/questions/55376338/how-to-join-nearby-bounding-boxes-in-opencv-python

void find_blue_video(Mat frame) {
	Mat kernel1 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	Mat kernel2 = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));

	Mat dst = frame;

	Mat img_hsv;
	cvtColor(dst, img_hsv, COLOR_BGR2HSV);

	Mat blue_mask, blue_img;

	//그림판 스포이드를 이용하여 대략적인 청색 표지판 hsv값 찾음 (70~90)
	Scalar lower_blue = Scalar(90, 165, 95); //(h,s,v)->(색상, 채도, 선명도)
	Scalar upper_blue = Scalar(125, 255, 255);

	//청색 영역 검출
	inRange(img_hsv, lower_blue, upper_blue, blue_mask);
	morphologyEx(blue_mask, blue_mask, MORPH_CLOSE, kernel2);
	morphologyEx(blue_mask, blue_mask, MORPH_OPEN, Mat());

	//원본이미지에 위에서 구한 청색 마스크 합성
	bitwise_and(dst, dst, blue_img, blue_mask);

	//imshow("blue_mask", blue_mask);
	//imshow("blue_img", blue_img);

	Mat img_canny;
	Canny(blue_img, img_canny, 100, 150);//canny 연산으로 에지 검출
	//imshow("canny", img_canny);

	Mat morpho;
	morphologyEx(img_canny, morpho, MORPH_CLOSE, kernel2);//모폴로지 닫힘
	//imshow("morpho", morpho);

	vector<vector<Point>> contours;//외곽선 저장할 변수
	vector<Vec4i>hierarchy;//외곽선의 계층 정보 담는 변수

	findContours(morpho, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//외곽선 검출

	//빨간색으로 외곽선 출력
	Mat dst2;
	dst.copyTo(dst2);
	drawContours(dst2, contours, -1, Scalar(255, 0, 255), 2);
	//imshow("dst2", dst2);

	vector<Rect>boundbox(contours.size());
	int i = 0;

	for (vector<Point>& pts : contours) {
		if (contourArea(pts) < 40)
			continue;
		vector<Point> approx;
		approxPolyDP(pts, approx, arcLength(pts, true) * 0.035, true);

		int vtc = (int)approx.size();

		//꼭지점 개수가 4개이거나 6개인 경우 바운딩 박스를 그림
		if (vtc == 4 || vtc == 6) {
			boundbox[i] = boundingRect(pts);
			i++;
		}
		//width와 height의 길이가 25 이상인 것만 박스 그리기
		for (int j = 0; j < boundbox.size(); j++) {
			if (boundbox[j].width >= 25 && boundbox[j].height >= 25)
				rectangle(dst, boundbox[j], Scalar(255, 0, 255), 2);
		}
	}
	
	imshow("result", dst);
}