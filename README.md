# ARM Cortex-M4 Bare-Metal Development

Bare-metal STM32F407VG project built from scratch — no HAL, no CubeMX, no abstraction layers. Pure register-level programming using ARM GCC toolchain and OpenOCD, developed entirely in VS Code.

---

## 📌 Project Overview

| Property | Details |
|----------|---------|
| **Board** | STM32F407VG Discovery |
| **Core** | ARM Cortex-M4 |
| **Toolchain** | ARM GCC (`arm-none-eabi-gcc`) |
| **Flash Tool** | OpenOCD |
| **IDE** | VS Code |
| **Language** | C + ARM Assembly |

---

## 📁 Project Structure

```
ARM-Cortex-M4/
├── src/
│   └── main.c                  # Main application (bare-metal C)
├── startup/
│   └── startup_stm32f407.s     # ARM Assembly startup file
├── linker/
│   └── stm32f407.ld            # Custom linker script
└── Makefile                    # Build + flash commands
```

---

## 🛠️ Prerequisites

Install the following tools and add them to your system PATH:

| Tool | Purpose | Download |
|------|---------|----------|
| ARM GCC Toolchain | Compile for Cortex-M4 | [developer.arm.com](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) |
| OpenOCD | Flash and debug | [openocd.org](https://openocd.org) |
| Make | Build system | via Chocolatey: `choco install make` |
| VS Code | Editor | [code.visualstudio.com](https://code.visualstudio.com) |

### VS Code Extensions
- **C/C++** (Microsoft)
- **Cortex-Debug** (marus25)

---

## ⚙️ How It Works

### Boot Flow

```
Power ON
   │
   ▼
Linker Script (stm32f407.ld)
   │  Defines memory layout: FLASH (1MB) and SRAM (128KB)
   │  Exports symbols: _estack, _sdata, _edata, _sidata, _sbss, _ebss
   │
   ▼
Startup File (startup_stm32f407.s)
   │  Vector table → _estack loaded into Stack Pointer by hardware
   │  Copies .data section from FLASH → SRAM
   │  Zeros .bss section in SRAM
   │
   ▼
main() (src/main.c)
   │  Enable GPIOD peripheral clock via RCC
   │  Configure PD12, PD13 as output pins
   │  Toggle LEDs in infinite loop
```

### Memory Layout

```
FLASH (0x08000000 - 0x080FFFFF)   SRAM (0x20000000 - 0x2001FFFF)
┌──────────────────────┐          ┌──────────────────────┐
│  Vector Table        │          │  .data (runtime)     │
│  .text (code)        │          │  .bss  (zeroed)      │
│  .rodata (constants) │          │  Stack (grows ↓)     │
│  .data (init values) │──copy──▶ │                      │
└──────────────────────┘          └──────────────────────┘
```

---

## 🚀 Build and Flash

### Build
```bash
make
```
Generates:
- `blink.elf` — for debugging
- `blink.bin` — raw binary
- `blink.hex` — Intel hex format

### Flash to Board
```bash
make flash
```
Uses OpenOCD with ST-Link V2 to program the STM32F407VG Discovery board.

### Clean
```bash
make clean
```

---

## 💡 Current Demo — Alternating LED Blink

Blinks the onboard LEDs on the STM32F407VG Discovery board alternately:

| Pin | LED Color | Behavior |
|-----|-----------|---------|
| PD12 | 🟢 Green | ON for 500ms |
| PD13 | 🟠 Orange | ON for 500ms (alternates with PD12) |

### Register-Level Code (no HAL)
```c
/* Enable GPIOD clock */
RCC_AHB1ENR |= (1 << 3);

/* Set PD12, PD13 as output */
GPIOD_MODER |= (1 << 24) | (1 << 26);

/* Toggle */
GPIOD_ODR |=  (1 << 12);   /* PD12 ON  */
GPIOD_ODR &= ~(1 << 13);   /* PD13 OFF */
```

---

## 🔌 Hardware Setup

```
STM32F407VG Discovery Board
┌─────────────────────────┐
│  PD12 → 🟢 Green  LED   │
│  PD13 → 🟠 Orange LED   │  ← Currently used
│  PD14 → 🔴 Red    LED   │
│  PD15 → 🔵 Blue   LED   │
└─────────────────────────┘
Connect ST-Link V2 via USB to flash and debug.
```

---

## 📖 Key Concepts Covered

- Writing a **linker script** from scratch (memory regions, sections, symbols)
- Writing a **startup file** in ARM Assembly (vector table, .data copy, .bss zero-fill)
- **Direct register access** without HAL or CMSIS
- Building with **ARM GCC** and a custom Makefile
- Flashing with **OpenOCD** via ST-Link

---

## 📜 License

MIT License — free to use, modify, and distribute.
