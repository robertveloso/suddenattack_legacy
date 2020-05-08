To build the exporter and importer plugins, you will need to copy over
the Max and/or Maya SDK files (these are not redistributable).  See the
readme files in the "sdkfiles" sub-folder for more information.

NOTE:

  Most MAX plug-ins have three build configurations:

    Release - standard release build
    Hybrid - standard debug build, links to release run-time library
    Debug - standard debug build, links to debug run-time library

  The debug build is only used by programmers who have access to debug
  builds of 3D Studio.  Hybrid is what most plug-in programmers use, as
  these builds work correctly with release builds of 3D Studio.

  LithTech MAX plug-ins only come with two configurations, release and
  debug.  In this case, debug is the same as a typical hybrid
  configuration.  If you want to run the plug-ins with a debug build of
  3D Studio, you MUST create a new configuration that links to the
  debug run-time libraries.
