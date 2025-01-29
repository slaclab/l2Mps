################
### Lib name ###
################
LIB_NAME = l2Mps

#####################
### Arch Versions ###
#####################
# Host version
#HARCH      = rhel6
# Buildroot versions
BR_ARCHES += buildroot-2019.08

########################
### Package versions ###
########################
CPSW_VERSION     = R4.5.2
BOOST_VERSION    = 1.64.0
YAML_CPP_VERSION = yaml-cpp-0.5.3_boost-1.64.0
TIRPC_VERSION    = 1.3.5

########################
### Path definitions ###
########################
# Location of BuildRoot home:
BUILDROOT_HOME=$(PACKAGE_SITE_TOP)/linuxRT/$(BR_VERSION)
# Location CrossCompiler HOME:
XCROSS_HOME=$(BUILDROOT_HOME)/host/linux-x86_64/x86_64/usr/bin/x86_64-buildroot-linux-gnu-

ifeq ($(PACKAGE_SITE_TOP),)
$(error PACKAGE_SITE_TOP is not defined! Please set it in the environment or otherwise)
endif

# Package top
PACKAGE_TOP      = $(PACKAGE_SITE_TOP)
# Packages location
CPSW_DIR         = $(PACKAGE_TOP)/cpsw/framework/$(CPSW_VERSION)/$(TARCH)
BOOST_DIR        = $(PACKAGE_TOP)/boost/$(BOOST_VERSION)/$(TARCH)
YAML_CPP_DIR     = $(PACKAGE_TOP)/yaml-cpp/$(YAML_CPP_VERSION)/$(TARCH)
TIRPC_DIR        = $(PACKAGE_TOP)/tirpc/$(TIRPC_VERSION)/$(TARCH)

