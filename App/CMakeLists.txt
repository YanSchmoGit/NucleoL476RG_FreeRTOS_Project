# App/CMakeLists.txt
target_sources(${CMAKE_PROJECT_NAME}
        PRIVATE
        app.c
        Tasks/readSensorData_task.c
        Tasks/showSensorData_task.c
        Tasks/showErrorData_task.c
        Tasks/tasks.c
        RTOS/queues.c
        RTOS/mutex.c
)

target_include_directories(${CMAKE_PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Tasks
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOS

)