 if(NOT DEFINED ENV{OTSDAQ_CMSOUTERTRACKER_DIR})

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "    ${BoldYellow}========================================================================================================${Reset}") 
    MESSAGE(STATUS "    ${BoldYellow}MIDDLEWARE${Reset} [stand-alone/middleware]: [${BoldCyan}Ph2_ACF/Utils/CMakeLists.txt${Reset}]. ${BoldRed}Begin...${Reset}") 
    MESSAGE(STATUS " ") 

    # Includes
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${UHAL_UHAL_INCLUDE_PREFIX})
    
    # Library
    link_directories(${UHAL_UHAL_LIB_PREFIX})

    # Find source files
    file(GLOB HEADERS *.h)
    file(GLOB SOURCES *.cc)
    
    # Add the library
    add_library(Ph2_Utils STATIC ${SOURCES} ${HEADERS})
    set(LIBS ${LIBS} pugixml boost_thread)
    TARGET_LINK_LIBRARIES(Ph2_Utils ${LIBS})
    
    # Check for ZMQ installed
    if(ZMQ_FOUND)
        if(PH2_USBINSTLIB_FOUND)
            include_directories(${PH2_USBINSTLIB_INCLUDE_DIRS})
            include_directories(${ZMQ_INCLUDE_DIRS})
            link_directories(${PH2_USBINSTLIB_LIBRARY_DIRS})
    
            set(LIBS ${LIBS} ${ZMQ_LIBRARIES} ${PH2_USBINSTLIB_LIBRARIES})
            TARGET_LINK_LIBRARIES(Ph2_Utils ${LIBS})
            set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__ZMQ__ -D__USBINST__")
        endif(PH2_USBINSTLIB_FOUND)
    else(ZMQ_FOUND)
            list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/UsbUtilities.cc)
            list(REMOVE_ITEM HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/UsbUtilities.h)
    endif(ZMQ_FOUND)
    
    ####################################
    ## EXECUTABLES
    ####################################

    file(GLOB BINARIES RELATIVE ${PROJECT_SOURCE_DIR}/Utils *.cc)

    message("--     ${BoldCyan}#### Building the following executables: ####${Reset}")
    foreach( sourcefile ${BINARIES} )
        string(REPLACE ".cc" "" name ${sourcefile})
        message(STATUS "    ${name}")
    endforeach(sourcefile ${BINARIES})
    message("--     ${BoldCyan}#### End ####${Reset}")

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "    ${BoldYellow}MIDDLEWARE${Reset} [stand-alone/middleware]: [${BoldCyan}Ph2_ACF/Utils/CMakeLists.txt${Reset}]. ${BoldGreen}DONE!${Reset}") 
    MESSAGE(STATUS "    ${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS " ") 

else()

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "    ${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS "    ${BoldYellow}MIDDLEWARE${Reset} [otsdaq/middleware]: [${BoldCyan}Ph2_ACF/Utils/CMakeLists.txt${Reset}]. ${BoldRed}Begin...${Reset}") 
    MESSAGE(STATUS " ") 

    include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/uhal/uhal_2_4_2/cactuscore/uhal/uhal/include)
    include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/uhal/uhal_2_4_2/cactuscore/uhal/log/include)
    include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/uhal/uhal_2_4_2/cactuscore/uhal/grammars/include)
    #include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/otsdaq-cmsoutertracker/Ph2_ACF)
    #include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/uhal/uhal_2_4_2/cactuscore/extern/pugixml/RPMBUILD/SOURCES/include/)

    cet_set_compiler_flags(
     EXTRA_FLAGS -Wno-reorder -Wl,--undefined
     )
    
    cet_make(LIBRARY_NAME Ph2_Utils_${Ph2_ACF_Master}
             LIBRARIES
             pthread
             ${Boost_SYSTEM_LIBRARY}
             EXCLUDE UsbUtilities.h UsbUtilities.cc
            )
    
    install_headers()
    install_source()

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "    ${BoldYellow}MIDDLEWARE${Reset} [otsdaq/middleware]: [${BoldCyan}Ph2_ACF/Utils/CMakeLists.txt${Reset}]. ${BoldGreen}DONE!${Reset}") 
    MESSAGE(STATUS "    ${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS " ") 

endif()
