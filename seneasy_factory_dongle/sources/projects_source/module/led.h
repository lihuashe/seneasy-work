#ifndef _LED_H_
#define _LED_H_

#define LED_TIMER_PERIOD                    (50)
#define LED_INDEX(n)                        ((n) - 1)

#define LED_PIN_NUM                            ( EN_GPIO_31 )
#define LED_STATE_ON_LEVEL                     ( GPIO_LEVEL_LO )

#define LED_TIMER                               (STIM1)
#define LED_TIMER_IRQ                           (STIM1_IRQ2)
#define LED_TIMER_CH                            (STIM_CH2)
#define LED_TIMER_INT_MSK                       (STIM_CH2_INT_MATCH)
#define LED_TIMER_STIM_IRQ_HANDLER              STIM1_IRQ2_Handler

typedef enum
{
    EN_GPIO_0 = 0,
    EN_GPIO_1 = 1,
    EN_GPIO_2 = 2,
    EN_GPIO_3 = 3,
    EN_GPIO_4 = 4,
    EN_GPIO_5 = 5,
    EN_GPIO_6 = 6,
    EN_GPIO_7 = 7,
    EN_GPIO_8 = 8,
    EN_GPIO_9 = 9,
    EN_GPIO_10 = 10,
    EN_GPIO_11 = 11,
    EN_GPIO_12 = 12,
    EN_GPIO_13 = 13,
    EN_GPIO_14 = 14,
    EN_GPIO_15 = 15,
    EN_GPIO_16 = 16,
    EN_GPIO_17 = 17,
    EN_GPIO_18 = 18,
    EN_GPIO_19 = 19,
    EN_GPIO_20 = 20,
    EN_GPIO_21 = 21,
    EN_GPIO_22 = 22,
    EN_GPIO_23 = 23,
    EN_GPIO_24 = 24,
    EN_GPIO_25 = 25,
    EN_GPIO_26 = 26,
    EN_GPIO_27 = 27,
    EN_GPIO_28 = 28,
    EN_GPIO_29 = 29,
    EN_GPIO_30 = 30,
    EN_GPIO_31 = 31,
    EN_GPIO_32 = 32,
    EN_GPIO_33 = 33,
    EN_GPIO_34 = 34,
    EN_GPIO_35 = 35,
    EN_GPIO_36 = 36,
    EN_GPIO_37 = 37,
    EN_GPIO_38 = 38,
    EN_GPIO_39 = 39,
    EN_GPIO_40 = 40,
    EN_GPIO_41 = 41,
    EN_GPIO_42 = 42,
    EN_GPIO_43 = 43,
    EN_GPIO_44 = 44,
    EN_GPIO_45 = 45,
    EN_GPIO_46 = 46,
    EN_GPIO_47 = 47,
    EN_GPIO_48 = 48,
    EN_GPIO_49 = 49,
    EN_GPIO_50 = 50,
    EN_GPIO_51 = 51,
    EN_GPIO_52 = 52,
    EN_GPIO_53 = 53,
    EN_GPIO_54 = 54,
    EN_GPIO_55 = 55,
    EN_GPIO_56 = 56,
    EN_GPIO_57 = 57,
    EN_GPIO_58 = 58,
    EN_GPIO_59 = 59,
    EN_GPIO_60 = 60,
    EN_GPIO_61 = 61,
    EN_GPIO_62 = 62,
    EN_GPIO_63 = 63,
    EN_GPIO_NULL = 0xFF,
} EN_GPIO_NUM_T;

typedef enum
{
    EN_LED_OFF              = 0,
    EN_LED_ON               = 1,
} enLedOnOff_t;

typedef enum
{
    TIMEROUT_STOP,
    MANUAL_STOP,
} LedEndResultDef_t;

typedef enum
{
    EN_LED_STATE_ON                     = 0x01,
    EN_LED_STATE_BLINK                  = 0x02,
    EN_LED_STATE_ON_WITH_TIMEOUT        = 0x03,
    EN_LED_STATE_BLINK_WITH_TIMEOUT     = 0x04,
    EN_LED_STATE_OFF                    = 0x05,
    EN_LED_STATE_DISABLE                = 0x06,
} enLedState_t;

typedef struct
{
    EN_GPIO_NUM_T enLedPinNum;
    EN_GPIO_LEVEL_T enLedOnLevel;
} stLed_Cfg_t;

typedef struct
{
    EN_GPIO_NUM_T enLedPinNum;
    EN_GPIO_LEVEL_T enLedOnLevel;
    enLedState_t enLedLastState;
    enLedState_t enLedCurrentState;
    uint32_t u32LedOnTimeTimes;
    uint32_t u32LedBlinkCount;
    uint16_t u16LedBlinkTimes;
    uint16_t u16LedBlinkFreq;

} stLedParam_t;

typedef void (* led_event_callback_t )( uint8_t u8Event, uint8_t* pu8Data, uint16_t u16DataLen);

EN_ERR_STA_T led_init( led_event_callback_t led_event_callback_cb );
EN_ERR_STA_T led_on( void );
EN_ERR_STA_T led_on_with_timeout( uint32_t u32TimeMs);
EN_ERR_STA_T led_blink(uint16_t u16Freq);
EN_ERR_STA_T led_blink_with_timeout(uint16_t u16Freq, uint16_t u16FlashTimes);
EN_ERR_STA_T led_off( void );


#endif