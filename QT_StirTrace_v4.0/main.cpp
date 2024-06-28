#include "cxxopts.hpp"
#include "evaluation.h"
#include "imgproc.h"
#include "mainwindow.h"
#include "preprocessing.h"
#include <QApplication>
#include <QFileInfo>
#include <stdio.h>

int main(int argc, char *argv[]) {
#ifdef Q_WS_X11
    bool useGUI = getenv("DISPLAY") != 0; // check for xserver availability
#else
    bool useGUI = true;
#endif
    bool batchMode = false;
    bool showCMDLineHelp = false;
    bool combineMode = false;
    bool eightBitConversion = false;
    bool baseline = false;
    QString NoiseLevels = "";
    QString MedianFilters = "";
    QString LineRemoval = "";
    QString ColumnRemoval = "";
    QString RotAngles = "";
    QString XStretchFactors = "";
    QString YShearFactors = "";
    QString ScaleFactors = "";
    QString CropFactors = "";
    QString BandingFrequency = "";
    QString FilePath = "";
    QString SaltAndPepper = "";
    QString SubstrateFile = "";
    QString GaussianNoise = "";
    QString EvalMode = "";
    QString PreProcessMode = "";
    QString ShiftParams = "";
    QString TiltParams = "";
    QStringList validCMDLineSwitches;
    evaluation eval = evaluation();
    QStringList tmp = eval.getModes();
    QString validEvalModes = "";
    for (int i = 0; i < tmp.count(); i++)
        validEvalModes += tmp.at(i) + " ";
    preprocessing preproc = preprocessing();
    tmp = preproc.getModes();
    QString validPreProcModes = "";
    for (int i = 0; i < tmp.count(); i++)
        validPreProcModes += tmp.at(i) + " ";
    QString DoubleScalingFactors = "";
    QString passportscaling = "";
    QString haarcascade = "/home/mhilde/src/QT_StirTrace/haarcascade_frontalface_default.xml";

    cxxopts::Options options(
      "StirTrace", "QT_StirTrace command line interface help (if multiple options for argument, separate by ',')");

    options.add_options()

      ("d", "Path of files to process", cxxopts::value<std::string>())

        ("b", "Non-interactive Batch Mode")

          ("n", "Additive White Noise", cxxopts::value<std::string>())

            ("g", "Additive Gaussian Noise (µ=0)", cxxopts::value<std::string>())

              ("p", "Salt and Pepper Noise", cxxopts::value<std::string>())

                ("m", "Median Cut Filtering", cxxopts::value<std::string>())

                  ("l", "Removal of Lines", cxxopts::value<std::string>())

                    ("c", "Removal of Columns", cxxopts::value<std::string>())

                      ("r", "Rotation", cxxopts::value<std::string>())

                        ("x", "Stretching in X Direction", cxxopts::value<std::string>())

                          ("y", "Shearing in Y Direction", cxxopts::value<std::string>())

                            ("s", "Scaling", cxxopts::value<std::string>())

                              ("o", "Cropping", cxxopts::value<std::string>())

                                ("a", "Banding Artifacts", cxxopts::value<std::string>())

                                  ("f", "Shift Values, e.g. to simulate a different parameterization",
                                   cxxopts::value<std::string>())

                                    ("i", "Tilt Simulation, specify planes as triplets A#B#C,...",
                                     cxxopts::value<std::string>())

                                      ("8", "convert data to 8 bit range")

                                        ("e", "Embed Trace on Substrate", cxxopts::value<std::string>())

                                          ("t", "Combine Filtering Techniques")

                                            ("eval",
                                             std::string("Perform Evaluation, modes: ") +
                                               validEvalModes.toUtf8().constData(),
                                             cxxopts::value<std::string>())

                                              ("preprocess",
                                               std::string("Perform a Preprocessing, modes: ") +
                                                 validPreProcModes.toUtf8().constData(),
                                               cxxopts::value<std::string>())

                                                ("baseline", "Determine Baseline Performance")

                                                  ("passport",
                                                   "Enable Passport Scaling, specify "
                                                   "parameters by WIDTHxHEIGHT",
                                                   cxxopts::value<std::string>())

                                                    ("doublescale",
                                                     "Scaling to scaling parameter(s) "
                                                     "and back to original size",
                                                     cxxopts::value<std::string>())

                                                      ("haarcascade",
                                                       "Full path pointing to the "
                                                       "OpenCV Haar Cascade to be "
                                                       "used",
                                                       cxxopts::value<std::string>())

                                                        ("h,help", "Show this help message");

    auto arguments = options.parse(argc, argv);

    if (arguments.count("help")) {
        printf("%s\n", options.help().c_str());
        return 0;
    }

    printf("Arguments:\n");

    if (arguments.count("d")) {
        FilePath = QString::fromStdString(arguments["d"].as<std::string>());
        printf("\tPath: %s\n", FilePath.toUtf8().constData());
    }
    if (arguments.count("b")) {
        batchMode = true;
        printf("\tBatch Mode\n");
    }

    if (arguments.count("n")) {
        NoiseLevels = QString::fromStdString(arguments["n"].as<std::string>());
        printf("\tNoise Levels: %s\n", NoiseLevels.toUtf8().constData());
    }
    if (arguments.count("p")) {
        SaltAndPepper = QString::fromStdString(arguments["p"].as<std::string>());
        printf("\tSalt and Pepper: %s\n", SaltAndPepper.toUtf8().constData());
    }
    if (arguments.count("g")) {
        GaussianNoise = QString::fromStdString(arguments["g"].as<std::string>());
        printf("\tGaussian Noise: %s\n", GaussianNoise.toUtf8().constData());
    }
    if (arguments.count("m")) {
        MedianFilters = QString::fromStdString(arguments["m"].as<std::string>());
        printf("\tMedian Filters: %s\n", MedianFilters.toUtf8().constData());
    }
    if (arguments.count("l")) {
        LineRemoval = QString::fromStdString(arguments["l"].as<std::string>());
        printf("\tLine Removal: %s\n", LineRemoval.toUtf8().constData());
    }
    if (arguments.count("c")) {
        ColumnRemoval = QString::fromStdString(arguments["c"].as<std::string>());
        printf("\tColumn Removal: %s\n", ColumnRemoval.toUtf8().constData());
    }
    if (arguments.count("r")) {
        RotAngles = QString::fromStdString(arguments["r"].as<std::string>());
        printf("\tRotation: %s\n", RotAngles.toUtf8().constData());
    }
    if (arguments.count("x")) {
        XStretchFactors = QString::fromStdString(arguments["x"].as<std::string>());
        printf("\tStretching in X Direction: %s\n", XStretchFactors.toUtf8().constData());
    }
    if (arguments.count("y")) {
        YShearFactors = QString::fromStdString(arguments["y"].as<std::string>());
        printf("\tShearing in Y Direction: %s\n", YShearFactors.toUtf8().constData());
    }
    if (arguments.count("s")) {
        ScaleFactors = QString::fromStdString(arguments["s"].as<std::string>());
        printf("\tScaling: %s\n", ScaleFactors.toUtf8().constData());
    }
    if (arguments.count("o")) {
        CropFactors = QString::fromStdString(arguments["o"].as<std::string>());
        printf("\tCropping: %s\n", CropFactors.toUtf8().constData());
    }
    if (arguments.count("a")) {
        BandingFrequency = QString::fromStdString(arguments["a"].as<std::string>());
        printf("\tBanding Artifacts: %s\n", BandingFrequency.toUtf8().constData());
    }
    if (arguments.count("f")) {
        ShiftParams = QString::fromStdString(arguments["f"].as<std::string>());
        printf("\tShift Values: %s\n", ShiftParams.toUtf8().constData());
    }
    if (arguments.count("i")) {
        TiltParams = QString::fromStdString(arguments["i"].as<std::string>());
        printf("\tTilt Simulation: %s\n", TiltParams.toUtf8().constData());
    }
    if (arguments.count("8")) {
        eightBitConversion = true;
        printf("\t8 bit conversion\n");
    }
    if (arguments.count("e")) {
        SubstrateFile = QString::fromStdString(arguments["e"].as<std::string>());
        printf("\tEmbed Trace on Substrate: %s\n", SubstrateFile.toUtf8().constData());
    }
    if (arguments.count("t")) {
        combineMode = true;
        printf("\tCombine Filtering Techniques\n");
    }
    if (arguments.count("eval")) {
        EvalMode = QString::fromStdString(arguments["eval"].as<std::string>());
        printf("\tEvaluation: %s\n", EvalMode.toUtf8().constData());
    }
    if (arguments.count("preprocess")) {
        PreProcessMode = QString::fromStdString(arguments["preprocess"].as<std::string>());
        printf("\tPreprocessing: %s\n", PreProcessMode.toUtf8().constData());
    }
    if (arguments.count("baseline")) {
        baseline = true;
        printf("\tDetermine Baseline Performance\n");
    }
    if (arguments.count("passport")) {
        passportscaling = QString::fromStdString(arguments["passport"].as<std::string>());
        printf("\tPassport Resizing: %s\n", passportscaling.toUtf8().constData());
    }
    if (arguments.count("doublescale")) {
        DoubleScalingFactors = QString::fromStdString(arguments["doublescale"].as<std::string>());
        printf("\tDouble Scaling: %s\n", DoubleScalingFactors.toUtf8().constData());
    }
    if (arguments.count("haarcascade")) {
        haarcascade = QString::fromStdString(arguments["haarcascade"].as<std::string>());
        printf("\tHaar Cascade: %s\n", haarcascade.toUtf8().constData());
    }

    if ((batchMode) || (!useGUI)) {
        QFileInfo FI = QFileInfo(FilePath);
        if (FI.isDir()) {
            imgproc *processing =
              new imgproc(FilePath, NoiseLevels, MedianFilters, LineRemoval, ColumnRemoval, RotAngles, XStretchFactors,
                          YShearFactors, ScaleFactors, CropFactors, BandingFrequency, SaltAndPepper, GaussianNoise,
                          SubstrateFile, eightBitConversion, EvalMode, PreProcessMode, ShiftParams, TiltParams,
                          baseline, passportscaling, haarcascade, DoubleScalingFactors);
            if (!combineMode)
                processing->startProcessing();
            else {
                processing->startCombinedProcessing();
            }
        } else {
            printf("error: [%s] is not a valid directory\n", FilePath.toUtf8().constData());
            return 1;
        }
        return 0;
    } else {
        QApplication a(argc, argv, useGUI);
        MainWindow w(a.arguments());
        w.show();

        return a.exec();
    }
}
