/*!

        \file                   Tool.h
        \brief                                   Controller of the System, overall wrapper of the framework
        \author                                  Georg AUZINGER
        \version                 1.0
        \date                                    06/02/15
        Support :                                mail to : georg.auzinger@cern.ch

 */


#ifndef __TOOL_H__
#define __TOOL_H__

#include "../System/SystemController.h"
#include "../Utils/ContainerStream.h"

#ifdef __USE_ROOT__
    #include "TROOT.h"
    #include "TSystem.h"
    #include "TFile.h"
    #include "TObject.h"
    #include "TCanvas.h"
#endif


class DetectorContainer;
class DetectorDataContainer;
class ChannelGroupHandler;
class ChannelGroupBase;
class ScanBase;


#ifdef __HTTP__
#include "THttpServer.h"
#endif


typedef std::vector<uint16_t> MaskedChannels;
typedef std::map<std::string , MaskedChannels> MaskedChannelsList;
typedef std::vector<std::pair< std::string, uint16_t> > RegisterVector;

/*!
 * \class Tool
 * \brief A base class for all kinds of applications that have to use ROOT that inherits from SystemController which does not have any dependence on ROOT
 */
class Tool : public Ph2_System::SystemController
{

    #ifdef __USE_ROOT__
        using ChipHistogramMap = std::map<Chip*, std::map<std::string, TObject*> >;
        using ModuleHistogramMap = std::map<Module*, std::map<std::string, TObject*> >;
        using BeBoardHistogramMap = std::map<BeBoard*, std::map<std::string, TObject*> >;
        using CanvasMap = std::map<Ph2_HwDescription::FrontEndDescription*, TCanvas*>;
    #endif

    using TestGroupChannelMap =  std::map< int, std::vector<uint8_t> >;

  public:
    // using ChannelOccupancy                    = std::vector<float>; //strip        : occupancy
    // using ChipOccupancyPerChannelMap           = std::map<uint8_t,ChannelOccupancy             >; //cbc          : { strip  : occupancy }
    // using ModuleOccupancyPerChannelMap        = std::map<uint8_t,ChipOccupancyPerChannelMap    >; //module       : { cbc    : { strip : occupancy } }

    // using ChipGlobalOccupancyMap               = std::map<uint8_t,float>; //cbc          : { strip  : occupancy }
    // using ModuleGlobalOccupancyMap            = std::map<uint8_t,ChipGlobalOccupancyMap    >; //module       : { cbc    : { strip : occupancy } }
    // using BackEndBoardOccupancyMap  = std::map<uint8_t,ModuleOccupancyPerChannelMap >; //backEndBoard : { module : { cbc   : { strip : occupancy } } }

    Tool();
    #ifdef __USE_ROOT__    
        #ifdef __HTTP__
            Tool (THttpServer* pServer);
        #endif
    #endif
    Tool (const Tool& pTool);
    ~Tool();

    void Inherit      (Tool* pTool);
    void Inherit      (Ph2_System::SystemController* pSystemController);
    void resetPointers();
    void Destroy      ();
    void SoftDestroy  ();

    #ifdef __USE_ROOT__
        void bookHistogram ( Chip* pChip, std::string pName, TObject* pObject );

        void bookHistogram ( Module* pModule, std::string pName, TObject* pObject );

        void bookHistogram ( BeBoard* pBeBoard, std::string pName, TObject* pObject );

        TObject* getHist ( Chip* pChip, std::string pName );

        TObject* getHist ( Module* pModule, std::string pName );
        
        TObject* getHist ( BeBoard* pBeBoard, std::string pName );

        void WriteRootFile();
    #endif

    void SaveResults();

    /*!
     * \brief Create a result directory at the specified path + ChargeMode + Timestamp
     * \param pDirectoryname : the name of the directory to create
     * \param pDate : apend the current date and time to the directoryname
     */
    void CreateResultDirectory ( const std::string& pDirname, bool pMode = true, bool pDate = true );

    /*!
     * \brief Initialize the result Root file
     * \param pFilename : Root filename
     */
    #ifdef __USE_ROOT__
        void InitResultFile ( const std::string& pFilename );
        void CloseResultFile();
        void StartHttpServer ( const int pPort = 8080, bool pReadonly = true );
        void HttpServerProcess();
    #endif

