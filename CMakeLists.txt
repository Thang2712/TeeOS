cmake_minimum_required(VERSION 4.2)
project(TEEOS C ASM)

enable_language(ASM)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCE_FILES
    src/loader.s
    src/kernel.c
    src/gdt.c
    src/drivers/driver.c
    src/drivers/keyboard.c
    src/drivers/mouse.c
    src/hardwarecommunication/pci.c
    src/hardwarecommunication/port.c
    src/hardwarecommunication/interrupts.c
    src/interruptstubs.s
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -ffreestanding -fno-stack-protector -fno-pie -fno-builtin -nostdlib -D__i386__")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -m32")

add_executable(mykernel.bin ${SOURCE_FILES})

set_target_properties(mykernel.bin PROPERTIES LINK_FLAGS "-m32 -T ${CMAKE_SOURCE_DIR}/linker.ld -nostdlib")
