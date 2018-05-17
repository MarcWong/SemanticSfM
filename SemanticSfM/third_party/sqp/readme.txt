MATLAB Compiler SDK

1. Prerequisites for Deployment 

. Verify the MATLAB Runtime is installed and ensure you    
  have installed version 9.1 (R2016b).   

. If the MATLAB Runtime is not installed, do the following:
  (1) enter
  
      >>mcrinstaller
      
      at MATLAB prompt. The MCRINSTALLER command displays the 
      location of the MATLAB Runtime installer.

  (2) run the MATLAB Runtime installer.

Or download the Linux 64-bit version of the MATLAB Runtime for R2016b 
from the MathWorks Web site by navigating to

   http://www.mathworks.com/products/compiler/mcr/index.html
   
   
For more information about the MATLAB Runtime and the MATLAB Runtime installer, see 
Package and Distribute in the MATLAB Compiler SDK documentation  
in the MathWorks Documentation Center.    


2. Files to Deploy and Package

Files to package for Shared Libraries
=====================================
-libFmincon.so
-libFmincon.h
-MCRInstaller.zip 
   -if end users are unable to download the MATLAB Runtime using the above  
    link, include it when building your component by clicking 
    the "Runtime downloaded from web" link in the Deployment Tool
-This readme file

3. Definitions

For information on deployment terminology, go to 
http://www.mathworks.com/help. Select MATLAB Compiler >   
Getting Started > About Application Deployment > 
Deployment Product Terms in the MathWorks Documentation 
Center.


4. Appendix 

A. Linux x86-64 systems:
In the following directions, replace MCR_ROOT by the directory where the MATLAB Runtime 
   is installed on the target machine.

(1) Set the environment variable XAPPLRESDIR to this value:

    MCR_ROOT/v91/X11/app-defaults


(2) If the environment variable LD_LIBRARY_PATH is undefined, set it to the concatenation 
   of the following strings:

    MCR_ROOT/v91/runtime/glnxa64:
    MCR_ROOT/v91/bin/glnxa64:
    MCR_ROOT/v91/sys/os/glnxa64:
    MCR_ROOT/v91/sys/opengl/lib/glnxa64

    If it is defined, set it to the concatenation of these strings:

    ${LD_LIBRARY_PATH}: 
    MCR_ROOT/v91/runtime/glnxa64:
    MCR_ROOT/v91/bin/glnxa64:
    MCR_ROOT/v91/sys/os/glnxa64:
    MCR_ROOT/v91/sys/opengl/lib/glnxa64

   For more detail information about setting the MATLAB Runtime paths, see Package and 
   Distribute in the MATLAB Compiler SDK documentation in the MathWorks Documentation 
   Center.


     
        NOTE: To make these changes persistent after logout on Linux 
              or Mac machines, modify the .cshrc file to include this  
              setenv command.
        NOTE: The environment variable syntax utilizes forward 
              slashes (/), delimited by colons (:).  



