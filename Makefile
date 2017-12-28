#
# OMNeT++/OMNEST Makefile for flora
#
# This file was generated with the command:
#  opp_makemake -f --deep -Isrc/LoRa -Isrc/LoRaApp -Isrc/LoRaEnergyModules -Isrc/LoRaPhy -Isrc/misc -I../inet/src/inet/physicallayer/analogmodel/packetlevel -I../inet/src -I../inet/src/inet/physicallayer/contract/packetlevel -I../inet/src/inet/physicallayer/pathloss -I.. -I../inet/src/inet/linklayer/base -I../inet/src/inet/physicallayer/base/packetlevel -I../inet/src/inet/physicallayer/common/packetlevel -I../inet/src/inet/power/storage -I../inet/src/inet/physicallayer/energyconsumer -KINET_PROJ=../inet -DINET_IMPORT -I. -I$(INET_PROJ)/src -L$(INET_PROJ)/src -lINET$(D)
#

# Name of target to be created (-o option)
TARGET = flora$(D)$(EXE_SUFFIX)
TARGET_DIR = .

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(QTENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)
#USERIF_LIBS = $(QTENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -Isrc/LoRa \
    -Isrc/LoRaApp \
    -Isrc/LoRaEnergyModules \
    -Isrc/LoRaPhy \
    -Isrc/misc \
    -I../inet/src/inet/physicallayer/analogmodel/packetlevel \
    -I../inet/src \
    -I../inet/src/inet/physicallayer/contract/packetlevel \
    -I../inet/src/inet/physicallayer/pathloss \
    -I.. \
    -I../inet/src/inet/linklayer/base \
    -I../inet/src/inet/physicallayer/base/packetlevel \
    -I../inet/src/inet/physicallayer/common/packetlevel \
    -I../inet/src/inet/power/storage \
    -I../inet/src/inet/physicallayer/energyconsumer \
    -I. \
    -I$(INET_PROJ)/src

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = $(LDFLAG_LIBPATH)$(INET_PROJ)/src  -lINET$(D)

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/src/LoRa/LoRaGWMac.o \
    $O/src/LoRa/LoRaGWRadio.o \
    $O/src/LoRa/LoRaMac.o \
    $O/src/LoRa/LoRaRadio.o \
    $O/src/LoRa/NetworkServerApp.o \
    $O/src/LoRa/PacketForwarder.o \
    $O/src/LoRaApp/SimpleLoRaApp.o \
    $O/src/LoRaEnergyModules/LoRaEnergyConsumer.o \
    $O/src/LoRaPhy/LoRaAnalogModel.o \
    $O/src/LoRaPhy/LoRaBandListening.o \
    $O/src/LoRaPhy/LoRaHataOkumura.o \
    $O/src/LoRaPhy/LoRaLogNormalShadowing.o \
    $O/src/LoRaPhy/LoRaMedium.o \
    $O/src/LoRaPhy/LoRaModulation.o \
    $O/src/LoRaPhy/LoRaPathLossOulu.o \
    $O/src/LoRaPhy/LoRaReceiver.o \
    $O/src/LoRaPhy/LoRaReception.o \
    $O/src/LoRaPhy/LoRaTransmission.o \
    $O/src/LoRaPhy/LoRaTransmitter.o \
    $O/src/misc/DevAddr.o \
    $O/src/LoRa/LoRaMacControlInfo_m.o \
    $O/src/LoRa/LoRaMacFrame_m.o \
    $O/src/LoRaApp/LoRaAppPacket_m.o \
    $O/src/LoRaPhy/LoRaRadioControlInfo_m.o

# Message files
MSGFILES = \
    src/LoRa/LoRaMacControlInfo.msg \
    src/LoRa/LoRaMacFrame.msg \
    src/LoRaApp/LoRaAppPacket.msg \
    src/LoRaPhy/LoRaRadioControlInfo.msg

# SM files
SMFILES =

# Other makefile variables (-K)
INET_PROJ=../inet

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIBS = $(OPPMAIN_LIB) $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)
ifneq ($(TOOLCHAIN_NAME),clangc2)
LIBS += -Wl,-rpath,$(abspath $(INET_PROJ)/src)
endif

COPTS = $(CFLAGS) $(IMPORT_DEFINES) -DINET_IMPORT $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $(TARGET_DIR)/$(TARGET)

$(TARGET_DIR)/% :: $O/%
	@mkdir -p $(TARGET_DIR)
	$(Q)$(LN) $< $@
ifeq ($(TOOLCHAIN_NAME),clangc2)
	$(Q)-$(LN) $(<:%.dll=%.lib) $(@:%.dll=%.lib)
endif

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile $(CONFIGFILE)
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET) $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders smheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE) | msgheaders smheaders
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc $(MSGCOPTS) $?

%_sm.cc %_sm.h: %.sm
	$(qecho) SMC: $<
	$(Q)$(SMC) -c++ -suffix cc $(SMCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

smheaders: $(SMFILES:.sm=_sm.h)

clean:
	$(qecho) Cleaning $(TARGET)
	$(Q)-rm -rf $O
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET)
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET:%.dll=%.lib)
	$(Q)-rm -f $(call opp_rwildcard, . , *_m.cc *_m.h *_sm.cc *_sm.h)

cleanall:
	$(Q)$(MAKE) -s clean MODE=release
	$(Q)$(MAKE) -s clean MODE=debug
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

# include all dependencies
-include $(OBJS:%.o=%.d)
