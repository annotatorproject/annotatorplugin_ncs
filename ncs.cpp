#include "ncs.h"
#include "widget.h"

#include <ctype.h>
#include <iostream>
#include <utility>

#include <AnnotatorLib/Annotation.h>
#include <AnnotatorLib/Commands/NewAnnotation.h>
#include <AnnotatorLib/Frame.h>
#include <AnnotatorLib/Session.h>
#include <QDebug>
#include <QtGui/QPainter>
#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>

using namespace Annotator::Plugins;

NCS::NCS() {
}

NCS::~NCS() {}

QString NCS::getName() { return "NeuralComputeStick"; }

QWidget *NCS::getWidget() { return &widget; }

bool NCS::setFrame(shared_ptr<Frame> frame, cv::Mat image) {
  this->lastFrame = this->frame;
  this->frame = frame;
  this->frameImg = image;
  return lastFrame != frame;
}

// first call
void NCS::setObject(shared_ptr<Object> object) {
  this->object = object;
}

shared_ptr<Object> NCS::getObject() const { return object; }

// second call
void NCS::setLastAnnotation(shared_ptr<Annotation> annotation) {
  if (!annotation || annotation->getObject() != object) return;
}

std::vector<shared_ptr<Commands::Command>> NCS::getCommands() {
  std::vector<shared_ptr<Commands::Command>> commands;
  if (object == nullptr || frame == nullptr || lastFrame == nullptr ||
      this->lastAnnotation == nullptr || lastFrame == frame)
    return commands;
  return commands;
}

