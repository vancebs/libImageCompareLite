//
// Created by vance on 2016/8/21.
//

#ifndef NATIVEIMAGEMERGE_MULTITHREADTASK_H
#define NATIVEIMAGEMERGE_MULTITHREADTASK_H

typedef void*(*ThreadRun)(void*);

class MultiThreadTask {
public:
    static void start(ThreadRun threadRun, int argc, void** args);
};


#endif //NATIVEIMAGEMERGE_MULTITHREADTASK_H
