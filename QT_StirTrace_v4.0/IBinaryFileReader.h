#ifndef IBINARYFILEREADER_H
#define IBINARYFILEREADER_H
#include <opencv2/core/core.hpp>
#include <qt4/Qt/qstring.h>
#include <qt4/Qt/qstringlist.h>
#include <vector>

class IBinaryFileReader {
  public:
    IBinaryFileReader() {}
    virtual bool readData(QString file) {
        return true;
    }
    virtual cv::Mat getImage(QString ImageType) {
        return cv::Mat();
    }
    virtual QStringList getTypes() {
        return QStringList();
    }

    virtual double getScale() = 0;
    virtual long getTime() = 0;

  private:
    QStringList imageTypes;
    std::vector<cv::Mat> images;
};

#endif // IBINARYFILEREADER_H
