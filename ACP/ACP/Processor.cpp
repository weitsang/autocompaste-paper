//
//  Processor.cpp
//  ACP
//
//  Created by Raghav Ramesh on 22/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "Processor.h"

using namespace cv;
using namespace std;

Processor::Processor() {
    
}


void Processor::setPage(Page page) {
    this->page = page;
}


Page Processor::getPage() {
    return page;
}


void Processor::resizeImage(int width, int height) {
    if (page.getImage().size().width != width) {
        cv::resize(page.getImage(), page.getImage(), page.getSizeRes(), 0, 0, INTER_CUBIC);
    } else {
        page.getImage().copyTo(page.getImage());
    }
}


void Processor::prepareImageForOCR() {
    cv::GaussianBlur(page.getImage(), page.getImage(), Size(3, 3), 0, 0);
    // Otsu's threshold already applied by Tesseract - use with caution
    cv::adaptiveThreshold(page.getImage(), page.getImage(), 255.f, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 5, 4.1f);
}


// For camera setup as given in the demo video of ACP-Paper
void Processor::rotateImageClockwise(double angle) {
    Size srcSize = page.getImage().size();
    Size dstSize(srcSize.height, srcSize.width);
    
    int len = max<int>(page.getImage().cols, page.getImage().rows);
    Point2f center(len/2., len/2.);
    page.setImage(getRotationMatrix2D(center, angle, 1.0));
}


void Processor::initialiseTesseractAPI() {
    tessAPI = new tesseract::TessBaseAPI();
    if(tessAPI->Init(NULL, "eng")) {
        cout << "Could not initialise Tesseract." << endl;
    }
}


string Processor::extractTextFromImage(Mat image) {
    initialiseTesseractAPI();
    string text;
    tessAPI->SetImage(image.data, image.cols, image.rows,
                      image.channels(), (int)image.step);
    text = tessAPI->GetUTF8Text();
    tessAPI->End();
    return text;
}


// TODO: requires testing
string Processor::replaceUnwantedCharactersWithSpace(string text) {
    string allowedCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890!@#$%^&*()-+_,.:;[]{}<>/\'\"? ";
    size_t nonalpha = text.find_first_not_of(allowedCharacters);
    
    while(nonalpha != string::npos) {
        text.at(nonalpha) = ' ';
        nonalpha = text.find_first_not_of(allowedCharacters);
    }
    return text;
}


// For removing blemishes
Mat Processor::erodeImage(int, void*) {
    int erosionType = MORPH_RECT;
    int erosionSize = 13;
    Mat element = getStructuringElement(erosionType, Size(2*erosionSize+1, 2*erosionSize+1), Point(erosionSize, erosionSize));
    
    cv::erode(this->getPage().getImage(), this->getPage().getImage(), element);
    return this->getPage().getImage();
}


Mat Processor::dilateImage(int, void*) {
    int dilationType = MORPH_RECT;
    double dilationSize = 1.0;
    Mat element = getStructuringElement( dilationType,
                                        Size(2*dilationSize, 2*dilationSize),
                                        Point(dilationSize, dilationSize));
    /// Apply the dilation operation
    dilate(this->getPage().getImage(), this->getPage().getImage(), element);
    return this->getPage().getImage();
}


// Splits parent image into several sub-parts TODO: fails sometimes, requires testing
vector<Mat> Processor::cutImageGivenWhiteLineLocations(vector<int> whiteLineLocations) {
    vector<Mat> images;
    Mat image = this->getPage().getImage();
    int width = image.size().width;
    sort(whiteLineLocations.begin(), whiteLineLocations.end());

    for (int i = 0, y = 0, height = 0; i < whiteLineLocations.size(); y = whiteLineLocations[i], i++) {
        height = whiteLineLocations[i] - y;
        Rect rect = Rect(0, y, width, height);
        images.push_back(Mat(image, rect));
    }
    return images;
}


