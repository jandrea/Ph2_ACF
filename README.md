# CMS Ph2 ACF (Acquisition & Control Framework)


### Contains:

- A middleware API layer, implemented in C++, which wraps the firmware calls and handshakes into abstracted functions
- A C++ object-based library describing the system components (CBCs, RD53, Hybrids, Boards) and their properties (values, status)


## Middleware for the Inner-Tracker (IT) system
```diff
+ Last change made to this section: 13/01/2020
```
Setup the FC7:
1. Install `wireshark` in order to figure out which is the MAC address of your FC7 board (`sudo yum install wireshark`, then run `sudo tshark -i ethernet_card`, where `ethernet_card` is the name of the ethernet card of your PC to which the FC7 is connected to)
2. In `/etc/ethers` put `mac_address fc7.board.1` and in `/etc/hosts` put `192.168.1.80 fc7.board.1`
3. Restart the network: `sudo /etc/init.d/network restart`
4. Install the rarpd daemon (version for CENTOS6 should work just fine even for CENTOS7): `sudo yum install rarp_file_name.rpm` from https://centos.pkgs.org/6/epel-x86_64/rarpd-ss981107-42.el6.x86_64.rpm.html
5. Start the rarpd daemon: `sudo systemctl start rarpd` or `rarp -e -A` (to start rarpd automatically after bootstrap: `sudo systemctl enable rarpd`)

More details on the hardware needed to setup the system can be bound here: https://espace.cern.ch/Tracker-Upgrade/DAQ/SitePages/Home.aspx

Setup the firmware:
1. Check whether the DIP switches on FC7 board are setup for the use of a microSD card (`out-in-in-in-out-in-in-in`)
2. Insert a microSD card in the PC and run `/sbin/fdisk -l` to understand to which dev it's attached to (`/dev/sd_card_name`)
3. Upload a golden firmware* on the microSD card (read FC7 manual or run `dd if=sdgoldenimage.img of=/dev/sd_card_name bs=512`)
4. Download the proper IT firmware version from https://gitlab.cern.ch/cmstkph2-IT/d19c-firmware/releases
5. Plug the microSD card in the FC7
6. From Ph2_ACF use the command `fpgaconfig` to upload the proper IT firmware (see instructions: `Setup and run the IT-DAQ` before running this command)

*A golden firmware is any stable firmware either from IT or OT, and it's needed just to initialize the IPbus communication at bootstrap (in order to create and image of the microSD card you can use the command: `dd if=/dev/sd_card_name conv=sync,noerror bs=128K | gzip -c > sdgoldenimage.img.gz`) <br />
A golden firmware can be downloaded from here: https://cernbox.cern.ch/index.php/s/5tUCio08PEfTf0a

Setup and run the IT-DAQ:
1. `sudo yum install pugixml-devel` (if necesary run `sudo yum install epel-release` before point 1.)
2. Install: `boost` by running `sudo yum install boost-devel`, `CERN ROOT` from https://root.cern.ch, and `IPbus` from http://ipbus.web.cern.ch/ipbus (either using `sudo yum` or from source)
3. Checkout the DAQ code from git: `git clone https://gitlab.cern.ch/cmsinnertracker/Ph2_ACF.git`
4. `cd Ph2_ACF; mkdir myBuild; cd myBuild; cmake ..; make -j4; cd ..; source setup.sh`
5. `mkdir choose_a_name`
6. `cp settings/RD53Files/CMSIT_RD53.txt choose_a_name`
7. `cp settings/CMSIT.xml choose_a_name`
8. `cd choose_a_name`
9. Edit the file `CMSIT.xml` in case you want to change some parameters needed for the calibrations or for configuring the chip
10. Run the command: `CMSIT_miniDAQ -f CMSIT.xml -r` to reset the FC7 (just once)
11. Run the command: `CMSIT_miniDAQ -f CMSIT.xml -c name_of_the_calibration` (or `CMSIT_miniDAQ --help` for help)

Basic list of commands for the `fpgaconfig` program (run from the `choose_a_name` directory):
- Run the command: `fpgaconfig -c CMSIT.xml -l` to check which firmware is on the microSD card
- Run the command: `fpgaconfig -c CMSIT.xml -f firmware_file_name_on_the_PC -i firmware_file_name_on_the_microSD` to upload a new firmware to the microSD card
- Run the command: `fpgaconfig -c CMSIT.xml -i firmware_file_name_on_the_microSD` to load a new firmware from the microSD card to the FPGA
- Run the command: `fpgaconfig --help` for help

