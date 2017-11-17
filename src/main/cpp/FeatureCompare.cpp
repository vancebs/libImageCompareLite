//
// Created by vance on 2016/8/21.
//

#include "FeatureCompare.h"
#include <algorithm>
//#include <ext/hash_map>
//#include <hash_map>
//#include <unordered_map>
#include <map>
#include "MultiThreadTask.h"

#define DEBUG_IMG_XOR           "xor.png"
#define DEBUG_IMG_MASK1         "mask1.png"
#define DEBUG_IMG_MASK2         "mask2.png"
#define DEBUG_IMG_FEATURE1      "feature1.png"
#define DEBUG_IMG_FEATURE2      "feature2.png"

#define COLOR_MASKED 0x00000000
#define COLOR_NOT_MASKED 0xFFFFFFFFFF

#define MAX_COLOR_DIFF 10
#define MAX_COLOR_DIFF_POWER2 (MAX_COLOR_DIFF * MAX_COLOR_DIFF)

#define COLOR_DIFF_A(c1, c2) ((((c1) >> 24) & 0xFF) - (((c2) >> 24) & 0xFF))
#define COLOR_DIFF_R(c1, c2) ((((c1) >> 16) & 0xFF) - (((c2) >> 16) & 0xFF))
#define COLOR_DIFF_G(c1, c2) ((((c1) >> 8) & 0xFF) - (((c2) >> 8) & 0xFF))
#define COLOR_DIFF_B(c1, c2) (((c1) & 0xFF) - ((c2) & 0xFF))

#define MIN_FEATURE_PIXEL_COUNT 50
#define MIN_FEATURE_RECT_DIFF 0.05f
#define MAX_FEATURE_HEIGHT_DIFF 1
#define MAX_FEATURE_PIXEL_COUNT_DIFF 3

#define FEATURE_INCLUDE_POINT(f, x, y) { \
    if ((x) < (f).left) (f).left = (x); \
    else if ((x) > (f).right) (f).right = (x); \
    if ((y) < (f).top) (f).top = (y); \
    else if ((y) > (f).bottom) (f).bottom = (y); \
}

#define PUSH_POINT_LEFT(x, y, index, width, height, pPixels, queue, tmpIndex) { \
    if ((x - 1) >= 0) { \
        tmpIndex = index - 1; \
        if (pPixels[tmpIndex] != COLOR_MASKED) { \
            queue.push(tmpIndex); \
        } \
    } \
}
#define PUSH_POINT_RIGHT(x, y, index, width, height, pPixels, queue, tmpIndex) { \
    if ((x + 1) < width) { \
        tmpIndex = index + 1; \
        if (pPixels[tmpIndex] != COLOR_MASKED) { \
            queue.push(tmpIndex); \
        } \
    } \
}
#define PUSH_POINT_TOP(x, y, index, width, height, pPixels, queue, tmpIndex) { \
    if ((y - 1) >= 0) { \
        tmpIndex = index - width; \
        if (pPixels[tmpIndex] != COLOR_MASKED) { \
            queue.push(tmpIndex); \
        } \
    } \
}
#define PUSH_POINT_BOTTOM(x, y, index, width, height, pPixels, queue, tmpIndex) { \
    if ((y + 1) < height) { \
        tmpIndex = index + width; \
        if (pPixels[tmpIndex] != COLOR_MASKED) { \
            queue.push(tmpIndex); \
        } \
    } \
}

#define GENERATE_MASK_ARG_INIT(arg, pObj, pXor, pTrimmed, pMask) { \
    arg.pCompare = pObj; \
    arg.pXorBmp = pXor; \
    arg.pTrimmedBmp = pTrimmed; \
    arg.pMaskBmp = pMask; \
}

#define GENERATE_FEATURES_ARG_INIT(arg, pObj, pMask, pFeatures_) { \
    arg.pCompare = pObj; \
    arg.pMaskBmp = pMask; \
    arg.pFeatures = pFeatures_; \
}

