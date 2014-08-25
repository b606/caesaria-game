set( CMAKE_SYSTEM_NAME Linux )
set( CMAKE_SYSTEM_PROCESSOR i686 )
#-----<configuration>-----------------------------------------------

# One compiler generates either 32 bit or 64 bit code, -m32 says generate 32 bit:
set( CMAKE_CXX_FLAGS_INIT "-m32" CACHE STRING "C++ flag which generates 32 bit code." )
set( CMAKE_C_FLAGS_INIT "-m32" CACHE STRING "C flag which generates 32 bit code." )
set( CMAKE_SHARED_LINKER_FLAGS "-m32" CACHE STRING "Linker flag which finds 32 bit code." )
set( CMAKE_MODULE_LINKER_FLAGS "-m32" CACHE STRING "Linker flag which finds 32 bit code." )
set( CMAKE_EXE_LINKER_FLAGS "-m32" CACHE STRING "Linker flag which finds 32 bit code." )

