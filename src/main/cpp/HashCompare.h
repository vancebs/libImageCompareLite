//
// Created by vance on 2016/8/21.
//

#ifndef NATIVEIMAGEMERGE_HASHCOMPARE_H
#define NATIVEIMAGEMERGE_HASHCOMPARE_H

#include "inc.h"
#include "NativeBitmap.h"
#include "Debugger.h"

class HashCompare {
public:
    jint compare(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, IN Debugger* pDebugger = NULL);

private:
    void xorHashCode(IN jint len, IN const jint* hash1, IN const jint* hash2, OUT jint* xorHash);
    void generateMask(IN jint len, IN const jint* xorHash, IN const jint* hash, OUT jint* maskHash);
    jint compareMask(IN jint len, IN const jint* hash1, IN const jint* maskHash1, IN const jint* hash2, IN const jint* maskHash2);

    static void generateHash(IN const NativeBitmap& bmp, OUT jint* hash);
    static jint generateHashLine(IN const jint* pPixels, IN const jint len);
};


#endif //NATIVEIMAGEMERGE_HASHCOMPARE_H
