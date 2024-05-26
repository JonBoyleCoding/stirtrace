#ifndef IMGPROC_H
#define IMGPROC_H
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <Qt/qstring.h>
class imgproc
{
private:
    QString path;
    QString NoiseLevels;
    QString MedianFilters;
    QString LineRemoval;
    QString ColumnRemoval;
    QString RotAngles;
    QString XStretchFactors;
    QString YShearFactors;
    QString ScaleFactors;
    QString CropFactors;
    QString BandingFrequency;
    QString SaltAndPepperFactor;
    QString GaussianNoiseLevels;
    QString EmbedFileName;
    QString EvalMode;
    QString PreProcessMode;
    QString ShiftParameters;
    QString TiltParameters;
    QString DouleScaleParameters;
    bool DoEmbed;
    bool eightBitConversion;
    bool withBaseline;
    cv::Mat Substrate;
    void processMat(QString filename, QString filepath);
    void evaluateFile(QString filename, QString filepath);
    void processMatCombined(QString filename, QString filepath);
    cv::Scalar getMedian(cv::Mat input);
    short SaltAndPepperHelper(int noiseLevel);
    cv::Mat convertToEightBitColorDepth(cv::Mat input);
    QString PassportScaleParameters;
    QString HaarCascade;
public:
    imgproc();
    imgproc(QString Path);
    imgproc(QString Path, QString NoiseLevels, QString MedianFilters, QString LineRemoval, QString ColumnRemoval, QString RotAngles, QString XStretchFactors, QString YShearFactors, QString ScaleFactors, QString CropFactors, QString BandingFrequency, QString SaltAndPepperFactors, QString GaussianNoise);
    imgproc(QString Path, QString NoiseLevels, QString MedianFilters, QString LineRemoval, QString ColumnRemoval, QString RotAngles, QString XStretchFactors, QString YShearFactors, QString ScaleFactors, QString CropFactors, QString BandingFrequency, QString SaltAndPepperFactors, QString GaussianNoise, QString SubstrateFile, bool eightBitConversin, QString EvaluationMode, QString PreProcessingMode, QString ShiftFactor, QString TiltParameter, bool enableBaseline, QString PassportScale, QString haarCascadeFile, QString DoubleScale);
    cv::Mat removeBlackPixels(cv::Mat input, cv::Scalar mean);
    void startProcessing();
    void startCombinedProcessing();
    //within StirTrace there is no need for making this functions public, however, it allows for reusing imgproc.
    cv::Mat additiveNoise(int noiseLevel, cv::Mat input);
    cv::Mat gaussianNoise(int noiseLevel, cv::Mat input);
    cv::Mat medianCutFiltering(int kernelSize, cv::Mat input);
    cv::Mat removeLines(int frequency, cv::Mat input);
    cv::Mat removeColumns(int frequency, cv::Mat input);
    cv::Mat rotation (double angle, cv::Mat input);
    cv::Mat Xstretching (double param, cv::Mat input);
    cv::Mat Yshearing(double param, cv::Mat input);
    cv::Mat rescale(double scalingfactor, cv::Mat input);
    cv::Mat cropping(double croppercentage, cv::Mat input);
    cv::Mat banding(int width, double param, cv::Mat input, cv::Scalar mean);
    cv::Mat SaltAndPepperNoise(int noiseLevel, cv::Mat input);
    cv::Mat embedOnSubstrate(cv::Mat trace, cv::Scalar median);
    cv::Mat shiftValues(cv::Mat trace, double shiftFactor);
    cv::Mat tilt(double gradientA, double gradientB, double constantC, cv::Mat input);
    cv::Mat passportScale(int targetWidth, int targetHeight, cv::Mat input);
    cv::Mat DoubleScale(double scalingfactor, cv::Mat input);
};

#endif // IMGPROC_H
