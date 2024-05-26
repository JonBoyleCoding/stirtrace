#include "ddrawreader.h"
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <iostream>

double DDRawReader::scale = 1;
long DDRawReader::time = 0;

union FloatReader {
    float f;
    unsigned char c[0];
};

DDRawReader::DDRawReader() {}

double DDRawReader::getScale() {
    return this->scale;
}

long DDRawReader::getTime() {
    return this->time;
}

bool DDRawReader::readData(QString file) {
    QByteArray Btmpdata;
    QByteArray Bmetadata;

    QFileInfo FI(file);

    QFile *infile = new QFile(file);
    infile->open(QIODevice::ReadOnly);
    Bmetadata = infile->readAll();
    infile->close();

    QString metadata(Bmetadata);
    QStringList metadataLines = metadata.split("\n");
    QStringList lineSplit;
    QString curLine;
    QString intensityfile;
    QString topographyfile;
    QString colorfile;
    QString colorintensityfile;

    int height;
    int width;
    double zscale;
    for (int line = 0; line < metadataLines.length(); line++) {
        curLine = metadataLines.at((line));
        curLine.truncate(curLine.length() - 1);
        lineSplit = curLine.split("=");
        if (lineSplit.at(0) == "ScaleX")
            this->scale = (lineSplit.at(1).toDouble() / 100000.0);
        if (lineSplit.at(0) == "Height")
            height = (lineSplit.at(1).toInt());
        if (lineSplit.at(0) == "Width")
            width = (lineSplit.at(1).toInt());
        if (lineSplit.at(0) == "StartTime")
            this->time = lineSplit.at(1).toInt();
        if (lineSplit.at(0) == "intensity")
            intensityfile = lineSplit.at(1);
        if (lineSplit.at(0) == "topograpy")
            topographyfile = lineSplit.at(1);
        if (lineSplit.at(0) == "color")
            colorfile = lineSplit.at(1);
        if (lineSplit.at(0) == "colorintensity")
            colorintensityfile = lineSplit.at(1);
    }
    if (!intensityfile.isNull()) {
        QFile *infile = new QFile(FI.absolutePath() + "/" + intensityfile);
        infile->open(QIODevice::ReadOnly);
        Btmpdata = infile->readAll();

        infile->close();

        cv::Mat intensity = cv::Mat(height, width, CV_32FC1);
        for (int h = 0; h < height; h++)
            for (int w = 0; w < width; w++) {
                FloatReader intval;
                for (size_t i = 0; i < sizeof(FloatReader); ++i) {
                    long pos = 8 + (((w + (width * h))) * 4) + (i);
                    intval.c[i] = Btmpdata.at(pos);
                }
                intensity.at<float>(h, w) = intval.f;
            }
        this->imageTypes << "intensity";
        this->images.push_back(intensity);
    }

    if (!topographyfile.isNull()) {
        QFile *infile = new QFile(FI.absolutePath() + "/" + intensityfile);
        infile->open(QIODevice::ReadOnly);
        Btmpdata = infile->readAll();

        infile->close();

        cv::Mat topography = cv::Mat(height, width, CV_32FC1);
        for (int h = 0; h < height; h++)
            for (int w = 0; w < width; w++) {
                FloatReader intval;
                for (size_t i = 0; i < sizeof(FloatReader); ++i) {
                    long pos = 8 + (((w + (width * h))) * 4) + (i);
                    intval.c[i] = Btmpdata.at(pos);
                }
                topography.at<float>(h, w) = intval.f;
            }
        this->imageTypes << "topography";
        this->images.push_back(topography);
    }

    return this->imageTypes.count() > 0;
}

cv::Mat DDRawReader::getImage(QString ImageType) {
    int index = this->imageTypes.indexOf(ImageType);
    if (index >= 0)
        return this->images.at(index);
    return cv::Mat();
}

QStringList DDRawReader::getTypes() {
    return this->imageTypes;
}
