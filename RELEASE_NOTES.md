# Release notes

Release notes for the SLAC LCLS2 HPS MPS Driver.

## Releases:
* __R2.6.1__: 2022-09-22 J. Mock
  * Fix bug in parameter name for SALT diagnostic parameters introduced in previous tag

* __R2.6.0__: 2022-09-21 J. Mock
  * Add new SALT diagnostic registers: `MpsTxPktPeriod`, `MpsRxPktPeriod`, `MpsTxPktPeriodMax`,
    `MpsRxPktPeriodMax`, `MpsTxPktPeriodMin`, `MpsRxPktPeriodMin`, `DiagnosticStrbCnt`,
    `MpsPllLockCnt`, `MpsTxEofeSentCnt`, and `MpsRxErrDetCnt`.

* __R2.5.0__: 2022-05-03 J. Mock
  * Change how idle table is initialized in SW

* __R2.4.0__: 2020-01-28 J. Vasquez
  * Update CPSW framework to version R4.4.1, and buildroot architecture to buildroot-2019.08

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
