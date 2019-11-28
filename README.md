# Calculus Prime P&C

This project provides a rating engine for calculating insurance tariffs.

## Contents

- [Getting started](#getting-started)
- [External dependencies](#external-dependencies)
- [Building instructions](#building-instructions)
- [Licensing](#license)

## <a name="getting-started"></a>Getting started

The basic directory layout is as follows:

* antlr4 - ANTLR4 grammar and command file to generate C++ files
* generated - The generated ANTLR4 C++ files will go here
* include - Directory with C++ header files. Add this to the include paths of the project that uses Calculus Prime P&C
* lib - Output directory for Windows builds
* src - Directory with C++ source files
* test - Directory with test cases


## <a name="external-dependencies"></a>External dependencies

The following third party software is required to build Calculus Prime P&C:

* ANTLR: https://www.antlr.org version 4.7
* boost: https://www.boost.org version 1.70 or later
* log4cplus (optional): https://sourceforge.net/p/log4cplus/wiki/Home version 1.2 

## <a name="building-instructions"></a>Building instructions

Calculus Prime P&C is built as a static library which must be linked together with the build artifact that uses this library.
Before the Calculus Prime P&C library can be built the C++ files for the ANTLR grammer have to be generated. 
This can be done by calling the script antlr/generate.cmd with the path to the ANTLR4 jar file
as parameter, e.g. generate.cmd antlr-4.7-complete.jar. The java executable is required to run the ANTLR generator and should be found in the PATH environment variable.


### Building on Windows

A Visual Studio 2017 solution file is provided (calculusprime.sln in the root folder). 
Several property sheets are provided where include paths and settings for third party libraries must be configured (antlr.props, boost.props, log4cplus.props in the root folder).
In the property sheet calculusprime.props additional settings for the Calculus Prime P&C project that shall be shared among all build configurations can be configured.
If the log4cplus library shall not be used the macro CALCULUSPRIME_NO_LOG4CPLUS has to be defined in this property sheet and in the project that uses the Calculus Prime P&C library.

The static libraries that are created by the Visual Studio build are placed in the lib/ directory in the root folder. 
The appropriate library (Debug/Release, x86/x64) has to be linked together with the build artifact that uses Calculus Prime P&C. 


### Building on Linux

An Eclipse 3.8 CDT project file is provided int the root folder. 
The include and linker paths for the third party libraries antlr, boost and log4cplus have to be configured in the project settings. 
If the log4cplus library shall not be used the macro CALCULUSPRIME_NO_LOG4CPLUS has to be defined in this project and in the project that uses the Calculus Prime P&C library.

While building a directory that has the same name as the build configuration (Debug or Release) is created in the root folder and contains the static library that is built. 
This library has to has to be linked together with the build artifact that uses Calculus Prime P&C. 

## <a name="license"></a>License

This project is under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.