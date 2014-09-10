//
//  GPUProcessor.cpp
//  AutoComPasteV2
//
//  Created by Raghav Ramesh on 5/9/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "GPUProcessor.h"

class GPUProcessor : public Processor {
    void displayImageInWindow(GpuMat src) {
        imshow("result", src);
    }

    void rotateClockwise(double angle) {
        Size src_sz = src.size();
        Size dst_sz(src_sz.height, src_sz.width); 

        int len = std::max<int>(src.cols, src.rows); 
        Point2f center(len/2., len/2.);
        Mat rot_mat = cv::getRotationMatrix2D(center, angle, 1.0);
        warpAffine(src, dst, rot_mat, dst_sz);  
    }

    bool storeFramesForAveraging(GpuMat src, int size, vector<GpuMat> &Tarray) {
        GpuMat tmp; 
        src.copyTo(tmp);
        if(Tarray.size() < size)
        {
            Tarray.push_back(tmp);
            return true;
        }
        else
            return false;
    }

    void resizeImage(int width, int height) {
        if (src.size().width != width)
            resize(src, dst, cvSize(width, height), 0, 0, cv::INTER_CUBIC);
        else
            src.copyTo(dst);
    }

    void applyHoughTransform(GpuMat src, GpuMat &dst, int threshold, unsigned int *nb, int thickness, double &Fangle, double acute) {
        std::vector<cv::Vec4i> lines;
        
        vector<double> textlines;
        int against = 0;
        Fangle=0;
        //acute = 50;

        cv::HoughLinesP(src, lines, 1, CV_PI/180,thresh, src.size().width/4.f, 10);
        Mat cdst =  Mat::zeros(src.size(),CV_8UC1);;

        
        //cv::Mat disp_lines(size, CV_8UC1, cv::Scalar(0, 0, 0));
        double lineangle,angle = 0.0;
        *nb = lines.size();
        for (unsigned i = 0; i < *nb; ++i)
        {
            lineangle = atan2((double)lines[i][3] - lines[i][1],
                    (double)lines[i][2] - lines[i][0]);
            Fangle +=lineangle;
        }
        
        Fangle /=(*nb);// mean angle, in radians.
        Fangle *= 180/M_PI;

        //if(Fangle < 10||Fangle >-10)
        for (unsigned i = 0; i < *nb; ++i)
        {
            lineangle = atan2((double)lines[i][3] - lines[i][1],
                        (double)lines[i][2] - lines[i][0]);
            lineangle *= 180/M_PI;

                    if(lineangle>Fangle-acute&&lineangle<Fangle+acute)
            cv::line(cdst, cv::Point(lines[i][0], lines[i][1]),
                cv::Point(lines[i][2], lines[i][3]), cv::Scalar(255, 255 ,255),thickness,CV_AA);

        }
        cdst.copyTo(dst);
    }

