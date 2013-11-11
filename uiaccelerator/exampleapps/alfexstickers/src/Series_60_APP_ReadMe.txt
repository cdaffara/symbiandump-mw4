//________________________________________________________________________
//             Series 60 APP Stationery Specific Information
//________________________________________________________________________ 

**Note - The Series 60 SDK only supports the following targets: WINSCW, 
Thumb UREL, ARMI UREL. If you import an ARM4 target, it will generate 
errors due to the missing directories.**

** Note - For Platform Security Apps (OS 9.x) you can use GCCE targets as well.
The package (.pkg) file generation defaults for a GCCE application for the source location
of the resources. If you are building with ARMV5 you will want to change this location
for successful sis file creation.

//------------------------------------------------------------------------
//	General Overview
//------------------------------------------------------------------------

This project stationery is designed to get you up and running quickly with 
CodeWarrior for Symbian OS. The project includes all of the necessary files
needed for building and debugging a generic Series 60 APP within the 
WINSCW emulator and target environments.

The project can support multiple targets that are dependant on your target 
environment, examples are:  WINSCW UDEB, WINSCW UREL, ARM4 UDEB, ARM4 UREL, 
ARMI UDEB, ARMI UREL, Thumb UDEB, Thumb UREL, and Build All.
The Debug & Release targets are almost identical, but are 
set to build at different compiler optimizations, Minimum(0) & Most(1) 
respectively.  The Build All target has been included for 
your convenience and simply builds all of the other targets.

//------------------------------------------------------------------------
//	Getting Started
//------------------------------------------------------------------------

Most of the target settings are already configured for you, but you
may need to configure the debugger settings if you are debugging on a device 
other than the emulator. Once you have a project created from this stationery,
open the Edit pull-down menu and select Debug Settings... (Alt+F7).
Select the Remote Debugging pref panel from the left column. 
If want to debug via the Symbian MetroTRK, select the Connection that's set up 
in the IDE Preferences for Remote Debugging via the Symbian MetroTRK.
Otherwise, select the Connection that you've set up for an alternate device.
We recommend that you make sure the project is working properly before you
begin adding your own code.  Select Make and then Debug from the Project 
pull-down menu.  If everything is working properly, the debugger window 
should come up and be stopped at either E32main() or the designated breakpoint.
If there is a problem, please consult the Targeting_Manual_Symbian_OS.pdf
for more information on debugging via the MetroTRK, or other devices.

**Note - Series 60 does not currently support on-device debugging via Metro-TRK on OS versions less than 9.x.

//------------------------------------------------------------------------
//	Adding your own code
//------------------------------------------------------------------------

Once everything is working as expected, you can begin adding your own code
to the project.  Keep in mind that we provide this as an example of how to
get up and running quickly with CodeWarrior.  Feel free to modify any of 
the source provided. 