    void dumpConfigFiles();
    // general stuff that can be useful
    void setSystemTestPulse ( uint8_t pTPAmplitude, uint8_t pTestGroup, bool pTPState = false, bool pHoleMode = false );
    //enable test pulse
    void enableTestPulse(bool enableTP);

    //enable commissioning loops and Test Pulse
    void setFWTestPulse();
    // make test groups for everything Test pulse or Calibration
    void SetTestAllChannels   (bool pAllChan)            { fAllChan            = pAllChan;            }
    void SetTestPulse         (bool pTestPulse)          { fTestPulse          = pTestPulse;          }
    void SetModuleBroadcast   (bool pDoBroadcast)        { fDoModuleBroadcast  = pDoBroadcast;        }
    void SetBoardBroadcast    (bool pDoBroadcast)        { fDoBoardBroadcast   = pDoBroadcast;        }
    void SetSkipMaskedChannels(bool pSkipMaskedChannels) { fSkipMaskedChannels = pSkipMaskedChannels; }
    //for hybrid testing
    void CreateReport();
    void AmmendReport (std::string pString );

    // helper methods
    void ProcessRequests()
    {
        #ifdef __HTTP__
            if (fHttpServer) fHttpServer->ProcessRequests();
        #endif
    }

    std::string getResultFileName()
    {
        if (!fResultFileName.empty() )
            return fResultFileName;
        else
            return "";
    }

    void setRegBit ( uint16_t& pRegValue, uint8_t pPos, bool pValue )
    {
        pRegValue ^= ( -pValue ^ pRegValue ) & ( 1 << pPos );
    }

    void toggleRegBit ( uint16_t& pRegValue, uint8_t pPos )
    {
        pRegValue ^= 1 << pPos;
    }

    bool getBit ( uint16_t& pRegValue, uint8_t pPos )
    {
        return ( pRegValue >> pPos ) & 1;
    }

    /*!
    * \brief reverse the endianess before writing in to the register
    * \param pDelay: the actual delay
    * \param pGroup: the actual group number
    * \return the reversed endianness
    */
    inline uint8_t to_reg ( uint8_t pDelay, uint8_t pGroup )
    {

        uint8_t cValue = ( ( reverse ( pDelay ) ) & 0xF8 ) |
                         ( ( reverse ( pGroup ) ) >> 5 );

        //LOG(DBUG) << std::bitset<8>( cValue ) << " cGroup " << +pGroup << " " << std::bitset<8>( pGroup ) << " pDelay " << +pDelay << " " << std::bitset<8>( pDelay ) ;
        return cValue;
    }
    /*!
    * \brief reverse the byte
    * \param n:the number to be reversed
    * \return the reversed number
    */
    inline uint8_t reverse ( uint8_t n )
    {
        // Reverse the top and bottom nibble then swap them.
        return ( fLookup[n & 0xF] << 4 ) | fLookup[n >> 4];
    }

    unsigned char fLookup[16] =
    {
        0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
        0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf,
    }; /*!< Lookup table for reverce the endianness */

    std::map<uint8_t, std::string> fChannelMaskMapCBC3 =
    {
        { 0, "MaskChannel-008-to-001" },
        { 1, "MaskChannel-016-to-009" },
        { 2, "MaskChannel-024-to-017" },
        { 3, "MaskChannel-032-to-025" },
        { 4, "MaskChannel-040-to-033" },
        { 5, "MaskChannel-048-to-041" },
        { 6, "MaskChannel-056-to-049" },
        { 7, "MaskChannel-064-to-057" },
        { 8, "MaskChannel-072-to-065" },
        { 9, "MaskChannel-080-to-073" },
        {10, "MaskChannel-088-to-081" },
        {11, "MaskChannel-096-to-089" },
        {12, "MaskChannel-104-to-097" },
        {13, "MaskChannel-112-to-105" },
        {14, "MaskChannel-120-to-113" },
        {15, "MaskChannel-128-to-121" },
        {16, "MaskChannel-136-to-129" },
        {17, "MaskChannel-144-to-137" },
        {18, "MaskChannel-152-to-145" },
        {19, "MaskChannel-160-to-153" },
        {20, "MaskChannel-168-to-161" },
        {21, "MaskChannel-176-to-169" },
        {22, "MaskChannel-184-to-177" },
        {23, "MaskChannel-192-to-185" },
        {24, "MaskChannel-200-to-193" },
        {25, "MaskChannel-208-to-201" },
        {26, "MaskChannel-216-to-209" },
        {27, "MaskChannel-224-to-217" },
        {28, "MaskChannel-232-to-225" },
        {29, "MaskChannel-240-to-233" },
        {30, "MaskChannel-248-to-241" },
        {31, "MaskChannel-254-to-249" }
    };

