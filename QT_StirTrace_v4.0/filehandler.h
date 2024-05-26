#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <Qt/qstring.h>
#include <Qt/qstringlist.h>
#include <opencv2/core/core.hpp>
#include <vector>

class fileHandler
{
public:
    fileHandler();
    int openFile(QString filename);
    cv::Mat getImage(QString imageType);
    QStringList getImageTypes();
    void saveFile(QString BaseFilename, cv::Mat image); // this function expects the filename without the file extension
    void saveFile(QString BaseFilename, cv::Mat image, QString imageType); // this function expects the filename without the file extension
    QString getBaseFileName();
    double getScale();
private:
    static bool rawData;
    static QString fileExt;
    static QString inputFile;
    static double scale;
    static long time;
    bool multiImage;
    QStringList imageTypes;
    std::vector<cv::Mat> images;
};

#endif // FILEHANDLER_H