The program `CMSIT_miniDAQ` is the portal for all calibrations and for data taking.
Through `CMSIT_miniDAQ`, and with the right command line option, you can run the following scans/ calibrations/ operation mode:
```
1. Latency scan
2. PixelAlive
3. Noise scan
4. SCurve scan
5. Gain scan
6. Threshold equalization
7. Gain optimization
8. Threshold minimization
9. Injection delay scan
10. Clock delay scan
11. Physics
```
It might be useful to create one `CMSIT.xml` file for each "set" of calibrations. In the following it is reported the suggested sequence of calibrations, implemented in bash shell script:
```
#!/bin/bash
if [ $# -ne 1 ]
then
    echo "You should provide one, and only one, argument [step1, step2, step3, step4, step5, help]"
elif [ $1 == "step1" ]
then
    CMSIT_miniDAQ -f CMSIT_noise.xml -c noise # Masks noisy pixels
    echo "noise" >> calibDone.txt

    CMSIT_miniDAQ -f CMSIT_scurve.xml -c pixelalive # Masks dead pixels
    echo "pixelalive" >> calibDone.txt

    CMSIT_miniDAQ -f CMSIT_noise.xml -c thrmin
    echo "thrmin" >> calibDone.txt

    echo "Choose whether to accept new threshold (i.e. copy it into the xml file(s))"
    read -p "Press any key to continue... " -n1 -s
    echo
elif [ $1 == "step2" ]
then
    CMSIT_miniDAQ -f CMSIT_scurve.xml -c threqu
    echo "scurve" >> calibDone.txt
    echo "threqu" >> calibDone.txt

    CMSIT_miniDAQ -f CMSIT_scurve.xml -c scurve
    echo "scurve" >> calibDone.txt

    CMSIT_miniDAQ -f CMSIT_noise.xml -c noise # Masks noisy pixels @ new threshold
    echo "noise" >> calibDone.txt

    CMSIT_miniDAQ -f CMSIT_noise.xml -c thrmin
    echo "thrmin" >> calibDone.txt

    echo "Choose whether to accept new threshold (i.e. copy it into the xml file(s))"
    read -p "Press any key to continue... " -n1 -s
    echo
elif [ $1 == "step3" ]
then
    CMSIT_miniDAQ -f CMSIT_scurve.xml -c scurve
    echo "scurve" >> calibDone.txt

    CMSIT_miniDAQ -f CMSIT_gain.xml -c gain
    echo "gain" >> calibDone.txt

    CMSIT_miniDAQ -f CMSIT_gain.xml -c gainopt
    echo "gainopt" >> calibDone.txt

    echo "Choose whether to accept new Krummenacher current (i.e. copy it into the xml file(s))"
    echo "- Set nTRIGxEvent = 1 and DoFast = 1 in the xml file(s)"
    echo "- Set VCAL_HIGH to MIP value in the xml file(s)"
    read -p "Press any key to continue... " -n1 -s
    echo
elif [ $1 == "step4" ]
then
    CMSIT_miniDAQ -f CMSIT_scurve.xml -c injdelay
    echo "latency" >> calibDone.txt
    echo "injdelay" >> calibDone.txt

    echo "Choose whether to accept new LATENCY_CONFIG and INJECTION_SELECT (i.e. copy them into the xml file(s))"
    echo "- Set DoFast to whatever value you prefer in the xml files(s)"
    read -p "Press any key to continue... " -n1 -s
    echo
elif [ $1 == "step5" ]
then
    CMSIT_miniDAQ -f CMSIT_scurve.xml -c scurve
    echo "scurve" >> calibDone.txt
elif [ $1 == "help" ]
then
    echo "Available options are:"
    echo "- step1 [noise + pixelalive + thrmin]"
    echo "- step2 [(scurve)threqu + scurve + noise + thrmin]"
    echo "- step3 [scurve + gain + gainopt]"
    echo "- step4 [(latency)injdelay]"
    echo "- step5 [scurve]"
else
    echo "Argument not recognized: $1"
fi
```
Here you can find a detailed description of the various calibrations: https://cernbox.cern.ch/index.php/s/O07UiVaX3wKiZ78
- Software git branch / tag : `chipPolymorphism` / `IT-v3.0`
- Firmware tag: `3.0`
- Mattermost forum: `cms-it-daq` (https://mattermost.web.cern.ch/cms-it-daq/)

### ~=-=~ End of Inner-Tracker section ~=-=~

<hr>


### Setup

Firmware for the FC7 can be found in /firmware. Since the "old" FMC flavour is deprecated, only new FMCs (both connectors on the same side) are supported.
You'll need Xilinx Vivado and a Xilinx Platform Cable USB II (http://uk.farnell.com/xilinx/hw-usb-ii-g/platform-cable-configuration-prog/dp/1649384)
For more information on the firmware, please check the doc directory of https://gitlab.cern.ch/cms_tk_ph2/d19c-firmware


### Setup on CC7 (Scroll down for instructions on setting up on SLC6)

1. Check which version of gcc is installed on your CC7, it should be > 4.8 (could be the default on CC7):

        $> gcc --version

2. On CC7 you also need to install boost v1.53 headers (default on this system) as they don't ship with uHAL any more:

        $> sudo yum install boost-devel

3. Install uHAL. The uHAL version should be 2.5 or lower. Version 2.6 does not work with the middleware at the moment! 

    First create a new ipbus repo for yum:

        $> sudo cat > /etc/yum.repos.d/ipbus-sw.repo << EOF
        $> [ipbus-sw-base]
        $> name=IPbus software repository
        $> baseurl=http://www.cern.ch/ipbus/sw/release/2.5/centos7_x86_64/base/RPMS
        $> enabled=1
        $> gpgcheck=0

        $> [ipbus-sw-updates]
        $> name=IPbus software repository updates
        $> baseurl=http://www.cern.ch/ipbus/sw/release/2.5/centos7_x86_64/updates/RPMS
        $> enabled=1
        $> gpgcheck=0

    Then install uHAL as follows:

        $> sudo yum clean all
        $> sudo yum groupinstall uhal

4. Install CERN ROOT 6.10 on CC7, as well as libusb and xorg:

        $> sudo yum install root
        $> sudo yum install root-net-http root-graf3d-gl root-physics root-montecarlo-eg root-graf3d-eve root-geom libusb-devel xorg-x11-xauth.x86_64

5. Install CMAKE > 2.8:

        $> sudo yum install cmake


### Setup on SLC6

1. Install a gcc compiler version > 4.8 - on scientific linux you can obtain this by installing devtoolset-2 or devtoolset-2.1:

        $> sudo wget -O /etc/yum.repos.d/slc6-devtoolset.repo http://linuxsoft.cern.ch/cern/devtoolset/slc6-devtoolset.repo
        $> sudo yum install devtoolset-2
        $> . /opt/rh/devtoolset-2/enable   # add this to your .bashrc
        $> sudo ln -s /opt/rh/devtoolset-2/root/usr/bin/* /usr/local/bin/
        $> hash -r

    This should give you a more recent gcc (e.g. gcc 4.8.2)

        $> gcc --version

    Alternatively you can use a gcc version > 4.8 from AFS

2. Install uHAL. The uHAL version should be 2.5 or lower. Version 2.6 does not work with the middleware at the moment! 

    First create a new ipbus repo for yum:

        $> sudo cat > /etc/yum.repos.d/ipbus-sw.repo << EOF
        $> [ipbus-sw-base]
        $> name=IPbus software repository
        $> baseurl=http://www.cern.ch/ipbus/sw/release/2.5/centos7_x86_64/base/RPMS
        $> enabled=1
        $> gpgcheck=0

        $> [ipbus-sw-updates]
        $> name=IPbus software repository updates
        $> baseurl=http://www.cern.ch/ipbus/sw/release/2.5/centos7_x86_64/updates/RPMS
        $> enabled=1
        $> gpgcheck=0

    Then install uHAL as follows:

        $> sudo yum clean all
        $> sudo yum groupinstall uhal

3. Install CERN ROOT version 5.34.32 [Instructions](http://root.cern.ch/drupal/content/installing-root-source) - make sure to use "fixed location installation" when building yourself. If root is installed on a CERN computer of virtual machine you can use:
       
        $> sudo yum install root
        $> sudo yum install root-net-http root-graf3d-gl root-physics root-montecarlo-eg root-graf3d-eve root-geom libusb-devel xorg-x11-xauth.x86_64

4. Install CMAKE > 2.8. On SLC6 the default is cmake 2.8

        $> sudo yum install cmake


### The Ph2_ACF Software : 

Follow these instructions to install and compile the libraries:
(provided you installed the latest version of gcc, µHal,  mentioned above).

1. Clone the GitHub repo and run cmake
  
        $> git clone https://:@gitlab.cern.ch:8443/fravera/Ph2_ACF.git
        $> cd Ph2_ACF
        $> source setup.sh
        $> cd build 
        $> cmake ..

2. Do a `make -jN` in the build/ directory or alternatively do `make -C build/ -jN` in the Ph2_ACF root directory.

3. Don't forget to `source setup.sh` to set all the environment variables correctly.


4. Launch 

        $> systemtest --help

    command if you want to test the parsing of the HWDescription.xml file.

5. Launch

        $> datatest --help

    command if you want to test if you can correctly read data

6. Launch

        $> calibrate --help

    to calibrate a hybrid,

        $> hybridtest --help

    to test a hybird's I2C registers and input channel connectivity

          $> cmtest --help

    to run the CM noise study

          $> pulseshape --help

    to measure the analog pulseshape of the cbc

          $> configure --help

    to apply a configuration to the CBCs

7. Launch

          $> commission --help

    to do latency & threshold scans

8. Launch 

          $> fpgaconfig --help

    to upload a new FW image to the GLIB

9. Launch

          $> miniDAQ --help

    to save binary data from the GLIB to file

10. Launch

          $> miniDQM --help

    to run the DQM code from the June '15 beamtest


### Nota Bene:

When you write a register in the Glib or the Cbc, the corresponding map of the HWDescription object in memory is also updated, so that you always have an exact replica of the HW Status in the memory.

Register values are:
  - 8-bit unsigend integers for the CBCs that should be edited in hex notation, i.e. '0xFF'
  - 32-bit unsigned integers for the GLIB: decimal values

For debugging purpose, you can activate DEV_FLAG in the sources or in the Makefile and also activate the uHal log in RegManager.cc.


### External Clock and Trigger:

Please see the D19C FW  [documentation](https://gitlab.cern.ch/cms_tk_ph2/d19c-firmware/blob/master/doc/Middleware_Short_Guide.md) for instructions on how to use external clock and trigger with the various FMCs (DIO5 and CBC3 FMC)


### Example HWDescription.xml File with DIO5 support:

```xml
<?xml version="1.0" encoding="utf-8"?>
<HwDescription>
  <BeBoard Id="0" boardType="D19C" eventType="VR">
      <connection id="board" uri="chtcp-2.0://localhost:10203?target=192.168.1.81:50001" address_table="file://settings/address_tables/d19c_address_table.xml" />

    <Module FeId="0" FMCId="0" ModuleId="0" Status="1">
        <Global>
            <Settings threshold="550" latency="26"/>
            <TestPulse enable="0" polarity="0" amplitude="0xFF" channelgroup="0" delay="0" groundothers="1"/>
            <ClusterStub clusterwidth="4" ptwidth="3" layerswap="0" off1="0" off2="0" off3="0" off4="0"/>
            <Misc analogmux="0b00000" pipelogic="0" stublogic="0" or254="1" tpgclock="1" testclock="1" dll="4"/>
            <ChannelMask disable=""/>
        </Global>
        <CBC_Files path="./settings/CbcFiles/" />
        <CBC Id="0" configfile="CBC3_default.txt" />
        <CBC Id="1" configfile="CBC3_default.txt" />
    </Module>

    <SLink>
        <DebugMode type="FULL"/>
        <ConditionData type="I2C" Register="VCth1" FeId="0" CbcId="0"/>
        <ConditionData type="User" UID="0x80" FeId="0" CbcId="0"> 0x22 </ConditionData>
        <ConditionData type="HV" FeId="0" Sensor="2"> 250 </ConditionData>
        <ConditionData type="TDC" FeId="0xFF"/>
    </SLink>

    <!--CONFIG-->
    <Register name="clock_source">3</Register> <!-- 3 - default (internal oscillator), 2 - backplane, 0 - AMC13 -->
    <Register name="fc7_daq_cnfg">
	<!-- Clock control -->
	<Register name="clock">
	    <Register name="ext_clk_en"> 0 </Register>
	</Register>
        <!-- TTC -->
        <Register name="ttc">
            <Register name="ttc_enable"> 0 </Register>
        </Register>
        <!-- Fast Command Block -->
        <Register name="fast_command_block">
		<Register name="triggers_to_accept"> 0 </Register>
		<Register name="trigger_source"> 3 </Register>
		<Register name="user_trigger_frequency"> 1 </Register>
		<Register name="stubs_mask"> 1 </Register>
                <!--this is the delay for the stub trigger-->
		<Register name="stub_trigger_delay_value"> 0 </Register>
                <Register name="stub_trigger_veto_length"> 0 </Register>
		<Register name="test_pulse">
			<Register name="delay_after_fast_reset"> 50 </Register>
			<Register name="delay_after_test_pulse"> 200 </Register>
			<Register name="delay_before_next_pulse"> 400 </Register>
			<Register name="en_fast_reset"> 1 </Register>
			<Register name="en_test_pulse"> 1 </Register>
			<Register name="en_l1a"> 1 </Register>
		</Register>
                <Register name="ext_trigger_delay_value"> 50 </Register>
                <Register name="antenna_trigger_delay_value"> 200 </Register>
                <Register name="delay_between_two_consecutive"> 10 </Register>
                <Register name="misc">
                        <Register name="backpressure_enable"> 1 </Register>
                        <Register name="stubOR"> 1 </Register>
                        <Register name="initial_fast_reset_enable"> 0 </Register>
                </Register>
        </Register>
	<!-- I2C manager -->
        <Register name="command_processor_block">
	</Register>
	<!-- Phy Block -->
	<Register name="physical_interface_block">
		<Register name="i2c">
                	<Register name="frequency"> 4 </Register>
		</Register>
	</Register>
	<!-- Readout Block -->
    	<Register name="readout_block">
            <Register name="packet_nbr"> 99 </Register>
            <Register name="global">
		    <Register name="data_handshake_enable"> 1 </Register>
                    <Register name="int_trig_enable"> 0 </Register>
                    <Register name="int_trig_rate"> 0 </Register>
                    <Register name="trigger_type"> 0 </Register>
                    <Register name="data_type"> 0 </Register>
                    <!--this is what is commonly known as stub latency-->
                    <Register name="common_stubdata_delay"> 194 </Register>
            </Register>
    	</Register>
	<!-- DIO5 Block -->
	<Register name="dio5_block">
	    <Register name="dio5_en"> 0 </Register>
            <Register name="ch1">
                <Register name="out_enable"> 1 </Register>
                <Register name="term_enable"> 0 </Register>
                <Register name="threshold"> 0 </Register>
            </Register>
	    <Register name="ch2">
                <Register name="out_enable"> 0 </Register>
                <Register name="term_enable"> 1 </Register>
                <Register name="threshold"> 50 </Register>
            </Register>
	    <Register name="ch3">
                <Register name="out_enable"> 1 </Register>
                <Register name="term_enable"> 0 </Register>
                <Register name="threshold"> 0 </Register>
            </Register>
	    <Register name="ch4">
                <Register name="out_enable"> 0 </Register>
                <Register name="term_enable"> 1 </Register>
                <Register name="threshold"> 50 </Register>
            </Register>
	    <Register name="ch5">
                <Register name="out_enable"> 0 </Register>
                <Register name="term_enable"> 1 </Register>
                <Register name="threshold"> 50 </Register>
            </Register>
	</Register>
	<!-- TLU Block -->
	<Register name="tlu_block">
		<Register name="handshake_mode"> 2 </Register>
		<Register name="tlu_enabled"> 0 </Register>
	</Register>
    </Register>
  </BeBoard>

<Settings>

    <!--[>Calibration<]-->
    <Setting name="TargetVcth">0x78</Setting>
    <Setting name="TargetOffset">0x50</Setting>
    <Setting name="Nevents">50</Setting>
    <Setting name="TestPulsePotentiometer">0x00</Setting>
    <Setting name="HoleMode">0</Setting>
    <Setting name="VerificationLoop">1</Setting>

    <!--Signal Scan Fit-->
	  <Setting name="InitialVcth">0x78</Setting>
	  <Setting name="SignalScanStep">2</Setting>
    <Setting name="FitSignal">0</Setting>

</Settings>
</HwDescription>
```


### Known Issues:

uHAL exceptions and UDP timeouts when reading larger packet sizes from the GLIB board: this can happen for some users (cause not yet identified) but can be circumvented by changing the line

"ipbusudp-2.0://192.168.000.175:50001"

in the connections.xml file to

"chtcp-2.0://localhost:10203?target=192.168.000.175:50001"

and then launching the CACTUS control hub by the command:

`/opt/cactus/bin/controlhub_start`

This uses TCP protocol instead of UDP which accounts for packet loss but decreases the performance.


### Support, Suggestions ?

For any support/suggestions, mail to fabio.raveraSPAMNOT@cern.ch, mauro.dinardoSPAMNOT@cern.ch
