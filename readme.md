# StepToSky & FlightFactor Logging library

---

- The library is distributed under [BSD (3-Clause)](http://opensource.org/licenses/BSD-3-Clause) license.
  For more information read the [license](license.txt) file.
- The library uses [SemVer](http://semver.org/).
- The library requires C++ 11 or higher.
- Conan reference ```stsff-logging/X.Y.Z@stsff/stable```  
  Available in [steptosky bintray](https://bintray.com/steptosky/conan-open-source) remote without prebuilt packages.

---
``` cpp
using namespace stsff::logging;

BaseLogger logger("my-logger");
LMessage(logger) << "my message";

LVar(logVar, logger).message() << "declare variable logVar and set the level";
logVar << "can use the logVar now";

// see documentation for more information
```
---

#### warning 
- As the library hasn't got the major version yet
  the library's interface and logic are not stable and can be changed at any time.


#### dependencies
- [Cmake](https://cmake.org) - building tool.
- [Conan](https://www.conan.io) - package manager.
- [Conan Package Tools](https://github.com/conan-io/conan-package-tools) - if you want to build the project with those tools.
- [Python](https://www.python.org) - is needed for the Conan.
- [Doxygen](http://www.stack.nl/~dimitri/doxygen) - if you want to generate the documentation.
- [Gtest and Gmock 1.8](https://github.com/google/googletest) - testing (is used via the conan) . 

#### documentation
Run from the root folder ```doxygen doxyfile``` the result will be in the ```doc-generated``` folder.  
The ```doxygen``` has to be accessible through your ```PATH``` environment variable.

#### environment variables
| Tools  | Variables | Type | Description |
|-------:|----------:|:----:|:------------|
| conan  | **CONAN_TESTING_REPORT_DIR** | _string_ | A path for storing tests results. Default value is specified in the cmake script. |
| conan  |      **CONAN_BUILD_TESTING** |  _0/1_   | Enables/disables building and running the tests.  If you set ```BUILD_TESTING=ON``` as a parameter while running ```cmake``` command it will auto-set ```CONAN_BUILD_TESTING=1```.  |
| cmake  |       **TESTING_REPORT_DIR** | _string_ | You can specify the directory for the tests reports, it can be useful for CI. Default value is specified in the cmake script. |
| cmake  |            **BUILD_TESTING** | _ON/OFF_ | Enables/disables building test projects. This is standard cmake variable. |

**Note:** sometimes you will need to delete the file ```cmake/conan.cmake``` then the newer version of this file will be downloaded from the Internet while running ```cmake``` command.  
This file is responsible for cmake and conan interaction.


## Building

- You have to install `conan` and [add](https://docs.conan.io/en/latest/reference/commands/misc/remote.html):  
    - If it isn't available [conan-center](https://bintray.com/conan/conan-center) remote the link can be `https://conan.bintray.com`.
    - [bincrafters bintray](https://bintray.com/bincrafters/public-conan) remote.

You may use some ways to build this library.
1. **Using cmake as usually.**   
    _These scripts are just examples probably you will need to adjust them for your purposes._  
    This approach will do:  
    - build the library or just generate project for your IDE.
    - test the library if you specify necessary environment variables.
    - install it into specified directory.
  
Usually this approach is for developing.  

**Examples:**  
Windows (.bat) For Visual Studio 2017.
``` batch
::==========================================================
@echo off
:: Fix problems with some symbols.
REM change CHCP to UTF-8
CHCP 65001
CLS
::==========================================================
:: Sometimes conan asks you about to login to private repositories
:: if you have added ones. So you can put into the root repository folder
:: the file called 'set-conan-user.bat' and write there something like this:
::      call conan user -p <password> -r <remote alias>  <user name>
if exist set-conan-user.bat call set-conan-user.bat
::==========================================================
:: Creating building DIR and use it as working one.
set dir="msvc-2019"
if not exist %dir% mkdir %dir%
cd %dir%
::==========================================================
:: Generating Visual Studio project.
:: If you want to select specific tool-set use -T.
:: Example VS 2015: call cmake -G "Visual Studio 15 Win64" -T v140 ...
call cmake -G "Visual Studio 16" ../ ^
    -DBUILD_SHARED_LIBS=OFF ^
    -DCMAKE_INSTALL_PREFIX=../output ^
    -DBUILD_TESTING=ON
::==========================================================
:: Building
:: Keep it commented if you want to generate VS project only
:: (without building), otherwise uncomment it.
::call cmake --build . --target install --config Release
::call cmake --build . --target install --config Debug
::==========================================================
cd ../
pause
::==========================================================
```


Unix (Linux/Mac (.sh)).
``` bash
#===========================================================
# Setting building dir name
dir_name="build-release"
#===========================================================
# Creating building DIR and use it as working one.
rm -r ${dir_name}
mkdir ${dir_name}
cd ${dir_name}
#===========================================================
# Generating make files
cmake -G"Unix Makefiles" ../ \
    -DCMAKE_BUILD_TYPE="Release" \
    -DCMAKE_INSTALL_PREFIX=../output \
    -DBUILD_TESTING=ON
#===========================================================
# Building
cmake --build . --target install
#===========================================================
cd ../
#===========================================================
```
---

2. **Using [conan create](https://docs.conan.io/en/latest/reference/commands/creator/create.html).**  
    _These scripts are just examples probably you will need to adjust them for your purposes._   
    This approach will do:  
    - export conan recipe.
    - build the library in conan local cache.
    - test the library if you specify necessary environment variables.
    - test conan package (`test_package`).
    - as the result you have the library in your local conan cache in the channel you have specified.

Usually this approach is used when you want to build and put the library into conan local cache with certain settings.  
You may also [upload](https://docs.conan.io/en/latest/reference/commands/creator/upload.html) the built library into external conan remote later.

**Examples:**  
Windows (.bat) For Visual Studio 2017 x64.

``` batch
call conan create . stsff/develop ^
     -s compiler="Visual Studio" ^
     -s compiler.version=15 ^
     -s compiler.runtime=MD ^
     -s build_type=Release ^
     -s arch=x86_64 ^
     -o stsff-logging:shared=False ^
     -e CONAN_TESTING_REPORT_DIR="report/conan-test" ^
     -e CONAN_BUILD_TESTING=1 ^
    --build=stsff-logging

pause
:: remove build dir in test package
rd /s/q "test_package/build"
```

Unix (Linux/Mac (.sh)). Fix the example for you use case.
``` bash
conan create . stsff/develop \
     -s compiler="apple-clang" \
     -s compiler.version=9.1 \
     -s build_type=Release \
     -s arch=x86_64 \
     -o stsff-logging:shared=False \
     -e CONAN_TESTING_REPORT_DIR="report/conan-test" \
     -e CONAN_BUILD_TESTING=1 \
    --build=stsff-logging
```
---


3. **You may also use just [conan export](https://docs.conan.io/en/latest/reference/commands/creator/export.html).**   
    This approach will do:  
    - export conan recipe to your local cache.
    - when project-consumer includes this library the necessary packaged will be auto-built.
    - as the result you have the library in your local conan cache in the channel you have specified.

Usually it may be used instead of previous issue when you don't want pre-build and test the library.

**Examples:**  
``` batch
conan . export stsff/develop
```
---

4. **Using [conan package tools](https://github.com/conan-io/conan-package-tools).**   
    _These scripts are just examples probably you will need to adjust them for your purposes._   
    This approach will do:  
    - create matrix with various configurations.
    - run conan create for each configuration.
    - as the result you have the library in your local conan cache in the channel you have specified with various configurations.
    - the builds may also be auto-uploaded into external conan remote.

Usually this approach is used in CI for building various configurations.  

**Examples:**  
Windows (.bat) building with the conan package tools.
``` batch
::==========================================================
:: Sometimes conan asks you about to login to private repositories
:: if you have added ones. So you can put into the root repository folder
:: the file called 'set-conan-user.bat' and write there something like this:
::      call conan user -p <password> -r <remote alias> <user name>
if exist set-conan-user.bat call set-conan-user.bat
::==========================================================
set CONAN_USERNAME=stsff
set CONAN_CHANNEL=testing
set CONAN_BUILD_TESTING=1
set CONAN_BUILD_POLICY=outdated
set CONAN_VISUAL_VERSIONS=15
set CONAN_ARCHS=x86_64
::==========================================================

call python build.py

:: uncomment next line if you want to remove all packages from all versions from specified channel
:: call conan remove stsff-logging/*stsff/testing -f

pause

:: remove build dir in test package
rd /s/q "test_package/build"
```

Unix (Linux/Mac (.sh)). Fix the example for you use case.
``` bash
CONAN_USERNAME=stsff
CONAN_CHANNEL=testing
CONAN_BUILD_TESTING=1
CONAN_BUILD_POLICY=outdated
CONAN_APPLE_CLANG_VERSIONS=9.1
CONAN_ARCHS=x86_64

python build.py

# uncomment next line if you want to remove all packages from all versions from specified channel
# conan remove stsff-logging/*stsff/testing -f
```
---

## Memo for the library developers
- [release-checklist](release-checklist.md) see this file when you are making the release.
- [lasote docker hub](https://hub.docker.com/u/lasote/).


## Copyright
Copyright � 2019, StepToSky and FlightFactor teams. All rights reserved.  
[www.steptosky.com](http://www.steptosky.com/) & [www.flightfactor.aero](http://www.flightfactor.aero)
