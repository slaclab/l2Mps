################
### Lib name ###
################
LIB_NAME = l2Mps

#####################
### Arch Versions ###
#####################
# Host version
HARCH      = rhel6
# Buildroot versions
BR_ARCHES += buildroot-2016.11.1

########################
### Package versions ###
########################
CPSW_VERSION     = R4.1.0
BOOST_VERSION    = 1.64.0
YAML_CPP_VERSION = yaml-cpp-0.5.3_boost-1.64.0

########################
### Path definitions ###
########################
# Location of BuildRoot home:
BUILDROOT_HOME=/afs/slac/package/linuxRT/$(BR_VERSION)
# Location CrossCompiler HOME:
XCROSS_HOME=$(BUILDROOT_HOME)/host/linux-x86_64/x86_64/usr/bin/x86_64-buildroot-linux-gnu-

# Package top
PACKAGE_TOP      = /afs/slac/g/lcls/package
# Packages location
CPSW_DIR         = $(PACKAGE_TOP)/cpsw/framework/$(CPSW_VERSION)/$(TARCH)
BOOST_DIR        = $(PACKAGE_TOP)/boost/$(BOOST_VERSION)/$(TARCH)
YAML_CPP_DIR     = $(PACKAGE_TOP)/yaml-cpp/$(YAML_CPP_VERSION)/$(TARCH)


