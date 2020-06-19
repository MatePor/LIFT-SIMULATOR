Techniki Programowania 2020 - Projekt nr 4
ACiR 1. 180149 180030
=== Symulator windy ===

INSTRUKCJA:
- |START| -  odświeża obraz, inicjalizuje rysunki
- |CALL| - przywołanie windy na piętro wskazywane przez "floor to go"
- |SPAWN PERSON| - tworzenie ludzika na piętrze "floor to spawn", który będzie chciał pojechać na piętro "floor to go" 
- |-1| i |+1| - zmiana piętra na którym ma pojawić się ludzik
- |5|,|4|,|3|,|2|,|1|,|0| - przyciski do wyboru piętra docelowego dla ludzika lub ręcznego przyzwania przyciskiem |CALL|

========================================================================
    WIN32 APPLICATION : draw2 Project Overview
========================================================================

AppWizard has created this draw application for you.

This file contains a summary of what you will find in each of the files that
make up your draw application.


draw.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

draw.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

draw2.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
AppWizard has created the following resources:

draw2.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
    Visual C++.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

draw2.ico
    This is an icon file, which is used as the application's icon (32x32).
    This icon is included by the main resource file draw.rc.

small.ico
    This is an icon file, which contains a smaller version (16x16)
    of the application's icon. This icon is included by the main resource
    file draw.rc.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named draw.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
