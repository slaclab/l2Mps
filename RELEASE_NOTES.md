# Release notes

Release notes for the SLAC LCLS2 HPS MPS Driver.

## Releases:

  * Add new SALT diagnostic registers: `MpsTxPktPeriod`, `MpsRxPktPeriod`, `MpsTxPktPeriodMax`,
    `MpsRxPktPeriodMax`, `MpsTxPktPeriodMin`, `MpsRxPktPeriodMin`, `DiagnosticStrbCnt`,
    `MpsPllLockCnt`, `MpsTxEofeSentCnt`, and `MpsRxErrDetCnt`.

* __R3.5.0__: 2021-02-04 J. Vasquez
  * Add support for the new Digital Link Node. The new type is `MPS_DN`.
  * Update list of app types for MPS nodes, to match FW definitions.
    The new types are `MPS_LN` (for link nodes), and `MPS_AN` for application nodes.

* __R3.4.1__: 2020-05-13 J. Vasquez
  * Bug fix: Instantiate the LinkNode object only on Link Nodes.
    Link nodes are BLM or MPS application types, installed in slot 2.

* __R3.4.0__: 2020-05-01 J. Vasquez
  * Add support for the Link Node's software input.

* __R3.3.0__: 2020-02-07 J. Vasquez
  * update CPSW framework to version R4.4.1 and buildroot architecture to buildroot-2019.08

* __R3.2.0__: 2019-12-12 J. Vasquez
  * Update CPSW framework to version R4.3.1.

* __R3.1.0__: 2019-09-23 J. Vasquez
  * Update CPSW framework to version R4.2.0.

* __R3.0.0__: 2019-07-31 J. Vasquez
  * Scale factor is now formed by a slope and a offset value. setScaleFactor/getScaleFactor
    function were change for setScaleSlope/getScaleSlope and setScaleOffset/getScaleOffset.
  * Add support for BSI functions to read the crate ID and the slot number form the firmware
    application.
  * When creating a MPS node object, it will now automatically crate the application objects
    based on the application type define in the firmware application.
  * Add method to load a YAML configuration file.
  * Add raw value (value of the firmware register) to the threshold data.

* __R2.3.0__: 2019-10-29 J. Vasquez
  * Update CPSW framework to version R4.3.1.

* __R2.2.0__: 2019-09-23 J. Vasquez
  * Update CPSW framework to version R4.2.0.

* __R2.1.2__: 2019-08-13 J.Vasquez
  * Bug fix: argument name was shadowing the protected member name. This was causing the polling thread
    to consume a lot of CPU.

* __R2.1.1__: 2019-07-12 J. Vasquez
  * Update CPSW framework to version R4.1.2.

* __R2.1.0__: 2019-06-27 J. Vasquez
  * Update CPSW framework to version R4.1.0.

* __R2.0.0__: 2019-02-26 J. Vasquez
  * Update CPSW framework to version R3.6.6, boost to 1.64.0 and yaml-cpp to yaml-cpp-0.5.3_boost-1.64.0.
  * Divide the scale factor use on thresholds into slope and offset.

* __R1.2.0__: 2018-10-26 J. Vasquez
  * Update CPSW framework to R3.6.4

* __R1.1.0__: 2018-06-28 J. Vasquez
  * Add new CPSW wrapper class which catches errors from CPSW and set valid flags which is
    forward to the user via the API call using pairs of status and value.
  * Code improvements and minor bug fixes.

* __R1.0.2__: 2018-06-18 J. Vasquez
  * Use std::thread for threads.
  * Use std::function instead of pointers for callback functions.
  * Use boost::make_shared factory function to create shared_ptr.
  * Fix order of libraries on makefile, and link the whole pthread lib into the static binaries.
  * Minor code improvements.

* __R1.0.1__: 2018-04-09 J. Vasquez
  * Make the base class destructor virtual.
  * Remove debug message from destructors.
  * Remove unused headers.

* __R1.0.0__: 2018-02-27 J. Vasquez
  * First stable release.
