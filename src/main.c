#include <stdint.h>
#include "../Driver/Mco/Mco.h"
#include "../Driver/Clock/Clock.h"

/* ── Register addresses ──────────────────────────────────────── */
#define RCC_BASE        0x40023800UL
#define GPIOD_BASE      0x40020C00UL
#define SYSTICK_BASE    0xE000E010UL

#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE   + 0x30))
#define GPIOD_MODER     (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_ODR       (*(volatile uint32_t *)(GPIOD_BASE + 0x14))
#define SYSTICK_CTRL    (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define SYSTICK_LOAD    (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define SYSTICK_VAL     (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))

/* ── Bit definitions ─────────────────────────────────────────── */
#define GPIODEN         (1U << 3)
#define PD12_OUTPUT     (1U << 24)
#define PD13_OUTPUT     (1U << 26)
#define GREEN_LED       (1U << 12)
#define ORANGE_LED      (1U << 13)
#define SYSTICK_CLOCK   168000000UL

/* ── SysTick ─────────────────────────────────────────────────── */
void systick_init(void)
{
    /* Step 1 — clock first, everything depends on it              */
    SystemClock_Config();   // SYSCLK = 168 MHz

    /* Step 2 — MCO after clock is stable                          */
    MCO_Config();           // MCO2 = SYSCLK/4 = 42 MHz on PC9

    /* Step 3 — SysTick reload at correct 168 MHz                  */
    SYSTICK_LOAD = (SYSTICK_CLOCK / 1000) - 1;   // 167999
    SYSTICK_VAL  = 0;
    SYSTICK_CTRL = (1U << 2) | (1U << 0);         // CLKSOURCE + ENABLE
}

void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++)
    {
        while (!(SYSTICK_CTRL & (1U << 16)));   // wait COUNTFLAG
    }
}

/* ── Main ────────────────────────────────────────────────────── */
int main(void)
{
    /* 1. Enable GPIOD clock                                       */
    RCC_AHB1ENR |= GPIODEN;

    /* 2. PD12 and PD13 as output                                  */
    GPIOD_MODER |= PD12_OUTPUT | PD13_OUTPUT;

    /* 3. Init SysTick (clock → MCO → SysTick inside)             */
    systick_init();

    /* 4. Blink LEDs                                               */
    while (1)
    {
        GPIOD_ODR |=  GREEN_LED;
        GPIOD_ODR &= ~ORANGE_LED;
        delay_ms(500);

        GPIOD_ODR &= ~GREEN_LED;
        GPIOD_ODR |=  ORANGE_LED;
        delay_ms(500);
    }

    return 0;
}