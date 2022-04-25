#include "millis.h"
#include "stm8s_tim4.h"

__IO uint32_t current_millis = 0; //--IO: volatile read/write

const MILIS_Module milis = {
    .init = milis_init,
    .get = milis_get,
};

void milis_init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // FREQ MCU 16MHz
    TIM4_DeInit;                                   // DeInit of TIM4
    TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);    // Inicialization of timer
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);              // Set timer to 0
    TIM4_ARRPreloadConfig(ENABLE);                 // Enable change of top
    onInterrupt();                                 // enable interupt
    TIM4_Cmd(ENABLE);                              // start of Timer
}

uint32_t millis_get(void)
{
    return current_millis;
}

// Interupt event, happen every 1 ms
onInterrupt(TIM4_UPD_OVF_IRQHandler, 23)
{
    // increase 1, for millis() function
    current_millis++;

    TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}