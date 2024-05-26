#include "imgproc.h"
#include "evaluation.h"
#include "filehandler.h"
#include "indicators.hpp"
#include "preprocessing.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

imgproc::imgproc() {}

imgproc::imgproc(QString Path) {
    this->path = Path;
}

imgproc::imgproc(QString Path, QString NoiseLevels, QString MedianFilters, QString LineRemoval, QString ColumnRemoval,
                 QString RotAngles, QString XStretchFactors, QString YShearFactors, QString ScaleFactors,
                 QString CropFactors, QString BandingFrequency, QString SaltAndPepper, QString GaussianNoise,
                 QString SubstrateFile, bool eightBitConversion, QString EvaluationMode, QString PreProcessingMode,
                 QString ShiftFactor, QString TiltParameter, bool enableBaseline, QString PassportScale,
                 QString HaarCascadeFile, QString DoubleScale) {
    this->path = Path;
    this->NoiseLevels = NoiseLevels;
    this->MedianFilters = MedianFilters;
    this->LineRemoval = LineRemoval;
    this->ColumnRemoval = ColumnRemoval;
    this->RotAngles = RotAngles;
    this->XStretchFactors = XStretchFactors;
    this->YShearFactors = YShearFactors;
    this->ScaleFactors = ScaleFactors;
    this->CropFactors = CropFactors;
    this->BandingFrequency = BandingFrequency;
    this->SaltAndPepperFactor = SaltAndPepper;
    this->GaussianNoiseLevels = GaussianNoise;
    if (SubstrateFile.length() > 0) {
        fileHandler substrateData = fileHandler();
        this->Substrate = substrateData.openFile(SubstrateFile);
        this->DoEmbed = true;
        this->EmbedFileName = SubstrateFile;
        this->EmbedFileName.truncate(SubstrateFile.length() - 4);
    } else {
        this->Substrate = cv::Mat();
        this->DoEmbed = false;
        this->EmbedFileName = "";
    }
    this->eightBitConversion = eightBitConversion;
    this->EvalMode = EvaluationMode;
    this->PreProcessMode = PreProcessingMode;
    this->ShiftParameters = ShiftFactor;
    this->TiltParameters = TiltParameter;
    this->withBaseline = enableBaseline;
    this->PassportScaleParameters = PassportScale;
    this->DouleScaleParameters = DoubleScale;
    this->HaarCascade = HaarCascadeFile;
}

cv::Mat imgproc::removeBlackPixels(Mat input, Scalar mean) {
    if (input.channels() == 3) {
        Mat3b temp = input;
        for (Mat3b::iterator it = temp.begin(); it != temp.end(); it++)
            if (*it == Vec3b(0, 0, 0))
                *it = Vec3b(mean[0], mean[1], mean[2]);
    }
    if (input.channels() == 1) {
        int bitdepth = input.depth();
        switch (bitdepth) {
        case 0: {
            Mat1b temp1 = input;
            for (Mat1b::iterator it = temp1.begin(); it != temp1.end(); it++)
                if (*it == 0)
                    *it = mean[0];
            break;
        }
        case 2: {
            Mat1w temp2 = input;
            for (Mat1w::iterator it = temp2.begin(); it != temp2.end(); it++)
                if (*it == 0)
                    *it = mean[0];
            break;
        }
        case 4: {
            Mat1i temp3 = input;
            for (Mat1i::iterator it = temp3.begin(); it != temp3.end(); it++)
                if (*it == 0)
                    *it = mean[0];
            break;
        }
        }
    }
    return input;
}

