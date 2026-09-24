//
// Created by yan on 9/22/26.
//

#include "mutex.h"
#include "cmsis_os.h"


osMutexId_t lcdMutexHandle;

const osMutexAttr_t lcdMutex_attributes = {
    .name = "lcdMutex"
  };

void createMutexes()
{
        lcdMutexHandle = osMutexNew(&lcdMutex_attributes);
};