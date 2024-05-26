#ifndef IBINARYFILEWRITER_H
#define IBINARYFILEWRITER_H
#include <opencv2/core/core.hpp>
#include <qt4/Qt/qstring.h>

class IBinaryFileWriter
{
public:
    IBinaryFileWriter() {}
    virtual void writeRawData(cv::Mat img, QString filename, double scale, double time) {}
private:
};

#endif // IBINARYFILEWRITER_H
