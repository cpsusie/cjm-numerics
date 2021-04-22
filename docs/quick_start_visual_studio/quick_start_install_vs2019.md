# CJM Numerics Visual Studio Quick Start Installation Guide
Copyright © 2020-2021, CJM Screws, LLC

This guide provides a walkthrough on how to install this CJM Numerics UInt128 Library for usage with Visual Studio 2019.  It may also apply, with a few variations, to Visual Studio Code.  Please note that this project requires using a C++ compiler that supports C++20.  For Microsoft's compiler, the minimum version is 16.9.3.  

There are two different avenues for installing this header-only library:  
1. Using vcpkg (recommended).
2. Copying the library files to a system include folder used by your environment.

## Installation Using Vcpkg

Vcpkg is a permissively licensed open-source package management system published by Microsoft.  It works on Windows, Mac and Linux and provides some limited support for other platforms.  You can get vcpkg from Microsoft's repository [here][1].  If you use Microsoft's repository, after following Microsoft's instructions and installing vcpkg and integrating it with Visual Studio (or Visual Studio Code), you must download the vcpkg port folder from [the latest cjm-numerics-uint128 release.][2]  You only need the 7-zip file.  Vcpkg will download the needed files from the repo for you.  

![Downloading 7-zip file from Releases.](images/downloading_port/download_from_releases.PNG)

Alternatively you can download vcpkg from my fork of their repository [here][3].  This fork is identical to Microsoft's except it comes with the port for cjm-numerics-uint128.  Once I publish a stable release of this library, I will apply to have this library included in Microsoft's repository.  
  
### Copying Port Folder  

Assuming, you downloaded vcpkg from Microsoft's repository and have downloaded the port for this library, copy it to vcpkg's ports folder as shown:  
  
![Copy over entire folder to Ports](images/downloading_port/copy_folder_to_vcpkg_port_folder.PNG)



### Install the Package  

Before following these steps, make sure you have copied over the port folder as shown above and [integrated vcpkg with Visual Studio.][4]  Navigate to your root vcpkg folder in powershell or a DOS prompt then, to install cjm-numerics-uint128 for x64 windows enter: `./vcpkg install cjm-numerics-uint128:x64-windows`. You should see something like the following as a result:  

> PS D:\Users\csusie\source\repos\vcpkg> .\vcpkg.exe install cjm-numerics-uint128:x64-windows  
Computing installation plan...  
The following packages will be built and installed:  
    cjm-numerics-uint128[core]:x64-windows -> 0.0.0.6-alpha  
Detecting compiler hash for triplet x64-windows...  
Could not locate cached archive:   D:\Users\csusie\AppData\Local\vcpkg\archives\b6\b609669426fc2b682e067bf2842f46761a28d26f.zip  
Starting package 1/1: cjm-numerics-uint128:x64-windows  
Building package cjm-numerics-uint128[core]:x64-windows...  
-- Using cached D:/Users/csusie/source/repos/vcpkg/downloads/cpsusie-cjm-numerics-v0.0.0.6-alpha.tar.gz  
-- Cleaning sources at D:/Users/csusie/source/repos/vcpkg/buildtrees/cjm-numerics-uint128/src/.0.6-alpha-e994768164.clean. Use --editable to skip cleaning for the packages you specify.  
-- Extracting source D:/Users/csusie/source/repos/vcpkg/downloads/cpsusie-cjm-numerics-v0.0.0.6-alpha.tar.gz  
-- Using source at D:/Users/csusie/source/repos/vcpkg/buildtrees/cjm-numerics-uint128/src/.0.6-alpha-e994768164.clean  
-- Found external ninja('1.10.2').  
-- Configuring x64-windows  
-- Up-to-date: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include/  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/numerics  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/numerics/cjm_configuration.hpp  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/numerics/cjm_numeric_concepts.hpp  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/numerics/fixed_uint_container.hpp  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/numerics/fixed_uint_container_math.hpp  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/numerics/numerics.hpp  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/numerics/numerics.inl  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/numerics/numerics_configuration.hpp  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/numerics/numerics_configuration.inl  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/numerics/uint128.hpp  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/numerics/uint128.inl  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/string  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/string/cjm_string.hpp  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/string/cjm_string.inl  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/include//cjm/string/istream_utils.hpp  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/share/cjm-numerics-uint128/copyright  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/share/cjm-numerics-uint128/NOTICES.txt  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/share/cjm-numerics-uint128/NOTICES.md  
-- Installing: D:/Users/csusie/source/repos/vcpkg/packages/cjm-numerics-uint128_x64-windows/share/cjm-numerics-uint128/README.md  
-- Performing post-build validation  
-- Performing post-build validation done  
Stored binary cache: D:\Users\csusie\AppData\Local\vcpkg\archives\b6\b609669426fc2b682e067bf2842f46761a28d26f.zip  
Building package cjm-numerics-uint128[core]:x64-windows... done  
Installing package cjm-numerics-uint128[core]:x64-windows...  
Installing package cjm-numerics-uint128[core]:x64-windows... done  
Elapsed time for package cjm-numerics-uint128:x64-windows: 8.47 s  
Total elapsed time: 13.67 s  
The package cjm-numerics-uint128:x64-windows is header only and can be used from CMake via:  
    find_path(CJM_NUMERICS_UINT128_INCLUDE_DIRS "cjm/numerics/cjm_configuration.hpp")
    target_include_directories(main PRIVATE ${CJM_NUMERICS_UINT128_INCLUDE_DIRS})  
    
Repeat the command above but this time specify x86-windows: `.\vcpkg.exe install cjm-numerics-uint128:x86-windows`.  The results should be similar to the above.  
  


  [1]: https://github.com/microsoft/vcpkg
  [2]: https://github.com/cpsusie/cjm-numerics/releases/download/v0.0.0.6-alpha/vcpkg_port_folder_cjm-numerics-uint128.7z
  [3]: https://github.com/cpsusie/vcpkg.git
  [4]: https://vcpkg.readthedocs.io/en/latest/users/integration/
