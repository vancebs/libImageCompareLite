//
// Created by vance on 2016/8/21.
//

#include "HashCompare.h"

#define TAG "HashCompare"

#define HASH_OFFSET_LEFT 50
#define HASH_OFFSET_RIGHT 100

#define __DEBUG 1

jint HashCompare::compare(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, IN Debugger* pDebugger) {
    // generate hash
    TRACE_BEGIN(pDebugger);
    jint height = bmp1.getHeight();
    jint hash1[height];
    jint hash2[height];
    generateHash(bmp1, hash1);
    generateHash(bmp2, hash2);
    TRACE_END(pDebugger, "generate hash");
    PRINT_HASH_IMAGE(pDebugger, "hash1.png", bmp1, hash1);
    PRINT_HASH_IMAGE(pDebugger, "hash2.png", bmp2, hash2);

    // generate xor
    TRACE_BEGIN(pDebugger);
    jint hashxor[height];
    xorHashCode(height, hash1, hash2, hashxor);
    TRACE_END(pDebugger, "xor");
    PRINT_HASH_MASK_IMAGE(pDebugger, "xor1.png", bmp1, hashxor);
    PRINT_HASH_MASK_IMAGE(pDebugger, "xor2.png", bmp2, hashxor);

    // generate mask
    TRACE_BEGIN(pDebugger);
    jint hashmask1[height];
    jint hashmask2[height];
    generateMask(height, hashxor, hash1, hashmask1);
    generateMask(height, hashxor, hash2, hashmask2);
    TRACE_END(pDebugger, "mask");
    PRINT_HASH_MASK_IMAGE(pDebugger, "mask1.png", bmp1, hashmask1);
    PRINT_HASH_MASK_IMAGE(pDebugger, "mask2.png", bmp2, hashmask2);

    // compare
    TRACE_BEGIN(pDebugger);
    jint distance = compareMask(height, hash1, hashmask1, hash2, hashmask2);
    TRACE_END(pDebugger, "compare");

    return distance;
}

inline jint HashCompare::compareMask(IN jint len, IN const jint* hash1, IN const jint* maskHash1, IN const jint* hash2, IN const jint* maskHash2) {
    jint maxDistance = 0;
    jint maxSameCount = 0;
    jint distance;
    jint sameCount;

    for (jint i=0; i<len; i++) {
        distance = i;
        sameCount = 0;
        for (jint h1 = i, h2 = 0; h2 < len - i; h1++, h2++) {
            if (maskHash1[h1] != 0 && maskHash2[h2] != 0 && hash1[h1] == hash2[h2]) {
                sameCount++;
            }
        }

#if __DEBUG
        LOGD("==MyTest==", "distance: %d, sameCount: %d", distance, sameCount);
#endif

        // save the max
        if (maxSameCount < sameCount) {
            maxDistance = distance;
            maxSameCount = sameCount;
        }

        // check whether it is necessary to compare next distance
        if (maxSameCount > (len - distance)) {
            // unnecessary to compare more. the next distance is less than the max same count
            break;
        }
    }

#if __DEBUG
    LOGD("==MyTest==", "maxDistance: %d, maxSameCount: %d", maxDistance, maxSameCount);
#endif

    return maxDistance;
}

inline void HashCompare::generateHash(IN const NativeBitmap& bmp, OUT jint* hash) {
    jint* pPixels = bmp.getPixels() + HASH_OFFSET_LEFT;
    jint height = bmp.getHeight();
    jint width = bmp.getWidth();
    jint lineLen = HASH_OFFSET_RIGHT - HASH_OFFSET_LEFT + 1;

    for (jint i=0; i<height; i++) {
        hash[i] = generateHashLine(pPixels, lineLen);
        pPixels += width;
    }
}

inline void HashCompare::xorHashCode(IN jint len, IN const jint* hash1, IN const jint* hash2, OUT jint* xorHash) {
    for (int i=0; i<len; i++) {
        xorHash[i] = hash1[i] ^ hash2[i];
    }
}

inline void HashCompare::generateMask(IN jint len, IN const jint* xorHash, IN const jint* hash, OUT jint* maskHash) {
    memcpy(maskHash, xorHash, len * sizeof(jint));
    jint prev, next;
    for (int i=0; i<len; i++) {
        if (maskHash[i] != 0) {
            prev = i - 1;
            next = i + 1;
            if (prev >= 0 && prev < len && maskHash[prev] == 0 && hash[i] == hash[prev]){
                maskHash[i] = 0;
                continue;
            }
            if (next >= 0 && next < len && maskHash[next] == 0 && hash[i] == hash[next]) {
                maskHash[i] = 0;
                continue;
            }
        }
    }
}

inline jint HashCompare::generateHashLine(IN const jint* pPixels, IN const jint len) {
    jint h = 1;
    for (jint i=0; i<len; i++) {
        h = 31 * h + pPixels[i];
    }
    return h;
}
