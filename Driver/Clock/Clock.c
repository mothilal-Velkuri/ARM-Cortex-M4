/* ─────────────────────────────────────────────────────────────────
   File    : drivers/clock/clock.c
   Brief   : Bare metal HSE + PLL 168MHz clock configuration
   Target  : STM32F407VGTx
   ───────────────────────────────────────────────────────────────── */
#include "Clock.h"

/* ── Base addresses (RM0090 Table 1) ──────────────────────────── */
#define FLASH_BASE      0x40023C00UL
#define RCC_BASE        0x40023800UL

/* ── FLASH registers ──────────────────────────────────────────── */
#define FLASH_ACR       (*(volatile unsigned int *)(FLASH_BASE + 0x00))

/* ── RCC registers ────────────────────────────────────────────── */
#define RCC_CR          (*(volatile unsigned int *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR     (*(volatile unsigned int *)(RCC_BASE + 0x04))
#define RCC_CFGR        (*(volatile unsigned int *)(RCC_BASE + 0x08))


/* ── FLASH_ACR bits ───────────────────────────────────────────── */
#define FLASH_ACR_LATENCY_5WS   (5 << 0)
#define FLASH_ACR_PRFTEN        (1 << 8)
#define FLASH_ACR_ICEN          (1 << 9)
#define FLASH_ACR_DCEN          (1 << 10)

/* ── RCC_CR bits ──────────────────────────────────────────────── */
#define RCC_CR_HSEON            (1 << 16)
#define RCC_CR_HSERDY           (1 << 17)
#define RCC_CR_PLLON            (1 << 24)
#define RCC_CR_PLLRDY           (1 << 25)

/* ── RCC_PLLCFGR bits ─────────────────────────────────────────── */
#define PLLSRC_HSE              (1 << 22)

/* ── RCC_CFGR bits ────────────────────────────────────────────── */
#define SW_PLL                  (2 << 0)
#define SWS_PLL                 (2 << 2)
#define HPRE_DIV1               (0 << 4)
#define PPRE1_DIV4              (5 << 10)
#define PPRE2_DIV2              (4 << 13)


void SystemClock_Config(void)
{
    /* 1. Flash wait states
          168MHz needs 5 wait states (RM0090 Table 10)               */
    FLASH_ACR = FLASH_ACR_LATENCY_5WS |
                FLASH_ACR_PRFTEN      |
                FLASH_ACR_ICEN        |
                FLASH_ACR_DCEN;

    /* 2. Enable HSE (8MHz crystal on Discovery board)                */
    RCC_CR |= RCC_CR_HSEON;
    while (!(RCC_CR & RCC_CR_HSERDY));

    /* 3. Configure PLL
          PLLM = 8   → VCO input  = 8MHz  / 8   = 1   MHz
          PLLN = 336 → VCO output = 1MHz  × 336 = 336 MHz
          PLLP = 00  → SYSCLK     = 336MHz / 2  = 168 MHz
          PLLQ = 7   → USB        = 336MHz / 7  = 48  MHz            */
    RCC_PLLCFGR = (8   << 0)  |
                  (336 << 6)  |
                  (0   << 16) |
                  (7   << 24) |
                  PLLSRC_HSE;

    /* 4. Enable PLL                                                   */
    RCC_CR |= RCC_CR_PLLON;
    while (!(RCC_CR & RCC_CR_PLLRDY));

    /* 5. Bus prescalers
          AHB  = SYSCLK / 1 = 168 MHz
          APB1 = HCLK   / 4 = 42  MHz
          APB2 = HCLK   / 2 = 84  MHz                                */
    RCC_CFGR |= HPRE_DIV1  |
                PPRE1_DIV4 |
                PPRE2_DIV2;

    /* 6. Switch SYSCLK → PLL                                         */
    RCC_CFGR |= SW_PLL;
    while ((RCC_CFGR & (3 << 2)) != SWS_PLL);
}
