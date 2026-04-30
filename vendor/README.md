# Vendor Packages

Thu muc nay dung de dat vendor component ma project can de build.
Vendor component la code cua nha san xuat, vi du HAL driver va CMSIS device.

Project nay uu tien dung cac repo component nho thay vi clone full `STM32CubeF1`
hoac `STM32CubeF4`, vi:

- nhe hon
- de share hon
- de dung hon tren ca Windows va Linux
- tach ro vendor layer va BSP layer

Vendor component hien tai:

- `CMSIS_5/`
- `cmsis-device-f1/`
- `cmsis-device-f4/`
- `stm32f1xx-hal-driver/`
- `stm32f4xx-hal-driver/`

Nguon goc third-party:

- `CMSIS_5/` duoc lay tu ARM
- `cmsis-device-*` va `stm32*hal-driver/` duoc lay tu STMicroelectronics

Muc tieu cua repo nay la de hoc tap, porting, va chia se de clone ve build nhanh.
Vi vay cac vendor component duoc commit cung repo thay vi bat buoc nguoi moi
phai clone them bang tay o buoc dau tien.

Dung Makefile:

```bash
make vendor-core
make vendor-f1
make vendor-f4

# hoac clone tat ca
make vendor
```

Hoac clone thu cong:

```bash
git clone --depth 1 https://github.com/ARM-software/CMSIS_5.git vendor/CMSIS_5
git clone --depth 1 https://github.com/STMicroelectronics/cmsis-device-f1.git vendor/cmsis-device-f1
git clone --depth 1 https://github.com/STMicroelectronics/cmsis-device-f4.git vendor/cmsis-device-f4
git clone --depth 1 https://github.com/STMicroelectronics/stm32f1xx-hal-driver.git vendor/stm32f1xx-hal-driver
git clone --depth 1 https://github.com/STMicroelectronics/stm32f4xx-hal-driver.git vendor/stm32f4xx-hal-driver
```

Phan tach vai tro:

- `vendor/` cho HAL driver, CMSIS core, CMSIS device
- `stm32f103/`, `stm32f411ce/` cho startup/linker/system theo target chip
- `bsp/` de danh cho board-specific code cua du an tu viet sau nay
