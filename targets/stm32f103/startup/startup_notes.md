# Startup Porting Notes

Bo startup da duoc tach thanh 2 lop:

## 1. Lop chung cho Cortex-M

- [startup/startup_portable_cortexm.h](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/startup/startup_portable_cortexm.h)
- [startup/startup_portable_cortexm.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/startup/startup_portable_cortexm.c)

Phan nay dung chung cho rat nhieu dong chip Cortex-M:

- copy `.data`
- clear `.bss`
- goi `SystemInit()`
- goi `__libc_init_array()`
- goi `main()`

## 2. Lop rieng cho tung MCU family

- [targets/stm32f103/startup/startup_stm32f103xb.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/startup/startup_stm32f103xb.c)

Phan nay chi giu:

- bang vector `.isr_vector`
- danh sach IRQ cua dong chip

## Cach port sang chip khac

Neu sang chip khac, em thuong chi can:

1. giu nguyen `startup/startup_portable_cortexm.*`
2. tao file vector moi cho dong chip moi
3. doi danh sach IRQ
4. doi `SystemInit()` theo dong chip
5. giu linker symbols tu linker script

## Vi du nhung dong co the tai su dung phan chung

- STM32F0/F1/F3/F4/G0/G4/L4
- GD32 dong Cortex-M tuong tu
- mot so dong NXP / Renesas Cortex-M neu doi `SystemInit()`

## Gioi han

Phan startup chung nay phu hop nhat khi:

- dung GNU toolchain
- dung Cortex-M co runtime C tieu chuan

Neu em doi sang IAR/Keil, startup flow van giong nhau, nhung mot so ten symbol co the khac.
