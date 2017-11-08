#ifndef NCS_H
#define NCS_H

#include "widget.h"

#include <annotator/plugins/plugin.h>
#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtGui/QIcon>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <mvnc.h>

using namespace AnnotatorLib;
using std::shared_ptr;

namespace AnnotatorLib {
class Session;
}

namespace Annotator {
namespace Plugins {

class NCS : public Plugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "annotator.ncs" FILE
                        "ncs.json")
  Q_INTERFACES(Annotator::Plugin)

 public:
  NCS();
  ~NCS();
  QString getName() override;
  QWidget *getWidget() override;

  bool setFrame(shared_ptr<Frame> frame, cv::Mat image) override;
  void setObject(shared_ptr<Object> object) override;
  shared_ptr<Object> getObject() const override;
  void setLastAnnotation(shared_ptr<Annotation> annotation) override;
  std::vector<shared_ptr<Commands::Command>> getCommands() override;

 protected:
  cv::Mat frameImg;
  shared_ptr<Annotation> lastAnnotation = nullptr;
  shared_ptr<Object> object = nullptr;

  Widget widget;

  shared_ptr<Frame> frame = 0;
  shared_ptr<Frame> lastFrame = 0;

  typedef unsigned short half;
  const int networkDim = 224;
  float networkMean[3] = {0.40787054*255.0, 0.45752458*255.0, 0.48109378*255.0};
  #define NAME_SIZE 100

  mvncStatus retCode;
  void *deviceHandle;
  char devName[NAME_SIZE];

  unsigned int graphFileLen;
  void* graphFileBuf;
  void* graphHandle;

  bool graphLoaded = false;

  static void *LoadFile(const char *path, unsigned int *length);
  static half *LoadImage(cv::Mat resizedMat, int reqsize, float *mean);
};
}
}

#endif  // NCS_H
