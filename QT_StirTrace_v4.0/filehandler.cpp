#include "filehandler.h"
/*#include "frtreader.h"
#include "vk4reader.h"*/
#include "ddrawreader.h"
#include "ddrawwriter.h"
#include <QDir>
#include <QFileInfo>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

QString fileHandler::fileExt="";
QString fileHandler::inputFile="";
bool fileHandler::rawData=false;
double fileHandler::scale=1;
long fileHandler::time=0;

union FloatReader
{
        float   f;
        unsigned char   c[0];
};
union Int32Reader
{
        int   f;
        unsigned char   c[0];
};

fileHandler::fileHandler()
{
    this->rawData=false;
    this->multiImage=false;
}

int fileHandler::openFile(QString filename) {

    QFileInfo fileInfo=QFileInfo(filename);
    this->inputFile=filename;
    this->inputFile.truncate(filename.length()-(1+fileInfo.suffix().length()));
    QStringList ImageFiles;
    ImageFiles << "*.bmp" << "*.BMP" << "*.png" << "*.PNG" << "*.jpg" << "*.JPG" << "*.jp2" << "*.JP2" << "*.tiff";
    if (QDir::match(ImageFiles, filename)) {
        this->images.push_back(cv::imread(filename.toUtf8().constData(), CV_LOAD_IMAGE_UNCHANGED));
        this->rawData=false;
        this->fileExt=fileInfo.suffix();
        this->multiImage=false;
        this->imageTypes << "";
        return 1;
    }
/*    QStringList FRTFile;
    FRTFile << "*.frt" << "*.FRT";
    if (QDir::match(FRTFile, filename)) {
        FRTReader frtinput=FRTReader();
        frtinput.readData(filename);
        QStringList types=frtinput.getTypes();
        for (int i = 0 ; i < types.count() ; i++) {
            this->images.push_back(frtinput.getImage(types.at(i)));
            this->imageTypes << types.at(i);
        }
        this->scale=frtinput.getScale();
        this->time=frtinput.getTime();
        this->rawData=true;
        this->fileExt="dd+";
        this->multiImage=true;
        return this->imageTypes.count();
    }
    QStringList VK4File;
    VK4File << "*.vk4" << "*.VK4";
    if (QDir::match(VK4File, filename)) {
        VK4Reader vk4input=VK4Reader();
        vk4input.readData(filename);
        QStringList types=vk4input.getTypes();
        for (int i = 0 ; i < types.count() ; i++) {
            this->images.push_back(vk4input.getImage(types.at(i)));
            this->imageTypes << types.at(i);
        }
        this->scale=vk4input.getScale();
        this->time=vk4input.getTime();
        this->rawData=true;
        this->fileExt="dd+";
        this->multiImage=true;
        return this->imageTypes.count();
    }*/
    QStringList DDPlusFile;
    DDPlusFile << "*.dd+" << "*.DD+";
    if (QDir::match(DDPlusFile, filename)) {
        DDRawReader DDRawInput=DDRawReader();
        DDRawInput.readData(filename);
        QStringList types=DDRawInput.getTypes();
        for (int i = 0 ; i < types.count() ; i++) {
            this->images.push_back(DDRawInput.getImage(types.at(i)));
            this->imageTypes << types.at(i);
        }
        this->scale=DDRawInput.getScale();
        this->time=DDRawInput.getTime();
        this->rawData=true;
        this->fileExt="dd+";
        this->multiImage=true;
        return this->imageTypes.count();
    }

    QStringList BinFile;
    //BinFile << "*.raw" << "*.RAW";
    BinFile << "*.raw" << "*.RAW" << "*.bin" << "*.BIN";
    if (QDir::match(BinFile, filename)) {
        QFile* infile = new QFile(filename);
        infile->open(QIODevice::ReadOnly);
        int height=0;
        int width=0;
        QByteArray Bintensity;
        Bintensity=infile->readAll();
        infile->close();
        Int32Reader intread;
        for (int i=0; i < 4 ; i++) {
            intread.c[i] = Bintensity.at(i);
        }
        width = intread.f;
        for (int i=0; i < 4 ; i++) {
            intread.c[i] = Bintensity.at(i+4);
        }
        height = intread.f;
        cv::Mat intensity=cv::Mat(height,width,CV_32FC1);
        for (int h=0; h<height;h++)
        for (int w=0;w<width;w++) {
            FloatReader intval;
            for (size_t i = 0; i < sizeof(FloatReader); ++i)
            {
                long pos=8+ (((w+(width*h)))*4) +(i);
                intval.c[i]=Bintensity.at(pos);
            }
            intensity.at<float>(h,w) = intval.f;
        }
        this->rawData=true;
        //this->fileExt="raw";
        this->fileExt="bin";
        this->multiImage=false;
        this->imageTypes << "";
        this->images.push_back(intensity);
        this->scale=1;
        return 1;
    }

    return 0;
}

double fileHandler::getScale() {
    return this->scale;
}

QString fileHandler::getBaseFileName() {
    return this->inputFile;
}

void fileHandler::saveFile(QString BaseFilename, cv::Mat image) {
    if (this->rawData) {
        DDRawWriter rawWriter =DDRawWriter();
        rawWriter.writeRawData(image,BaseFilename, this->scale, this->time);
    } else {
        cv::imwrite((BaseFilename+"."+this->fileExt).toUtf8().constData(), image);
        //copy the meta data file to the new directory
        if (QFile::exists(this->inputFile+".xml")) {
         if (QFile::exists(BaseFilename+".xml"))
          {
               QFile::remove(BaseFilename+".xml");
          }

            QFile::copy(this->inputFile+".xml", BaseFilename+".xml");
        }
    }
}

QStringList fileHandler::getImageTypes() {
    return this->imageTypes;
}

cv::Mat fileHandler::getImage(QString imageType) {
    return this->images.at(this->imageTypes.indexOf(imageType));
}