    int computeSkewTextInImage(GpuMat src) {
        if(startocr == -1)
        {
            startt = omp_get_wtime(); // Returns a value in seconds of the time elapsed from some point.
            startocr = 1;
        }
        else
            startocr = 0;

        GpuMat cdstr;
        double mag=0;

        GpuMat cdst;
        bitwise_not(src, src);
        resizeimage(src,cdst,gWidth,gHeight);
        resizeimage(summage,summage,gWidth,gHeight);
        
    //  Canny(src,src, 40, 120,3);
    //  cv::bitwise_not(cdst, cdst);
        angle = 90;
        //collect good lines
        if(!imagesum(cdst,nPasses,sumofgarray))
        {
            
            while(!sumofgarray.empty())
            {
                bitwise_or(sumofgarray.back(),summage,summage);
                sumofarray.pop_back();
            }
            sumofarray.clear();
            //compare good lines
            if(!imagesum(gsummage,2,difgarray))
            {
                GpuMat a,b,c,error,or,and,xor;
                double werror=0,white;

                difgarray.back().copyTo(c);
                difgarray.back().copyTo(b);
                difgarray.pop_back();


                difgarray.back().copyTo(a);
                
                
            
                bitwise_and(a,c,or);
                bitwise_xor(a,c,error);
                imshow("a",a);

                white = countNonZero(or);
                werror = countNonZero(error);
                imshow("error",error);
                /*
            
                cv::Mat output1 = cv::Mat::zeros(c.size(), CV_8UC3);
                cv::Mat output2 = cv::Mat::zeros(c.size(), CV_8UC3);
                std::vector < std::vector<cv::Point2i > > bloba, blobc;

                
                
                cv::threshold(a, a, 0.0, 1.0, cv::THRESH_BINARY);
                cv::threshold(c, c, 0.0, 1.0, cv::THRESH_BINARY);

                FindBlobs(a,bloba);
                FindBlobs(c,blobc);

                unsigned char red = 255 * (rand()/(1.0 + RAND_MAX));
                unsigned char green = 255 * (rand()/(1.0 + RAND_MAX));
                unsigned char blue = 255 * (rand()/(1.0 + RAND_MAX));


                if(!bloba.empty())
                {
                    std::sort(bloba.begin(),bloba.end(),[](vector<Point2i > a,vector<Point2i > b){ return (a.size()<b.size());});
                    std::sort(blobc.begin(),blobc.end(),[](vector<Point2i > a,vector<Point2i > b){ return (a.size()<b.size());});

                    for(size_t k=0; k < bloba.back().size(); k++) {
                        
                        int x = bloba.back()[k].x;
                        int y = bloba.back()[k].y;

                        output1.at<cv::Vec3b>(y,x)[0] = blue;
                        output1.at<cv::Vec3b>(y,x)[1] = green;
                        output1.at<cv::Vec3b>(y,x)[2] = red;
                        
                    }
                    for(size_t j=0; j < blobc.back().size(); j++) {

                        int x = blobc.back()[j].x;
                        int y = blobc.back()[j].y;

                        output2.at<cv::Vec3b>(y,x)[0] = blue;
                        output2.at<cv::Vec3b>(y,x)[1] = green;
                        output2.at<cv::Vec3b>(y,x)[2] = red;
                    
                    }
                
            //      cout<<matchShapes(a,c,CV_CONTOURS_MATCH_I2,0)<<endl;
                    //cout<<matchShapes(bloba.back(),blobc.back(),CV_CONTOURS_MATCH_I2,0)<<endl;
                    imshow("bloba",output1);
                    imshow("blobb",output2);
                }
                */
                //cout<<"\b";

                cout<<"error\t"<< werror/white*100<<"%      "<<(omp_get_wtime()-startt)<<"\n";
                if(werror/white>=0.40&&!movement)
                {
                    movement = true;

                }
                else if(werror/white<0.40&&!movement)
                {
                    cout<<" no error :\n";//<< howwhite(error)*100<<"%      "<<(omp_get_wtime()-startt)*1000<<"ms";
                    difgarray.pop_back();
                    difgarray.push_back(b);
                    
                    
                }
                else if(werror/white<0.40&&movement)
                {
                    cout<<" no error :\n";//<< howwhite(error)*100<<"%      "<<(omp_get_wtime()-startt)*1000<<"ms";
                    difgarray.pop_back();
                    difgarray.push_back(b);
                    movement=false;
                    
                }
                else
                {
                    double error;
                    if (std::abs(countNonZero(a))>std::abs(countNonZero(c)))
                        error = std::abs((double)countNonZero(c))/std::abs((double)countNonZero(a));
                    else
                        error = std::abs((double)countNonZero(a))/std::abs((double)countNonZero(c));
                    
                    movement=false;
                    cout<<"\nerror white diff :"<<1-error<<"\n";
                    
                    while(!difgarray.empty())
                        difgarray.clear();
                    while(!sumofarray.empty())
                        sumofgarray.clear();
            

                    if(1-error>0.15)
                    startocr = 1;
                }

                
                startt = omp_get_wtime();
            }
            //a or b - a and b


        //  rotate(summage,cdstr,90);
        //  imshow("ahhsahd",cdstr);
            
            //reset the sum
            
                //gsummage = GpuMat::zeros(summage.size(),CV_8UC1);
            
        }



        bitwise_not(src, src);
        
        return startocr;
    }
};