// Calculates the sum of the pixels of each line, compares against adjacent lines and returns only those that are actually white
vector<int> Processor::findWhiteLines(Mat img) {
    vector<int> whiteLines;
    map<int, int> actualWhiteLines;
    int highest = 0;
    
    for (int i = 0; i < img.rows-1; i++) {
        Scalar s = sum(Mat(img, Rect(0, i, img.cols - 1, 1)));
        Scalar s2 = sum(Mat(img, Rect(0, i+1, img.cols - 1, 1)));
        Scalar s3 = s - s2;
        
        if ((int)s3[0] == 0) {
            highest = (highest < (int)s[0]) ? (int)s[0] : highest;
            actualWhiteLines[i] = (int)s[0];
        }
    }
    
    // Iterate through map to eliminate non white lines and push only white lines
    typedef map<int, int>::iterator it_type;
    for(it_type iterator = actualWhiteLines.begin(); iterator != actualWhiteLines.end(); iterator++) {
        if (iterator->second == highest)
            whiteLines.push_back(iterator->first);
    }
    
    return whiteLines;
}


// Returns the splitting locations (average of first white line after and last white line
// before line containing black pixels) from a list of white lines identified
vector<int> Processor::getSplittingLocations() {
    vector<int> splittingLocations;
    Mat image = this->getPage().getImage();
    vector<int> whiteLines = findWhiteLines(image);
    stack<int> splitter;
    
    splitter.push(whiteLines[0]);
    for (int i = 0; i<whiteLines.size(); i++) {
        if (whiteLines[i+1] - whiteLines[i] > 1) {
            splitter.push(whiteLines[i]);
            splitter.push(whiteLines[i+1]);
        }
    }
    splitter.push(whiteLines[whiteLines.size()-1]);
    
    int a, b;
    while(!splitter.empty()) {
        a = splitter.top();
        splitter.pop();
        b = splitter.top();
        splitter.pop();
        splittingLocations.push_back((a+b)/2);
    }
    return splittingLocations;
}



// Currently unused methods
vector<Mat> Processor::cutImage(int x_coord, int y_coord) {
    Mat bigImage = page.getImage();
    Mat smallImage = Mat(bigImage, Rect(0, 0, x_coord, y_coord));
    
    Size smallSize(x_coord, y_coord);
    vector<Mat> smallImages;
    
    for (int y = 0; y < bigImage.rows; y += smallSize.height) {
        for (int x = 0; x < bigImage.cols; x += smallSize.width) {
            Rect rect = Rect(x, y, smallSize.width, smallSize.height);
            smallImages.push_back(Mat(bigImage, rect));
        }
    }
    return smallImages;
}


// The following two methods were tried to identify white regions in the image
vector<Rect> Processor::detectLetters(cv::Mat img) {
    std::vector<cv::Rect> boundRect;
    cv::Mat img_gray, img_sobel, img_threshold, element;
    cvtColor(img, img_gray, CV_BGR2GRAY);
    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) );
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(img_threshold, contours, 0, 1);
    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
        if (contours[i].size()>100)
        {
            cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
            cv::Rect appRect( boundingRect( cv::Mat(contours_poly[i]) ));
            if (appRect.width>appRect.height)
                boundRect.push_back(appRect);
        }
    return boundRect;
}


void Processor::drawContours(int, void *) {
    Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    int thresh = 100;
    RNG rng(12345);
    
    /// Detect edges using Threshold
    threshold(this->getPage().getImage(), threshold_output, thresh, 255, THRESH_BINARY);
    /// Find contours
    findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    
    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point>> contours_poly( contours.size() );
    vector<Rect> boundRect(contours.size());
    vector<Point2f>center(contours.size());
    vector<float>radius(contours.size());
    
    for(int i = 0; i < contours.size(); i++) {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]) );
        minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
    }
    
    
    /// Draw polygonal contour + bonding rects + circles
    Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
        cv::Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        cv::drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        cout << "Color:" << color << endl;
        //        circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
    }
    
    /// Show in a window
    namedWindow("Contours", CV_WINDOW_AUTOSIZE);
    imshow("Contours", drawing);
}


