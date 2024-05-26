#include <QtGui/QApplication>
#include "mainwindow.h"
#include "imgproc.h"
#include "evaluation.h"
#include "preprocessing.h"
#include <stdio.h>
#include <QFileInfo>

int main(int argc, char *argv[])
{
#ifdef Q_WS_X11
    bool useGUI = getenv("DISPLAY") != 0; //check for xserver availability
#else
    bool useGUI = true;
#endif
    bool batchMode=false;
    bool showCMDLineHelp=false;
    bool combineMode=false;
    bool eightBitConversion=false;
    bool baseline=false;
    QString NoiseLevels="";
    QString MedianFilters="";
    QString LineRemoval="";
    QString ColumnRemoval="";
    QString RotAngles="";
    QString XStretchFactors="";
    QString YShearFactors="";
    QString ScaleFactors="";
    QString CropFactors="";
    QString BandingFrequency="";
    QString FilePath="";
    QString SaltAndPepper="";
    QString SubstrateFile="";
    QString GaussianNoise="";
    QString EvalMode="";
    QString PreProcessMode="";
    QString ShiftParams="";
    QString TiltParams="";
    QStringList validCMDLineSwitches;
    evaluation eval = evaluation();
    QStringList tmp = eval.getModes();
    QString validEvalModes="";
    for (int i =0 ; i < tmp.count(); i++) validEvalModes+=tmp.at(i)+" ";
    preprocessing preproc = preprocessing();
    tmp = preproc.getModes();
    QString validPreProcModes="";
    for (int i =0 ; i < tmp.count(); i++) validPreProcModes+=tmp.at(i)+" ";
    QString DoubleScalingFactors="";
    QString passportscaling="";
    QString haarcascade="/home/mhilde/src/QT_StirTrace/haarcascade_frontalface_default.xml";

    validCMDLineSwitches << "-n" << "-m" << "-l" << "-c" << "-r" << "-x" << "-y" << "-s" << "-o" << "-a" << "-b" << "-h" << "-d" << "-p" << "-e" << "-t" << "-g" << "-8" << "--eval" << "--preprocess" << "-f" << "-i" << "--baseline" << "--passport" << "--doublescale" << "--haarcascade";

    QApplication a(argc, argv, useGUI);
        for (int i=0; i< a.arguments().size(); i++) {
            //printf("[%d] %s\n",i, argv[i]);
            switch (validCMDLineSwitches.indexOf(a.arguments().at(i))) {
                case 0 : NoiseLevels=a.arguments().at(++i); break;
                case 1 : MedianFilters=a.arguments().at(++i); break;
                case 2 : LineRemoval=a.arguments().at(++i); break;
                case 3 : ColumnRemoval=a.arguments().at(++i); break;
                case 4 : RotAngles=a.arguments().at(++i); break;
                case 5 : XStretchFactors=a.arguments().at(++i); break;
                case 6 : YShearFactors=a.arguments().at(++i); break;
                case 7 : ScaleFactors=a.arguments().at(++i); break;
                case 8 : CropFactors=a.arguments().at(++i); break;
                case 9 : BandingFrequency=a.arguments().at(++i); break;
                case 10 : batchMode=true; break;
                case 11 : batchMode=true;
                          showCMDLineHelp=true;
                          break;
                case 12 : FilePath=a.arguments().at(++i); break;
                case 13 : SaltAndPepper=a.arguments().at(++i); break;
                case 14 : SubstrateFile=a.arguments().at(++i); break;
                case 15 : combineMode=true; break;
                case 16 : GaussianNoise=a.arguments().at(++i); break;
                case 17 : eightBitConversion=true; break;
                case 18 : EvalMode=a.arguments().at(++i); break;
                case 19 : PreProcessMode=a.arguments().at(++i); break;
                case 20 : ShiftParams=a.arguments().at(++i); break;
                case 21 : TiltParams=a.arguments().at(++i); break;
                case 22 : baseline=true; break;
                case 23 : passportscaling=a.arguments().at(++i);; break;
                case 24 : DoubleScalingFactors=a.arguments().at(++i); break;
                case 25 : haarcascade=a.arguments().at(++i); break;
                default: break;
            }
        }
        if ((batchMode)||(!useGUI)) {
            if ((showCMDLineHelp)||(a.arguments().size()<3)) {
            printf("QT_StirTrace command line interface help\n\n");
            printf("Usage: %s [options]\n\n", argv[0]);
            printf("Options:\n");
            printf("-h\tshow this help screen\n");
            printf("-b\tenter non-interactive mode\n");
            printf("-d [path]\tpath of files to process\n");
            printf("-n parameters\t Additive White Noise\n");
            printf("-g parameters\t Additive Gaussian Noise (µ=0)\n");
            printf("-m parameters\t Median Cut Filtering\n");
            printf("-l parameters\t Removal of Lines\n");
            printf("-c parameters\t Removal of Columns\n");
            printf("-r parameters\t Rotation\n");
            printf("-x parameters\t Stretching in X Direction\n");
            printf("-y parameters\t Shearing in Y Direction\n");
            printf("-s parameters\t Scaling\n");
            printf("-o parameters\t Cropping\n");
            printf("-a parameters\t Banding Artifacts\n");
            printf("-p parameters\t Salt and Pepper Noise\n");
            printf("-f parameters\t Shift Values, e.g. to simulate a different parameterization\n");
            printf("-i parameters\t Tilt Simulation, specify planes as triplets A#B#C,...\n");
            printf("-8 convert data to 8 bit range\n");
            printf("-e filename\t Embed Trace on Substrate\n");
            printf("-t\t Combine Filtering Techniques\n");
            printf("--eval evalMethod\t Perform Evaluation, modes: %s\n",validEvalModes.toUtf8().constData());
            printf("--preprocess PreprocessingMethod\t Perform a Preprocessing, modes: %s\n",validPreProcModes.toUtf8().constData());
            printf("--baseline\t Determine Baseline Performance\n");
            printf("--passport parameters\t Enable Passport Scaling, specify parameters by WIDTHxHEIGHT\n");
            printf("--doublescale parameters\t Scaling to scaling parameter(s) and back to original size\n");
            printf("--haarcascade filename\t Full path pointing to the OpenCV Haar Cascade to be used\n");
        } else {
            QFileInfo FI=QFileInfo(FilePath);
            if (FI.isDir()) {
                imgproc* processing=new imgproc(FilePath, NoiseLevels, MedianFilters, LineRemoval, ColumnRemoval, RotAngles, XStretchFactors, YShearFactors, ScaleFactors, CropFactors, BandingFrequency, SaltAndPepper, GaussianNoise, SubstrateFile, eightBitConversion, EvalMode, PreProcessMode, ShiftParams, TiltParams, baseline, passportscaling, haarcascade, DoubleScalingFactors);
                if (!combineMode) processing->startProcessing();
                    else {
                        processing->startCombinedProcessing();
                    }
            } else {
                printf("error: [%s] is not a valid directory\n",FilePath.toUtf8().constData());
                return 1;
            }
        }
        return 0;
    } else {
            MainWindow w(a.arguments());
            w.show();


        return a.exec();
    }
}