    // decode bend LUT for a given CBC
    std::map<uint8_t, double> decodeBendLUT(Chip* pChip);
    
    //method to unmask a channel group
    // void maskChannelFromOtherGroups (Chip* pChip, int pTestGroup);

    // then a method to un-mask pairs of channels on a given CBC
    void unmaskPair(Chip* cChip ,  std::pair<uint8_t,uint8_t> pPair);

    //select the group of channels for injecting the pulse
    void selectGroupTestPulse(Chip* cChip, uint8_t pTestGroup);

    // // Two dimensional dac scan
    void scanDacDac(const std::string &dac1Name, const std::vector<uint16_t> &dac1List, const std::string &dac2Name, const std::vector<uint16_t> &dac2List, uint32_t numberOfEvents, std::vector<std::vector<DetectorDataContainer*>> detectorContainerVectorOfVector, int32_t numberOfEventsPerBurst = -1);
    // // Two dimensional dac scan per BeBoard
    void scanBeBoardDacDac(uint16_t boardIndex, const std::string &dac1Name, const std::vector<uint16_t> &dac1List, const std::string &dac2Name, const std::vector<uint16_t> &dac2List, uint32_t numberOfEvents, std::vector<std::vector<DetectorDataContainer*>> detectorContainerVector, int32_t numberOfEventsPerBurst = -1);
    // One dimensional dac scan
    void scanDac(const std::string &dacName, const std::vector<uint16_t> &dacList, uint32_t numberOfEvents, std::vector<DetectorDataContainer*> detectorContainerVector, int32_t numberOfEventsPerBurst = -1);
    // One dimensional dac scan per BeBoard
    void scanBeBoardDac(uint16_t boardIndex, const std::string &dacName, const std::vector<uint16_t> &dacList, uint32_t numberOfEvents, std::vector<DetectorDataContainer*> &detectorContainerVector, int32_t numberOfEventsPerBurst = -1);
    // bit wise scan
    void bitWiseScan(const std::string &dacName, uint32_t numberOfEvents, const float &targetOccupancy, int32_t numberOfEventsPerBurst = -1);
    // bit wise scan per BeBoard
    void bitWiseScanBeBoard(uint16_t boardIndex, const std::string &dacName, uint32_t numberOfEvents, const float &targetOccupancy, int32_t numberOfEventsPerBurst = -1);
    // set dac and measure data
    void setDacAndMeasureData(const std::string &dacName, const uint16_t dacValue, uint32_t numberOfEvents, int32_t numberOfEventsPerBurst = -1);
    // set dac and measure data per BeBoard
    void setDacAndMeasureBeBoardData(uint16_t boardIndex, const std::string &dacName, const uint16_t dacValue, uint32_t numberOfEvents, int32_t numberOfEventsPerBurst = -1);
    // measure data
    void measureData(uint32_t numberOfEvents, int32_t numberOfEventsPerBurst = -1, uint32_t numberOfTriggersPerL1A = 1);
    // measure data per BeBoard
    void measureBeBoardData(uint16_t boardIndex, uint32_t numberOfEvents, int32_t numberOfEventsPerBurst = -1, uint32_t numberOfTriggersPerL1A = 1);
    // measure data per BeBoard and per group
    // void measureBeBoardDataPerGroup(uint16_t boardIndex, const uint16_t numberOfEvents, const ChannelGroupBase *cTestChannelGroup);
    // set global DAC for all CBCs in the BeBoard
    void setAllGlobalDacBeBoard(uint16_t boardIndex, const std::string &dacName, DetectorDataContainer &globalDACContainer);
    //Set global DAC for all Chips in the BeBoard
    void setAllLocalDacBeBoard(uint16_t boardIndex, const std::string &dacName, DetectorDataContainer &globalDACContainer);
    //Set same global DAC for all Chips
    void setSameGlobalDac(const std::string &dacName, const uint16_t dacValue);
    //Set same global DAC for all Chips in the BeBoard
    void setSameGlobalDacBeBoard(BeBoard* pBoard, const std::string &dacName, const uint16_t dacValue);
    //Set same local DAC list for all Chips
    void setSameLocalDac(const std::string &dacName, const uint16_t dacValue);
    //Set same local DAC for all Chips in the BeBoard
    void setSameLocalDacBeBoard(BeBoard* pBoard, const std::string &dacName, const uint16_t dacValue);
    //Set same DAC for all Chips in the BeBoard (it is able to recognize if the dac is local or global)
    void setSameDacBeBoard(BeBoard* pBoard, const std::string &dacName, const uint16_t dacValue);
    //Set same DAC list for all Chips (it is able to recognize if the dac is local or global)
    void setSameDac(const std::string &dacName, const uint16_t dacValue);