void Processor::displayImage(Mat image) {
    imshow("Result", image);
//    warpAffine(page.getImage(), page.getImage(), rotatedMat, dstSize);
}


// Needs to be added for improving accuracy
void Processor::deskewImage(Mat image, double angle, Mat &rotated) {
    bitwise_not(image, image);
    vector<Point> points;
    Mat_<uchar>::iterator iter = image.begin<uchar>();
    Mat_<uchar>::iterator end = image.end<uchar>();
    
    for (; iter != end; ++iter)
        if (*iter)
            points.push_back(iter.pos());
    
    RotatedRect box = minAreaRect(Mat(points));
    Mat rotatedMat = getRotationMatrix2D(box.center, angle, 1);
    warpAffine(image, rotated, rotatedMat, image.size(), INTER_CUBIC);
    Size boxSize = box.size;
    if (box.angle < -45.f)
        swap(boxSize.width, boxSize.height);
    
    Mat cropped;
    getRectSubPix(rotated, boxSize, box.center, rotated);
    bitwise_not(rotated, rotated);
    cout << "Deskewing image.." << endl;
}


// Methods for page flip

double page::computeSkew(){
	
	cvtColor(Frame, Frame, CV_BGR2GRAY);
	GaussianBlur(Frame, Frame, Size(3, 3), 0, 0);
	adaptiveThreshold(Frame, Frame, 255.f, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 5, 4.1f);
	
	cv::bitwise_not(Frame, Frame);
		
	cv::Size size = Frame.size();
	std::vector<cv::Vec4i> lines;

	cv::HoughLinesP(Frame, lines, 1, CV_PI / 180, 100, 100, 100);
		
	double initialAngle = 0.;
	double skewAngle = 0.;
	double lineAngle = 0.;
	int lineCount = 0;
	unsigned nb_lines = lines.size();

	for (unsigned i = 0; i < nb_lines; ++i)
	{
		initialAngle += atan2((double)lines[i][3] - lines[i][1],
			(double)lines[i][2] - lines[i][0]);
	}

	initialAngle /= nb_lines; 
	initialAngle = initialAngle * 180 / CV_PI;

		
	return initialAngle;
}

void page::deskew(Mat &destination, double angle){
	
	std::vector<cv::Point> points;
	cv::Mat_<uchar>::iterator it = Frame.begin<uchar>();
	cv::Mat_<uchar>::iterator end = Frame.end<uchar>();

	for (; it != end; ++it)
	if (*it)
		points.push_back(it.pos());

	cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));

	cv::Mat rotationMat = cv::getRotationMatrix2D(box.center, angle, 1);

	cv::Mat rotated;
	cv::warpAffine(Frame, rotated, rotationMat, Frame.size(), cv::INTER_CUBIC);

	rotated.copyTo(destination);
}

void page::computeHoughLine(Mat image, Mat &destination){

	
	cv::medianBlur(image, image, 3);

	std::vector<cv::Vec4i> lines;
	double meanAngle = 0.0;
	Mat cannyOutput;
	
	Canny(image, cannyOutput, 50, 200, 3);
	cv::HoughLinesP(cannyOutput, lines, 1, CV_PI / 180, 100, 100, 50);
	
	Mat houghOutput = Mat::zeros(image.size(), CV_8UC1);;
	double lineangle, angle = 0.0;
	unsigned int numberOfLines = lines.size();
	
	for (unsigned i = 0; i < numberOfLines; ++i)
	{
		lineangle = atan2((double)lines[i][3] - lines[i][1],
			(double)lines[i][2] - lines[i][0]);
		meanAngle += lineangle;
	}

	meanAngle /= numberOfLines;// mean angle, in radians.
	meanAngle *= 180 / M_PI;

	
	for (unsigned i = 0; i < numberOfLines; ++i)
	{
		lineangle = atan2((double)lines[i][3] - lines[i][1],
			(double)lines[i][2] - lines[i][0]);
		lineangle *= 180 / M_PI;

		if (lineangle > meanAngle - 20 && lineangle < meanAngle + 20)
			cv::line(houghOutput, cv::Point(lines[i][0], lines[i][1]),
			cv::Point(lines[i][2], lines[i][3]), cv::Scalar(255, 255, 255), 3, CV_AA);

	}
	
	whitePixelCount = std::abs((double)countNonZero(houghOutput));

	houghOutput.copyTo(destination);
}



