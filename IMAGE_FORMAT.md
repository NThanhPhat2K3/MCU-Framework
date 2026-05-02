# Thiet Ke Image Header Va CRC

Tai lieu nay mo ta implementation hien tai cua `MCU-Framework` sau khi nang cap boot theo huong `header + state + crc`.

Muc tieu cua ban nay:

- xac thuc image truoc khi jump
- phat hien image ghi do dang
- cho phep `App` boot an toan hon ma chua can A/B slot
- giu kien truc don gian, phu hop voi bo nho flash han che

Ban nay chua giai quyet rollback app cu. Neu update hong giua chung, thiet bi se roi ve `Programmer` thay vi quay lai app truoc do.

## 1. Y tuong tong quat

Moi image deu co:

- vector table o dau image
- image header o offset co dinh `0x200`
- `image_size` va `crc32` duoc patch sau build

`BootManager` se:

1. doc header
2. validate image
3. neu `App` hop le thi promote `PENDING -> VALID`
4. jump vao `App`
5. neu `App` loi thi fallback sang `Programmer`

`Programmer` se:

- erase vung App
- ghi image moi
- reset ve `BootManager`

Luc boot lai, `BootManager` moi la noi quyet dinh cuoi cung image do co duoc chay hay khong.

## 2. Vi sao header khong nam o byte dau image

Ban dau y tuong la dat header ngay dau partition. Nhung tren Cortex-M, byte dau image phai la vector table:

- word 0: initial MSP
- word 1: `Reset_Handler`

Neu dat header len dau flash thi MCU se khong reset vao image duoc.

Vi vay implementation hien tai dat:

- vector table o `IMAGE_ADDR`
- image header o `IMAGE_ADDR + 0x200`

Offset `0x200` du rong de chua vector table va cac exception/IRQ thong dung. Linker da co assert de tranh vector table de len header.

## 3. Dinh dang header hien tai

Header duoc khai bao tai [boot_image.h](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/common/boot_image.h:1):

```c
typedef struct
{
    uint32_t magic;
    uint16_t header_version;
    uint16_t header_size;
    uint32_t image_type;
    uint32_t image_state;
    uint32_t image_size;
    uint32_t vector_addr;
    uint32_t crc32;
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t version_patch;
    uint32_t reserved[6];
} boot_image_header_t;
```

Kich thuoc header:

- `64 bytes`

Gia tri co dinh:

- `BOOT_IMAGE_MAGIC = 0x494D4731`
- `BOOT_IMAGE_HEADER_VERSION = 1`
- `BOOT_IMAGE_HEADER_OFFSET = 0x200`

Y nghia tung truong:

- `magic`: nhan dien image cua framework
- `header_version`: version cua format header
- `header_size`: de validator biet dung layout nao
- `image_type`: `BootManager / Programmer / App`
- `image_state`: trang thai boot duoc hay khong
- `image_size`: tong kich thuoc binary cua image
- `vector_addr`: dia chi vector table that su
- `crc32`: CRC32 cua image, bo qua vung header
- `version_*`: thong tin version de log/debug

## 4. Image type va state

Image type hien tai:

```c
BOOT_IMAGE_TYPE_BOOTMANAGER = 1
BOOT_IMAGE_TYPE_PROGRAMMER  = 2
BOOT_IMAGE_TYPE_APP         = 3
```

State duoc ma hoa theo kieu than thien voi flash, de co the doi bit tu `1 -> 0`:

```c
BOOT_IMAGE_STATE_EMPTY   = 0xFFFFFFFF
BOOT_IMAGE_STATE_PENDING = 0xFFFFFFFE
BOOT_IMAGE_STATE_VALID   = 0xFFFFFFFC
BOOT_IMAGE_STATE_INVALID = 0xFFFFFFF8
```

Y nghia:

- `EMPTY`: vung flash chua co image hop le
- `PENDING`: image moi duoc tao, cho lan boot dau verify
- `VALID`: image da duoc xac nhan hop le
- `INVALID`: image khong duoc phep boot

Mac dinh header duoc emit nhu sau:

- `BootManager`: `VALID`
- `Programmer`: `VALID`
- `App`: `PENDING`

Sau khi `App` pass validate trong `BootManager`, framework se ghi state tu `PENDING` sang `VALID`.

## 5. CRC32 hien tai

CRC32 duoc tinh trong [boot_image.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/common/boot_image.c:1) bang software CRC voi polynomial:

- `0xEDB88320`

CRC khong tinh tren toan bo header. Cu the:

- tinh tren bytes tu dau image den truoc header
- bo qua toan bo `boot_image_header_t`
- tinh tiep phan con lai cua binary

Ly do:

- tranh bai toan CRC cua chinh no
- de patch `image_size` va `crc32` sau build
- de `BootManager` co the tinh lai doc lap

## 6. Build pipeline hien tai

Luong build hien tai:

