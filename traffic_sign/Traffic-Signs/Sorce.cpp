#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat find_green_video(Mat frame);
void find_blue();
void find_green_img();

int main() {

	VideoCapture cap("img/road_video.mp4");

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
	src = imread("D:/University/Junior/snd_semester/Open_CV/team_project/img/road_sign03.jpg", IMREAD_COLOR);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	Mat dst;
	resize(src, dst, Size(800, 800));//사진 (800,800)으로 크기 변환
	Mat gray_src;
	cvtColor(dst, gray_src, COLOR_BGR2GRAY);


	Mat img_hsv;
	cvtColor(dst, img_hsv, COLOR_BGR2HSV);

	Mat green_mask, green_img;

	//그림판 스포이드를 이용하여 대략적인 녹색 표지판 h값 찾음 (70~90)
	Scalar lower_green = Scalar(75, 120, 80); //(h,s,v)->(색상, 채도, 선명도)
	Scalar upper_green = Scalar(90, 255, 150);

	//초록색 영역 검출
	inRange(img_hsv, lower_green, upper_green, green_mask);

	//원본이미지에 위에서 구한 초록색 마스크 합성
	bitwise_and(dst, dst, green_img, green_mask);

	Mat img_canny;
	Canny(green_img, img_canny, 100, 150);//canny 연산으로 에지 검출

	Mat morpho;
	morphologyEx(img_canny, morpho, MORPH_CLOSE, kernel2);//모폴로지 닫힘

	vector<vector<Point>> contours;//외곽선 저장할 변수
	vector<Vec4i>hierarchy;//외곽선의 계층 정보 담는 변수
	findContours(morpho, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//외곽선 검출

	//빨간색으로 외곽선 출력
	Mat dst2;
	dst.copyTo(dst2);
	drawContours(dst2, contours, -1, Scalar(0, 0, 255), 2);

	for (vector<Point>& pts : contours) {
		if (contourArea(pts) < 40)
			continue;
		vector<Point> approx;
		approxPolyDP(pts, approx, arcLength(pts, true) * 0.035, true);

		int vtc = (int)approx.size();

		//꼭지점 개수가 4개이거나 6개인 경우 바운딩 박스를 그림
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
	src = imread("D:/University/Junior/snd_semester/Open_CV/team_project/img/road_sign02.jpg", IMREAD_COLOR);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	Mat dst;
	resize(src, dst, Size(800, 800));//사진 (800,800)으로 크기 변환
	Mat gray_src;
	cvtColor(dst, gray_src, COLOR_BGR2GRAY);


	Mat img_hsv;
	cvtColor(dst, img_hsv, COLOR_BGR2HSV);

	Mat blue_mask, blue_img;

	//그림판 스포이드를 이용하여 대략적인 청색 표지판 h값 찾음 (70~90)
	Scalar lower_blue = Scalar(105, 165, 95); //(h,s,v)->(색상, 채도, 선명도)
	Scalar upper_blue = Scalar(125, 255, 250);

	//청색 영역 검출
	inRange(img_hsv, lower_blue, upper_blue, blue_mask);
	//morphologyEx(blue_mask, blue_mask, MORPH_OPEN, kernel1);
	//morphologyEx(blue_mask, blue_mask, MORPH_CLOSE, kernel1);

	//원본이미지에 위에서 구한 청색 마스크 합성
	bitwise_and(dst, dst, blue_img, blue_mask);

	imshow("blue_mask", blue_mask);
	imshow("blue_img", blue_img);

	Mat img_canny;
	Canny(blue_img, img_canny, 100, 150);//canny 연산으로 에지 검출
	imshow("canny", img_canny);

	Mat morpho;
	morphologyEx(img_canny, morpho, MORPH_CLOSE, kernel2);//모폴로지 닫힘
	imshow("morpho", morpho);

	vector<vector<Point>> contours;//외곽선 저장할 변수
	vector<Vec4i>hierarchy;//외곽선의 계층 정보 담는 변수
	findContours(morpho, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//외곽선 검출

	//빨간색으로 외곽선 출력
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

		//꼭지점 개수가 4개이거나 6개인 경우 바운딩 박스를 그림
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
	resize(frame, dst, Size(800, 800));//사진 (800,800)으로 크기 변환
	Mat gray_src;
	cvtColor(dst, gray_src, COLOR_BGR2GRAY);


	Mat img_hsv;
	cvtColor(dst, img_hsv, COLOR_BGR2HSV);

	Mat green_mask, green_img;

	//그림판 스포이드를 이용하여 대략적인 녹색 표지판 h값 찾음 (70~90)
	Scalar lower_green = Scalar(75, 120, 50); //(h,s,v)->(색상, 채도, 선명도)
	Scalar upper_green = Scalar(95, 255, 150);

	//초록색 영역 검출
	inRange(img_hsv, lower_green, upper_green, green_mask);
	//morphologyEx(green_mask, green_mask, MORPH_OPEN, kernel1);
	morphologyEx(green_mask, green_mask, MORPH_CLOSE, kernel2);

	//원본이미지에 위에서 구한 초록색 마스크 합성
	bitwise_and(dst, dst, green_img, green_mask);

	imshow("green_mask", green_mask);
	imshow("green_img", green_img);

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
	int num = 0;

	for (vector<Point>& pts : contours) {
		if (contourArea(pts) < 40)
			continue;
		vector<Point> approx;
		approxPolyDP(pts, approx, arcLength(pts, true) * 0.035, true);

		int vtc = (int)approx.size();

		//꼭지점 개수가 4개이거나 6개인 경우 바운딩 박스를 그림
		if (vtc == 4 || vtc == 6) {
			boundbox[num] = boundingRect(pts);

			Rect rc = boundingRect(pts);
			rectangle(dst, rc, Scalar(0, 0, 255), 2);
			return dst;
		}
		num++;
	}
	return dst;
}
//https://stackoverflow.com/questions/55376338/how-to-join-nearby-bounding-boxes-in-opencv-python
