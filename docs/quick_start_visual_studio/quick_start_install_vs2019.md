# CJM Numerics Visual Studio Quick Start Installation Guide
Copyright © 2020-2021, CJM Screws, LLC

This guide provides a walkthrough on how to install this CJM Numerics UInt128 Library for usage with Visual Studio 2019.  It may also apply, with a few variations, to Visual Studio Code.  Please note that this project requires using a C++ compiler that supports C++20.  For Microsoft's compiler, the minimum version is 16.9.3.  

There are two different avenues for installing this header-only library:  
1. Using vcpkg (recommended).
2. Copying the library files to a system include folder used by your environment.

## Installation Using Vcpkg

Vcpkg is a permissively licensed open-source package management system published by Microsoft.  It works on Windows, Mac and Linux and provides some limited support for other platforms.  You can get vcpkg from Microsoft's repository [here][1].  If you use Microsoft's repository, after following Microsoft's instructions and installing vcpkg and integrating it with Visual Studio (or Visual Studio Code), you must download the vcpkg port folder from [the latest cjm-numerics-uint128 release.][2]  

Alternatively you can download vcpkg from my fork of their repository [here][3].  This fork is identical to Microsoft's except it comes with the port for cjm-numerics-uint128.  Once I publish a stable release of this library, I will apply to have this library included in Microsoft's repository.  
  
### Copying Port Folder  

Assuming, you downloaded vcpkg from Microsoft's repository and have downloaded the port for this library, copy it to vcpkg's ports folder as shown:  


  [1]: https://github.com/microsoft/vcpkg
  [2]: https://github.com/cpsusie/cjm-numerics/releases/download/v0.0.0.6-alpha/vcpkg_port_folder_cjm-numerics-uint128.7z
  [3]: https://github.com/cpsusie/cjm-numerics.git
