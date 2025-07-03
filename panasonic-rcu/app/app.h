/**
 * @file app.h
 * @author likeliang (likeliang@seneasy.com)
 * @brief app_main
 * @version 0.1
 * @date 2025-03-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef APP_APP_H_
#define APP_APP_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "pan216_ext.h"
#include "control_protocol.h"
#include "control_state.h"


typedef uint8_t ModelType;


    typedef enum
    {
        A7535 = 0x01,
        XN297L
    } ChipMode;

    typedef struct _app
    {
        uint8_t unique_id[4];
        ChipMode chipMode;
        ModelType modelType;
    } app_t;

    extern app_t app;
    extern const app_t app_default;
    extern const RFConfig_t a7535_gRfConfig;
    extern const RFConfig_t xn297l_gRfConfig;

    void app_init();
    void app_proc();
    void app_change_chipMode(ChipMode cm);
#define APP_DEFAULT_INFO {                 \
    .unique_id = {0xdd, 0xcc, 0xdd, 0xcc}, \
    .chipMode = XN297L,                    \
    .modelType = MODEL_TYPE}

#ifdef __cplusplus
}
#endif

#endif // APP_APP_H_
