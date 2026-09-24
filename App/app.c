//
// Created by yan on 7/26/26.
//

#include "app.h"



void appInit(void)
{

    // Create queues
    createQueues();

    // Create mutexes
    createMutexes();

    // Create tasks
    createTasks();
}
