# Release notes

Release notes for the SLAC LCLS2 HPS MPS Driver.

## Releases:
* __R1.0.3__: 2018-06-28 J. Vasquez
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
