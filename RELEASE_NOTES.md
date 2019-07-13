# Release notes

Release notes for the SLAC LCLS2 HPS MPS Driver.

## Releases:
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
