if(NOT DEFINED ENV{OTSDAQ_CMSOUTERTRACKER_DIR})

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "    ${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS "    ${BoldYellow}MIDDLEWARE${Reset} [stand-alone/middleware]: [${BoldCyan}Ph2_ACF/HWInterface/CMakeLists.txt${Reset}]. ${BoldRed}Begin...${Reset}")  
    MESSAGE(STATUS " ") 

    #includes
    include_directories(${UHAL_UHAL_INCLUDE_PREFIX})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${PROJECT_SOURCE_DIR}/HWDescription)
    include_directories(${PROJECT_SOURCE_DIR}/HWInterface)
    include_directories(${PROJECT_SOURCE_DIR})
    
    #replace this with find_package
    link_directories(${UHAL_UHAL_LIB_PREFIX})
    
    #find source files
    file(GLOB HEADERS *.h)
    file(GLOB SOURCES *.cc)
    
    #add the library
    add_library(Ph2_Interface STATIC ${SOURCES} ${HEADERS})
    set(LIBS ${LIBS} Ph2_Description cactus_uhal_uhal cactus_uhal_log boost_regex)
    TARGET_LINK_LIBRARIES(Ph2_Interface ${LIBS})

    ####################################
    ## EXECUTABLES
    ####################################

    file(GLOB BINARIES RELATIVE ${PROJECT_SOURCE_DIR}/HWInterface *.cc)

    message("--     ${BoldCyan}#### Building the following executables: ####${Reset}")
    foreach( sourcefile ${BINARIES} )
        string(REPLACE ".cc" "" name ${sourcefile})
        message(STATUS "    ${name}")
    endforeach(sourcefile ${BINARIES})
    message("--     ${BoldCyan}#### End ####${Reset}")

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "    ${BoldYellow}MIDDLEWARE${Reset} [stand-alone/middleware]: [${BoldCyan}Ph2_ACF/HWInterface/CMakeLists.txt${Reset}]. ${BoldGreen}DONE!${Reset}") 
    MESSAGE(STATUS "    ${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS " ") 
    
else()

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "    ${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS "    ${BoldYellow}MIDDLEWARE${Reset} [otsdaq/middleware]: [${BoldCyan}Ph2_ACF/HWInterface/CMakeLists.txt${Reset}]. ${BoldRed}Begin...${Reset}") 
    MESSAGE(STATUS " ") 

    include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/uhal/uhal_2_4_2/cactuscore/uhal/uhal/include)
    include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/uhal/uhal_2_4_2/cactuscore/uhal/log/include)
    include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/uhal/uhal_2_4_2/cactuscore/uhal/grammars/include)
    #include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/otsdaq-cmsinnertracker/Ph2_ACF)
    #include_directories($ENV{OTSDAQ_CMSOUTERTRACKER_DIR}/uhal/uhal_2_4_2/cactuscore/extern/pugixml/RPMBUILD/SOURCES/include/)
 
    cet_set_compiler_flags(
     EXTRA_FLAGS -Wno-reorder -Wl,--undefined
    )
     
    cet_make(LIBRARY_NAME Ph2_Interface_${Ph2_ACF_Master}
             LIBRARIES
             Ph2_Description_${Ph2_ACF_Master}
             cactus_uhal_uhal
            )
    
    file(GLOB HEADERS *.h)

    install_headers()
    install_source()

    MESSAGE(STATUS " ") 
    MESSAGE(STATUS "    ${BoldYellow}MIDDLEWARE${Reset} [otsdaq/middleware]: [${BoldCyan}Ph2_ACF/HWInterface/CMakeLists.txt${Reset}]. ${BoldGreen}DONE!${Reset}") 
    MESSAGE(STATUS "    ${BoldBlue}========================================================================================================${Reset}") 
    MESSAGE(STATUS " ") 

endif()
