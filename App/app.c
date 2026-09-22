//
// Created by yan on 7/26/26.
//

#include "app.h"
#include "../Interfaces/Inc/Spi.h"




void appInit(void)
{

    // Create Queues
    createQueues();

    // Create tasks
    createTasks();


}
