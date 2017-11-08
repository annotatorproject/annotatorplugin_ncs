

#include "ncs.h"
#include "widget.h"

#include <ctype.h>
#include <iostream>
#include <utility>

#include <AnnotatorLib/Annotation.h>
#include <AnnotatorLib/Commands/NewAnnotation.h>
#include <AnnotatorLib/Frame.h>
#include <AnnotatorLib/Session.h>

#include "fp16.h"

#define GRAPH_FILE_NAME "./graph"

using namespace Annotator::Plugins;

NCS::NCS() {
    retCode = mvncGetDeviceName(0, devName, NAME_SIZE);
    if (retCode != MVNC_OK)
    {
        std::cerr << "No NCS devices found\n" << std::endl;
    }
    retCode = mvncOpenDevice(devName, &deviceHandle);
    if (retCode != MVNC_OK)
    {
        std::cerr << "Could not open NCS device\n" << std::endl;
    }
    std::cout << "Successfully opened NCS device!\n" << std::endl;

    graphFileBuf = LoadFile(GRAPH_FILE_NAME, &graphFileLen);

    retCode = mvncAllocateGraph(deviceHandle, &graphHandle, graphFileBuf, graphFileLen);
    if (retCode != MVNC_OK)
    {
        std::cerr << "Could not allocate graph for file: %s\n" << GRAPH_FILE_NAME << std::endl;
        std::cerr << "Error from mvncAllocateGraph is: %d\n" << retCode << std::endl;
    }else{
        graphLoaded = true;
    }
}

NCS::~NCS() {
    free(graphFileBuf);
    retCode = mvncCloseDevice(deviceHandle);
    deviceHandle = nullptr;
}

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

    if(graphLoaded){
        half* imageBufFp16 = LoadImage(frameImg, networkDim, networkMean);

        unsigned int lenBufFp16 = 3*networkDim*networkDim*sizeof(*imageBufFp16);

        retCode = mvncLoadTensor(graphHandle, imageBufFp16, lenBufFp16, NULL);
        if (retCode != MVNC_OK)
        {
            std::cerr << "Could not load tensor\n" << std::endl;
            std::cerr << "Error from mvncLoadTensor is: %d\n" << retCode << std::endl;
        }
        else
        {
            void* resultData16;
            void* userParam;
            unsigned int lenResultData;
            retCode = mvncGetResult(graphHandle, &resultData16, &lenResultData, &userParam);
            if (retCode == MVNC_OK)
            {
                std::cout << "resultData is %d bytes which is %d 16-bit floats.\n" << lenResultData << lenResultData/(int)sizeof(half) << std::endl;

                int numResults = lenResultData / sizeof(half);
                float* resultData32;
                resultData32 = (float*)malloc(numResults * sizeof(*resultData32));
                fp16tofloat(resultData32, (unsigned char*)resultData16, numResults);

                float maxResult = 0.0;
                int maxIndex = -1;
                for (int index = 0; index < numResults; index++)
                {
                    if (resultData32[index] > maxResult)
                    {
                        maxResult = resultData32[index];
                        maxIndex = index;
                    }
                }
                std::cout << "Index of top result is: " << maxIndex << std::endl;
                std:: cout << "Probability of top result is: " << resultData32[maxIndex] << std::endl;
            }
        }
    }
    return commands;
}

NCS::half *NCS::LoadImage(cv::Mat resizedMat, int reqsize, float *mean){
    int width, height, cp, i;
    unsigned char *imgresized;
    float *imgfp32;
    half *imgfp16;

    width = resizedMat.cols;
    height = resizedMat.rows;
    cp = resizedMat.depth();
    imgresized = (unsigned char*) malloc(3*reqsize*reqsize);
    if(!imgresized)
    {
        perror("malloc");
        return 0;
    }
    cv::Mat img_dst;
    cv::resize(resizedMat, img_dst, cv::Size(reqsize, reqsize), 0, 0, cv::INTER_CUBIC);
    imgresized = img_dst.data;
    //stbir_resize_uint8(resizedMat.data, width, height, 0, imgresized, reqsize, reqsize, 0, 3);
    imgfp32 = (float*) malloc(sizeof(*imgfp32) * reqsize * reqsize * 3);
    if(!imgfp32)
    {
        perror("malloc");
        return 0;
    }
    for(i = 0; i < reqsize * reqsize * 3; i++)
        imgfp32[i] = imgresized[i];
    imgfp16 = (half*) malloc(sizeof(*imgfp16) * reqsize * reqsize * 3);
    if(!imgfp16)
    {
        free(imgfp32);
        perror("malloc");
        return 0;
    }
    for(i = 0; i < reqsize*reqsize; i++)
    {
        float blue, green, red;
        blue = imgfp32[3*i+2];
        green = imgfp32[3*i+1];
        red = imgfp32[3*i+0];

        imgfp32[3*i+0] = blue-mean[0];
        imgfp32[3*i+1] = green-mean[1];
        imgfp32[3*i+2] = red-mean[2];
    }
    floattofp16((unsigned char *)imgfp16, imgfp32, 3*reqsize*reqsize);
    free(imgfp32);
    return imgfp16;
}

void *NCS::LoadFile(const char *path, unsigned int *length)
{
    FILE *fp;
    char *buf;

    fp = fopen(path, "rb");
    if(fp == NULL)
        return 0;
    fseek(fp, 0, SEEK_END);
    *length = ftell(fp);
    rewind(fp);
    if(!(buf = (char*) malloc(*length)))
    {
        fclose(fp);
        return 0;
    }
    if(fread(buf, 1, *length, fp) != *length)
    {
        fclose(fp);
        free(buf);
        return 0;
    }
    fclose(fp);
    return buf;
}
