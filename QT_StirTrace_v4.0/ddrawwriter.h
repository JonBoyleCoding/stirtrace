#ifndef DDRAWWRITER_H
#define DDRAWWRITER_H
#include "IBinaryFileWriter.h"
#include <opencv2/core/core.hpp>
#include <QString>

class DDRawWriter: public IBinaryFileWriter
{
public:
    DDRawWriter();
    void writeRawData(cv::Mat img, QString filename, double scale, double time);
};

#endif // DDRAWWRITER_H
