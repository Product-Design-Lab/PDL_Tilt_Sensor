#pragma once

#include "LSM6DS3.h"
#include "MovingAverage.h"
#include "FreeRTOS.h"
#include "task.h"

//for lagacy compatibility
enum IMU_DEBUG_STATUS {
    IMU_DEBUG_STATUS_NONE = 0,
    IMU_DEBUG_STATUS_RAW = 1,
    IMU_DEBUG_STATUS_FILTERED = 2,
    IMU_DEBUG_STATUS_ANGLE = 3,
    IMU_DEBUG_THRESHOLD = 4,
    IMU_DEBUG_AZUMITH = 5,
    IMU_DEBUG_MAX
};

class PDL_Tilt_Sensor {
public:

    enum IMU_DEBUG_STATUS {
        DEBUG_NONE = 0,
        DEBUG_RAW = 1,
        DEBUG_FILTERED = 2,
        DEBUG_ANGLE = 3,
        DEBUG_THRESHOLD = 4,
        DEBUG_AZUMITH = 5,
        DEBUG_MAX
    };

    using IMUEventCallback = void (*)(); // Type alias for callback functions

    typedef struct {
        float azumith;
        float azumith_magnitude;
    } AzumithCbsContext_t;

    using IMUAzumithUpdateCallback = void (*)(AzumithCbsContext_t *context);

    PDL_Tilt_Sensor();
    void init(uint32_t priority = 1);
    void deinit();
    void pause();
    void resume();
    void calibrate();
    float getAngleX() const;
    float getAngleY() const;
    bool isVertical() const;
    float getAzumith() const;
    float getAzumithMagnitude() const;

    void setDebugStatus(uint8_t status);
    void setVerticalThresholds(float x_lower, float x_upper, float y_lower, float y_upper);
    void setLoopDelay(uint32_t delay_ms);
    void setSampleSize(uint16_t samples);

    void setTiltedCallback(IMUEventCallback callback);
    void setLevelCallback(IMUEventCallback callback);
    void setAzumithUpdateCallback(IMUAzumithUpdateCallback callback);// set null to disable callback

private:
    static void imuTask(void *pvParameters);
    void processIMUData();
    void checkTiltStatus();
    void checkAzumith();

    LSM6DS3 imu;
    MovingAverage<int16_t, 8> ax_filter;
    MovingAverage<int16_t, 8> ay_filter;
    MovingAverage<int16_t, 8> az_filter;
    TaskHandle_t imu_task_handle;

    float angle_x;
    float angle_y;
    float azumith; // angle between x axis a projected vector on x-y plane
    float azumith_magnitude;// magnitude of the projected vector on x-y plane

    float x_lower_threshold;
    float x_upper_threshold;
    float y_lower_threshold;
    float y_upper_threshold;
    bool is_vertical;
    bool was_vertical;

    uint32_t loop_delay;
    uint8_t debug_status;

    float ax_offset;
    float ay_offset;

    TickType_t xLastWakeTime;

    IMUEventCallback tilted_callback;
    IMUEventCallback level_callback;
    IMUAzumithUpdateCallback azumith_update_callback;
};
