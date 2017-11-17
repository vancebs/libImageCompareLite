//
// Created by vance on 2016/8/6.
//

#ifndef NATIVEIMAGEMERGE_INC_H
#define NATIVEIMAGEMERGE_INC_H

#include <stdio.h>
#include <jni.h>
#include <vector>

#define DEBUG_TAG "ImageCompareLite"

#define IN
#define OUT
#define INOUT

#define SCROLL_DIRECTION_UNKNOWN  0
#define SCROLL_DIRECTION_UP  1
#define SCROLL_DIRECTION_DOWN  2

//#define _DEBUG_IMAGE
#define _DEBUG_PERFORMANCE
#define _DEBUG_LOG

#include "log.h"

#define NATIVE_BITMAP(pBmp) (*((NativeBitmap*)(pBmp)))




#endif //NATIVEIMAGEMERGE_INC_H