cv::Scalar imgproc::getMedian(Mat input) {
    cv::Scalar median = cv::Scalar(4);
    int bitdepth = input.depth();
    int i = 0;
    if (input.channels() == 1) {
        vector<double> values = vector<double>(input.rows * input.cols);
        switch (bitdepth) {
        case 0: // 8bit unsigned
        {
            Mat1b temp1 = input;
            for (Mat1b::iterator it = temp1.begin(); it != temp1.end(); it++)
                values[i++] = *it;
            break;
        }
        case 2: // 16bit unsigned
        {
            Mat1w temp2 = input;
            for (Mat1w::iterator it = temp2.begin(); it != temp2.end(); it++)
                values[i++] = *it;
            break;
        }
        case 4: // 32bit unsigned
        {
            Mat1i temp3 = input;
            for (Mat1i::iterator it = temp3.begin(); it != temp3.end(); it++)
                values[i++] = *it;
            break;
        }
        case 5: // 32bit float
        {
            Mat1f temp4 = input;
            for (Mat1f::iterator it = temp4.begin(); it != temp4.end(); it++)
                values[i++] = *it;
            break;
        }
        }

        std::sort(values.begin(), values.end());
        median[0] = values[(input.rows * input.cols) / 2];
    } else if (input.channels() == 3) {
        vector<double> values1 = vector<double>(input.rows * input.cols);
        vector<double> values2 = vector<double>(input.rows * input.cols);
        vector<double> values3 = vector<double>(input.rows * input.cols);

        switch (bitdepth) {
        case 0: // 8bit unsigned
        {
            Mat3b temp = input;
            for (Mat3b::iterator it = temp.begin(); it != temp.end(); it++) {
                Vec3b pixel = *it;
                values1[i] = pixel[0];
                values2[i] = pixel[1];
                values3[i] = pixel[2];
                i++;
            }
            break;
        }
        case 2: // 16bit unsigned
        {
            Mat3w temp2 = input;
            for (Mat3w::iterator it = temp2.begin(); it != temp2.end(); it++) {
                Vec3w pixel = *it;
                values1[i] = pixel[0];
                values2[i] = pixel[1];
                values3[i] = pixel[2];
                i++;
            }
            break;
        }
        case 4: // 32bit unsigned
        {
            Mat3i temp3 = input;
            for (Mat3i::iterator it = temp3.begin(); it != temp3.end(); it++) {
                Vec3i pixel = *it;
                values1[i] = pixel[0];
                values2[i] = pixel[1];
                values3[i] = pixel[2];
                i++;
            }
            break;
        }
        case 5: // 32bit float
        {
            Mat3f temp4 = input;
            for (Mat3f::iterator it = temp4.begin(); it != temp4.end(); it++) {
                Vec3f pixel = *it;
                values1[i] = pixel[0];
                values2[i] = pixel[1];
                values3[i] = pixel[2];
                i++;
            }
            break;
        }
        }
        std::sort(values1.begin(), values1.end());
        std::sort(values2.begin(), values2.end());
        std::sort(values3.begin(), values3.end());
        median[0] = values1[(input.rows * input.cols) / 2];
        median[1] = values2[(input.rows * input.cols) / 2];
        median[2] = values3[(input.rows * input.cols) / 2];
    }
    return median;
}
void imgproc::evaluateFile(QString filename, QString filepath) {
    fileHandler FileHandler = fileHandler();
    evaluation eval = evaluation();
    bool ok = eval.setEvalMode(this->EvalMode);
    if (!ok)
        return;
    ok = FileHandler.openFile(filepath + "/" + filename);
    if (!ok)
        return;
    double scale = FileHandler.getScale();
    QStringList imageTypes = FileHandler.getImageTypes();
    for (int index = 0; index < imageTypes.count(); index++) {
        cv::Mat input = FileHandler.getImage(imageTypes.at(index));
        if (input.rows == 0)
            return;
        QString outputPath = "/output";
        if (this->eightBitConversion) {
            input = convertToEightBitColorDepth(input);
            outputPath = outputPath + "8bpp";
        }
        // determine median
        cv::Scalar median = getMedian(input);

        QStringList foo = filename.split(".");
        QString fileext = foo[foo.length() - 1];
        QString baseName = filename;
        baseName.truncate(filename.length() - (fileext.length() + 1));
        baseName = baseName + "-" + imageTypes.at(index);

        // baseline performance
        if (withBaseline) {
            QDir dir = QDir(filepath);
            dir.mkpath(filepath + outputPath + "/baseline/");
            QString outputname = filepath + outputPath + "/baseline/" + baseName;
            eval.evaluate(input, scale, outputname);
        }

        // additive noise
        if (this->NoiseLevels.length() > 0) {
            QList<int> additiveNoiseLevels;
            QStringList noiseInput = this->NoiseLevels.split(",");
            for (int i = 0; i < noiseInput.length(); i++)
                additiveNoiseLevels << noiseInput[i].toInt();
            int noiseLevel;
            for (int i = 0; i < additiveNoiseLevels.count(); i++) {
                noiseLevel = (int)additiveNoiseLevels[i];
                cv::Mat temp = additiveNoise(noiseLevel, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/additiveNoise/" + QString::number(noiseLevel) + "/");
                QString outputname = filepath + outputPath + "/additiveNoise/" + QString::number(noiseLevel) + "/" +
                                     baseName + "_additiveNoise_" + QString::number(noiseLevel);
                eval.evaluate(temp, scale, outputname);
            }
        }

        // additive gaussian noise
        if (this->GaussianNoiseLevels.length() > 0) {
            QList<int> additiveGaussianNoiseLevels;
            QStringList GaussianNoiseInput = this->GaussianNoiseLevels.split(",");
            for (int i = 0; i < GaussianNoiseInput.length(); i++)
                additiveGaussianNoiseLevels << GaussianNoiseInput[i].toInt();
            int GaussianNoiseLevel;
            for (int i = 0; i < additiveGaussianNoiseLevels.count(); i++) {
                GaussianNoiseLevel = (int)additiveGaussianNoiseLevels[i];
                cv::Mat temp = gaussianNoise(GaussianNoiseLevel, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/additiveGaussianNoise/" + QString::number(GaussianNoiseLevel) +
                           "/");
                QString outputname = filepath + outputPath + "/additiveGaussianNoise/" +
                                     QString::number(GaussianNoiseLevel) + "/" + baseName + "_additiveGaussianNoise_" +
                                     QString::number(GaussianNoiseLevel);
                eval.evaluate(temp, scale, outputname);
            }
        }

        // median cut
        if (this->MedianFilters.length() > 0) {
            QList<int> medianKernels;
            QStringList medianInput = this->MedianFilters.split(",");
            for (int i = 0; i < medianInput.length(); i++)
                medianKernels << medianInput[i].toInt();
            int medianKernel;
            for (int i = 0; i < medianKernels.count(); i++) {
                medianKernel = (int)medianKernels[i];
                cv::Mat temp = medianCutFiltering(medianKernel, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/medianCut/" + QString::number(medianKernel) + "/");
                QString outputname = filepath + outputPath + "/medianCut/" + QString::number(medianKernel) + "/" +
                                     baseName + "_medianCut_" + QString::number(medianKernel);
                eval.evaluate(temp, scale, outputname);
            }
        }

        // delRow
        if (this->LineRemoval.length() > 0) {
            QList<int> delRows;
            QStringList drowInput = this->LineRemoval.split(",");
            for (int i = 0; i < drowInput.length(); i++)
                delRows << drowInput[i].toInt();
            int delRow;
            for (int i = 0; i < delRows.count(); i++) {
                delRow = (int)delRows[i];
                cv::Mat temp = removeLines(delRow, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/removeLines/" + QString::number(delRow) + "/");
                QString outputname = filepath + outputPath + "/removeLines/" + QString::number(delRow) + "/" +
                                     baseName + "_removeLines_" + QString::number(delRow);
                eval.evaluate(temp, scale, outputname);
            }
        }

        // delCol
        if (this->ColumnRemoval.length() > 0) {
            QList<int> delCols;
            QStringList dcolInput = this->ColumnRemoval.split(",");
            for (int i = 0; i < dcolInput.length(); i++)
                delCols << dcolInput[i].toInt();
            int delCol;
            for (int i = 0; i < delCols.count(); i++) {
                delCol = (int)delCols[i];
                cv::Mat temp = removeColumns(delCol, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/removeColumns/" + QString::number(delCol) + "/");
                QString outputname = filepath + outputPath + "/removeColumns/" + QString::number(delCol) + "/" +
                                     baseName + "_removeColumns_" + QString::number(delCol);
                eval.evaluate(temp, scale, outputname);
            }
        }

        // rotation
        if (this->RotAngles.length() > 0) {
            QList<double> rotAngles;
            QStringList rotInput = this->RotAngles.split(",");
            for (int i = 0; i < rotInput.length(); i++)
                rotAngles << rotInput[i].toDouble();
            double rotAngle;
            for (int i = 0; i < rotAngles.count(); i++) {
                rotAngle = (double)rotAngles[i];
                cv::Mat temp = rotation(rotAngle, input.clone());
                temp = removeBlackPixels(temp, median);
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/rotation/" + QString::number(rotAngle) + "/");
                QString outputname = filepath + outputPath + "/rotation/" + QString::number(rotAngle) + "/" + baseName +
                                     "_rotation_" + QString::number(rotAngle);
                eval.evaluate(temp, scale, outputname);
            }
        }

        // xstretching
        if (this->XStretchFactors.length() > 0) {
            QList<double> xStretchFactors;
            QStringList xstretchInput = this->XStretchFactors.split(",");
            for (int i = 0; i < xstretchInput.length(); i++)
                xStretchFactors << xstretchInput[i].toDouble();
            double xStretchFactor;
            for (int i = 0; i < xStretchFactors.count(); i++) {
                xStretchFactor = (double)xStretchFactors[i];
                cv::Mat temp = Xstretching(xStretchFactor, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/XStretching/" + QString::number(xStretchFactor) + "/");
                QString outputname = filepath + outputPath + "/XStretching/" + QString::number(xStretchFactor) + "/" +
                                     baseName + "_xStretch_" + QString::number(xStretchFactor);
                eval.evaluate(temp, scale, outputname);
            }
        }

        // yshearing
        if (this->YShearFactors.length() > 0) {
            QList<double> yShearFactors;
            QStringList yshearInput = this->YShearFactors.split(",");
            for (int i = 0; i < yshearInput.length(); i++)
                yShearFactors << yshearInput[i].toDouble();
            double yShearFactor;
            for (int i = 0; i < yShearFactors.count(); i++) {
                yShearFactor = (double)yShearFactors[i];
                cv::Mat temp = Yshearing(yShearFactor, input.clone());
                temp = removeBlackPixels(temp, median);
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/yShearing/" + QString::number(yShearFactor) + "/");
                QString outputname = filepath + outputPath + "/yShearing/" + QString::number(yShearFactor) + "/" +
                                     baseName + "_yShear_" + QString::number(yShearFactor);
                eval.evaluate(temp, scale, outputname);
            }
        }

        // scaling
        if (this->ScaleFactors.length() > 0) {
            QList<double> scaleFactors;
            QStringList scaleInput = this->ScaleFactors.split(",");
            for (int i = 0; i < scaleInput.length(); i++)
                scaleFactors << scaleInput[i].toDouble();
            double scaleFactor;
            for (int i = 0; i < scaleFactors.count(); i++) {
                scaleFactor = (double)scaleFactors[i];
                cv::Mat temp = rescale(scaleFactor, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/rescale/" + QString::number(scaleFactor) + "/");
                QString outputname = filepath + outputPath + "/rescale/" + QString::number(scaleFactor) + "/" +
                                     baseName + "_rescale_" + QString::number(scaleFactor);
                eval.evaluate(temp, scale, outputname);
            }
        }

        // cropping
        if (this->CropFactors.length() > 0) {
            QList<double> cropFactors;
            QStringList cropInput = this->CropFactors.split(",");
            for (int i = 0; i < cropInput.length(); i++)
                cropFactors << cropInput[i].toDouble();
            double cropFactor;
            for (int i = 0; i < cropFactors.count(); i++) {
                cropFactor = (double)cropFactors[i];
                cv::Mat temp = cropping(cropFactor, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/cropping/" + QString::number(cropFactor) + "/");
                QString outputname = filepath + outputPath + "/cropping/" + QString::number(cropFactor) + "/" +
                                     baseName + "_cropping_" + QString::number(cropFactor);
                eval.evaluate(temp, scale, outputname);
                ;
            }
        }

        // banding
        if (this->BandingFrequency.length() > 0) {
            QList<double> bandingFactors;
            QStringList bandInput = this->BandingFrequency.split(",");
            for (int i = 0; i < bandInput.length(); i++)
                bandingFactors << bandInput[i].toDouble();
            double bandingFactor;
            for (int i = 0; i < bandingFactors.count(); i++) {
                bandingFactor = (double)bandingFactors[i];
                cv::Mat temp = banding(50, bandingFactor, input.clone(), median);
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/banding/" + QString::number(bandingFactor) + "/");
                QString outputname = filepath + outputPath + "/banding/" + QString::number(bandingFactor) + "/" +
                                     baseName + "_banding_" + QString::number(bandingFactor);
                eval.evaluate(temp, scale, outputname);
            }
        }
        // Salt and Pepper noise
        if (this->SaltAndPepperFactor.length() > 0) {
            QList<int> SaltAndPepperFactors;
            QStringList SaltAndPepperInput = this->SaltAndPepperFactor.split(",");
            for (int i = 0; i < SaltAndPepperInput.length(); i++)
                SaltAndPepperFactors << SaltAndPepperInput[i].toInt();
            double saltAndPepperFactor;
            for (int i = 0; i < SaltAndPepperFactors.count(); i++) {
                saltAndPepperFactor = (int)SaltAndPepperFactors[i];
                cv::Mat temp = SaltAndPepperNoise(saltAndPepperFactor, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/SaltAndPepper/" + QString::number(saltAndPepperFactor) + "/");
                QString outputname = filepath + outputPath + "/SaltAndPepper/" + QString::number(saltAndPepperFactor) +
                                     "/" + baseName + "_SaltAndPepper_" + QString::number(saltAndPepperFactor);
                eval.evaluate(temp, scale, outputname);
            }
        }

        // Embedding
        if (this->DoEmbed) {
            cv::Mat temp = embedOnSubstrate(input.clone(), median);
            QDir dir = QDir(filepath);
            dir.mkpath(filepath + outputPath + "/embed/" + this->EmbedFileName + "/");
            QString outputname = filepath + outputPath + "/embed/" + this->EmbedFileName + "/" + baseName + "_embed";
            eval.evaluate(temp, scale, outputname);
        }
        // plain 8bpp
        if (this->eightBitConversion) {
            QDir dir = QDir(filepath);
            dir.mkpath(filepath + outputPath + "/plain/");
            QString outputname = filepath + outputPath + "/plain/" + baseName + "_8bpp";
            eval.evaluate(input, scale, outputname);
        }

        // value shift
        if (this->ShiftParameters.length() > 0) {
            QList<double> ShiftFactors;
            QStringList ShiftInput = this->ShiftParameters.split(",");
            for (int i = 0; i < ShiftInput.length(); i++)
                ShiftFactors << ShiftInput[i].toDouble();
            double shiftFactor;
            for (int i = 0; i < ShiftFactors.count(); i++) {
                shiftFactor = (double)ShiftFactors[i];
                cv::Mat temp = shiftValues(input.clone(), shiftFactor);
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/shift/" + QString::number(shiftFactor) + "/");
                QString outputname = filepath + outputPath + "/shift/" + QString::number(shiftFactor) + "/" + baseName +
                                     "_shift_" + QString::number(shiftFactor);
                eval.evaluate(temp, scale, outputname);
            }
        }

        // tilt simulation
        if (this->TiltParameters.length() > 0) {
            QStringList TiltInput = this->TiltParameters.split(",");
            for (int i = 0; i < TiltInput.length(); i++) {
                QStringList TiltParam = TiltInput[i].split("#");
                double A = TiltParam[0].toDouble();
                double B = TiltParam[1].toDouble();
                double C = TiltParam[2].toDouble();
                cv::Mat temp = tilt(A, B, C, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/tilt/(" + QString::number(A) + "," + QString::number(B) + "," +
                           QString::number(C) + ")/");
                QString outputname = filepath + outputPath + "/tilt/(" + QString::number(A) + "," + QString::number(B) +
                                     "," + QString::number(C) + ")/" + baseName + "_tilt_(" + QString::number(A) + "," +
                                     QString::number(B) + "," + QString::number(C) + ")";
                eval.evaluate(temp, scale, outputname);
            }
        }

        // DoubleScaling
        if (this->DouleScaleParameters.length() > 0) {
            QList<double> DoubleScaleFactors;
            QStringList DoubleScaleInput = this->DouleScaleParameters.split(",");
            for (int i = 0; i < DoubleScaleInput.length(); i++)
                DoubleScaleFactors << DoubleScaleInput[i].toDouble();
            double DoubleScaleFactor;
            for (int i = 0; i < DoubleScaleFactors.count(); i++) {
                DoubleScaleFactor = (double)DoubleScaleFactors[i];
                cv::Mat temp = DoubleScale(DoubleScaleFactor, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/DoubleScale/" + QString::number(DoubleScaleFactor) + "/");
                QString outputname = filepath + outputPath + "/DoubleScale/" + QString::number(DoubleScaleFactor) +
                                     "/" + baseName + "_DoubleScale_" + QString::number(DoubleScaleFactor);
                eval.evaluate(temp, scale, outputname);
            }
        }

        // Passport Scaling
        if (this->PassportScaleParameters.length() > 0) {
            QList<int> PassportScaleParameterWidth;
            QList<int> PassportScaleParameterHeight;
            QStringList PassportScaleInputParam = this->PassportScaleParameters.split(",");
            for (int i = 0; i < PassportScaleInputParam.length(); i++) {
                QStringList PassportScaleParamParsed = PassportScaleInputParam[i].split("x");
                if (PassportScaleParamParsed.length() == 2) {
                    PassportScaleParameterWidth << PassportScaleParamParsed[0].toInt();
                    PassportScaleParameterHeight << PassportScaleParamParsed[1].toInt();
                }
            }
            int PSwidth, PSheight;
            for (int i = 0; i < PassportScaleParameterWidth.count(); i++) {
                PSwidth = (int)PassportScaleParameterWidth[i];
                PSheight = (int)PassportScaleParameterHeight[i];
                cv::Mat temp = passportScale(PSwidth, PSheight, input.clone());
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/PassportScale/" + QString::number(PSwidth) + "x" +
                           QString::number(PSheight) + "/");
                QString outputname = filepath + outputPath + "/PassportScale/" + QString::number(PSwidth) + "x" +
                                     QString::number(PSheight) + "/" + baseName + "_PassportScale_" +
                                     QString::number(PSwidth) + "x" + QString::number(PSheight);
                eval.evaluate(temp, scale, outputname);
            }
        }
    }
}

void imgproc::processMat(QString filename, QString filepath) {
    fileHandler FileHandler = fileHandler();
    bool ok = FileHandler.openFile(filepath + "/" + filename);
    QStringList imageTypes = FileHandler.getImageTypes();
    for (int imageType = 0; imageType < imageTypes.length(); imageType++) {
        cv::Mat input = FileHandler.getImage(imageTypes.at(imageType));
        if (input.rows == 0)
            return;
        QString outputPath = "/output";
        if (this->eightBitConversion) {
            input = convertToEightBitColorDepth(input);
            outputPath = outputPath + "8bpp";
        }
        // determine median
        cv::Scalar median = getMedian(input);

        QStringList foo = filename.split(".");
        QString fileext = foo[foo.length() - 1];
        QString baseName = filename;
        baseName.truncate(filename.length() - (fileext.length() + 1));
        baseName = baseName + "-" + imageTypes.at(imageType);

        // additive noise
        if (this->NoiseLevels.length() > 0) {
            QList<int> additiveNoiseLevels;
            QStringList noiseInput = this->NoiseLevels.split(",");
            for (int i = 0; i < noiseInput.length(); i++)
                additiveNoiseLevels << noiseInput[i].toInt();
            int noiseLevel;
            for (int i = 0; i < additiveNoiseLevels.count(); i++) {
                noiseLevel = (int)additiveNoiseLevels[i];
                cv::Mat temp = additiveNoise(noiseLevel, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/additiveNoise/" + QString::number(noiseLevel) + "/");
                QString outputname = filepath + outputPath + "/additiveNoise/" + QString::number(noiseLevel) + "/" +
                                     baseName + "_additiveNoise_" + QString::number(noiseLevel);
                FileHandler.saveFile(outputname, temp);
            }
        }

        // additive gaussian noise
        if (this->GaussianNoiseLevels.length() > 0) {
            QList<int> additiveGaussianNoiseLevels;
            QStringList GaussianNoiseInput = this->GaussianNoiseLevels.split(",");
            for (int i = 0; i < GaussianNoiseInput.length(); i++)
                additiveGaussianNoiseLevels << GaussianNoiseInput[i].toInt();
            int GaussianNoiseLevel;
            for (int i = 0; i < additiveGaussianNoiseLevels.count(); i++) {
                GaussianNoiseLevel = (int)additiveGaussianNoiseLevels[i];
                cv::Mat temp = gaussianNoise(GaussianNoiseLevel, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/additiveGaussianNoise/" + QString::number(GaussianNoiseLevel) +
                           "/");
                QString outputname = filepath + outputPath + "/additiveGaussianNoise/" +
                                     QString::number(GaussianNoiseLevel) + "/" + baseName + "_additiveGaussianNoise_" +
                                     QString::number(GaussianNoiseLevel);
                FileHandler.saveFile(outputname, temp);
            }
        }

        // median cut
        if (this->MedianFilters.length() > 0) {
            QList<int> medianKernels;
            QStringList medianInput = this->MedianFilters.split(",");
            for (int i = 0; i < medianInput.length(); i++)
                medianKernels << medianInput[i].toInt();
            int medianKernel;
            for (int i = 0; i < medianKernels.count(); i++) {
                medianKernel = (int)medianKernels[i];
                cv::Mat temp = medianCutFiltering(medianKernel, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/medianCut/" + QString::number(medianKernel) + "/");
                QString outputname = filepath + outputPath + "/medianCut/" + QString::number(medianKernel) + "/" +
                                     baseName + "_medianCut_" + QString::number(medianKernel);
                FileHandler.saveFile(outputname, temp);
            }
        }

        // delRow
        if (this->LineRemoval.length() > 0) {
            QList<int> delRows;
            QStringList drowInput = this->LineRemoval.split(",");
            for (int i = 0; i < drowInput.length(); i++)
                delRows << drowInput[i].toInt();
            int delRow;
            for (int i = 0; i < delRows.count(); i++) {
                delRow = (int)delRows[i];
                cv::Mat temp = removeLines(delRow, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/removeLines/" + QString::number(delRow) + "/");
                QString outputname = filepath + outputPath + "/removeLines/" + QString::number(delRow) + "/" +
                                     baseName + "_removeLines_" + QString::number(delRow);
                FileHandler.saveFile(outputname, temp);
            }
        }

        // delCol
        if (this->ColumnRemoval.length() > 0) {
            QList<int> delCols;
            QStringList dcolInput = this->ColumnRemoval.split(",");
            for (int i = 0; i < dcolInput.length(); i++)
                delCols << dcolInput[i].toInt();
            int delCol;
            for (int i = 0; i < delCols.count(); i++) {
                delCol = (int)delCols[i];
                cv::Mat temp = removeColumns(delCol, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/removeColumns/" + QString::number(delCol) + "/");
                QString outputname = filepath + outputPath + "/removeColumns/" + QString::number(delCol) + "/" +
                                     baseName + "_removeColumns_" + QString::number(delCol);
                FileHandler.saveFile(outputname, temp);
            }
        }

        // rotation
        if (this->RotAngles.length() > 0) {
            QList<double> rotAngles;
            QStringList rotInput = this->RotAngles.split(",");
            for (int i = 0; i < rotInput.length(); i++)
                rotAngles << rotInput[i].toDouble();
            double rotAngle;
            for (int i = 0; i < rotAngles.count(); i++) {
                rotAngle = (double)rotAngles[i];
                cv::Mat temp = rotation(rotAngle, input.clone());
                temp = removeBlackPixels(temp, median);
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/rotation/" + QString::number(rotAngle) + "/");
                QString outputname = filepath + outputPath + "/rotation/" + QString::number(rotAngle) + "/" + baseName +
                                     "_rotation_" + QString::number(rotAngle);
                FileHandler.saveFile(outputname, temp);
            }
        }

        // xstretching
        if (this->XStretchFactors.length() > 0) {
            QList<double> xStretchFactors;
            QStringList xstretchInput = this->XStretchFactors.split(",");
            for (int i = 0; i < xstretchInput.length(); i++)
                xStretchFactors << xstretchInput[i].toDouble();
            double xStretchFactor;
            for (int i = 0; i < xStretchFactors.count(); i++) {
                xStretchFactor = (double)xStretchFactors[i];
                cv::Mat temp = Xstretching(xStretchFactor, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/XStretching/" + QString::number(xStretchFactor) + "/");
                QString outputname = filepath + outputPath + "/XStretching/" + QString::number(xStretchFactor) + "/" +
                                     baseName + "_xStretch_" + QString::number(xStretchFactor);
                FileHandler.saveFile(outputname, temp);
            }
        }

        // yshearing
        if (this->YShearFactors.length() > 0) {
            QList<double> yShearFactors;
            QStringList yshearInput = this->YShearFactors.split(",");
            for (int i = 0; i < yshearInput.length(); i++)
                yShearFactors << yshearInput[i].toDouble();
            double yShearFactor;
            for (int i = 0; i < yShearFactors.count(); i++) {
                yShearFactor = (double)yShearFactors[i];
                cv::Mat temp = Yshearing(yShearFactor, input.clone());
                temp = removeBlackPixels(temp, median);
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/yShearing/" + QString::number(yShearFactor) + "/");
                QString outputname = filepath + outputPath + "/yShearing/" + QString::number(yShearFactor) + "/" +
                                     baseName + "_yShear_" + QString::number(yShearFactor);
                FileHandler.saveFile(outputname, temp);
            }
        }

        // scaling
        if (this->ScaleFactors.length() > 0) {
            QList<double> scaleFactors;
            QStringList scaleInput = this->ScaleFactors.split(",");
            for (int i = 0; i < scaleInput.length(); i++)
                scaleFactors << scaleInput[i].toDouble();
            double scaleFactor;
            for (int i = 0; i < scaleFactors.count(); i++) {
                scaleFactor = (double)scaleFactors[i];
                cv::Mat temp = rescale(scaleFactor, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/rescale/" + QString::number(scaleFactor) + "/");
                QString outputname = filepath + outputPath + "/rescale/" + QString::number(scaleFactor) + "/" +
                                     baseName + "_rescale_" + QString::number(scaleFactor);
                FileHandler.saveFile(outputname, temp);
            }
        }

        // cropping
        if (this->CropFactors.length() > 0) {
            QList<double> cropFactors;
            QStringList cropInput = this->CropFactors.split(",");
            for (int i = 0; i < cropInput.length(); i++)
                cropFactors << cropInput[i].toDouble();
            double cropFactor;
            for (int i = 0; i < cropFactors.count(); i++) {
                cropFactor = (double)cropFactors[i];
                cv::Mat temp = cropping(cropFactor, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/cropping/" + QString::number(cropFactor) + "/");
                QString outputname = filepath + outputPath + "/cropping/" + QString::number(cropFactor) + "/" +
                                     baseName + "_cropping_" + QString::number(cropFactor);
                FileHandler.saveFile(outputname, temp);
            }
        }

        // banding
        if (this->BandingFrequency.length() > 0) {
            QList<double> bandingFactors;
            QStringList bandInput = this->BandingFrequency.split(",");
            for (int i = 0; i < bandInput.length(); i++)
                bandingFactors << bandInput[i].toDouble();
            double bandingFactor;
            for (int i = 0; i < bandingFactors.count(); i++) {
                bandingFactor = (double)bandingFactors[i];
                cv::Mat temp = banding(50, bandingFactor, input.clone(), median);
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/banding/" + QString::number(bandingFactor) + "/");
                QString outputname = filepath + outputPath + "/banding/" + QString::number(bandingFactor) + "/" +
                                     baseName + "_banding_" + QString::number(bandingFactor);
                FileHandler.saveFile(outputname, temp);
            }
        }
        // Salt and Pepper noise
        if (this->SaltAndPepperFactor.length() > 0) {
            QList<int> SaltAndPepperFactors;
            QStringList SaltAndPepperInput = this->SaltAndPepperFactor.split(",");
            for (int i = 0; i < SaltAndPepperInput.length(); i++)
                SaltAndPepperFactors << SaltAndPepperInput[i].toInt();
            double saltAndPepperFactor;
            for (int i = 0; i < SaltAndPepperFactors.count(); i++) {
                saltAndPepperFactor = (int)SaltAndPepperFactors[i];
                cv::Mat temp = SaltAndPepperNoise(saltAndPepperFactor, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/SaltAndPepper/" + QString::number(saltAndPepperFactor) + "/");
                QString outputname = filepath + outputPath + "/SaltAndPepper/" + QString::number(saltAndPepperFactor) +
                                     "/" + baseName + "_SaltAndPepper_" + QString::number(saltAndPepperFactor);
                FileHandler.saveFile(outputname, temp);
            }
        }

        // Embedding
        if (this->DoEmbed) {
            cv::Mat temp = embedOnSubstrate(input.clone(), median);
            QDir dir = QDir(filepath);
            dir.mkpath(filepath + outputPath + "/embed/" + this->EmbedFileName + "/");
            QString outputname = filepath + outputPath + "/embed/" + this->EmbedFileName + "/" + baseName + "_embed";
            FileHandler.saveFile(outputname, temp);
        }
        // plain 8bpp
        if (this->eightBitConversion) {
            QDir dir = QDir(filepath);
            dir.mkpath(filepath + outputPath + "/plain/");
            QString outputname = filepath + outputPath + "/plain/" + baseName + "_8bpp";
            FileHandler.saveFile(outputname, input);
        }

        // value shift
        if (this->ShiftParameters.length() > 0) {
            QList<double> ShiftFactors;
            QStringList ShiftInput = this->ShiftParameters.split(",");
            for (int i = 0; i < ShiftInput.length(); i++)
                ShiftFactors << ShiftInput[i].toDouble();
            double shiftFactor;
            for (int i = 0; i < ShiftFactors.count(); i++) {
                shiftFactor = (double)ShiftFactors[i];
                cv::Mat temp = shiftValues(input.clone(), shiftFactor);
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/shift/" + QString::number(shiftFactor) + "/");
                QString outputname = filepath + outputPath + "/shift/" + QString::number(shiftFactor) + "/" + baseName +
                                     "_shift_" + QString::number(shiftFactor);
                FileHandler.saveFile(outputname, temp);
            }
        }
        // tilt simulation
        if (this->TiltParameters.length() > 0) {
            QStringList TiltInput = this->TiltParameters.split(",");
            for (int i = 0; i < TiltInput.length(); i++) {
                QStringList TiltParam = TiltInput[i].split("#");
                double A = TiltParam[0].toDouble();
                double B = TiltParam[1].toDouble();
                double C = TiltParam[2].toDouble();
                cv::Mat temp = tilt(A, B, C, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/tilt/(" + QString::number(A) + "," + QString::number(B) + "," +
                           QString::number(C) + ")/");
                QString outputname = filepath + outputPath + "/tilt/(" + QString::number(A) + "," + QString::number(B) +
                                     "," + QString::number(C) + ")/" + baseName + "_tilt_(" + QString::number(A) + "," +
                                     QString::number(B) + "," + QString::number(C) + ")";
                FileHandler.saveFile(outputname, temp);
            }
        }
        // DoubleScaling
        if (this->DouleScaleParameters.length() > 0) {
            QList<double> DoubleScaleFactors;
            QStringList DoubleScaleInput = this->DouleScaleParameters.split(",");
            for (int i = 0; i < DoubleScaleInput.length(); i++)
                DoubleScaleFactors << DoubleScaleInput[i].toDouble();
            double DoubleScaleFactor;
            for (int i = 0; i < DoubleScaleFactors.count(); i++) {
                DoubleScaleFactor = (double)DoubleScaleFactors[i];
                cv::Mat temp = DoubleScale(DoubleScaleFactor, input.clone());
                if (DoEmbed)
                    temp = embedOnSubstrate(temp.clone(), median);
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/DoubleScale/" + QString::number(DoubleScaleFactor) + "/");
                QString outputname = filepath + outputPath + "/DoubleScale/" + QString::number(DoubleScaleFactor) +
                                     "/" + baseName + "_DoubleScale_" + QString::number(DoubleScaleFactor);
                FileHandler.saveFile(outputname, temp);
            }
        }

        // Passport Scaling
        if (this->PassportScaleParameters.length() > 0) {
            QList<int> PassportScaleParameterWidth;
            QList<int> PassportScaleParameterHeight;
            QStringList PassportScaleInputParam = this->PassportScaleParameters.split(",");
            for (int i = 0; i < PassportScaleInputParam.length(); i++) {
                QStringList PassportScaleParamParsed = PassportScaleInputParam[i].split("x");
                if (PassportScaleParamParsed.length() == 2) {
                    PassportScaleParameterWidth << PassportScaleParamParsed[0].toInt();
                    PassportScaleParameterHeight << PassportScaleParamParsed[1].toInt();
                }
            }
            int PSwidth, PSheight;
            for (int i = 0; i < PassportScaleParameterWidth.count(); i++) {
                PSwidth = (int)PassportScaleParameterWidth[i];
                PSheight = (int)PassportScaleParameterHeight[i];
                cv::Mat temp = passportScale(PSwidth, PSheight, input.clone());
                QDir dir = QDir(filepath);
                dir.mkpath(filepath + outputPath + "/PassportScale/" + QString::number(PSwidth) + "x" +
                           QString::number(PSheight) + "/");
                QString outputname = filepath + outputPath + "/PassportScale/" + QString::number(PSwidth) + "x" +
                                     QString::number(PSheight) + "/" + baseName + "_PassportScale_" +
                                     QString::number(PSwidth) + "x" + QString::number(PSheight);
                FileHandler.saveFile(outputname, temp);
            }
        }
    }
}

void imgproc::startProcessing() {
    QDir dir = QDir(path);
    QStringList files = dir.entryList(QDir::Files, QDir::Name);

    using namespace indicators;

    indicators::ProgressBar bar{option::Start{" ["},
                                option::Fill{"█"},
                                option::Lead{"█"},
                                option::Remainder{"-"},
                                option::End{"]"},
                                option::PrefixText{"Processing Images:"},
                                option::MaxProgress(files.count()),
                                option::ShowPercentage{true},
                                option::BarWidth{10},
                                option::ForegroundColor{Color::yellow},
                                option::ShowElapsedTime{true},
                                option::ShowRemainingTime{true},
                                option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}};

    show_console_cursor(false);

    if (this->EvalMode == "") {
        for (int i = 0; i < files.count(); i++) {
            processMat(files[i], path);
            bar.tick();
        }
    } else {
        for (int i = 0; i < files.count(); i++) {
            evaluateFile(files[i], path);
            bar.tick();
        }
    }

    bar.mark_as_completed();
    show_console_cursor(true);
}

void imgproc::processMatCombined(QString filename, QString filepath) {
    fileHandler FileHandler = fileHandler();
    bool ok = FileHandler.openFile(filepath + "/" + filename);
    QStringList imageTypes = FileHandler.getImageTypes();
    cv::Mat input = FileHandler.getImage(imageTypes.at(0));
    if (input.rows == 0)
        return;
    QString outputPath = "/output";
    if (this->eightBitConversion) {
        input = convertToEightBitColorDepth(input);
        outputPath = outputPath + "8bpp";
    }
    // determine median
    cv::Scalar median = getMedian(input);

    QStringList foo = filename.split(".");
    QString fileext = foo[foo.length() - 1];
    QString baseName = filename;
    baseName.truncate(filename.length() - (fileext.length() + 1));

    QDir dir = QDir(filepath);
    dir.mkpath(filepath + outputPath + "/combined/");

    // additive noise
    QList<int> additiveNoiseLevels;
    if (this->NoiseLevels.length() > 0) {
        QStringList noiseInput = this->NoiseLevels.split(",");
        for (int i = 0; i < noiseInput.length(); i++)
            additiveNoiseLevels << noiseInput[i].toInt();
    } else
        additiveNoiseLevels << 0;

    // additive gaussian noise
    QList<int> additiveGaussianNoiseLevels;
    if (this->GaussianNoiseLevels.length() > 0) {
        QStringList GaussianNoiseInput = this->GaussianNoiseLevels.split(",");
        for (int i = 0; i < GaussianNoiseInput.length(); i++)
            additiveGaussianNoiseLevels << GaussianNoiseInput[i].toInt();
    } else
        additiveGaussianNoiseLevels << 0;

    // median cut
    QList<int> medianKernels;
    if (this->MedianFilters.length() > 0) {
        QStringList medianInput = this->MedianFilters.split(",");
        for (int i = 0; i < medianInput.length(); i++)
            medianKernels << medianInput[i].toInt();
    } else
        medianKernels << 0;

    // delRow
    QList<int> delRows;
    if (this->LineRemoval.length() > 0) {
        QStringList drowInput = this->LineRemoval.split(",");
        for (int i = 0; i < drowInput.length(); i++)
            delRows << drowInput[i].toInt();
    } else
        delRows << 0;

    // delCol
    QList<int> delCols;
    if (this->ColumnRemoval.length() > 0) {
        QStringList dcolInput = this->ColumnRemoval.split(",");
        for (int i = 0; i < dcolInput.length(); i++)
            delCols << dcolInput[i].toInt();
    } else
        delCols << 0;

    // rotation
    QList<double> rotAngles;
    if (this->RotAngles.length() > 0) {
        QStringList rotInput = this->RotAngles.split(",");
        for (int i = 0; i < rotInput.length(); i++)
            rotAngles << rotInput[i].toDouble();
    } else
        rotAngles << 0;

    // xstretching
    QList<double> xStretchFactors;
    if (this->XStretchFactors.length() > 0) {
        QStringList xstretchInput = this->XStretchFactors.split(",");
        for (int i = 0; i < xstretchInput.length(); i++)
            xStretchFactors << xstretchInput[i].toDouble();
    } else
        xStretchFactors << 0;

    // yshearing
    QList<double> yShearFactors;
    if (this->YShearFactors.length() > 0) {
        QStringList yshearInput = this->YShearFactors.split(",");
        for (int i = 0; i < yshearInput.length(); i++)
            yShearFactors << yshearInput[i].toDouble();
    } else
        yShearFactors << 0;

    // scaling
    QList<double> scaleFactors;
    if (this->ScaleFactors.length() > 0) {
        QStringList scaleInput = this->ScaleFactors.split(",");
        for (int i = 0; i < scaleInput.length(); i++)
            scaleFactors << scaleInput[i].toDouble();
    } else
        scaleFactors << 0;

    // cropping
    QList<double> cropFactors;
    if (this->CropFactors.length() > 0) {
        QStringList cropInput = this->CropFactors.split(",");
        for (int i = 0; i < cropInput.length(); i++)
            cropFactors << cropInput[i].toDouble();
    } else
        cropFactors << 0;

    // banding
    QList<double> bandingFactors;
    if (this->BandingFrequency.length() > 0) {
        QStringList bandInput = this->BandingFrequency.split(",");
        for (int i = 0; i < bandInput.length(); i++)
            bandingFactors << bandInput[i].toDouble();
    } else
        bandingFactors << 0;

    // Salt and Pepper noise
    QList<int> SaltAndPepperFactors;
    if (this->SaltAndPepperFactor.length() > 0) {
        QStringList SaltAndPepperInput = this->SaltAndPepperFactor.split(",");
        for (int i = 0; i < SaltAndPepperInput.length(); i++)
            SaltAndPepperFactors << SaltAndPepperInput[i].toInt();
    } else
        SaltAndPepperFactors << 0;

    // value shift
    QList<double> ShiftFactors;
    if (this->ShiftParameters.length() > 0) {
        QStringList ShiftInput = this->ShiftParameters.split(",");
        for (int i = 0; i < ShiftInput.length(); i++)
            ShiftFactors << ShiftInput[i].toDouble();

    } else
        ShiftFactors << 0;

    // tilt simulation
    QList<Vec3d> TiltFactors;
    if (this->TiltParameters.length() > 0) {
        QStringList TiltInput = this->TiltParameters.split(",");
        for (int i = 0; i < TiltInput.length(); i++) {
            QStringList TiltParam = TiltInput[i].split("#");
            double A = TiltParam[0].toDouble();
            double B = TiltParam[1].toDouble();
            double C = TiltParam[2].toDouble();
            Vec3d param = Vec3d(A, B, C);
            TiltFactors << param;
        }
    } else
        TiltFactors << Vec3d(0, 0, 0);

    // DoubleScaling
    QList<double> DoubleScaleFactors;
    if (this->DouleScaleParameters.length() > 0) {
        QStringList DoubleScaleInput = this->DouleScaleParameters.split(",");
        for (int i = 0; i < DoubleScaleInput.length(); i++)
            DoubleScaleFactors << DoubleScaleInput[i].toDouble();
    } else
        DoubleScaleFactors << 0;

    // Passport Scaling
    QList<Vec2i> PassportScaleFactors;
    if (this->PassportScaleParameters.length() > 0) {
        QStringList PassportScaleInputParam = this->PassportScaleParameters.split(",");
        for (int i = 0; i < PassportScaleInputParam.length(); i++) {
            QStringList PassportScaleParamParsed = PassportScaleInputParam[i].split("x");
            if (PassportScaleParamParsed.length() == 2) {
                Vec2i param = Vec2i(PassportScaleParamParsed[0].toInt(), PassportScaleParamParsed[1].toInt());
                PassportScaleFactors << param;
            }
        }
    } else
        PassportScaleFactors << Vec2i(0, 0);

    // done enumerating parameters
    // cv::Mat temp;
    QString nameTag;
    for (int addnoise = 0; addnoise < additiveNoiseLevels.length(); addnoise++) {
        cv::Mat temp = additiveNoise((int)additiveNoiseLevels[addnoise], input.clone());
        nameTag = "an_" + QString::number((int)additiveNoiseLevels[addnoise]);
        for (int addgaussiannoise = 0; addgaussiannoise < additiveGaussianNoiseLevels.length(); addgaussiannoise++) {
            cv::Mat tempG = gaussianNoise((int)additiveGaussianNoiseLevels[addgaussiannoise], temp.clone());
            QString nameTagG = nameTag + "_agn_" + QString::number((int)additiveGaussianNoiseLevels[addgaussiannoise]);
            for (int mediansize = 0; mediansize < medianKernels.length(); mediansize++) {
                cv::Mat tempM = medianCutFiltering((int)medianKernels[mediansize], tempG.clone());
                QString nameTagM = nameTagG + "_med_" + QString::number((int)medianKernels[mediansize]);
                for (int delRowC = 0; delRowC < delRows.length(); delRowC++) {
                    cv::Mat tempL = removeLines((int)delRows[delRowC], tempM.clone());
                    QString nameTagL = nameTagM + "_drw_" + QString::number((int)delRows[delRowC]);
                    for (int delColC = 0; delColC < delCols.length(); delColC++) {
                        cv::Mat tempC = removeColumns((int)delCols[delColC], tempL.clone());
                        QString nameTagC = nameTagL + "_dcl_" + QString::number((int)delCols[delColC]);
                        for (int xstr = 0; xstr < xStretchFactors.length(); xstr++) {
                            cv::Mat tempX = Xstretching((double)xStretchFactors[xstr], tempC.clone());
                            QString nameTagX = nameTagC + "_xstr_" + QString::number((int)xStretchFactors[xstr]);
                            for (int yshr = 0; yshr < yShearFactors.length(); yshr++) {
                                cv::Mat tempY = Yshearing((double)yShearFactors[yshr], tempX.clone());
                                tempY = removeBlackPixels(tempY, median);
                                QString nameTagY = nameTagX + "_yshr_" + QString::number((int)yShearFactors[yshr]);
                                for (int scle = 0; scle < scaleFactors.length(); scle++) {
                                    cv::Mat tempS = rescale((double)scaleFactors[scle], tempY.clone());
                                    QString nameTagS = nameTagY + "_scle_" + QString::number((int)scaleFactors[scle]);
                                    for (int crp = 0; crp < cropFactors.length(); crp++) {
                                        cv::Mat tempR = cropping((double)cropFactors[crp], tempS.clone());
                                        QString nameTagR = nameTagS + "_crp_" + QString::number((int)cropFactors[crp]);
                                        for (int bnd = 0; bnd < bandingFactors.length(); bnd++) {
                                            cv::Mat tempB =
                                                banding(50, (double)bandingFactors[bnd], tempR.clone(), median);
                                            QString nameTagB =
                                                nameTagR + "_bnd_" + QString::number((int)bandingFactors[bnd]);
                                            if (DoEmbed)
                                                tempB = embedOnSubstrate(tempB.clone(), median);
                                            for (int snp = 0; snp < SaltAndPepperFactors.length(); snp++) {
                                                cv::Mat tempP =
                                                    SaltAndPepperNoise((int)SaltAndPepperFactors[snp], tempB.clone());
                                                QString nameTagP = nameTagB + "_snp_" +
                                                                   QString::number((int)SaltAndPepperFactors[snp]);
                                                for (int rotA = 0; rotA < rotAngles.length(); rotA++) {
                                                    cv::Mat tempO = rotation((double)rotAngles[rotA], tempP.clone());
                                                    QString nameTagRot =
                                                        nameTagP + "_rot_" + QString::number((int)rotAngles[rotA]);
                                                    for (int shiftV = 0; shiftV < ShiftFactors.length(); shiftV++) {
                                                        cv::Mat tempSH =
                                                            shiftValues(tempO.clone(), ShiftFactors[shiftV]);
                                                        QString nameTagSH = nameTagRot + "_shift_" +
                                                                            QString::number((int)ShiftFactors[shiftV]);
                                                        for (int tiltV = 0; tiltV < TiltFactors.length(); tiltV++) {
                                                            cv::Mat tempTLT =
                                                                tilt(TiltFactors[tiltV][0], TiltFactors[tiltV][1],
                                                                     TiltFactors[tiltV][2], tempSH.clone());
                                                            QString nameTagTLT =
                                                                nameTagSH + "_tilt_" +
                                                                QString::number((int)TiltFactors[tiltV][0]) + "#" +
                                                                QString::number((int)TiltFactors[tiltV][1]) + "#" +
                                                                QString::number((int)TiltFactors[tiltV][2]);
                                                            for (int dscaleI = 0; dscaleI < DoubleScaleFactors.length();
                                                                 dscaleI++) {
                                                                cv::Mat tempDS = DoubleScale(
                                                                    DoubleScaleFactors[dscaleI], tempTLT.clone());
                                                                QString nameTagDS =
                                                                    nameTagTLT + "_doublescale_" +
                                                                    QString::number((int)DoubleScaleFactors[dscaleI]);
                                                                for (int PPSV = 0; PPSV < PassportScaleFactors.length();
                                                                     PPSV++) {
                                                                    cv::Mat tempPPS = passportScale(
                                                                        PassportScaleFactors[PPSV][0],
                                                                        PassportScaleFactors[PPSV][1], tempDS.clone());
                                                                    QString nameTagPPS =
                                                                        nameTagDS + "_passportScale_" +
                                                                        QString::number(
                                                                            (int)PassportScaleFactors[PPSV][0]) +
                                                                        "x" +
                                                                        QString::number(
                                                                            (int)PassportScaleFactors[PPSV][1]);

                                                                    QString FinalNameTag = nameTagPPS;
                                                                    cv::Mat finalMat = tempPPS;

                                                                    if (DoEmbed)
                                                                        FinalNameTag += "_" + this->EmbedFileName;
                                                                    dir.mkpath(filepath + outputPath + "/combined/" +
                                                                               FinalNameTag + "/");
                                                                    QString outputname = filepath + outputPath +
                                                                                         "/combined/" + FinalNameTag +
                                                                                         "/" + baseName + "_" +
                                                                                         FinalNameTag;
                                                                    FileHandler.saveFile(outputname, finalMat);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void imgproc::startCombinedProcessing() {
    QDir dir = QDir(path);
    QStringList files = dir.entryList(QDir::Files, QDir::Name);

    using namespace indicators;

    indicators::ProgressBar bar{option::Start{" ["},
                                option::Fill{"█"},
                                option::Lead{"█"},
                                option::Remainder{"-"},
                                option::End{"]"},
                                option::PrefixText{"Processing Images:"},
                                option::MaxProgress(files.count()),
                                option::ShowPercentage{true},
                                option::BarWidth{10},
                                option::ForegroundColor{Color::yellow},
                                option::ShowElapsedTime{true},
                                option::ShowRemainingTime{true},
                                option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}};

    show_console_cursor(false);

    for (int i = 0; i < files.count(); i++) {
        processMatCombined(files[i], path);
        bar.tick();
    }

    bar.mark_as_completed();
    show_console_cursor(true);
}

// white noise generation

cv::Mat imgproc::additiveNoise(int noiseLevel, cv::Mat input) {
    if (noiseLevel == 0)
        return input;
    try {
        cv::Mat noisepattern = cv::Mat(input.size(), CV_32FC1);

        double min = 0; //(-512.0*noiseLevel)/100.0;
        double max = 0; //(512.0*noiseLevel)/100.0;
        if (input.channels() == 1) {
            cv::Point minidx, maxidx;
            // determine the actual value range
            cv::minMaxLoc(input, &min, &max, &minidx, &maxidx);

            min = -1 * (max - min) / 50.0 * noiseLevel;
            max = (max - min) / 50.0 * noiseLevel;
        } else {
            min = (-512.0 * noiseLevel) / 100.0;
            max = (512.0 * noiseLevel) / 100.0;
        }

        double span = max - min;

        // generate noise pattern
        cv::randu(noisepattern, min, max); // uniform noise
        /*for (int i=0; i<noisepattern.rows; i++)
            for (int j=0; j<noisepattern.cols;j++) {
                noisepattern.at<float>(i,j)=(drand48()*span)+min;
            }
*/
        // transform noise matrix to target type

        if (input.channels() == 1) {
            noisepattern.convertTo(noisepattern, input.type());
        } else if (input.channels() == 3) {
            cv::Mat temp = cv::Mat(input.size(), input.type());
            cv::cvtColor(noisepattern, temp, CV_GRAY2RGB);
            temp.convertTo(noisepattern, input.type());
        }
        // cv::add(input,noisepattern, input);
        if ((noisepattern.type() == input.type()) && (noisepattern.size() == input.size())) {
            input = input + noisepattern;
        } else {
            if (noisepattern.type() != input.type()) {
                qDebug() << "type missmatch" << Qt::endl;
            }
            if (noisepattern.size() != input.size()) {
                qDebug() << "size missmatch" << Qt::endl;
            }
        }
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}

// gaussian noise generation

cv::Mat imgproc::gaussianNoise(int noiseLevel, cv::Mat input) {
    if (noiseLevel == 0)
        return input;
    try {
        cv::Mat noisepattern = cv::Mat(input.size(), CV_32FC1);

        double min = 0; //(-512.0*noiseLevel)/100.0;
        double max = 0; //(512.0*noiseLevel)/100.0;
        if (input.channels() == 1) {
            cv::Point minidx, maxidx;
            // determine the actual value range
            cv::minMaxLoc(input, &min, &max, &minidx, &maxidx);

            min = -1 * (max - min) / 100.0 * noiseLevel;
            max = (max - min) / 100.0 * noiseLevel;
        } else {
            min = (-512.0 * noiseLevel) / 100.0;
            max = (512.0 * noiseLevel) / 100.0;
        }

        double span = max - min;

        // generate noise pattern
        cv::randn(noisepattern, 0, span);

        // transform noise matrix to target type

        if (input.channels() == 1) {
            noisepattern.convertTo(noisepattern, input.type());
        } else if (input.channels() == 3) {
            cv::Mat temp = cv::Mat(input.size(), input.type());
            cv::cvtColor(noisepattern, temp, CV_GRAY2RGB);
            temp.convertTo(noisepattern, input.type());
        }
        // cv::add(input,noisepattern, input);
        if ((noisepattern.type() == input.type()) && (noisepattern.size() == input.size())) {
            input = input + noisepattern;
        } else {
            if (noisepattern.type() != input.type()) {
                qDebug() << "type missmatch" << Qt::endl;
            }
            if (noisepattern.size() != input.size()) {
                qDebug() << "size missmatch" << Qt::endl;
            }
        }
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}

cv::Mat imgproc::medianCutFiltering(int kernelSize, cv::Mat input) {
    if (kernelSize == 0)
        return input;
    try {
        if (kernelSize % 2 == 0)
            throw 5;
        if (input.depth() == 0)
            cv::medianBlur(input, input, kernelSize);
        else {
            if (kernelSize <= 5)
                cv::medianBlur(input, input, kernelSize);
            else
                throw 6;
        }
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}

cv::Mat imgproc::removeLines(int frequency, cv::Mat input) {
    if (frequency == 0)
        return input;
    try {
        // removes a line at frequency steps
        int linesToDelete = input.rows / frequency;
        cv::Mat output = Mat(input.rows - linesToDelete, input.cols, input.type());
        int counter = -1;
        for (int i = 0; i < output.rows; i++) {
            if (i % frequency == 0)
                counter++;
            output.row(i) = input.row(i + counter) * 1.0;
        }

        input = output;
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}

cv::Mat imgproc::removeColumns(int frequency, cv::Mat input) {
    if (frequency == 0)
        return input;
    // removes a column at frequency steps
    try {
        cv::Mat temp;
        cv::transpose(input, temp);
        temp = removeLines(frequency, temp);
        cv::transpose(temp, temp);
        input = temp;
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}

cv::Mat imgproc::rotation(double angle, cv::Mat input) {
    if (angle == 0)
        return input;
    try {
        Point2f center = Point2f(input.cols / 2, input.rows / 2);
        cv::Mat rotA = cv::getRotationMatrix2D(
            center, angle, 1.0); // getRotationMatrix2D(rectA.center,angle_correction*(rectA.angle-rectB.angle), 1.0);
        cv::Mat output;
        warpAffine(input, output, rotA, input.size());
        input = output;
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}

cv::Mat imgproc::Xstretching(double param, cv::Mat input) {
    if (param == 0)
        return input;
    try {
        cv::Mat transform = Mat(2, 3, CV_32FC1);
        transform.at<float>(0, 1) = 0;     // b
        transform.at<float>(0, 2) = 0;     // e
        transform.at<float>(1, 0) = 0;     // c
        transform.at<float>(1, 1) = 1;     // d
        transform.at<float>(1, 2) = 0;     // f
        transform.at<float>(0, 0) = param; // a
        cv::Mat output = cv::Mat(input.rows, input.cols * param, input.type());
        warpAffine(input, output, transform, output.size());
        input = output;
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}

cv::Mat imgproc::Yshearing(double param, cv::Mat input) {
    if (param == 0)
        return input;
    try {
        cv::Mat transform = Mat(2, 3, CV_32FC1);
        transform.at<float>(0, 1) = 0;     // b
        transform.at<float>(0, 2) = 0;     // e
        transform.at<float>(1, 0) = param; // c
        transform.at<float>(1, 1) = 1;     // d
        transform.at<float>(1, 2) = 0;     // f
        transform.at<float>(0, 0) = 1;     // a
        cv::Mat output = cv::Mat::ones(input.rows * (1.0 + param), input.cols, input.type());
        warpAffine(input, output, transform, output.size());
        input = output;
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}

cv::Mat imgproc::rescale(double scalingfactor, cv::Mat input) {
    if (scalingfactor == 0)
        return input;
    try {
        cv::Mat output = Mat(input.rows * (scalingfactor / 100.0), input.cols * (scalingfactor / 100.0), input.type());
        cv::resize(input, output, output.size(), 0, 0, INTER_LANCZOS4);
        input = output;
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}

cv::Mat imgproc::cropping(double croppercentage, cv::Mat input) {
    if (croppercentage == 0)
        return input;
    try {
        int nwidth = input.cols * (croppercentage / 100.0);
        int nheight = input.rows * (croppercentage / 100.0);
        cv::Mat roi = Mat(input, cv::Rect((input.cols - nwidth) / 2, (input.rows - nheight) / 2, nwidth, nheight));
        Mat temp = roi.clone();
        input = temp;
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}

cv::Mat imgproc::banding(int width, double param, cv::Mat input, cv::Scalar mean) {
    if (param == 0)
        return input;
    try {
        for (int i = 0; i < input.rows; i++) {
            // if ((drand48())>=1.0-(param)) {
            if ((((double)rand()) / (double)RAND_MAX) >= 1.0 - (param)) {
                Mat bandingArtifact;
                if (i + width < input.rows)
                    bandingArtifact = Mat(input, Rect(0, i, input.cols, width));
                else
                    bandingArtifact = Mat(input, Rect(0, i, input.cols, input.rows - i));
                bandingArtifact.setTo(mean);
                i += width;
            }
        }
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}

/* Helper function for embedding decision
 * retruns 0 for no embedding, -1 embedding of min, 1 for embedding of max
 * The noiseLevel describes the percentage [0...100] of embedded pixels
 */

short imgproc::SaltAndPepperHelper(int noiseLevel) {
    double randVal = ((double)rand()) / (double)RAND_MAX; // get uniformly distributed random number between 0.0 and 1.0
    randVal -= 0.5;
    double threshold = ((double)noiseLevel) / 200.0;
    if (fabs(randVal) < 0.5 - threshold)
        return 0;
    else {
        if (randVal < 0)
            return -1;
        if (randVal > 0)
            return 1;
    }
    return 0;
}

cv::Mat imgproc::SaltAndPepperNoise(int noiseLevel, Mat input) {
    if (noiseLevel == 0)
        return input;
    try {

        // generate noise pattern

        short helperRes;

        int bitdepth = input.depth();
        if (input.channels() == 1) {
            double min = 0; //(-512.0*noiseLevel)/100.0;
            double max = 0; //(512.0*noiseLevel)/100.0;
            cv::Point minidx, maxidx;
            // determine the actual value range for an embedding within this range
            cv::minMaxLoc(input, &min, &max, &minidx, &maxidx);
            switch (bitdepth) {
            case 0: // 8bit unsigned
            {
                Mat1b temp1 = input;
                Mat1b::iterator it;
                for (it = temp1.begin(); it != temp1.end(); it++) {
                    helperRes = SaltAndPepperHelper(noiseLevel);
                    if (helperRes != 0) {
                        if (helperRes < 0)
                            *it = (quint8)min;
                        if (helperRes > 0)
                            *it = (quint8)max;
                    }
                }
                break;
            }
            case 2: // 16bit unsigned
            {
                Mat1w temp2 = input;
                Mat1w::iterator it;
                for (it = temp2.begin(); it != temp2.end(); it++) {
                    helperRes = SaltAndPepperHelper(noiseLevel);
                    if (helperRes != 0) {
                        if (helperRes < 0)
                            *it = (quint16)min;
                        if (helperRes > 0)
                            *it = (quint16)max;
                    }
                }
                break;
            }
            case 4: // 32bit unsigned
            {
                Mat1i temp3 = input;
                Mat1i::iterator it;
                for (it = temp3.begin(); it != temp3.end(); it++) {
                    helperRes = SaltAndPepperHelper(noiseLevel);
                    if (helperRes != 0) {
                        if (helperRes < 0)
                            *it = (quint32)min;
                        if (helperRes > 0)
                            *it = (quint32)max;
                    }
                }
                break;
            }
            case 5: // 32bit float
            {
                Mat1f temp4 = input;
                Mat1f::iterator it;
                for (it = temp4.begin(); it != temp4.end(); it++) {
                    helperRes = SaltAndPepperHelper(noiseLevel);
                    if (helperRes != 0) {
                        if (helperRes < 0)
                            *it = (float)min;
                        if (helperRes > 0)
                            *it = (float)max;
                    }
                }
                break;
            }
            }

        } else if (input.channels() == 3) {

            // "standard" salt and pepper noise within the 24 bit rgb space
            double min = 0;
            double max = 255;

            switch (bitdepth) {
            case 0: // 8bit unsigned
            {
                Mat3b temp = input;
                for (Mat3b::iterator it = temp.begin(); it != temp.end(); it++) {
                    Vec3b minpix;
                    Vec3b maxpix;
                    minpix[0] = (quint8)min;
                    minpix[1] = (quint8)min;
                    minpix[2] = (quint8)min;
                    maxpix[0] = (quint8)max;
                    maxpix[1] = (quint8)max;
                    maxpix[2] = (quint8)max;
                    helperRes = SaltAndPepperHelper(noiseLevel);
                    if (helperRes != 0) {
                        if (helperRes < 0) {
                            *it = minpix;
                        }
                        if (helperRes > 0) {
                            *it = maxpix;
                        }
                    }
                }
                break;
            }
            case 2: // 16bit unsigned
            {
                Mat3w temp2 = input;
                for (Mat3w::iterator it = temp2.begin(); it != temp2.end(); it++) {
                    Vec3w minpix;
                    Vec3w maxpix;
                    minpix[0] = (quint16)min;
                    minpix[1] = (quint16)min;
                    minpix[2] = (quint16)min;
                    maxpix[0] = (quint16)max;
                    maxpix[1] = (quint16)max;
                    maxpix[2] = (quint16)max;
                    helperRes = SaltAndPepperHelper(noiseLevel);
                    if (helperRes != 0) {
                        if (helperRes < 0) {
                            *it = minpix;
                        }
                        if (helperRes > 0) {
                            *it = maxpix;
                        }
                    }
                }
                break;
            }
            case 4: // 32bit unsigned
            {
                Mat3i temp3 = input;
                for (Mat3i::iterator it = temp3.begin(); it != temp3.end(); it++) {
                    Vec3i minpix;
                    Vec3i maxpix;
                    minpix[0] = (quint32)min;
                    minpix[1] = (quint32)min;
                    minpix[2] = (quint32)min;
                    maxpix[0] = (quint32)max;
                    maxpix[1] = (quint32)max;
                    maxpix[2] = (quint32)max;
                    helperRes = SaltAndPepperHelper(noiseLevel);
                    if (helperRes != 0) {
                        if (helperRes < 0) {
                            *it = minpix;
                        }
                        if (helperRes > 0) {
                            *it = maxpix;
                        }
                    }
                }
                break;
            }
            case 5: // 32bit float
            {
                Mat3f temp4 = input;
                for (Mat3f::iterator it = temp4.begin(); it != temp4.end(); it++) {
                    Vec3f minpix;
                    Vec3f maxpix;
                    minpix[0] = (float)min;
                    minpix[1] = (float)min;
                    minpix[2] = (float)min;
                    maxpix[0] = (float)max;
                    maxpix[1] = (float)max;
                    maxpix[2] = (float)max;
                    helperRes = SaltAndPepperHelper(noiseLevel);
                    if (helperRes != 0) {
                        if (helperRes < 0) {
                            *it = minpix;
                        }
                        if (helperRes > 0) {
                            *it = maxpix;
                        }
                    }
                }
                break;
            }
            }
        }
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}

cv::Mat imgproc::convertToEightBitColorDepth(cv::Mat input) {
    cv::Mat output = Mat(input.size(), CV_8UC1);
    cv::normalize(input, input, 1, 255, NORM_MINMAX);
    input.convertTo(output, CV_8UC1);
    return output;
}

cv::Mat imgproc::embedOnSubstrate(Mat trace, Scalar median) {
    if (this->Substrate.size == trace.size) {
        trace = trace - median;
        if (trace.type() != this->Substrate.type()) {
            cv::Mat tmp;
            this->Substrate.convertTo(tmp, trace.type());
            trace = trace + tmp;
        } else {
            trace = trace + this->Substrate;
        }
    } else {
        if ((this->Substrate.cols > trace.cols) && (this->Substrate.rows > trace.rows)) {
            cv::Mat section =
                cv::Mat(this->Substrate, cv::Rect((this->Substrate.cols - trace.cols) / 2,
                                                  (this->Substrate.rows - trace.rows) / 2, trace.cols, trace.rows));
            if (trace.type() != this->Substrate.type()) {
                cv::Mat tmp;
                section.convertTo(tmp, trace.type());
                trace = trace + tmp;
            } else {
                trace = trace + section;
            }
        } else {
            cv::Mat section = cv::Mat(trace, cv::Rect((trace.cols - this->Substrate.cols) / 2,
                                                      (trace.rows - this->Substrate.rows) / 2, this->Substrate.cols,
                                                      this->Substrate.rows));
            if (trace.type() != this->Substrate.type()) {
                cv::Mat tmp, tmp2;
                this->Substrate.convertTo(tmp, trace.type());
                tmp2 = section + tmp;
                trace = tmp2;
            } else {
                cv::Mat tmp2;
                tmp2 = section + this->Substrate;
                trace = tmp2;
            }
        }
    }

    return trace;
}

cv::Mat imgproc::shiftValues(cv::Mat input, double shiftFactor) {
    if (shiftFactor == 0)
        return input;
    return input + shiftFactor;
}

cv::Mat imgproc::tilt(double gradientA, double gradientB, double constantC, Mat input) {

    try {

        int bitdepth = input.depth();
        if (input.channels() == 1) {
            switch (bitdepth) {
            case 0: // 8bit unsigned
            {
                Mat1b::iterator it;
                Mat1b plane = Mat(input.size(), input.type());
                it = plane.begin();
                for (int i = 0; i < input.rows; i++)
                    for (int j = 0; j < input.cols; j++) {
                        *it = (quint8)(gradientA * j + gradientB * i + constantC);
                        it++;
                    }
                input = input + plane;
                break;
            }
            case 2: // 16bit unsigned
            {
                Mat1w::iterator it;
                Mat1w plane = Mat(input.size(), input.type());

                it = plane.begin();
                for (int i = 0; i < input.rows; i++)
                    for (int j = 0; j < input.cols; j++) {
                        *it = (quint16)(gradientA * j + gradientB * i + constantC);
                        it++;
                    }
                input = input + plane;
                break;
            }
            case 4: // 32bit unsigned
            {
                Mat1i::iterator it;
                Mat1i plane = Mat(input.size(), input.type());

                it = plane.begin();
                for (int i = 0; i < input.rows; i++)
                    for (int j = 0; j < input.cols; j++) {
                        *it = (quint32)(gradientA * j + gradientB * i + constantC);
                        it++;
                    }
                input = input + plane;
                break;
            }
            case 5: // 32bit float
            {
                Mat1f::iterator it;
                Mat1f plane = Mat(input.size(), input.type());

                it = plane.begin();
                for (int i = 0; i < input.rows; i++)
                    for (int j = 0; j < input.cols; j++) {
                        *it = (float)(gradientA * j + gradientB * i + constantC);
                        it++;
                    }
                input = input + plane;
                break;
            }
            }

        } else if (input.channels() == 3) {

            switch (bitdepth) {
            case 0: // 8bit unsigned
            {
                Mat3b::iterator it;
                Mat3b plane = Mat(input.size(), input.type());

                it = plane.begin();
                Vec3b pix;
                for (int i = 0; i < input.rows; i++)
                    for (int j = 0; j < input.cols; j++) {
                        pix[0] = (quint8)(gradientA * j + gradientB * i + constantC);
                        pix[1] = (quint8)(gradientA * j + gradientB * i + constantC);
                        pix[2] = (quint8)(gradientA * j + gradientB * i + constantC);
                        *it = pix;
                        it++;
                    }
                input = input + plane;
                break;
            }
            case 2: // 16bit unsigned
            {
                Mat3w::iterator it;
                Mat3w plane = Mat(input.size(), input.type());
                it = plane.begin();
                Vec3w pix;
                for (int i = 0; i < input.rows; i++)
                    for (int j = 0; j < input.cols; j++) {
                        pix[0] = (quint16)(gradientA * j + gradientB * i + constantC);
                        pix[1] = (quint16)(gradientA * j + gradientB * i + constantC);
                        pix[2] = (quint16)(gradientA * j + gradientB * i + constantC);
                        *it = pix;
                        it++;
                    }
                input = input + plane;
                break;
            }
            case 4: // 32bit unsigned
            {
                Mat3i::iterator it;
                Mat3i plane = Mat(input.size(), input.type());
                it = plane.begin();
                Vec3i pix;
                for (int i = 0; i < input.rows; i++)
                    for (int j = 0; j < input.cols; j++) {
                        pix[0] = (quint32)(gradientA * j + gradientB * i + constantC);
                        pix[1] = (quint32)(gradientA * j + gradientB * i + constantC);
                        pix[2] = (quint32)(gradientA * j + gradientB * i + constantC);
                        *it = pix;
                        it++;
                    }
                input = input + plane;
                break;
            }
            case 5: // 32bit float
            {
                Mat3f::iterator it;
                Mat3f plane = Mat(input.size(), input.type());
                it = plane.begin();
                Vec3f pix;
                for (int i = 0; i < input.rows; i++)
                    for (int j = 0; j < input.cols; j++) {
                        pix[0] = (float)(gradientA * j + gradientB * i + constantC);
                        pix[1] = (float)(gradientA * j + gradientB * i + constantC);
                        pix[2] = (float)(gradientA * j + gradientB * i + constantC);
                        *it = pix;
                        it++;
                    }
                input = input + plane;
                break;
            }
            }
        }
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }

    return input;
}

cv::Mat imgproc::passportScale(int targetWidth, int targetHeight, Mat input) {
    if ((targetWidth == 0) || (targetHeight == 0))
        return input;
    float outRatio = (float)targetWidth / (float)targetHeight;
    try {
        cv::Mat output = Mat(targetHeight, targetWidth, input.type());

        // derived from OpenCV Sample Detector
        std::string classifierXML = this->HaarCascade.toStdString();
        cv::CascadeClassifier FaceCascade = cv::CascadeClassifier(classifierXML);
        FaceCascade.load(classifierXML);
        vector<Rect> Faces;
        Mat GrayImage;
        cvtColor(input, GrayImage, COLOR_RGB2GRAY);
        FaceCascade.detectMultiScale(GrayImage, Faces);
        int largest = 0;
        int area = 0;
        int cropwidth, cropheight;
        if (Faces.size() == 0) {
            // no face found...
            float inRatio = (float)input.cols / (float)input.rows;
            if (inRatio > outRatio) {
                // our input image is wider than the target image --> crop horizontally
                cropwidth = input.cols;
                cropheight = input.rows / outRatio;
            } else {
                // our input face is higher than the target image --> crop vertically
                cropheight = input.rows;
                cropwidth = input.cols * outRatio;
            }
            if (cropwidth > input.cols)
                cropwidth = input.cols;
            if (cropheight > input.rows)
                cropheight = input.rows;
            cv::Mat roi = Mat(
                input, cv::Rect((input.cols - cropwidth) / 2, (input.rows - cropheight) / 2, cropwidth, cropheight));
            Mat tempf = roi.clone();
            cv::resize(tempf, output, output.size(), 0, 0, INTER_LANCZOS4);
            input = output;
        } else {
            for (size_t i = 0; i < Faces.size(); i++) {
                if (Faces[i].area() > area) {
                    largest = i;
                    area = Faces[i].area();
                }
            }
            cv::Rect face = Faces[largest];
            // rectangle(input, face, Scalar(0,255,0));
            int FcenterX = face.tl().x + (face.width / 2);
            int FcenterY = face.tl().y + (face.height / 2);

            // determine aspect ratio of the face area
            float faceRatio = (float)face.width / (float)face.height;
            float faceMargin = 1.25;

            if (faceRatio > outRatio) {
                // our input face is wider than the target image --> crop horizontally
                cropwidth = face.width * faceMargin;
                cropheight = (face.height * faceMargin) / outRatio;
            } else {
                // our input face is higher than the target image --> crop vertically
                cropheight = face.height * faceMargin;
                cropwidth = (face.width * faceMargin) * outRatio;
            }
            // calculate top left point for the ROI to crop
            int TLX = FcenterX - (cropwidth / 2);
            if (TLX < 0)
                TLX = 0;
            if (TLX + cropwidth > input.cols)
                TLX = input.cols - cropwidth - 1;
            int TLY = FcenterY - (cropheight / 2);
            if (TLY < 0)
                TLY = 0;
            if (TLY + cropheight > input.rows)
                TLY = input.rows - cropheight - 1;
            // cv::Rect TL=cv::Rect(TLX-1,TLY-1,3,3);
            // rectangle(input, TL, Scalar(0,0,255));
            if (TLX < 0)
                TLX = 0;
            if (TLY < 0)
                TLY = 0;
            if (cropwidth > input.cols)
                cropwidth = input.cols;
            if (cropheight > input.rows)
                cropheight = input.rows;
            cv::Rect crop = cv::Rect(TLX, TLY, cropwidth, cropheight);
            cv::Mat cropped = Mat(input, crop);
            Mat temp = cropped.clone();
            cv::resize(temp, output, output.size(), 0, 0, INTER_LANCZOS4);
            input = output;
        }
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}

cv::Mat imgproc::DoubleScale(double scalingfactor, Mat input) {
    int origHeight = input.rows;
    int origWidth = input.cols;
    try {
        cv::Mat output = Mat(origHeight, origWidth, input.type());
        Mat temp = rescale(scalingfactor, input);
        cv::resize(temp, output, output.size(), 0, 0, INTER_LANCZOS4);
        input = output;
    } catch (int e) { qDebug() << "An exception occurred. Exception Nr. " << e << Qt::endl; }
    return input;
}
