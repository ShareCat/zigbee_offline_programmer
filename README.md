# zigbee_offline_programmer

## Demo
STM32F103C8T6 72MHz RAM:20K ROM:64K

* USART1: (DMA)download bin file to NXP zigbee
* USART2: debug(command line)
* USB: MSD(U flash disk)
  * 1. used to store the configuration file (config.txt, set the automatic/manual download, maxium download times, download check, etc.)
  * 2. to store the bin file, and you can store more than one, because you can configure the target bin file in the config.txt file.
* SPI1: SPI_FLASH(W25Q128)

* Since the STM32 (STM32F103C8T6) used in the test only has 20k RAM, which is not enough, so the sw button is used to make a choice when powering up and booting: 
  * 1. If pressed, it runs U disk mode, which is recognized by the computer and used to change or view configuration information and modify the downloaded firmware. 
  * 2. If not pressed, it runs download mode.

* Test board schematic: [TestBoard_Schematic_Diagram_V0.1.pdf](Doc/TestBoard_Schematic_Diagram_V0.1.pdf)

* The config file demo：[config.txt](Doc/config.txt)

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
