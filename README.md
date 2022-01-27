# zigbee_offline_programmer

## Demo
STM32F103C8T6 72MHz RAM:20K ROM:64K

* USART1: (DMA)download bin file to NXP zigbee
* USART2: debug(command line)
* USB: MSD(U盘)，1、用来存储配置文件(config.txt，设置自动/手动烧录，烧录次数，烧录完是否校验等)，2、存储需要下载的bin文件，可以存放多个，在配置文件中可以配置目标bin文件。
* SPI1: SPI_FLASH(W25Q128)

* 由于测试使用的STM32(STM32F103C8T6)只有20k内存，不够用，因此在上电启动时候通过sw按键做选择：1、按下的话，运行U盘模式，被电脑识别，用于更改或查看配置信息，修改下载固件。2、如果没有按下，就运行下载模式。

* 测试开发板原理图在Doc文件夹下: TestBoard_Schematic_Diagram_V0.1.pdf

* 配置文件模板在Doc文件夹下：config.txt

## 1. Support Chip:
- NXP_JN5139
- NXP_JN5142
- NXP_JN5148
- NXP_JN5168
- NXP_JN5169

## 2. Features:
- Auto Program Mode
- Mannual Program Mode
- Max Program Control
- Verify After Program
- Run After Program
- Flash Read Out Protection