1. link ELF
2. doi ELF thanh BIN
3. `tools/patch_image.py` patch `image_size`
4. `tools/patch_image.py` tinh va patch `crc32`
5. sinh HEX tu BIN da patch

Nghia la:

- runtime khong tu sinh CRC
- `Programmer` khong tu viet lai header theo tung truong
- image da mang san metadata dung ngay tu output build

## 7. Validation hien tai

Validator chinh nam o [boot_image.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/common/boot_image.c:1).

`boot_image_check_typed()` dang check:

- `magic`
- `header_version`
- `header_size`
- `image_type`
- `image_state` co bootable khong
- `vector_addr == image_addr`
- `image_size` co hop le va nam trong flash khong
- MSP co nam trong RAM khong
- reset vector co nam trong flash khong
- `crc32` co trung khong

Framework hien tai da co status chi tiet hon de debug:

- `BOOT_STATUS_INVALID_MAGIC`
- `BOOT_STATUS_INVALID_HEADER_VERSION`
- `BOOT_STATUS_INVALID_HEADER_SIZE`
- `BOOT_STATUS_INVALID_IMAGE_TYPE`
- `BOOT_STATUS_INVALID_STATE`
- `BOOT_STATUS_INVALID_VECTOR_ADDR`
- `BOOT_STATUS_INVALID_IMAGE_SIZE`
- `BOOT_STATUS_INVALID_STACK_PTR`
- `BOOT_STATUS_INVALID_RESET_VECTOR`
- `BOOT_STATUS_CRC_MISMATCH`

## 8. Boot policy hien tai

### BootManager

`BootManager` dang hoat dong theo chinh sach nay:

1. Neu shared RAM yeu cau vao `Programmer` thi uu tien check `Programmer`
2. Neu `Programmer` hop le thi jump
3. Neu khong co request, check `App`
4. Neu `App` hop le thi jump vao `App`
5. Neu `App` loi thi check `Programmer`
6. Neu `Programmer` hop le thi fallback vao `Programmer`

Neu co loi validate, `BootManager` se luu `error_code` vao shared RAM va in reason qua UART.

### Programmer

`Programmer` hien tai van theo flow don gian:

1. erase App partition
2. ghi chunk xuong flash
3. lenh `J` khong jump thang vao App nua
4. lenh `J` chi reset ve `BootManager`
5. `BootManager` validate lai roi moi quyet dinh co boot App hay khong

Lenh `I` trong `Programmer` hien tai co the bao:

- `app_magic`
- `app_header_version`
- `app_header_size`
- `app_type`
- `app_state`
- `app_size`
- `app_vector`
- `app_crc32`
- `app_version_major/minor/patch`
- `app_check`
- `update_status`
- `last_error`
- `last_error_name`

nen debug update hong de hon truoc.

## 8.1 Ghi chu thuc te ve state `PENDING`

Ban thiet ke ban dau co y tuong promote `PENDING -> VALID` sau lan boot dau thanh cong.
Tuy nhien voi STM32 internal flash hien tai, viec re-program metadata word in-place khong on dinh nhu mong doi.

Vi vay implementation dang dung chinh sach thuc te hon:

- `PENDING` van duoc coi la bootable
- `CRC32 + header validation` moi la dieu kien quyet dinh co boot hay khong
- `BootManager` khong ep doi state sang `VALID` nua

Neu sau nay muon co state machine day du hon, nen tach metadata sang mot khu vuc flash/journal rieng thay vi ghi de truc tiep len header word da duoc program.

## 9. File lien quan trong implementation

Code chinh:

- [boot_image.h](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/common/boot_image.h:1)
- [boot_image.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/common/boot_image.c:1)
- [boot_image_header.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/common/boot_image_header.c:1)
- [boot_jump.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/common/boot_jump.c:1)
- [bootloader/bootmanager/main.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/bootmanager/main.c:1)
- [bootloader/programmer/main.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/programmer/main.c:1)

Build va linker:

- [Makefile](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/Makefile:1)
- [sections_common.ld](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/linker/gnu/sections_common.ld:1)
- [patch_image.py](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/tools/patch_image.py:1)

## 10. Trade-off hien tai

Ban nay nang framework len kha nhieu, nhung van giu do phuc tap hop ly:

- uu diem:
  - boot chac tay hon
  - biet ro image hong o dau
  - giam nguy co boot nham image ghi do dang
  - tao nen tang de len A/B slot sau nay

- gioi han:
  - chua giu duoc app cu neu mat dien giua chung
  - chua co rollback
  - chua co image signature/authentication

## 11. Huong nang cap tiep

Neu muon di tiep theo huong "level kha+" thi 3 buoc hop ly nhat la:

1. them status/telemetry ro hon cho host update tool
2. danh dau `INVALID` ro rang hon khi update fail
3. them thong tin version/build vao lenh info cua `Programmer`

Neu mot ngay can len "level manh" thi moi can tinh toi:

- dual-slot App A/B
- rollback
- xac nhan app sau lan boot dau
