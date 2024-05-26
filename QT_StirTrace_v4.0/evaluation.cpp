#include "evaluation.h"
#include <qt4/Qt/qfile.h>
#include <qt4/Qt/qtextstream.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>

double segment_size;

evaluation::evaluation()
{
    this->EvaluationModes << "printedFP";
}

bool evaluation::setEvalMode(QString EvalMode) {
    if (this->EvaluationModes.indexOf(EvalMode)>=0) {
        this->EvaluationMode=EvalMode;
        return true;
    }
    return false;
}

QStringList evaluation::getModes() {
    return this->EvaluationModes;
}

bool MCompareVecH(const cv::Vec3f& a, const cv::Vec3f& b) {
    if (round(a[1])/segment_size!=round(b[1])/segment_size) return a[1]<b[1];
        return a[0]<b[0];
}
bool MCompareVecV(const cv::Vec3f& a, const cv::Vec3f& b) {
    if (round(a[0])/segment_size!=round(b[0])/segment_size) return a[0]<b[0];
        return a[1]<b[1];
}

void evaluation::evaluate(cv::Mat input, double scale, QString outputName) {
    int index = this->EvaluationModes.indexOf(this->EvaluationMode);
    switch (index) {
    case 0 : { //printedFP
        QFile output(outputName+".csv");
        if (output.open(QIODevice::ReadWrite)) {
            QTextStream csvoutput(&output);
            if (input.channels()==1) {
                std::vector<double> benf = benford(input);
                for (int i = 0 ; i < 9; i++) {
                    csvoutput << QString::number(benf.at(i), 'f') << ",";
                }
                std::vector<double> morph = morphologicFeatures(input,scale);
                for (int i = 0 ; i < morph.size(); i++) {
                     csvoutput << QString::number(morph.at(i), 'f') << ",";
                }
                input=subtractLeastSquarePlane(input);
                benf = benford(input);
                for (int i = 0 ; i < 9; i++) {
                     csvoutput << QString::number(benf.at(i), 'f') << ",";
                }
                morph = morphologicFeatures(input,scale);
                for (int i = 0 ; i < morph.size(); i++) {
                     csvoutput << QString::number(morph.at(i), 'f') << ",";
                }

            } else {
                cv::Mat channel[4];
                cv::split(input,channel);

                for (int chan = 0 ; chan<input.channels(); chan++) {
                    std::vector<double> benf = benford(channel[chan]);
                    for (int i = 0 ; i < 9; i++) {
                         csvoutput << QString::number(benf.at(i), 'f') << ",";
                    }
                    std::vector<double> morph = morphologicFeatures(channel[chan],scale);
                    for (int i = 0 ; i < morph.size(); i++) {
                         csvoutput << QString::number(morph.at(i), 'f') << ",";
                    }
                }

            }

            csvoutput.flush();
            output.close();
        }

        break;
    }
    default: break;
    }
}

cv::Mat evaluation::subtractLeastSquarePlane(cv::Mat input) {
     // compute regressor matrix
     // pixel x 3 matrix <- regressors
     // create resultmatrix at the same time
     int rows = input.rows * input.cols;
     int validcount = rows;

     double min, max;
     cv::Point minLoc;
     cv::Point maxLoc;
     cv::minMaxLoc( input, &min, &max, &minLoc, &maxLoc );

     cv::Mat* regressors = new cv::Mat (validcount, 3, CV_32FC1);
     cv::Mat result = cv::Mat (validcount, 1, CV_32FC1);
     cv::MatIterator_<float> it = regressors->begin<float>();
     cv::MatIterator_<float> sourceit = input.begin<float>();
     cv::MatIterator_<float> resultit = result.begin<float>();
     for (int i = 0; i < input.rows; i++) {
         for (int j = 0; j < input.cols; j++) {
                 double val = *sourceit;
                 if (val >= min && val <= max) {
                     *it = (float)i;
                     it++;
                     *it = (float)j;
                     it++;
                     *it = 1.0f;
                     *resultit = val;
                     resultit++;
                     it++;
                 }

             sourceit++;
         }
     }

     // create parameteter storage...
     // [gradient along cols, gradient along rows, constant]
     cv::Mat* params = new cv::Mat(3, 1, CV_32FC1);
     //solve the least-squres problem using opencv's embedded function
     cv::solve (*regressors, result, *params, cv::DECOMP_SVD);
     delete regressors;

     float rowg = params->at<float>(0,0); // gradient along rows
     float colg = params->at<float>(1,0); // gradient along cols
     float cong = params->at<float>(2,0); // constant
     delete params;
     // substract layer defined by params
     it = input.begin<float> ();
     for (int i = 0; i < input.rows; i++) {
         for (int j = 0; j < input.cols; j++) {
             *it = *it - (i * rowg + j * colg + cong);
             it++;
         }
     }
     return input;
}

