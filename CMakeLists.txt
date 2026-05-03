cmake_minimum_required(VERSION 3.15)

set(CMAKE_TOOLCHAIN_FILE "STM32WB55_toolchain.cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(stm32wb55_wled LANGUAGES C CXX ASM)

# Source files
add_executable(${PROJECT_NAME}
  src/stm32wbxx_it.c
  src/stm32wbxx_hal_msp.c
  startup/startup_stm32wb55cgux.s
  src/main.c
  src/syscall.c
  src/sysmem.c
  src/system_stm32wbxx.c
  src/hal/stm32wbxx_hal.c
  src/hal/stm32wbxx_hal_rcc.c
  src/hal/stm32wbxx_hal_rcc_ex.c
  src/hal/stm32wbxx_hal_pwr.c
  src/hal/stm32wbxx_hal_pwr_ex.c
  src/hal/stm32wbxx_hal_cortex.c
  src/hal/stm32wbxx_hal_gpio.c
  src/hal/stm32wbxx_hal_tim.c
  src/hal/stm32wbxx_hal_tim_ex.c
  src/hal/stm32wbxx_hal_dma.c

  src/lib/ws2812b.c
  src/lib/ws2812b_animations.c

)

# Directories with headers
target_include_directories(${PROJECT_NAME} PRIVATE
  src/lib
  include
  include/hal
  include/hal/Legacy
  include/cmsis
)

# Define compile options
target_compile_options(${PROJECT_NAME} PRIVATE
  -mcpu=cortex-m4
  -mthumb
  -mfloat-abi=hard
  -mfpu=fpv4-sp-d16
  # -g
  -O3
  -ffunction-sections # Place functions in separate sections
  -fdata-sections # Place data in separate sections
  -fno-exceptions # Disable exceptions for bare-metal code
)

# Linker options
target_link_options(${PROJECT_NAME} PRIVATE
  -mcpu=cortex-m4
  -mfpu=fpv4-sp-d16
  -mfloat-abi=hard
  -mthumb
  -Wl,--start-group
  -Wl,--end-group
  -static
)

# Defines here
target_compile_definitions(${PROJECT_NAME} PRIVATE
  STM32WB55xx # Define your WB series MCU
)

# Convert .elf firmware to .bin after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME} ${PROJECT_NAME}.bin
)

# Print firmware Flash and RAM usage
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}
)

