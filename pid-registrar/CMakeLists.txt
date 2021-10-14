cmake_minimum_required(VERSION 3.10)
project(pid-registrar)
add_executable(pid-registrar main.c)
install(
    TARGETS pid-registrar
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
