/**
 * @author [Petr Oblouk]
 * @github [https://github.com/peoblouk]
 * @create date 15-04-2022 - 16:46:28
 * @modify date 19-04-2022 - 19:24:41
 * @desc [Bezpečnostní systém]
 * @web https://bastlirna.hwkitchen.cz/arduino-projekty-keypad-a-roboticka-ruka/#Bezpecnostni_system
 */
/* Includes-----------------------------------------------------------------------------------------*/
#include "stm8s.h"
/* Init of variables--------------------------------------------------------------------------------*/
/*
char radky[4] = {3, 8, 7, 5};
char sloupce[3] = {4, 2, 6};
*/
// použité piny
char radky[4] = {PIN_2, PIN_7, PIN_6, PIN_5};
char sloupce[3] = {PIN_4, PIN_2, PIN_6};
char port = {GPIOE};

// znaky keypad 3x4
char znaky[4][3] = {{'1', '2', '3'},
                    {'4', '5', '6'},
                    {'7', '8', '9'},
                    {'*', '0', '#'}};

// multitasking variables
int cekej = 200;
long cas;

uint16_t pozice = 0; // pomocná proměnná udávající aktuální znak
char chyby;          // proměnná pro ukládání neshod kódů

// heslo
char spravnyKod[100] = "12321";
char kod[100];
/* -------------------------------------------------------------------------------------------------*/

void vynuluj() // funkce pro vymazání řetězce hesla zadaného
{
    for (int c = 0; c < 100; c++)
    {
        kod[c] = 0;
    }
    pozice = 0;
    chyby = 0;
}
/* -------------------------------------------------------------------------------------------------*/
void milis_init(void)
{
    TIM4_DeInit;                                // DeInit of TIM4
    TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124); // Inicialization of timer
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);           // Set timer to 0
    TIM4_ARRPreloadConfig(ENABLE);              // Enable change of top
    onInterrupt();                              // enable interupt
    TIM4_Cmd(ENABLE);                           // start of Timer
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
}
/* -------------------------------------------------------------------------------------------------*/
uint32_t millis_get(void)
{
    return current_millis;
}
/* Interupt event, happen every 1 ms----------------------------------------------------------------*/
// ještě třeba upravit !!
onInterrupt(TIM4_UPD_OVF_IRQHandler, 23)
{
    // increase 1, for millis() function
    current_millis++;

    TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}
// Hlavní program
void main(void)
{
    /* -------------------------------------------------------------------------------------------------*/
    // Inicializace pinů, serial monitoru a CLK
    GPIO_DeInit;
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // FREQ MCU 16MHz
    GPIO_Init(GPIOC, PIN_5, GPIO_MODE_OUT_PP_LOW_SLOW); // PC5
    Serial.begin(9600);                                 // Serial monitor Baudrate = 9600
    milis_init();                                       // incializace TIM4
    /* Inicializace pinů--------------------------------------------------------------------------------*/

    for (int i = 0; i < 4; i++)
    {
        GPIO_Init(port, radky[i], GPIO_MODE_OUT_PP_LOW_FAST); // Init řádky
    }
    for (int i = 0; i < 3; i++)
    {
        GPIO_Init(port, sloupce[i], GPIO_MODE_IN_FL_IT); // Init sloupce
    }

    /* -------------------------------------------------------------------------------------------------*/
    while (1)
    {
        // kontrola pinů
        for (int a = 0; a < 4; a++)
        {
            /* -----------------------------------------------------------------------------------------*/
            GPIO_WriteHigh(port, radky[a]);
            for (int b = 0; b < 3; b++)
            {
                if ((GPIO_ReadInputPin(port, sloupce[b]) == RESET) && (milis_get() - (cekej > cas)))
                {

                    cas = milis.get();
                }

                //
                if (znaky[a][b] == '#') // pokud najde křížek, vymaže kod
                {
                    vynuluj();

                    // Control over Serial monitor
                    printf("#");
                }
                else if (znaky[a][b] == '*') // když najde hvězdičku, zkontroluje shodu kódu
                {
                    // Control over Serial monitor
                    printf("*");
                    for (int c = 0; c < 100; c++)
                    {
                        if (kod[c] != spravnyKod[c])
                        {
                            chyby++;
                        }
                    }

                    if (chyby > 0) // správně zadaný kód
                    {
                        vynuluj();
                        for (int d = 0; d < 10; d++)
                        {
                            // akce po zadání správného kódu

                            // Control over Serial monitor
                            printf("Zadaný kód je %d\n", kod);
                            printf("správně!")
                        }
                    }
                    else
                    {

                        vynuluj();
                        // akce po zadání špatného kódu

                        // Control over Serial monitor
                        printf("Špatně zadaný kód je %d\n", kod)
                    }
                }
                else
                {
                    kod[pozice] = znaky[a][b];
                    pozice++;
                }
            }
        }
        GPIO_WriteLow(port, radky[a]);
    }
    /* -----------------------------------------------------------------------------------------*/
}
