cmake_minimum_required(VERSION 3.28)
project(EventDispatch C)

set(CMAKE_C_STANDARD 23)

add_executable(
    EventDispatch
    main.c
    event.c
    event.h
    table.c
    table.h
    dispatcher.c
    dispatcher.h
    rbuff.c
    rbuff.h
)
