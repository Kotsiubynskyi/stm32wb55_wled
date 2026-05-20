This is WLED library usage example for STM32WB55

The most complicated part is in `main.c` to correctly start timer in DMA mode.

## How to build and flash (for Ubuntu)

1. Install needed ARM development toolchain and other utilities:

    ```bash
    apt update; apt install -y cmake git usbutils ninja-build gcc-arm-none-eabi stlink-tools
    ```

1. Clone the project:

    ```bash
    git clone --recursive https://github.com/Kotsiubynskyi/stm32wb55_wled.git
    cd stm32wb55_wled
    ```

1. Generate build files:

    ```bash
    cmake -B build -G Ninja
    ```

1. Build project from ***build*** folder:

    ```bash
    cmake --build build
    ```

1. Insert ST-LINK programmer USB stick and make sure it's visible by OS:

    ```bash
    lsusb|grep STMicroelectronics
    ```

1. Upload built firmware to MCU:

    ```bash
    st-flash erase && st-flash --reset write build/stm32wb55_wled 0x8000000
    ```
