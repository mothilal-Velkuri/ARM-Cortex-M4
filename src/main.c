#include <stdint.h>

/* STM32F407 Register Addresses */
#define RCC_BASE        0x40023800
#define GPIOD_BASE      0x40020C00

#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE   + 0x30))
#define GPIOD_MODER     (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_ODR       (*(volatile uint32_t *)(GPIOD_BASE + 0x14))

/* STM32F407 Discovery LEDs */
#define GPIODEN         (1 << 3)    /* Enable GPIOD clock        */
#define PD12_OUTPUT     (1 << 24)   /* PD12 as output (MODER)    */
#define PD13_OUTPUT     (1 << 26)   /* PD13 as output (MODER)    */
#define GREEN_LED       (1 << 12)   /* PD12 - Green LED          */
#define ORANGE_LED      (1 << 13)   /* PD13 - Orange LED         */

void delay(volatile uint32_t count)
{
    while(count--);
}

int main(void)
{
    /* 1. Enable GPIOD clock */
    RCC_AHB1ENR |= GPIODEN;

    /* 2. Set PD12 and PD13 as output */
    GPIOD_MODER |= PD12_OUTPUT | PD13_OUTPUT;

    /* 3. Blink LEDs alternately forever */
    while(1)
    {
        /* PD12 ON, PD13 OFF */
        GPIOD_ODR |=  GREEN_LED;
        GPIOD_ODR &= ~ORANGE_LED;
        delay(500000);

        /* PD12 OFF, PD13 ON */
        GPIOD_ODR &= ~GREEN_LED;
        GPIOD_ODR |=  ORANGE_LED;
        delay(500000);
    }

    return 0;
}