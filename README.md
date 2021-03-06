SSCore
======

**SSCore** is Southern Stars' astronomical core code and data library. It contains routines for astronomical calculations like time conversion, coordinate transformation, orbital mechanics, lunar and planetary position computation. It also contains functions for importing, storing, and combining data from a variety of astronomical catalogs.

SSCore is written in highly portable, modern C++.  It has been tested on MacOS, iOS, Android, Linux, and Windows.  It uses common STL types and language features (string, vector, map; operator overloading) and stack-based memory management wherever possible to produce compact, highly efficient, optimized code.

This repository also includes simple test programs which serve as examples of how to use the API. Build environments for MacOS, iOS, Android, Windows, and Linux are included. The iOS version includes a plain C wrapper for the C++ classes which makes them useable from Swift. The Android version contains Java wrappers and JNI code which allows the C++ classes to be called from either a Java- or Kotlin-based Android environment.

License
-------

SSCore is Copyright © 2020 Southern Stars Group, LLC. All rights reserved.  Southern Stars grants you a license to use the SSCore code for any personal, private, research, or other **non-commercial** purpose.

If you wish to include any part of the SSCore code in a commercial product, please contact Southern Stars Group, LLC (timd@southernstars.com) for a commercial license.

SSCode
------

This directory contains the source code.  Here's an overview of the C++ classes inside the source files:

- **_SSAngle:_** Classes for converting angular values from radians to degress/hours, minutes, seconds; and vice-versa.
- **_SSConstellation:_** This subclass of SSObject stores information for constellations and asterisms, including the official IAU constellation names, abbreviations, boundaries; and shape/figure data.
- **_SSCoords:_** This class converts rectangular and spherical coordinates between different astronomical reference frames (fundamental/ICRS, equatorial, ecliptic, galactic, local horizon) at a particular time and geographic location.
- **_SSIdentifier:_** This class represents object identifiers used in a wide variety of astronomical catalogs with a unified system of 64-bit integers, and contains methods for converting identifiers from string representations ("HR 7001", "NGC 1976", etc.) to 64-bit integers and vice-versa.
- **_SSJPLDEphemeris:_** This class reads JPL's binary DE43x series of ephemeris files and computes very fast, accurate lunar and planetary positions from them.
- **_SSDynamics:_** Implements solar system and stellar dynamics. Computes position, motion, and visual magnitude of all types of celestial objects, as seen by an observer from a particular time and location, in a unified fashion.
- **_SSMatrix:_** Represents a 3x3 matrix, with routines for performing matrix and vector-matrix arithmetic.
- **_SSObject:_** Base class for all types of celestial objects (stars, planets, constellations, etc.)
- **_SSOrbit:_** This class stores Keplerian orbital elements, computes position/velocity at a given time from them, and vice-versa.
- **_SSPlanet:_** This subclass of SSObject represents all solar system objects (not just planets, but also moons, asteroids, comets, satellites, etc.)
- **_SSStar:_** This subclass of SSObject represents all objects outside the solar system, including stars, star clusters, nebulae, and galaxies. SSStar has special subclasses for double and variable stars, and for deep sky objects.
- **_SSTime:_** Classes for converting between Julian Dates and calendar dates/times; and between civil (UTC) and dynamic time (TDT).
- **_SSTLE:_** Routines for reading satellite orbital elements from TLE (Two/Three-Line Element) files, and computing satellite position/velocity from them using the SGP, SGP4, and SDP4 orbit models; and vice-versa.
- **_SSUtilities:_** A few useful string manipulation, angle conversion, and other utility functions that are not present in standard C++11.
- **_SSVector:_** Classes for converting points between spherical and rectangular coordinates, and for performing vector arithmetic operations.

**Coding Style Standards**

I comment copiously, especially function/method parameters and struct/class members. I use lots of whitespace, avoid TABs, and indent using four whitespaces. My braces use ANSI/ISO/BSD style, **_not_** K&R style. I start private member variable names with underscores. Raw pointer variable names start with p. Constants start with k.