    template<typename T, typename... H>
    ChannelContainerStream<T,H...> prepareChannelContainerStreamer(std::string appendName = "")
    {
        ChannelContainerStream<T,H...> theContainerStreamer(getCalibrationName() + appendName);
        return theContainerStreamer;
    }

    template<typename T, typename C, typename... H>
    ChipContainerStream<T,C,H...> prepareChipContainerStreamer(std::string appendName = "")
    {
        ChipContainerStream<T,C,H...> theContainerStreamer(getCalibrationName() + appendName);
        return theContainerStreamer;
    }

    template<typename T, typename C, typename M, typename... H>
    ModuleContainerStream<T,C,M,H...> prepareModuleContainerStreamer(std::string appendName = "")
    {
        ModuleContainerStream<T,C,M,H...> theContainerStreamer(getCalibrationName() + appendName);
        return theContainerStreamer;
    }


private:
    void doScanOnAllGroupsBeBoard(uint16_t boardIndex, uint32_t numberOfEvents, int32_t numberOfEventsPerBurst, ScanBase *scanFunctor);

    // Old scans without Detector Containers

    // // Two dimensional dac scan
    // void scanDacDac(const std::string &dac1Name, const std::vector<uint16_t> &dac1List, const std::string &dac2Name, const std::vector<uint16_t> &dac2List, const uint16_t &numberOfEvents, std::map<uint16_t, std::map<uint16_t, std::map<uint16_t, ModuleOccupancyPerChannelMap> > > &backEndOccupancyPerChannelMap, std::map<uint16_t, std::map<uint16_t, std::map<uint16_t, ModuleGlobalOccupancyMap > > > &backEndChipOccupanyMap);
    
    // // Two dimensional dac scan per BeBoard
    // void scanBeBoardDacDac(BeBoard* pBoard, const std::string &dac1Name, const std::vector<uint16_t> &dac1List, const std::string &dac2Name, const std::vector<uint16_t> &dac2List, const uint16_t &numberOfEvents, std::map<uint16_t, std::map<uint16_t, ModuleOccupancyPerChannelMap> > &moduleOccupancyPerChannelMap, std::map<uint16_t, std::map<uint16_t, ModuleGlobalOccupancyMap > > &backEndChipOccupanyMap);

    // // One dimensional dac scan
    // void scanDac(const std::string &dacName, const std::vector<uint16_t> &dacList, const uint16_t &numberOfEvents, std::map<uint16_t, std::map<uint16_t, ModuleOccupancyPerChannelMap> > &backEndOccupancyPerChannelMap, std::map<uint16_t, std::map<uint16_t, ModuleGlobalOccupancyMap > > &backEndChipOccupanyMap);

    // // One dimensional dac scan per BeBoard
    // void scanBeBoardDac(BeBoard* pBoard, const std::string &dacName, const std::vector<uint16_t> &dacList, const uint16_t &numberOfEvents, std::map<uint16_t, ModuleOccupancyPerChannelMap> &moduleOccupancyPerChannelMap, std::map<uint16_t, ModuleGlobalOccupancyMap > &moduleChipOccupanyMap);

