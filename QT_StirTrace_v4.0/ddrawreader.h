#ifndef DDRAWREADER_H
#define DDRAWREADER_H
#include "IBinaryFileReader.h"

class DDRawReader : public IBinaryFileReader {
  public:
    DDRawReader();
    bool readData(QString file);
    cv::Mat getImage(QString ImageType);
    QStringList getTypes();
    double getScale();
    long getTime();

  private:
    static double scale;
    static long time;
    QStringList imageTypes;
    std::vector<cv::Mat> images;
};

#endif // DDRAWREADER_H