void page::computeLineLength(vector<vector<Point>> contour){
	
	numberOfLines = contour.size();

	int maxHeight = 0;
	int largeY = 0;
	int smallY = 0;


	int yPreviousMinimum = 0;
	double averageHeight = 0;
	
	for (int i = 0; i < numberOfLines; i++){
		
		int size = contour[i].size();
		int xMax = 0;
		int xMin = 10000;
		int length = 0;
		int height = 0;

		int yMax = 0;
		int yMin = 10000;
		int whiteSpace = 0;

		
		
		for (int j = 0; j < size; j++){
			if (xMax < contour[i][j].x){
				xMax = contour[i][j].x;
			}
			if (xMin > contour[i][j].x){
				xMin = contour[i][j].x;
			}

			if (yMax < contour[i][j].y){
				yMax = contour[i][j].y;
			}
			if (yMin > contour[i][j].y){
				yMin = contour[i][j].y;
			}
		}

		length = xMax - xMin;
		height = yMax - yMin;
		averageHeight += height;

		if (maxHeight < height){
			maxHeight = height;
			largeY = yMax;
			smallY = yMin;
		}

		lineLength.push_back(length);
		lineHeight.push_back(height);

		if (i == 0){
			yPreviousMinimum = yMin;
			continue;
		}
		else{
			whiteSpace = yPreviousMinimum - yMax;
			whiteSpaceHeight.push_back(whiteSpace);
			yPreviousMinimum = yMin;
		}
	}

	averageHeight = averageHeight / numberOfLines;
	int lineCount = numberOfLines;
	for (int i = 0; i < lineCount; i++){
		if (lineHeight[i] < 0.4*averageHeight){
			lineHeight.erase(lineHeight.begin() + i);
			lineLength.erase(lineLength.begin() + i);
			lineCount--;
		}
	}

	numberOfLines = lineLength.size();

}

void page::computeContour(Mat image){

	imwrite("contourInput.jpg", image);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	for (int i = 0; i < 5; i++){
		dilate(image, image, Mat());
	}

	imwrite("dilation.jpg", image);

	Canny(image, image, 50, 200, 3);
	
	findContours(image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	imwrite("contour.jpg", image);

	computeLineLength(contours);
}

//Methods for obstacle detection

void page::deskewObstacle(Mat &destination, double angle){

	Mat copyOriginalFrame = originalFrame;

	Canny(copyOriginalFrame, copyOriginalFrame, 50, 200, 3);

	imwrite("copyCanny.jpg", copyOriginalFrame);
	imwrite("original.jpg", originalFrame);

	std::vector<cv::Point> points;
	cv::Mat_<uchar>::iterator it = copyOriginalFrame.begin<uchar>();
	cv::Mat_<uchar>::iterator end = copyOriginalFrame.end<uchar>();

	for (; it != end; ++it)
	if (*it)
		points.push_back(it.pos());

	cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));

	cv::Mat rotationMat = cv::getRotationMatrix2D(box.center, angle, 1);

	cv::Mat rotated;
	cv::warpAffine(copyOriginalFrame, rotated, rotationMat, copyOriginalFrame.size(), cv::INTER_CUBIC);

	rotated.copyTo(destination);

}

