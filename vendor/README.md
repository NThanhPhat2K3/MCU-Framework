# Vendor Packages

Thu muc nay dung de dat local vendor package ma project can de build, vi du:

- `STM32CubeF1/`
- `STM32CubeF4/`

Khuyen nghi clone:

```bash
git clone --recursive https://github.com/STMicroelectronics/STM32CubeF1.git vendor/STM32CubeF1
git clone --recursive https://github.com/STMicroelectronics/STM32CubeF4.git vendor/STM32CubeF4
```

Project hien tai uu tien:

- `vendor/` cho HAL/CMSIS/Middleware cua hang
- `bsp/` cho code board-specific do du an tu viet
