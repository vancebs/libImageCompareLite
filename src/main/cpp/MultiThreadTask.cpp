//
// Created by vance on 2016/8/21.
//

#include "MultiThreadTask.h"
#include <pthread.h>

void MultiThreadTask::start(ThreadRun threadRun, int argc, void** args) {
    pthread_t threads[argc];
    void* result;

    // create thread
    for (int i=0; i<argc; i++) {
        pthread_create(threads + i, NULL, threadRun, *(args + i));
    }

    // wait for thread finish
    for (int i=0; i<argc; i++) {
        pthread_join(*(threads + i), &result);
    }
}