vector<Vec4i> page::computeObstacleContour(Mat image){

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Mat imageCopy;
	image.copyTo(imageCopy);

	/*for (int i = 0; i < 2; i++){
		dilate(image, image, Mat());
	}*/

	imwrite("dilation.jpg", image);

	findContours(imageCopy, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	imwrite("contour.jpg", imageCopy);

	vector<Vec4i> coordinates = findObstacle(contours);
	return coordinates;
}

void page::removeObstacleRegion(vector<Vec4i> coordinates, Mat image){

	if (coordinates.size() != 0){

		for (int i = 0; i < coordinates.size(); i++){
			Vec4i obstacleRegion = coordinates[i];
			Point p1(obstacleRegion[0], obstacleRegion[1]);
			Point p2(obstacleRegion[2], obstacleRegion[3]);

			rectangle(image, p1, p2, Scalar(0, 0, 0), -1, 8, 0);

		}

	}
	
}


vector<Vec4i> page::findObstacle(vector<vector<Point>> contour){

	numberOfLines = contour.size();

	//std::vector<cv::Vec4i> coordinates;
	vector<Vec4i> coordinates;

	double averageHeight = 0;

	for (int i = 0; i < numberOfLines; i++){

		int size = contour[i].size();
		
		int height = 0;

		int yMax = 0;
		int yMin = 10000;
		
		for (int j = 0; j < size; j++){
			
			if (yMax < contour[i][j].y){
				yMax = contour[i][j].y;
			}
			if (yMin > contour[i][j].y){
				yMin = contour[i][j].y;
			}
		}

		height = yMax - yMin;
		averageHeight += height;
			
	}

	averageHeight = averageHeight / numberOfLines;
	
	for (int i = 0; i < numberOfLines; i++){

		int size = contour[i].size();

		int height = 0;

		int yMax = 0;
		int yMin = 10000;
		int xMax = 0;
		int xMin = 10000;

		int x1 = 0;
		int x2 = 0;

		for (int j = 0; j < size; j++){

			if (yMax < contour[i][j].y){
				yMax = contour[i][j].y;
			}
			if (yMin > contour[i][j].y){
				yMin = contour[i][j].y;
				
			}

			if (xMax < contour[i][j].x){
				xMax = contour[i][j].x;
			}
			if (xMin > contour[i][j].x){
				xMin = contour[i][j].x;
			}

		}

		height = yMax - yMin;
		if (height > 10 * averageHeight){
			coordinates.push_back(Vec4i(xMax, yMax, xMin, yMin));
		}

	}
	return coordinates;
	
}

// Methods for finding online content

void Document :: findOnlineContentUrl(string ocrString){

	replace(ocrString.begin(), ocrString.end(), ' ', '+');
	

	//string cmd = "curl \"https://www.googleapis.com/customsearch/v1?key=AIzaSyB64t7bbDK_sNG4maaAtbb99aYYYEJNT5I&cx=014325120938967253327:qpojbow-kjk&q=Edwin&alt=json\" |findstr \"formattedUrl\" > onlineSource.txt";

	string curl = "curl ";
	string searchString = ocrString + "\"";
	string request = "\"https://www.googleapis.com/customsearch/v1?key=AIzaSyB64t7bbDK_sNG4maaAtbb99aYYYEJNT5I&cx=014325120938967253327:qpojbow-kjk&alt=json&q=";
	request += searchString;
	string getUrl = " |findstr \"formattedUrl\" > onlineSource.txt";

	string cmd = curl + request + getUrl;
	int i = system(cmd.c_str());


}

string Document :: readUrlFile(string fileName){

	ifstream infile(fileName);
	string line;
	string url = "";
	
	while (std::getline(infile, line))
	{
		int count = 0;
		std::istringstream iss(line);
		string a, b;

		if (!(iss >> a >> b)) {
			break;
		}
		count++;
		if (count > 0){
			url = b.substr(0, b.size() - 1);
			break;
		}
		
	}
	return url;

}

void Document :: getTextFromLink(string url, string ocrText){

	string curl = "curl ";
	string request = url;

	string filter = " |findstr ";
	string searchTerm = "\"" + ocrText + "\"";
	string fileName = " > onlineText.txt";

	string cmd = curl + request + filter + searchTerm + fileName;
	int i = system(cmd.c_str());

}

string Document :: onlineContent(string ocrString){

	findOnlineContentUrl(ocrString);
	string url = readUrlFile("onlineSource.txt");
	if (url == ""){
		return ocrString;
	}
	else{
		getTextFromLink(url, ocrString);
	}

	return ocrString;
}