    // // bit wise scan
    // void bitWiseScan(const std::string &dacName, const uint16_t &numberOfEvents, const float &targetOccupancy, bool isOccupancyTheMaximumAccepted, std::map<uint16_t, ModuleOccupancyPerChannelMap> &backEndOccupanyPerChannelAtTargetMap, std::map<uint16_t, ModuleGlobalOccupancyMap> &backEndOccupanyAtTargetMap);

    // // bit wise scan per BeBoard
    // void bitWiseScanBeBoard(BeBoard* pBoard, const std::string &dacName, const uint16_t &numberOfEvents, const float &targetOccupancy, bool &isOccupancyTheMaximumAccepted, ModuleOccupancyPerChannelMap &moduleOccupancyPerChannelMap, ModuleGlobalOccupancyMap &moduleOccupancyMap);
    
    // // set dac and measure occupancy
    // void setDacAndMeasureOccupancy(const std::string &dacName, const uint16_t &dacValue, const uint16_t &numberOfEvents, std::map<uint16_t, ModuleOccupancyPerChannelMap> &backEndOccupancyPerChannelMap, std::map<uint16_t, ModuleGlobalOccupancyMap > &backEndChipOccupanyMap, float &globalOccupancy);

    // // set dac and measure occupancy per BeBoard
    // void setDacAndMeasureBeBoardOccupancy(BeBoard* pBoard, const std::string &dacName, const uint16_t &dacValue, const uint16_t &numberOfEvents, ModuleOccupancyPerChannelMap &moduleOccupancyPerChannelMap, ModuleGlobalOccupancyMap &cbcOccupanyMap, float &globalOccupancy);
    
    // // measure occupancy
    // void measureOccupancy(const uint16_t &numberOfEvents, std::map<uint16_t, ModuleOccupancyPerChannelMap> &backEndOccupancyPerChannelMap, std::map<uint16_t, ModuleGlobalOccupancyMap > &backEndChipOccupanyMap, float &globalOccupancy);

    // // measure occupancy
    // void measureBeBoardOccupancy(BeBoard* pBoard, const uint16_t &numberOfEvents, ModuleOccupancyPerChannelMap &moduleOccupancyPerChannelMap, ModuleGlobalOccupancyMap &cbcOccupanyMap, float &globalOccupancy);

    // // measure occupancy per group
    // void measureBeBoardOccupancyPerGroup(const std::vector<uint8_t> &cTestGrpChannelVec, BeBoard* pBoard, const uint16_t &numberOfEvents, ModuleOccupancyPerChannelMap &moduleOccupancyPerChannelMap);

    // //Set global DAC for all Chips in the BeBoard
    // void setAllGlobalDacBeBoard(BeBoard* pBoard, const std::string &dacName, const std::map<uint8_t, std::map<uint8_t, uint16_t> > &dacList);

    // //Set local DAC list for all Chips in the BeBoard
    // void setAllLocalDacBeBoard(BeBoard* pBoard, const std::string &dacName, const std::map<uint8_t, std::map<uint8_t, std::vector<uint16_t> > > &dacList);

  protected:
    DetectorDataContainer* fDetectorDataContainer {nullptr};
    #ifdef __USE_ROOT__    
        CanvasMap              fCanvasMap;
        ChipHistogramMap       fChipHistMap;
        ModuleHistogramMap     fModuleHistMap;
        BeBoardHistogramMap    fBeBoardHistMap;
    #endif

    FrontEndType           fType;
    TestGroupChannelMap    fTestGroupChannelMap;

    std::map< int, std::vector<uint8_t> > fMaskForTestGroupChannelMap;

    std::string fDirectoryName;             /*< the Directoryname for the Root file with results */
    #ifdef __USE_ROOT__    
        TFile*      fResultFile;                /*< the Name for the Root file with results */
    #endif
    std::string fResultFileName;
    #ifdef __HTTP__
        THttpServer* fHttpServer;
    #endif

    bool fSkipMaskedChannels;
    bool fAllChan;
    bool fMaskChannelsFromOtherGroups;
    bool fTestPulse;
    bool fDoModuleBroadcast;
    bool fDoBoardBroadcast;
    ChannelGroupHandler*   fChannelGroupHandler;

    std::string getCalibrationName();


};

#endif
