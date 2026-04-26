cmake_minimum_required(VERSION 4.2)
project(untitled C)

set(CMAKE_C_STANDARD 17)

add_executable(untitled kernel.c
        types.h
        gdt.h
        gdt.c
        port.h
        port.c
        interrupts.h
        interrupts.c
        keyboard.h
        keyboard.c
        mouse.h
        mouse.c)

enable_language(ASM)