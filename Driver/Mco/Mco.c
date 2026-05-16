/* ─────────────────────────────────────────────────────────────────
   File    : drivers/mco/mco.c
   Brief   : MCO2 output on PC9 → SYSCLK/4 = 42MHz
   Target  : STM32F407VGTx
   ───────────────────────────────────────────────────────────────── */
#include "Mco.h"

/* ── Base addresses ───────────────────────────────────────────── */
#define RCC_BASE        0x40023800UL
#define GPIOC_BASE      0x40020800UL

/* ── RCC registers ────────────────────────────────────────────── */
#define RCC_CFGR        (*(volatile unsigned int *)(RCC_BASE + 0x08))
#define RCC_AHB1ENR     (*(volatile unsigned int *)(RCC_BASE + 0x30))

/* ── GPIOC registers ──────────────────────────────────────────── */
#define GPIOC_MODER     (*(volatile unsigned int *)(GPIOC_BASE + 0x00))
#define GPIOC_OSPEEDR   (*(volatile unsigned int *)(GPIOC_BASE + 0x08))
#define GPIOC_AFRH      (*(volatile unsigned int *)(GPIOC_BASE + 0x24))

/* ── Bit definitions ──────────────────────────────────────────── */
#define GPIOCEN         (1 << 2)

void MCO_Config(void)
{
    /* Enable GPIOC clock                                             */
    RCC_AHB1ENR |= GPIOCEN;

    /* PC9 → Alternate Function mode  MODER[19:18] = 10              */
    GPIOC_MODER &= ~(3U << 18);
    GPIOC_MODER |=  (2U << 18);

    /* PC9 → Very high speed  OSPEEDR[19:18] = 11                    */
    GPIOC_OSPEEDR |= (3U << 18);

    /* PC9 → AF0 in AFRH[7:4]                                        */
    GPIOC_AFRH &= ~(0xF << 4);
    GPIOC_AFRH |=  (0x0 << 4);

    /* MCO2 source  = SYSCLK → bits[31:30] = 00                      */
    RCC_CFGR &= ~(3U << 30);   // clear MCO2 source    ← fixed
    RCC_CFGR |=  (2U << 30);   // set   MCO2PRE = 110  ← 6 not 0b110

    /* MCO2PRE = /4 → bits[29:27] = 110 → 168/4 = 42 MHz on PC9     */
    RCC_CFGR &= ~(7U << 27);   // clear MCO2PRE        ← fixed
    RCC_CFGR |=  (6U << 27);   // set   MCO2PRE = 110  ← 6 not 0b110
}