std::vector<double> evaluation::benford(cv::Mat input) {
    std::vector<double> output;
	int zeroval=0;
    double res[10];
    for (int i = 0 ; i<10; i++) {
        res[i]=0;
    }
    char temp[100];
    for (int i=0 ; i<input.cols; i++)
        for (int j=0 ; j<input.rows; j++) {
        if ((j<input.rows)&&(i<input.cols)){
            switch (input.depth()) {
                case 0: //8bit unsigned
                        {
                            snprintf(temp, 100, "%d", input.at<quint8>(j,i));
                            break;
                            }
                case 2: //16bit unsigned
                            {
                            snprintf(temp, 100, "%d", input.at<quint16>(j,i));
                            break;
                            }
                case 4: //32bit unsigned
                            {
                            snprintf(temp, 100, "%d", input.at<quint32>(j,i));
                            break;
                            }
                case 5: //32bit float
                        {
                        snprintf(temp, 100, "%lf", input.at<float>(j,i));
                        break;
                        }
            }
					if (temp=="0") zeroval++;
					else {
	                    if (((temp[0]-'0')>0)&&((temp[0]-'0')<=9)) {
                        	res[temp[0]-'0']++;
                   		} else { //we need to find the first valid number
                    	        int k=1;
                    	        while ((temp[k]!='\0')&&(k<100)&&(((temp[k]-'0')<=0)||((temp[k]-'0')>9))) k++;
                    	        if (((temp[k]-'0')>0)&&((temp[k]-'0')<=9))res[temp[k]-'0']++;
                    	}
					}
                }
        }
    for (int i = 1 ; i < 10; i++) {
        res[i]=res[i]/(double)((input.cols*input.rows)-zeroval);
        output.push_back(res[i]);
    }


    return output;
}

