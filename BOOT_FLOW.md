# Boot Flow

## Normal boot

```text
Reset
-> BootManager
-> shared init
-> no programmer request
-> App valid
-> jump App
```

## Update request from App

```text
App
-> app_request_programmer_and_reset()
-> shared.boot_request = PROGRAMMER
-> reset
-> BootManager
-> jump Programmer
```

## First bring-up without App

```text
Reset
-> BootManager
-> App invalid
-> Programmer valid
-> jump Programmer
```

## Jump mechanics

```text
boot_jump_to_image(addr)
-> read MSP at addr + 0
-> read Reset_Handler at addr + 4
-> prepare CPU state
-> jump Reset_Handler
-> startup
-> main()
```