Other general rules: functions should only have one return. Never use gotos. Split up functions longer than a page. Use polymorphism and avoid downcasting. Every rule has exceptions.

SSData
------

**SSData** is a collection of data on well-known astronomical objects, compiled and carefully vetted from a variety of modern astronomical catalogs and data sources.  It includes:

- **_Solar System Objects:_** the major planets (including Pluto!) and their natural satellites, with orbital and physical characteristics from [JPL Solar System Dynamics.](https://ssd.jpl.nasa.gov). Also current asteroid and comet data from the [Minor Planet Center](https://www.minorplanetcenter.net/iau/mpc.html), a selection of satellite TLE files from [CelesTrak](http://www.celestrak.com) and [N2YO](https://www.n2yo.com), and JPL's latest planetary and lunar [ephemeris](https://ssd.jpl.nasa.gov/?planet_eph_export).
- **_Stars:_** the brightest and nearest stars, compiled from NASA's [SKY2000 Master Star Catalog](https://ui.adsabs.harvard.edu/abs/2015yCat.5145....0M/abstract), Hipparcos, [RECONS](http://www.recons.org/), and other sources; with official IAU [star names](http://www.pas.rochester.edu/~emamajek/WGSN/).
- **_Deep Sky Objects:_** the Messier and Caldwell objects, with data from Wolfgang Steinicke's [Revised NGC and IC](http://www.klima-luft.de/steinicke/index_e.htm) catalogs, and enhanced using data from a few other sources.
- **_Constellations:_** the 88 IAU-sanctioned constellations, including official IAU [boundary](https://watcheroftheskies.net/constellations/boundaries.html) and [shape]( http://mkweb.bcgsc.ca/constellations/) data.

These files are mostly CSV-formatted text.  They can easily be imported into a spreadsheet or edited manually.  SSCore contains routines for reading and writing them as well.

SSTest
------

This directory contains a test program (SSTest.cpp), which hopefully serves as an example of how to use the C++ classes in the SSCode directory.  It also contains build environments for MacOS, iOS, Android, Windows, and Linux.  Building and running instructions for each platform follow.

- **_Android:_** open the **SSTest/Android** project with Android Studio 3.6 or later. Wait a minute for Gradle sync to complete. Then from the **Build** menu, select **Make Project.** After build completes, go to the **Run** menu and select **Debug 'sstest'**.
- **_MacOS:_** open **SSTest.xcodeproj** in the MacOS directory with Xcode 10 or later. From Xcode's **Product** menu, select **Run**. NOTE: by default, Xcode places the SSTest executable under `~/Library/Developer/Xcode/DerivedData`, so SSTest won't know where to find the `SSData` folder.  To fix this, from Xcode's **File** menu, select **Project Settings...**, then change **Derived Data** to **Project-relative Location** (DerivedData). This puts the `DerivedData` folder, which contains the SSTest executable, into the `SSTest/MacOS` folder adjacent to `SSData`.
- **_iOS:_** open **SSTest.xcodeproj** in the iOS directory with Xcode 10 or later. From Xcode's **Product** menu, select **Run**.  This will launch a test program in the iPhone Simulator.  There is no GUI, just text output which shows how to call the SSCore C++ classes from Swift using a plain-C wrapper (see `ContentView.swift`)
- **_Linux:_** cd to the `Linux` directory; then type `make`.  After build completes, type `./sstest ../../SSData .` The final . tells the `sstest` executable to place file output into the current directory.
- **_Windows:_** open **SSTest.sln** in Visual Studio 2017 or later. From Visual Studio's **Build** menu, select **Build Solution**.  Then from the **Debug** menu, select **Start Debugging** (or **Start Without Debugging** if you have selected a Release configuration.)  The Visual Studio project supports both x86 and x64 builds.

Version History
---------------

Version 1.0, 12 Apr 2020: Initial public release.