struct GenerateMaskArg {
    IN FeatureCompare* pCompare;
    IN NativeBitmap* pXorBmp;
    IN NativeBitmap* pTrimmedBmp;
    OUT NativeBitmap* pMaskBmp;
};

struct GenerateFeaturesArg {
    IN FeatureCompare* pCompare;
    IN NativeBitmap* pMaskBmp;
    OUT FeatureList* pFeatures;
};

jint FeatureCompare::compare(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, IN const jint scrollDirection, IN Debugger* pDebugger) {
    // xor
    TRACE_BEGIN(pDebugger);
    NativeBitmap xorBmp;
    xorBitmap(bmp1, bmp2, xorBmp);
    TRACE_END(pDebugger, "xor");
    PRINT_XOR_IMAGE(pDebugger, DEBUG_IMG_XOR, xorBmp);

    // mask
    TRACE_BEGIN(pDebugger);
    NativeBitmap mask1, mask2;
    generateMask(xorBmp, bmp1, mask1);
    generateMask(xorBmp, bmp2, mask2);
    xorBmp.release();
    TRACE_END(pDebugger, "mask");
    PRINT_IMAGE(pDebugger, DEBUG_IMG_MASK1, mask1);
    PRINT_IMAGE(pDebugger, DEBUG_IMG_MASK2, mask2);

    // feature
    TRACE_BEGIN(pDebugger);
    FeatureList featureList1, featureList2;
    generateFeatures(mask1, featureList1);
    generateFeatures(mask2, featureList2);
    TRACE_END(pDebugger, "feature");
    PRINT_FEATURE_IMAGE(pDebugger, DEBUG_IMG_FEATURE1, bmp1, featureList1);
    PRINT_FEATURE_IMAGE(pDebugger, DEBUG_IMG_FEATURE2, bmp2, featureList2);

    // compare
    TRACE_BEGIN(pDebugger);
    jint distance = compareFeatures(bmp1, mask1, featureList1, bmp2, mask2, featureList2, scrollDirection);
    mask1.release();
    mask2.release();
    TRACE_END(pDebugger, "compare");

    // return the distance
    return distance;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-stack-address"
jint FeatureCompare::compareWithMultiThread(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, IN const jint scrollDirection, IN Debugger* pDebugger) {
    // xor
    TRACE_BEGIN(pDebugger);
    NativeBitmap xorBmp;
    xorBitmap(bmp1, bmp2, xorBmp);
    TRACE_END(pDebugger, "xor");
    PRINT_XOR_IMAGE(pDebugger, DEBUG_IMG_XOR, xorBmp);

    // mask
    TRACE_BEGIN(pDebugger);
    NativeBitmap mask1, mask2;
    {
        GenerateMaskArg arg1, arg2;
        GENERATE_MASK_ARG_INIT(arg1, this, &xorBmp, (NativeBitmap*)&bmp1, &mask1);
        GENERATE_MASK_ARG_INIT(arg2, this, &xorBmp, (NativeBitmap*)&bmp2, &mask2);
        void *args[] = {&arg1, &arg2};
        MultiThreadTask::start(generateMaskThreadRun, 2, args);
    }
    xorBmp.release();
    TRACE_END(pDebugger, "mask");
    PRINT_IMAGE(pDebugger, DEBUG_IMG_MASK1, mask1);
    PRINT_IMAGE(pDebugger, DEBUG_IMG_MASK2, mask2);

    // feature
    TRACE_BEGIN(pDebugger);
    FeatureList featureList1, featureList2;
    {
        GenerateFeaturesArg arg1, arg2;
        GENERATE_FEATURES_ARG_INIT(arg1, this, &mask1, &featureList1);
        GENERATE_FEATURES_ARG_INIT(arg2, this, &mask2, &featureList2);
        void *args[] = {&arg1, &arg2};
        MultiThreadTask::start(generateFeaturesThreadRun, 2, args);
    }
    TRACE_END(pDebugger, "feature");
    PRINT_FEATURE_IMAGE(pDebugger, DEBUG_IMG_FEATURE1, bmp1, featureList1);
    PRINT_FEATURE_IMAGE(pDebugger, DEBUG_IMG_FEATURE2, bmp2, featureList2);

    // compare
    TRACE_BEGIN(pDebugger);
    jint distance = compareFeatures(bmp1, mask1, featureList1, bmp2, mask2, featureList2, scrollDirection);
    mask1.release();
    mask2.release();
    TRACE_END(pDebugger, "compare");

    // return the distance;
    return distance;
}
#pragma clang diagnostic pop

void FeatureCompare::xorBitmap(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, OUT NativeBitmap& xorBmp) {
    NativeBitmap::create(bmp1, xorBmp);

    jint* pPixels1 = xorBmp.getPixels();
    jint* pPixels2 = bmp2.getPixels();

    jint size = xorBmp.getPixelsCount();
    for (jint i=0; i<size; i++) {
        pPixels1[i] ^= pPixels2[i];
    }
}

void FeatureCompare::generateMask(IN const NativeBitmap& xorBmp, IN const NativeBitmap& bmp, OUT NativeBitmap& mask) {
    NativeBitmap::create(xorBmp, mask);
    jint* pBmpPixels = bmp.getPixels();
    jint* pMaskPixels = mask.getPixels();
    jint width = mask.getWidth();
    jint height = mask.getHeight();
    jint index = 0;
    float count, diff;
    jint i;
    jint pixel, tmpPixel;
    jint xStart, xEnd, yStart, yEnd;
    jint da, dr, dg, db;
    jint tx, ty;

    for (jint y=0; y<height; y++) {
        for (jint x=0; x<width; x++) {
            if (pMaskPixels[index] != COLOR_MASKED) {
                count = diff = 0.0f;
                pixel = bmp.getPixel(x, y);
                xStart = TRIM(x - 1, 0, width - 1); //mask.trimX(x - o); // TODO
                xEnd = TRIM(x + 1, 0, width - 1); //mask.trimX(x + o);
                yStart = TRIM(y - 1, 0, height - 1); //mask.trimY(y - o);
                yEnd = TRIM(y + 1, 0, height - 1); //mask.trimY(y + o);
                for (ty=yStart; ty<=yEnd; ty++) {
                    for (tx=xStart; tx<=xEnd; tx++) {
                        i = XY_TO_INDEX(tx, ty, width); // bmp.xyToIndex(x, y); spend too much CPU time. directly compute without call a method
                        tmpPixel = pBmpPixels[i];
                        if (pMaskPixels[i] == COLOR_MASKED) {
                            // generate color diff
                            da = COLOR_DIFF_A(pixel, tmpPixel);
                            dr = COLOR_DIFF_R(pixel, tmpPixel);
                            dg = COLOR_DIFF_G(pixel, tmpPixel);
                            db = COLOR_DIFF_B(pixel, tmpPixel);

                            diff += da*da + dr*dr + dg*dg + db*db;
                            count ++;
                        }
                    }
                }

                pMaskPixels[index] = ((count > 0) && ((diff / count) < MAX_COLOR_DIFF_POWER2)) ? (jint)COLOR_MASKED : (jint)COLOR_NOT_MASKED;
            }
            index ++;
        }
    }
}

void FeatureCompare::generateFeatures(IN const NativeBitmap& mask, OUT FeatureList& outFeatures) {
    std::queue<jint> queue;
    Feature tmpFeature;
    jint width = mask.getWidth();
    jint height = mask.getHeight();
    jint pixelsCount = mask.getPixelsCount();
    jint* pMaskPixels = mask.getPixels();
    jint x, y, index, tmpIndex;
    for (jint i=0; i<pixelsCount; i++) {
        if (pMaskPixels[i] == COLOR_MASKED) {
            // the point is already masked. ignore it.
            continue;
        }

        queue.push(i);

        // init tmpFeature
        tmpFeature.left = tmpFeature.right = X_FROM_INDEX(i, width);
        tmpFeature.top = tmpFeature.bottom = Y_FROM_INDEX(i, width);
        tmpFeature.pixelCount = 0;

        while (!queue.empty()) {
            // pop an index from queue
            index = queue.front();
            queue.pop();

            // ignore masked point
            if (pMaskPixels[index] == COLOR_MASKED) {
                continue;
            }

            // get x y
            x = X_FROM_INDEX(index, width);
            y = Y_FROM_INDEX(index, width);

            // update feature
            FEATURE_INCLUDE_POINT(tmpFeature, x, y);
            tmpFeature.pixelCount ++;

            // mask the current point
            pMaskPixels[index] = COLOR_MASKED;

            // push points around
            PUSH_POINT_LEFT(x, y, index, width, height, pMaskPixels, queue, tmpIndex);
            PUSH_POINT_TOP(x, y, index, width, height, pMaskPixels, queue, tmpIndex);
            PUSH_POINT_RIGHT(x, y, index, width, height, pMaskPixels, queue, tmpIndex);
            PUSH_POINT_BOTTOM(x, y, index, width, height, pMaskPixels, queue, tmpIndex);
        }

        // save feature
        if (tmpFeature.pixelCount > MIN_FEATURE_PIXEL_COUNT) { // if the rect is too small. It is not valuable
            outFeatures.push_back(tmpFeature);
        }
    }
    //LOGI("==MyTest==", "outFeatures size: %d", outFeatures.size());
}

jint FeatureCompare::compareFeatures(IN const NativeBitmap& bmp1, IN const NativeBitmap& mask1, IN FeatureList& features1, IN const NativeBitmap& bmp2, IN const NativeBitmap& mask2, IN FeatureList& features2, IN const jint scrollDirection) {
    std::sort(features1.begin(), features1.end(), sort_by_s);
    std::sort(features2.begin(), features2.end(), sort_by_s);

    //__gnu_cxx::hash_map<jint, jint> map;
    std::map<jint, jint> map;

    Feature* pF1;
    Feature* pF2;

    float diff;

    jint maxDistance = 0;
    jint maxDistanceCount = 0;
    jint distance;
    jint count; // We can use factor instead if count cause mistake.

    for (FeatureList::iterator i=features1.begin(); i != features1.end(); i++) {
        pF1 = &(*i);
        for(FeatureList::iterator j=features2.begin(); j != features2.end(); j++) {
            pF2 = &(*j);
            //LOGI("==MyTest==", "feature1[%d]: %d, feature2[%d]: %d", (i - features1.begin()), pF1->pixelCount, (j - features2.begin()), pF2->pixelCount);
            if (pF1->left == pF2->left && pF1->right == pF2->right && std::abs(FEATURE_HEIGHT(*pF1) - FEATURE_HEIGHT(*pF2)) < MAX_FEATURE_HEIGHT_DIFF && (pF1->top >= pF2->top)) {
                // width & height of two rectangle should not different too much.
                diff = diffRect(bmp1, mask1, *pF1, bmp2, mask2, *pF2);

                // save result and save the max
                if (diff < MIN_FEATURE_RECT_DIFF) {
                    // generate distance and save int map
                    distance = FEATURE_DISTANCE(*pF1, *pF2);
                    //LOGI("==MyTest==", "distance: %d, [%d,%d,%d,%d] [%d,%d,%d,%d]", distance, pF1->left, pF1->top, pF1->right, pF1->bottom, pF2->left, pF2->top, pF2->right, pF2->bottom);

                    // Feature's position y should be grater than distance. Or it should be a false match.
                    // Distance should also match the scroll direction
                    if (pF1->top > distance && checkScrollDirection(scrollDirection, distance)) {
                        //__gnu_cxx::hash_map<jint, jint>::iterator it = map.find(distance);
                        std::map<jint, jint>::iterator it = map.find(distance);
                        if (it != map.end()) {
                            (*it).second++;
                            count = (*it).second;
                        } else {
                            count = 1;
                            map[distance] = count;
                        }

                        // save the max distance result
                        if (count > maxDistanceCount) {
                            maxDistance = distance;
                            maxDistanceCount = count;
                        }
                    }
                }
            } else if ((pF1->pixelCount - pF2->pixelCount) >= MAX_FEATURE_PIXEL_COUNT_DIFF) {
                // unnecessary to compare next rectangles.
                break;
            }
        }
    }
//    for (std::unordered_map<jint, jint>::iterator i = map.begin(); i != map.end(); i++) {
//        LOGI("==MyTest==", "map [%d, %d]", i->first, i->second);
//    }

    return maxDistance;
}

#if 1
inline void FeatureCompare::findRect(IN const jint index, INOUT std::queue<jint>& queue, INOUT const NativeBitmap& mask, INOUT Feature& feature) {
    jint* pMaskPixels = mask.getPixels();
    jint width = mask.getWidth();
    jint height = mask.getHeight();
    jint x = X_FROM_INDEX(index, width);
    jint y = Y_FROM_INDEX(index, width);

    // update rect
    FEATURE_INCLUDE_POINT(feature, x, y);

    // mask the current point
    pMaskPixels[index] = COLOR_MASKED;
    feature.pixelCount ++;

    // check point around
    int nx, ny, i;
    nx = x - 1;
    if (nx >= 0 && nx < width) {
        i = index - 1;
        if (pMaskPixels[i] != COLOR_MASKED) {
            queue.push(i);
        }
    }
    nx = x + 1;
    if (nx >= 0 && nx < width) {
        i = index + 1;
        if (pMaskPixels[i] != COLOR_MASKED) {
            queue.push(i);
        }
    }
    ny = y - 1;
    if (ny >= 0 && ny < height) {
        i = index - width;
        if (pMaskPixels[i] != COLOR_MASKED) {
            queue.push(i);
        }
    }
    ny = y + 1;
    if (ny >= 0 && ny < height) {
        i = index + width;
        if (pMaskPixels[i] != COLOR_MASKED) {
            queue.push(i);
        }
    }
}
#else
void FeatureCompare::findRect(IN const jint index, INOUT std::queue<jint>& queue, INOUT const NativeBitmap& mask, INOUT Feature& feature) {
    jint* pMaskPixels = mask.getPixels();
    jint width = mask.getWidth();
    jint height = mask.getHeight();
    jint x = X_FROM_INDEX(index, width);
    jint y = Y_FROM_INDEX(index, width);

    jint minX, maxX, minY, maxY;
    jint minXIndex, maxXIndex, minYIndex, maxYIndex;

    // mask current point
    pMaskPixels[index] = COLOR_MASKED;
    feature.pixelCount ++;

    for (minX=x-1,minXIndex=index-1; minX>=0 && pMaskPixels[minXIndex] != COLOR_MASKED; minX--, minXIndex--) {
        pMaskPixels[minXIndex] = COLOR_MASKED;
        feature.pixelCount ++;
    }
    minX ++;
    for (maxX=x+1,maxXIndex=index+1; maxX<width && pMaskPixels[maxXIndex] != COLOR_MASKED; maxX++, maxXIndex++) {
        pMaskPixels[maxXIndex] = COLOR_MASKED;
        feature.pixelCount ++;
    }
    maxX --;
    for (minY=y-1,minYIndex=index-width; minY>=0 && pMaskPixels[minYIndex] != COLOR_MASKED; minY--, minYIndex-=width) {
        pMaskPixels[minYIndex] = COLOR_MASKED;
        feature.pixelCount ++;
    }
    minY ++;
    for (maxY=y+1,maxYIndex=index+width; maxY<height && pMaskPixels[maxYIndex] != COLOR_MASKED; maxY--, maxYIndex+=width) {
        pMaskPixels[maxYIndex] = COLOR_MASKED;
        feature.pixelCount ++;
    }
    maxY --;

    // update rect
    FEATURE_INCLUDE_POINT(feature, minX, minY);
    FEATURE_INCLUDE_POINT(feature, maxX, maxY);

    // mark all pixels in the rect
    // TODO

    // check point around
    int nx, ny, i;
    nx = x - 1;
    ny = y - 1;
    if (nx >= minX && nx <= maxX && ny >= minY && ny <= maxY) {
        i = index - width - 1;
        if (pMaskPixels[i] != COLOR_MASKED) {
            queue.push(i);
        }
    }
    nx = x + 1;
    ny = y - 1;
    if (nx >= minX && nx <= maxX && ny >= minY && ny <= maxY) {
        i = index - width + 1;
        if (pMaskPixels[i] != COLOR_MASKED) {
            queue.push(i);
        }
    }
    nx = x - 1;
    ny = y + 1;
    if (nx >= minX && nx <= maxX && ny >= minY && ny <= maxY) {
        i = index + width - 1;
        if (pMaskPixels[i] != COLOR_MASKED) {
            queue.push(i);
        }
    }
    nx = x + 1;
    ny = y + 1;
    if (nx >= minX && nx <= maxX && ny >= minY && ny <= maxY) {
        i = index + width + 1;
        if (pMaskPixels[i] != COLOR_MASKED) {
            queue.push(i);
        }
    }
}
#endif

bool FeatureCompare::sort_by_s(const Feature& obj1,const Feature& obj2) {
    return obj1.pixelCount > obj2.pixelCount;
}

inline float FeatureCompare::diffRect(IN const NativeBitmap& bmp1, IN const NativeBitmap& mask1, IN const Feature& f1, IN const NativeBitmap& bmp2, IN const NativeBitmap& mask2, IN const Feature& f2) {
    jint diff = 0;
    jint maxDiff = (jint)(f1.pixelCount * MIN_FEATURE_RECT_DIFF);
    jint width = bmp1.getWidth();
    jint* pPixels1 = bmp1.getPixels();
    jint* pPixels2 = bmp2.getPixels();
    jint* pMask1 = mask1.getPixels();
    jint* pMask2 = mask2.getPixels();
    jint index1, index2;
    for (jint y1=f1.top,y2=f2.top; y1<=f1.bottom && y2<=f2.bottom; y1++,y2++) {
        for (jint x1=f1.left, x2=f2.left; x1<=f1.right && x2<=f2.right; x1++, x2++) { // TODO
            index1 = XY_TO_INDEX(x1, y1, width);
            index2 = XY_TO_INDEX(x2, y2, width);
            if (pMask1[index1] != COLOR_MASKED) {
                if (pMask2[index2] == COLOR_MASKED || pPixels1[index1] != pPixels2[index2]) {
                    diff ++;

                    if (diff > maxDiff) {
                        // already greater than max diff. unnecessary to check more.
                        return 1.0f;
                    }
                }
            }
        }
    }

    return ((float)diff) / f1.pixelCount;
}

void* FeatureCompare::generateMaskThreadRun(void* arg) {
    GenerateMaskArg* pArg = (GenerateMaskArg*) arg;
    pArg->pCompare->generateMask(*(pArg->pXorBmp), *(pArg->pTrimmedBmp), *(pArg->pMaskBmp));
    return NULL;
}

void* FeatureCompare::generateFeaturesThreadRun(void* arg) {
    GenerateFeaturesArg* pArg = (GenerateFeaturesArg*) arg;
    pArg->pCompare->generateFeatures(*(pArg->pMaskBmp), *(pArg->pFeatures));
    return NULL;
}

bool FeatureCompare::checkScrollDirection(jint direction, jint distance) {
    switch (direction) {
        case SCROLL_DIRECTION_UP:
            return distance <= 0;
        case SCROLL_DIRECTION_DOWN:
            return distance >= 0;
        case SCROLL_DIRECTION_UNKNOWN:
        default:
            return true;
    }
}