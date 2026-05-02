# Tong Hop Kien Truc Va Nhat Ky Loi Da Xu Ly

Tai lieu nay dung de:

- ban giao cho nguoi review
- tom tat kien truc dang chay that
- ghi lai cac loi lon da gap trong qua trinh bring-up
- noi ro da fix gi, vi sao fix nhu vay

Tai lieu nay uu tien tinh thuc chien, khong viet theo kieu ly thuyet chung chung.

## 1. Muc tieu hien tai cua framework

`MCU-Framework` la mot framework bootloader 3 image nho gon cho Cortex-M:

- `BootManager`
- `Programmer`
- `App`

Muc tieu chinh hien tai:

- de doc
- de port sang board moi
- de debug bang serial
- de hoc kien truc boot/update ro rang

No chua phai bootloader production day du.

## 2. Kien truc dang chay

### 2.1 Ba image

`BootManager`

- la image vao dau tien sau reset
- doc boot request trong shared RAM
- validate `App`
- neu `App` hop le thi jump vao `App`
- neu `App` loi thi fallback sang `Programmer`

`Programmer`

- nhan lenh update qua UART
- erase App region
- ghi App image xuong flash
- co the tra thong tin image qua lenh `I`

`App`

- firmware chinh
- co the nhan ky tu `u` de yeu cau reset vao `Programmer`

### 2.2 Cac lop chinh

`bootloader/common/`

- logic chung cua boot
- image validation
- shared RAM
- packet protocol

`application/`

- app mau
- logic request update

`port/`

- interface portable cho:
  - system
  - uart
  - flash

`mcu/`

- backend theo ho MCU
- STM32F1 va STM32F4 dang co implementation rieng cho flash

`startup/`

- startup portable cho Cortex-M
- tu set `VTOR` theo image

`tools/`

- host flasher
- update client
- helper script

## 3. Boot flow dang dung

### 3.1 Normal boot

```text
Reset
-> BootManager
-> check shared request
-> validate App
-> jump App
```

### 3.2 Request update tu App

```text
App
-> nhan 'u'
-> ghi BOOT_REQ_PROGRAMMER vao shared RAM
-> reset
-> BootManager
-> jump Programmer
```

### 3.3 Update flow qua Programmer

```text
Host tool
-> vao Programmer
-> E (erase app)
-> W (ghi tung chunk)
-> J (ket thuc / reset)
-> BootManager validate lai App
-> jump App neu hop le
```

## 4. Image header dang dung

Framework hien tai da co `image header + CRC32 + state`.

Nhung implementation thuc te dang dung co 2 diem quan trong:

1. Header khong nam o byte dau image

- vector table van o `IMAGE_ADDR`
- header nam o `IMAGE_ADDR + 0x200`

2. `PENDING` dang duoc coi la bootable

- `BootManager` validate bang `header + CRC + MSP + reset vector`
- khong con ep promote `PENDING -> VALID` luc boot

Ly do:

- ghi de metadata state in-place trong internal flash STM32 khong on dinh nhu mong doi

## 5. Cac loi lon da gap va cach fix

Phan nay la phan quan trong nhat de nguoi review thay duoc thuc chat qua trinh.

### Loi 1. `VTOR` nam trong `main()` cua App

Hien tuong:

- `main()` cua App co dong set `SCB->VTOR = APP_ADDR`
- code app trong xau va low-level qua muc

Nguyen nhan:

- app dang tu lo startup detail

Cach fix:

- chuyen viec set `VTOR` vao startup portable
- moi image build voi define rieng:
  - `IMAGE_BOOTMANAGER`
  - `IMAGE_PROGRAMMER`
  - `IMAGE_APP`

Ket qua:

- `main()` cua App sach
- style gan giong framework/IDE hon

### Loi 2. Tai lieu image header ban dau khong khop implementation that

Hien tuong:

- spec noi header o dau image
- implementation that dat header o offset `0x200`

Nguyen nhan:

- reset vector bat buoc phai o byte dau image tren Cortex-M

Cach fix:

- cap nhat linker
- cap nhat `IMAGE_FORMAT.md`
- thong nhat header 64 bytes tai `.image_header`

### Loi 3. `BootManager` fail khi promote `PENDING -> VALID`

Log:

```text
[ boot manager entry ]
app promote failed: IO_ERROR
[ programmer entry ]
```

Nguyen nhan that:

- y tuong ghi de state metadata trong cung flash word khong on dinh tren STM32 internal flash
- du transition ve logic la hop le, runtime van co the fail khi re-program metadata da tung duoc ghi

Cac buoc da thu:

- chi ghi byte thap cua state
- khoa interrupt trong luc flash operation

Cach fix cuoi cung:

- bo viec ep promote `PENDING -> VALID` trong boot
- giu `PENDING` la bootable
- de `CRC + header validation` lam cong quyet dinh chinh

Ket qua:

- `BootManager` jump App on dinh

### Loi 4. Host tool doc response `INFO` bi cat nua chung

Hien tuong:

- `Programmer` tra `OK INFO` nhieu dong
- host tool doc duoc nua response roi bao:

```text
Unexpected programmer response
```

Nguyen nhan:

- host tool dung o newline dau tien
- firmware gui nhieu `UART write` nho

Cach fix:

- trong `update_client.py`, doi sang co che doc den khi UART im lang trong mot khoang ngan

Ket qua:

- response `INFO` nhieu dong duoc doc day du

### Loi 5. Handshake App -> Programmer trong flasher bi nham

Hien tuong:

- tool thay `GO PROGRAMMER`
- tuong da vao `Programmer`
- nhung thuc te do chi la App bao sap reset

Nguyen nhan:

- host tool nhan dien qua nong
- dua vao chuoi text cu

Cach fix:

- chi coi la vao `Programmer` khi gap:
  - `[ programmer entry ]`
  - hoac `OK INFO`
- them nhan dien cho banner:
  - `[ app entry ]`
  - `[ programmer entry ]`

Ket qua:

- `flasher.py` vao dung mode truoc khi gui lenh update

### Loi 6. `Programmer` erase duoc nhung write fail tren STM32F411CE

Log:

```text
OK ERASE
ERR WRITE
reason: IO_ERROR
flash_err: 0x00000040
flash_addr: 0x08010000
```

Chan doan:

- `0x00000040` tren STM32F4 la `FLASH_SR_PGPERR`
- day la `program parallelism error`

Nguyen nhan that:

- backend F4 dang ghi flash theo byte
- `PSIZE` / che do program khong hop voi pattern ghi tren con F411

Cach fix:

- doi backend F4 sang ghi theo `32-bit word`
- doi `PSIZE` sang word
- ghi tung 4 byte va pad `0xFF` neu chunk le

Ket qua:

- host tool flash duoc App tren F411

### Loi 7. Thieu thong tin khi debug flash failure

Hien tuong:

- log cu chi co `IO_ERROR`
- khong biet phan cung dang fail vi co nao

Cach fix:

- them API lay:
  - `last flash error detail`
  - `last flash error addr`
- `Programmer` in them:
  - `flash_err`
  - `flash_addr`

Ket qua:

- debug tu muc "doan" chuyen thanh "co bang chung tu thanh ghi phan cung"

## 6. Hanh vi hien tai cua tool update

Host tool hien tai:

- tu nhan dien App hay Programmer
- neu dang o App thi gui `u`
- cho reset
- doi vao `Programmer`
- erase App
- ghi App theo chunk
- ket thuc bang `J`

Ngoai ra:

- lenh `I` tra ve metadata image da parse duoc
- CLI wrapper co `--info-only`

## 7. Cac file quan trong nguoi review nen doc

Neu review kien truc:

1. [README.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/README.md:1)
2. [ARCHITECTURE.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/ARCHITECTURE.md:1)
3. [BOOT_FLOW.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/BOOT_FLOW.md:1)
4. [IMAGE_FORMAT.md](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/IMAGE_FORMAT.md:1)

Neu review phan boot/update code:

1. [boot_image.h](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/common/boot_image.h:1)
2. [boot_image.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/common/boot_image.c:1)
3. [boot_jump.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/common/boot_jump.c:1)
4. [bootmanager/main.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/bootmanager/main.c:1)
5. [programmer/main.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/bootloader/programmer/main.c:1)
6. [update_client.py](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/tools/update_client.py:1)

Neu review backend phan cung:

1. [port_flash_stm32f1.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/mcu/stm32f1/port_flash_stm32f1.c:1)
2. [port_flash_stm32f4.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/mcu/stm32f4/port_flash_stm32f4.c:1)
3. [port_system_stm32.c](/home/phat/Desktop/00.Developer/01.code/CPM/p1.settings/MCU-Framework/mcu/stm32/port_system_stm32.c:1)

## 8. Tinh trang hien tai

Da chay duoc:

- `BootManager -> App`
- `App -> Programmer` qua lenh `u`
- host tool vao `Programmer`
- erase App
- ghi App tren `stm32f411ce`
- boot lai vao App

Da co:

- image header
- CRC32 validation
- state co y nghia thuc te
- host tool co the doc info image
- log debug kha day du cho bring-up

## 9. Gioi han hien tai

Van chua co:

- rollback App cu
- dual-slot A/B
- xac thuc signature
- khu metadata rieng de commit state an toan
- host tool parse/report theo format machine-readable dep hon

## 10. Huong nang cap tiep theo de xuat

Neu tiep tuc theo huong hoc thuat + practical, 3 buoc hop ly nhat la:

1. Don dep warning va ky thuat no

- bo hoac tai su dung `boot_image_mark_state()`
- ra soat warning compiler con lai

2. Nang tool len muc de review/QA de hon

- them nut `Read Info` cho `flasher.py`
- parse va hien `app_check`, `crc`, `state`, `version` dep hon tren GUI

3. Tach metadata/state sang khu rieng neu muon state machine xin hon

- luc do moi quay lai bai toan `PENDING -> VALID`
- tranh ghi de metadata vao word da program trong image header
