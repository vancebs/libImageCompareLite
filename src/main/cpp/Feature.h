//
// Created by vance on 2016/8/22.
//

#ifndef NATIVEIMAGEMERGE_FEATURE_H
#define NATIVEIMAGEMERGE_FEATURE_H

#include "inc.h"

#define FEATURE_WIDTH(f) ((f).right - (f).left + 1)
#define FEATURE_HEIGHT(f) ((f).bottom - (f).top + 1)
#define FEATURE_DISTANCE(f1, f2) std::abs((f2).top - (f1).top)

struct Feature {
    jint left;
    jint top;
    jint right;
    jint bottom;

    jint pixelCount;
};

typedef std::vector<Feature> FeatureList;

#endif //NATIVEIMAGEMERGE_FEATURE_H
