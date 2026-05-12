#include <stdint.h>

/* STM32F407 Register Addresses */
#define RCC_BASE        0x40023800
#define GPIOD_BASE      0x40020C00
#define SYSTICK_BASE    0xE000E010

/* RCC */
#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE   + 0x30))

/* GPIOD */
#define GPIOD_MODER     (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_ODR       (*(volatile uint32_t *)(GPIOD_BASE + 0x14))

/* SysTick registers */
#define SYSTICK_CTRL    (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define SYSTICK_LOAD    (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define SYSTICK_VAL     (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))

/* LED pins */
#define GPIODEN         (1 << 3)
#define PD12_OUTPUT     (1 << 24)
#define PD13_OUTPUT     (1 << 26)
#define GREEN_LED       (1 << 12)
#define ORANGE_LED      (1 << 13)

/* SysTick runs at 168MHz (system clock) */
#define SYSTICK_CLOCK   168000000UL

/* ── SysTick functions ───────────────────── */

void systick_init(void)
{
    /* Reload value for 1ms tick at 168MHz */
    /* LOAD = (clock / 1000) - 1           */
    SYSTICK_LOAD = (SYSTICK_CLOCK / 1000) - 1;  /* 167999 */

    /* Clear current value */
    SYSTICK_VAL  = 0;

    /* Enable SysTick:
       Bit 0 = ENABLE    (1 = counter enabled)
       Bit 1 = TICKINT   (0 = no interrupt, polling mode)
       Bit 2 = CLKSOURCE (1 = use processor clock 168MHz) */
    SYSTICK_CTRL = (1 << 2) | (1 << 0);
}

void delay_ms(uint32_t ms)
{
    for(uint32_t i = 0; i < ms; i++)
    {
        /* Wait until COUNTFLAG (bit 16) is set */
        /* COUNTFLAG is set when counter reaches 0 */
        while(!(SYSTICK_CTRL & (1 << 16)));
    }
}

/* ── Main ───────────────────────────────── */

int main(void)
{
    /* 1. Enable GPIOD clock */
    RCC_AHB1ENR |= GPIODEN;

    /* 2. Set PD12 and PD13 as output */
    GPIOD_MODER |= PD12_OUTPUT | PD13_OUTPUT;

    /* 3. Initialize SysTick for 1ms tick */
    systick_init();

    /* 4. Blink LEDs alternately using precise delay */
    while(1)
    {
        /* PD12 ON, PD13 OFF */
        GPIOD_ODR |=  GREEN_LED;
        GPIOD_ODR &= ~ORANGE_LED;
        delay_ms(500);              /* precise 500ms! */

        /* PD12 OFF, PD13 ON */
        GPIOD_ODR &= ~GREEN_LED;
        GPIOD_ODR |=  ORANGE_LED;
        delay_ms(500);              /* precise 500ms! */
    }

    return 0;
}