if(NOT DEFINED ENV{OTSDAQ_CMSOUTERTRACKER_DIR})

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "    ${BoldYellow}========================================================================================================${Reset}") 
    MESSAGE(STATUS "    ${BoldYellow}MIDDLEWARE${Reset} [stand-alone/middleware]: [${BoldCyan}Ph2_ACF/tools/CMakeLists.txt${Reset}]. ${BoldRed}Begin...${Reset}") 
    MESSAGE(STATUS " ") 

    #includes
    include_directories(${UHAL_UHAL_INCLUDE_PREFIX})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${PROJECT_SOURCE_DIR}/HWDescription)
    include_directories(${PROJECT_SOURCE_DIR}/HWInterface)
    include_directories(${PROJECT_SOURCE_DIR}/Utils)
    include_directories(${PROJECT_SOURCE_DIR}/DQMUtils)
    include_directories(${PROJECT_SOURCE_DIR}/System)
    include_directories(${PROJECT_SOURCE_DIR})

    #library dirs
    link_directories(${UHAL_UHAL_LIB_PREFIX})

    #find source files
    file(GLOB HEADERS *.h)
    file(GLOB SOURCES *.cc)

    #add the library
    add_library(Ph2_Tools STATIC ${SOURCES} ${HEADERS})

    #check for ZMQ installed
    if(ZMQ_FOUND)
        #here, now check for UsbInstLib
        if(PH2_USBINSTLIB_FOUND)

            #add include directoreis for ZMQ and USBINSTLIB
            include_directories(${PH2_USBINSTLIB_INCLUDE_DIRS})
            link_directories(${PH2_USBINSTLIB_LIBRARY_DIRS})
            include_directories(${ZMQ_INCLUDE_DIRS})

            #and link against the libs
            set(LIBS ${LIBS} ${ZMQ_LIBRARIES} ${PH2_USBINSTLIB_LIBRARIES})
            set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__ZMQ__ -D__USBINST__")
        endif()
    endif()

    #check for AMC13 libraries
    if(${CACTUS_AMC13_FOUND})
        include_directories(${PROJECT_SOURCE_DIR}/AMC13)
        include_directories(${UHAL_AMC13_INCLUDE_PREFIX})
        link_directories(${UHAL_AMC13_LIB_PREFIX})
        set(LIBS ${LIBS} cactus_amc13_amc13 Ph2_Amc13)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__AMC13__")
    endif()

    #check for AntennaDriver
    if(${PH2_ANTENNA_FOUND})
        include_directories(${PH2_ANTENNA_INCLUDE_DIRS})
        link_directories(${PH2_ANTENNA_LIBRARY_DIRS})
        set(LIBS ${LIBS} usb ${PH2_ANTENNA_LIBRARIES})
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__ANTENNA__")
    endif()

    #last but not least, find root and link against it
    if(NoDataShipping)    
        if(${ROOT_FOUND})
            include_directories(${ROOT_INCLUDE_DIRS})
            set(LIBS ${LIBS} ${ROOT_LIBRARIES})
            if(NoDataShipping)
                set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} $ENV{UseRootFlag}")
            endif()

            #check for THttpServer
            if(${ROOT_HAS_HTTP})
                set(LIBS ${LIBS} ${ROOT_RHTTP_LIBRARY})
                set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__HTTP__")
            endif()
        endif()
    endif()

    set(LIBS ${LIBS} Ph2_Description Ph2_Interface Ph2_Utils Ph2_System)
    if(NoDataShipping)
        set(LIBS ${LIBS} Ph2_DQMUtils)
    endif()

    TARGET_LINK_LIBRARIES(Ph2_Tools ${LIBS})

    ####################################
    ## EXECUTABLES
    ####################################

    file(GLOB BINARIES RELATIVE ${PROJECT_SOURCE_DIR}/tools *.cc)

    message("--     ${BoldCyan}#### Building the following executables: ####${Reset}")
    foreach( sourcefile ${BINARIES} )
        string(REPLACE ".cc" "" name ${sourcefile})
        message(STATUS "    ${name}")
    endforeach(sourcefile ${BINARIES})
    message("--     ${BoldCyan}#### End ####${Reset}")

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "    ${BoldYellow}MIDDLEWARE${Reset} [stand-alone/middleware]: [${BoldCyan}Ph2_ACF/tools/CMakeLists.txt${Reset}]. ${BoldGreen}DONE!${Reset}") 
    MESSAGE(STATUS "    ${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS " ") 

else()

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "    ${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS "    ${BoldYellow}MIDDLEWARE${Reset} [otsdaq/middleware]: [${BoldCyan}Ph2_ACF/tools/CMakeLists.txt${Reset}]. ${BoldRed}Begin...${Reset}") 
    MESSAGE(STATUS " ") 
    
    include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/uhal/uhal_2_4_2/cactuscore/uhal/uhal/include)
    include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/uhal/uhal_2_4_2/cactuscore/uhal/log/include)
    include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/uhal/uhal_2_4_2/cactuscore/uhal/grammars/include)
    #include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/otsdaq-cmsinnertracker/Ph2_ACF)
    #include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/uhal/uhal_2_4_2/cactuscore/extern/pugixml/RPMBUILD/SOURCES/include/)
    

    cet_set_compiler_flags(
     EXTRA_FLAGS -Wno-reorder -Wl,--undefined
     )
    
    cet_make(LIBRARY_NAME Ph2_Tools_${Ph2_ACF_Master}
            LIBRARIES
            pthread
            ${Boost_SYSTEM_LIBRARY}
            )
    
    install_headers()
    install_source()

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "    ${BoldYellow}MIDDLEWARE${Reset} [otsdaq/middleware]: [${BoldCyan}Ph2_ACF/tool/CMakeLists.txt${Reset}]. ${BoldGreen}DONE!${Reset}") 
    MESSAGE(STATUS "    ${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS " ") 

endif()
