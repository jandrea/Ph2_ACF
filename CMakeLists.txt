string(ASCII 27 Esc)
set(Reset       "${Esc}[m"    )
set(Bold        "${Esc}[1m"   )
set(Red         "${Esc}[31m"  )
set(Green       "${Esc}[32m"  )
set(Yellow      "${Esc}[33m"  )
set(Blue        "${Esc}[34m"  )
set(Magenta     "${Esc}[35m"  )
set(Cyan        "${Esc}[36m"  )
set(White       "${Esc}[37m"  )
set(BoldRed     "${Esc}[1;31m")
set(BoldGreen   "${Esc}[1;32m")
set(BoldYellow  "${Esc}[1;33m")
set(BoldBlue    "${Esc}[1;34m")
set(BoldMagenta "${Esc}[1;35m")
set(BoldCyan    "${Esc}[1;36m")
set(BoldWhite   "${Esc}[1;37m")

if(NOT DEFINED ENV{OTSDAQ_CMSOUTERTRACKER_DIR}) 


    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS "${BoldYellow}MIDDLEWARE${Reset} [stand-alone]: Compilation of the middleware package") 
    MESSAGE(STATUS "${BoldYellow}MIDDLEWARE${Reset} [stand-alone]: [${BoldCyan}${BASE_DIR}/CMakeLists.txt${Reset}]") 
    MESSAGE(STATUS " ") 

    cmake_minimum_required(VERSION 2.8)
    project(Ph2_ACF)

    #set the output directory
    if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
       MESSAGE( STATUS "${BoldRed}" )
       MESSAGE( FATAL_ERROR "${Reset}In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
    endif()

    # ---------- Setup output Directories -------------------------
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY 
        ${PROJECT_SOURCE_DIR}/lib 
        CACHE PATH
        "Single Directory for all Libraries"
       )

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${PROJECT_SOURCE_DIR}/bin
        CACHE PATH
        "Single Directory for all Executables."
       )

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_SOURCE_DIR}/bin
        CACHE PATH
        "Single Directory for all static libraries."
       )
    # ---------- Setup output Directories -------------------------


    #set the cmakeL module path
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
    
    set(CompileForShep "$ENV{CompileForShep}")
    set(CompileForHerd "$ENV{CompileForHerd}")
    if( (NOT CompileForShep) AND (NOT CompileForHerd) )
    set(NoDataShipping TRUE)
    else()
    set(NoDataShipping FALSE)
    endif()
    
    MESSAGE(STATUS "CompileForShep = ${CompileForShep}")
    MESSAGE(STATUS "CompileForHerd = ${CompileForHerd}")
    MESSAGE(STATUS "NoDataShipping = ${NoDataShipping}")

    # If it's a recent gcc compiler, then let's add some
    # compiler options to avoid warnings promoted to errors

    set (GCC_NOERROR_FLAGS "-Wno-error=maybe-uninitialized -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=unused-result -Wno-error=deprecated-declarations")
    set (GCC_6_NOERROR_FLAGS "-Wno-error=misleading-indentation")
    set (GCC_7_NOERROR_FLAGS "-Wno-expansion-to-defined")
    set (GCC_8_NOERROR_FLAGS "-Wno-error=catch-value -Wno-error=class-memaccess -Wno-error=format-overflow")
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
      EXECUTE_PROCESS( COMMAND gcc -dumpversion OUTPUT_VARIABLE GCC_VERSION )
      if(NOT (GCC_VERSION VERSION_LESS 6))
        set (GCC_NOERROR_FLAGS "${GCC_NOERROR_FLAGS} ${GCC_6_NOERROR_FLAGS}")
      endif()
      if(NOT (GCC_VERSION VERSION_LESS 7))
        set (GCC_NOERROR_FLAGS "${GCC_NOERROR_FLAGS} ${GCC_7_NOERROR_FLAGS}")
      endif()
      if(NOT (GCC_VERSION VERSION_LESS 8))
        set (GCC_NOERROR_FLAGS "${GCC_NOERROR_FLAGS} ${GCC_8_NOERROR_FLAGS}")
      endif()
    endif ()
    set (CMAKE_CXX_FLAGS "-g -std=c++1y -gdwarf-4 -O3 -fno-omit-frame-pointer -Werror -pedantic -Wno-unused-parameter -Wno-ignored-qualifiers -Wno-missing-field-initializers -std=c++1y -Wall -Werror=return-type -Wextra -Winit-self -Wno-unused-local-typedefs -Woverloaded-virtual -Wnon-virtual-dtor -Wdelete-non-virtual-dtor ${GCC_NOERROR_FLAGS} -pthread  -Wcpp -fPIC ${CMAKE_CXX_FLAGS}")
    if(NoDataShipping)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} $ENV{UseRootFlag}")
    endif()



    #check for external dependences
    message("--")
    message("--     ${BoldCyan}#### Checking for external Dependencies ####${Reset}")
    if(CompileForShep OR NoDataShipping)
        #ROOT
        find_package(ROOT COMPONENTS RHTTP)
        if(ROOT_FOUND)
            #check for THttpServer
            if(EXISTS ${ROOT_RHTTP_LIBRARY})
                message(STATUS "    Found THttp Server support - enabling compiler flags")
                set(ROOT_HAS_HTTP TRUE)
            elseif()
                message(STATUS "    ROOT built without THttp Server - disabling compiler flags")
            endif()
            # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -")
            message(STATUS "    Using ROOT version ${ROOT_VERSION}")
        endif(ROOT_FOUND)
    endif()

    find_package(CACTUS)      #========================================================
    if(${CACTUS_AMC13_FOUND})
        message(STATUS "    enabling AMC13 component")
    else(${CACTUS_AMC13_FOUND})
        message(STATUS "    continuing without AMC13 components")
    endif(${CACTUS_AMC13_FOUND})

    find_package(PH2_ANTENNA) #========================================================
    if(PH2_ANTENNA_FOUND)
        message(STATUS "    building the Antenna components")
    else(PH2_ANTENNA_FOUND)
        message(STATUS "    continuing without Antenna components")
    endif(PH2_ANTENNA_FOUND)

    #ZeroMQ optional
    find_package(ZMQ)         #========================================================
    if(ZMQ_FOUND)
        #message(STATUS "Found ZMQ Library - checking for Ph2_UsbInstLib")

        #libPh2_UsbInstLib
        find_package(PH2_USBINSTLIB)
        if(PH2_USBINSTLIB_FOUND)
            #message(STATUS "    Found Ph2_UsbInstLib - enabling support")
        endif(PH2_USBINSTLIB_FOUND)
    endif(ZMQ_FOUND)

    #Boost
    file(GLOB_RECURSE uhal_boost /opt/cactus/*version.hpp)
    if(uhal_boost)
        MESSAGE(STATUS "    Found boost installation that comes with uHAL, using this version") 
            set(Boost_NO_SYSTEM_PATHS TRUE)
            if(Boost_NO_SYSTEM_PATHS)
                if(${CACTUS_FOUND})
                    set(BOOST_ROOT ${CACTUS_ROOT})
                else(${CACTUS_FOUND})
                    set(BOOST_ROOT /opt/cactus)
                endif(${CACTUS_FOUND})
                set(BOOST_INCLUDE_DIRS ${BOOST_ROOT}/include)
                set(BOOST_LIBRARY_DIRS ${BOOST_ROOT}/lib)
            endif(Boost_NO_SYSTEM_PATHS)
    else(uhal_boost)
        MESSAGE(STATUS "    No boost headers found with uHAL (you are most likely using CC7) - make sure to install boost 1.53 (default version on CC7)")
    endif(uhal_boost)

    find_package(Boost 1.53 REQUIRED system filesystem thread program_options)
    #find_package(Boost 1.53 REQUIRED filesystem system-mt )

    message("--     ${BoldCyan}#### Done ####${Reset}")

    #all the subdirs
    if(${CACTUS_AMC13_FOUND})
        add_subdirectory(AMC13)
    endif()

    MESSAGE(STATUS " ")
    if(CompileForHerd OR NoDataShipping)
        add_subdirectory(tools)
        add_subdirectory(HWDescription)
        add_subdirectory(HWInterface)
        add_subdirectory(System)
    endif()

    if(CompileForHerd)
        add_subdirectory(miniDAQ)
    endif()

    if(CompileForShep OR NoDataShipping)
        add_subdirectory(DQMUtils)
        add_subdirectory(RootUtils)
    endif()

    if(NoDataShipping OR (CompileForShep AND CompileForHerd))
        add_subdirectory(src)
    endif()

    if(NoDataShipping)
        add_subdirectory(RootWeb)
    endif()

    add_subdirectory(Utils)
    add_subdirectory(NetworkUtils)

    message("--     ${BoldRed}#### Don't forget to set the path and ld_library_path by sourcing setup.sh before running ###${Reset}")
    #set(ENV{PATH} "$ENV{PATH}:${PROJECT_SOURCE_DIR}/bin")
    #set(ENV{LD_LIBRARY_PATH} "${PROJECT_SOURCE_DIR}/lib:$ENV(LD_LIBRARY_PATH)")

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "${BoldYellow}MIDDLEWARE${Reset} [stand-alone]: [${BoldCyan}${BASE_DIR}/CMakeLists.txt${Reset}]") 
    MESSAGE(STATUS "${BoldYellow}MIDDLEWARE${Reset} [stand-alone]: Compilation of the middleware package done.") 
    MESSAGE(STATUS "${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS " ") 

else() # ------------------------------- Compilation in the otsdaq environment ---------------------------------------------

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS "${BoldYellow}MIDDLEWARE${Reset} [otsdaq]: Compilation of the middleware package for OTSDAQ") 
    MESSAGE(STATUS "${BoldYellow}MIDDLEWARE${Reset} [otsdaq]: [${BoldCyan}${BASE_DIR}/CMakeLists.txt${Reset}]") 
    MESSAGE(STATUS " ") 

    MESSAGE (STATUS "${CMAKE_CXX_FLAGS}")

    add_subdirectory(tools)
#    add_subdirectory(RootWeb)
    add_subdirectory(DQMUtils)
    add_subdirectory(Utils)
    add_subdirectory(RootUtils)
    add_subdirectory(NetworkUtils)
    add_subdirectory(HWDescription)
    add_subdirectory(HWInterface)
    add_subdirectory(System)
    add_subdirectory(miniDAQ)

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "${BoldYellow}MIDDLEWARE${Reset} [otsdaq]: [${BoldCyan}${BASE_DIR}/CMakeLists.txt${Reset}]") 
    MESSAGE(STATUS "${BoldYellow}MIDDLEWARE${Reset} [otsdaq]: Compilation of the middleware package done.") 
    MESSAGE(STATUS "${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS " ") 

endif()

