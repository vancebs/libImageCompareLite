//
// Created by vance on 2016/8/21.
//

#ifndef NATIVEIMAGEMERGE_FEATURECOMPARE_H
#define NATIVEIMAGEMERGE_FEATURECOMPARE_H

#include "inc.h"
#include "NativeBitmap.h"
#include "Debugger.h"
#include <queue>
#include "Feature.h"

class FeatureCompare {
public:
    jint compare(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, IN Debugger* pDebugger = NULL);
    jint compareWithMultiThread(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, IN Debugger* pDebugger = NULL);

protected:
    void xorBitmap(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, OUT NativeBitmap& xorBmp);
    void generateMask(IN const NativeBitmap& xorBmp, IN const NativeBitmap& bmp, OUT NativeBitmap& mask);
    void generateFeatures(IN const NativeBitmap& mask, OUT FeatureList& outFeatures);
    jint compareFeatures(IN const NativeBitmap& bmp1, IN const NativeBitmap& mask1, IN FeatureList& features1, IN const NativeBitmap& bmp2, IN const NativeBitmap& mask2, IN  FeatureList& features2);

private:
    void findRect(IN const int index, INOUT std::queue<jint>& queue, INOUT const NativeBitmap& mask, INOUT  Feature& feature);

    bool static sort_by_s(const Feature& obj1,const Feature& obj2);
    float diffRect(IN const NativeBitmap& bmp1, IN const NativeBitmap& mask1, IN const Feature& f1, IN const NativeBitmap& bmp2, IN const NativeBitmap& mask2, IN const Feature& f2);

    static void* generateMaskThreadRun(void* arg);
    static void* generateFeaturesThreadRun(void* arg);
};


#endif //NATIVEIMAGEMERGE_FEATURECOMPARE_H