std::vector<double> evaluation::morphologicFeatures(cv::Mat mat, double scale) {
    mat.convertTo(mat, CV_32FC1);
    std::vector<double> morphologicFeat;
    int kernelsize = (int)(scale);
    int maxsize = std::min(mat.rows,mat.cols);
    if ((kernelsize % 2) == 0 ) {
            maxsize--;
    }
    kernelsize = std::min(kernelsize, maxsize);
    if ((kernelsize % 2) == 0 ) {
            kernelsize++;
    }
    double sigma = 0.025 * scale;
    cv::Mat BlurTemp = cv::Mat (mat.rows, mat.cols, CV_32FC1);
    cv::GaussianBlur(mat, BlurTemp, cv::Size(kernelsize, kernelsize), sigma, sigma, cv::BORDER_DEFAULT);
    mat=mat-BlurTemp;
    cv::Mat gray;
    cv::normalize(mat, mat, 1, 255, cv::NORM_MINMAX);
    mat.convertTo(gray, CV_8UC1);
    int minradius = scale / 500;
    int bucketsize = 3*scale / 500; //bucketsize 6 micron
    int numbuckets = 15;
    std::vector<float> rpdf=std::vector<float>(numbuckets+1);
    std::vector<float> hpdf=std::vector<float>(numbuckets+1);
    std::vector<float> vpdf=std::vector<float>(numbuckets+1);
    std::vector<float> nnpdf=std::vector<float>(numbuckets+1);
    for (int i=0 ; i<=numbuckets; i++) {
        rpdf[i]=0;
        hpdf[i]=0;
        vpdf[i]=0;
        nnpdf[i]=0;
    }
    std::vector<cv::Vec3f> circles;
    cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2, 2);
    std::vector<double> cc=std::vector<double>(10);
    for (int i=1 ; i<=10; i++) {
        cc[i-1] = crystalDetect(mat, 10, 0.05*(double)i);
    }


    double thresh=0.9;
    double threshold = cv::threshold(gray, gray, (cv::mean(gray)[0]*thresh), 1, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

    double r_mean=0,h_mean=0,v_mean=0, nn_mean=0, meanfill=0;
    double r_stddev=0;
    std::vector<std::vector<cv::Point> > vec_contour;

   cv::findContours(gray, vec_contour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
   circles=std::vector<cv::Vec3f>(vec_contour.size());
   //test
   cv::Point2f centerPoint;
   float PointRadius;

   // begin circle detection

    for( int i = 0; i< vec_contour.size(); i++ )
         {
           cv::minEnclosingCircle(vec_contour[i], centerPoint, PointRadius);
           circles[i][0]=centerPoint.x;
           circles[i][1]=centerPoint.y;
           circles[i][2]=PointRadius;
         }
    // end circle detection
    if (circles.size()>0) {
        // begin Detection Property Stuff KHD+11
        std::vector<double> dists=nearestNeighborDistances(circles, mat.cols*mat.rows);
        double dist_min, dist_max, dist_span, dist_mean, dist_stddev;
        long double dist_accu=0;
        dist_min=mat.cols*mat.rows;
        dist_max=0;
        for (int i=0;i<dists.size();i++) {
            if (dists.at(i)<dist_min) dist_min=dists.at(i);
            if (dists.at(i)>dist_max) dist_max=dists.at(i);
            dist_accu+=dists.at(i);
        }
        dist_mean=dist_accu/dists.size();
        dist_accu=0;
        for (int i=0;i<dists.size();i++) {
            dist_accu+=pow((double)(dists.at(i)-dist_mean),2);
        }
        dist_stddev=sqrt(dist_accu)/dists.size();
        dist_span=dist_max-dist_min;
        std::vector<double> circleshapes=circleAnalyzer(circles, gray);
        int fillitems=0;
        for (int i=0; i<circleshapes.size(); i++) {
            if (circleshapes[i]<=1) {
                meanfill+=circleshapes[i];
                fillitems++;
            }
        }
        meanfill=meanfill/(double)fillitems;
        // end Detection Property Stuff KHD+11
        // begin Feature Extraction
        segment_size=(150/scale*500);
        double temp=0;
        for (size_t i = 0 ; i<circles.size(); i++) {
            temp+=circles[i][2]/scale*1000;
            if (round(circles[i][2])/bucketsize<=15) rpdf[round(circles[i][2])/bucketsize]+=1.0/circles.size();
                else rpdf[15]+=1.0/circles.size();
        }
        r_mean=temp/circles.size();
        temp=0;
        for (size_t i = 0 ; i<circles.size(); i++) {
            temp+=(circles[i][2]/scale*1000-r_mean)*(circles[i][2]/scale*1000-r_mean);
        }
        r_stddev=sqrt(temp/circles.size());
        //lets look for horizontal dot distances
        std::sort(circles.begin(), circles.end(),MCompareVecH);
        int validdists=0;

        bucketsize = 40*scale/500; //bucketsize 40 micron
        for (size_t i = 0 ; i<circles.size()-1; i++) {

            if ((abs(circles[i][1]-circles[i+1][1])<150*(scale/500))&&(abs(circles[i][0]-circles[i+1][0])<300*(scale/500))) {
                validdists++;
                temp+=abs(circles[i][0]-circles[i+1][0]);
                if (round(abs(circles[i][0]-circles[i+1][0])/scale*1000)/bucketsize<=15) hpdf[round(abs(circles[i][0]-circles[i+1][0])/scale*1000)/bucketsize]++;
                else hpdf[15]++;
            }
        }
        h_mean=temp/validdists;
        for (int i=0 ; i<=numbuckets; i++) hpdf[i]=hpdf[i]/validdists;
        //lets look for vertical dot distances
        std::sort(circles.begin(), circles.end(),MCompareVecV);
        validdists=0;
        temp=0;

        for (size_t i = 0 ; i<circles.size()-1; i++) {

            if ((abs(circles[i][1]-circles[i+1][1])<300*(scale/500))&&(abs(circles[i][0]-circles[i+1][0])<150*(scale/500))) {
                validdists++;
                temp+=abs(circles[i][1]-circles[i+1][1]);
                if (round(abs(circles[i][1]-circles[i+1][1])/scale*1000)/bucketsize<=15) vpdf[round(abs(circles[i][1]-circles[i+1][1])/scale*1000)/bucketsize]++;
                else vpdf[15]++;
            }
        }
        v_mean=temp/validdists;
        for (int i=0 ; i<=numbuckets; i++) vpdf[i]=vpdf[i]/validdists;
        temp=0;
        bucketsize = 20*scale/1000; //bucketsize 20 micron
        for (int i = 0 ; i<dists.size(); i++) {
            temp+=dists[i];
            if ((dists[i]/scale*1000)/bucketsize<=15) nnpdf[(dists[i]/scale*1000)/bucketsize]++;
                else nnpdf[15]++;
        }
        nn_mean=temp/dists.size();
        for (int i=0 ; i<=numbuckets; i++) nnpdf[i]=nnpdf[i]/dists.size();
    }

    for (int counter=0; counter<cc.size(); counter++) morphologicFeat.push_back(cc[counter]);
    morphologicFeat.push_back(circles.size());
    morphologicFeat.push_back(r_mean);
    morphologicFeat.push_back(r_stddev);
    morphologicFeat.push_back(h_mean);
    morphologicFeat.push_back(v_mean);
    morphologicFeat.push_back(nn_mean);
    morphologicFeat.push_back(meanfill);
    for (int counter=1; counter<rpdf.size(); counter++) morphologicFeat.push_back(rpdf[counter]);
    for (int counter=1; counter<hpdf.size(); counter++) morphologicFeat.push_back(hpdf[counter]);
    for (int counter=1; counter<vpdf.size(); counter++) morphologicFeat.push_back(vpdf[counter]);
    for (int counter=1; counter<nnpdf.size(); counter++) morphologicFeat.push_back(nnpdf[counter]);

    return morphologicFeat;

}

double evaluation::crystalDetect(cv::Mat mat, int Blocksize, double thresh){
    cv::Mat gray;
    mat.convertTo(gray, CV_8UC1);
    cv::GaussianBlur(gray, gray, cv::Size(7, 7), 2, 2);
    cv::Canny(gray, gray, 66, 33);
    double res=0;
    double temp=0;
    int num=0;
    double foo;
    double mult=1; //pow(10,-1*log10(mean));
    for (int row = 0 ; row < mat.rows; row+=Blocksize)
        for (int col = 0 ; col < mat.cols ; col+=Blocksize) {
            temp = 0;
            num = 0;
            for (int i=0 ; i<Blocksize; i++)
                for (int j=0 ; j<Blocksize; j++)
                    if ((row+j<mat.rows)&&(col+i<mat.cols)) {
                        if (gray.at<unsigned char>(row+j,col+i)>0) temp++;
                        num++;
                    }
            if (temp>((double)num)*thresh) res++;
    }
    return res/((mat.cols/Blocksize)*(mat.rows/Blocksize));
}
std::vector<double> evaluation::nearestNeighborDistances(std::vector<cv::Vec3f> circles, double maxdist) {
    std::vector<double> distances=std::vector<double>(circles.size());
    double mindist, dist;
    if (circles.size()>0) {
        for (int i = 0 ; i<circles.size(); i++) {
            mindist=maxdist;
            for (int j = 1 ; j<circles.size(); j++){
              if (i!=j) {
                  dist = sqrt(pow((double)(circles[i][0]-circles[j][0]),2)+pow((double)(circles[i][1]-circles[j][1]),2));
                  if ((dist<mindist)&&(dist>circles[i][2])&&(dist>circles[j][2])) mindist=dist;
              }
          }
        distances[i]=mindist;
        }
    }

    return distances;
}

std::vector<double> evaluation::circleAnalyzer(std::vector<cv::Vec3f> circles, cv::Mat input) {
    cv::Mat temp=input.clone();
    int area;

    cv::Rect rect;
    std::vector<double> filling=std::vector<double>(circles.size());
    if (circles.size()>0) {
        for (int i = 0 ; i<circles.size(); i++) {
            temp=input.clone();
            area=cv::floodFill(temp, cv::Point(circles[i][0],circles[i][1]), cv::Scalar(128), &rect, cv::Scalar(1), cv::Scalar(1), 4);
            filling[i]=area/(M_PI*circles[i][2]*circles[i][2]);
        }
    }

    return filling;
}

