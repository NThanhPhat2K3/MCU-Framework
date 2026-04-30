# STM32F103 Linker Guide

Tai lieu nay giai thich nhanh:

- vi sao `BootManager`, `Programmer`, `App` phai co linker rieng
- `.isr_vector` la gi
- cac symbol nhu `_sidata`, `_sdata`, `_edata`, `_sbss`, `_ebss`, `_estack` dung de lam gi

## 1. Moi image la mot firmware rieng

Trong bo mau nay co 3 image:

- `BootManager`
- `Programmer`
- `App`

Moi image:

- nam o dia chi flash khac nhau
- co vector table rieng
- co `Reset_Handler` rieng
- co section `.text`, `.data`, `.bss` rieng

Nen moi image phai duoc link voi mot linker script rieng.

## 2. Linker script khong "goi" code

Linker script chi tra loi cac cau hoi:

- image nay bat dau o dau?
- vector table dat o dau?
- RAM dung vung nao?
- stack top la bao nhieu?
- symbol cho startup nam o dau?

Linker script khong tu nhay vao `main()`.

Viec "vao image nao" la do `BootManager` quyet dinh o runtime.

## 3. Vi du voi `Programmer`

Trong [ld/programmer.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/ld/programmer.ld):

```ld
FLASH (rx) : ORIGIN = 0x08004000, LENGTH = 16K
```

Dieu nay co nghia:

- image `Programmer` duoc dat bat dau tai `0x08004000`

Va:

```ld
.isr_vector :
{
    KEEP(*(.isr_vector))
} > FLASH
```

Dieu nay co nghia:

- section `.isr_vector` se duoc dat o dau vung `FLASH`
- ma `FLASH` cua image `Programmer` bat dau tai `0x08004000`

Nen vector table cua `Programmer` nam o:

- `0x08004000`

## 4. `.isr_vector` den tu dau?

`.isr_vector` khong duoc "define" trong linker script.

No duoc tao boi startup file, vi du:

- [startup/startup_stm32f103xb.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/portable_boot_example/stm32f103/startup/startup_stm32f103xb.c)

Startup file tao ra:

- vector table
- `Reset_Handler`
- cac IRQ handler mac dinh

Linker chi dat section do vao dung dia chi.

## 5. Tai sao `PROGRAMMER_ADDR` khong phai dia chi `main()`

`PROGRAMMER_ADDR` la dia chi dau image, khong phai dia chi `main()`.

Tai dau image:

- word 0: initial MSP
- word 1: `Reset_Handler`

Khi `BootManager` goi:

```c
boot_jump_to_image(PROGRAMMER_ADDR);
```

thi flow la:

```text
doc MSP tai PROGRAMMER_ADDR
doc Reset_Handler tai PROGRAMMER_ADDR + 4
set MSP
set VTOR
jump vao Reset_Handler
Reset_Handler moi goi main()
```

## 6. Cac symbol startup la gi?

Trong startup portable, co cac symbol nhu:

- `_sidata`
- `_sdata`
- `_edata`
- `_sbss`
- `_ebss`
- `_estack`

Nhung symbol nay duoc linker script tao ra de startup biet vung nho can xu ly.

### `_sidata`

Dia chi trong flash cua du lieu khoi tao cho section `.data`.

### `_sdata`

Diem bat dau cua `.data` trong RAM.

### `_edata`

Diem ket thuc cua `.data` trong RAM.

Startup se copy du lieu tu `_sidata` sang tu `_sdata` den `_edata`.

### `_sbss`

Diem bat dau cua `.bss` trong RAM.

### `_ebss`

Diem ket thuc cua `.bss` trong RAM.

Startup se zero tu `_sbss` den `_ebss`.

### `_estack`

Dinh stack ban dau cua image.

Gia tri nay duoc dat o word dau cua vector table.

## 7. Vi sao doi vung flash thi phai doi linker script

Vi neu doi `ORIGIN` cua image, thi:

- vector table doi dia chi
- `.text` doi dia chi
- `.data` load address doi
- symbol startup doi

Nen moi image va moi layout flash can linker script phu hop.

## 8. Cach tach linker cho de port

Ban hien tai da tach theo huong:

- `linker/gnu/sections_common.ld`
  - chua bo cuc `.isr_vector`, `.text`, `.data`, `.bss`, `.noinit`
- `bootmanager.ld`
  - chi mo ta memory map cua BootManager
- `programmer.ld`
  - chi mo ta memory map cua Programmer
- `app.ld`
  - chi mo ta memory map cua App

Day la huong nen dung khi:

- em co nhieu image
- em muon lam nhieu variant STM32
- em muon sau nay port sang ho MCU khac

## 9. Gioi han cua "linker common"

`linker/gnu/sections_common.ld` thuong chi common tot trong cung toolchain GNU ld.

Neu doi sang:

- Keil
- IAR
- MSP toolchain
- Renesas toolchain

thi syntax file linker se khac.

Nhung em van nen giu y tuong kien truc:

- 1 lop section layout dung chung
- 1 lop memory map rieng theo image/chip

## 10. Quy tac nho nhanh

- `image address` = dia chi dau firmware image
- `image address` khong phai dia chi `main()`
- linker script = ban do bo nho cua image
- startup file = noi tao vector table va `Reset_Handler`
- `BootManager` = noi quyet dinh nhay vao image nao luc runtime
