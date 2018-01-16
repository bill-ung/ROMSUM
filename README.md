# ROMSUM

[ROMSUM](http://roms.ufopinball.com/) is a utility to calculate the checksum of a pinball EPROM/ROM file.

This project was started back in the mid 90's as a tool produced for the pinball community.  Solid-state pinball machines used EPROM (erasable programmable read-only
memory) chips stored the program code for each game.  To help ensure proper function and reliability, the CPU would run a basic checksum on each EPROM chip at boot time.

As personal computer usage grew, avid pinball hobbyists would download the EPROM code from existing chips, and save them as files on their PC.  Some would also "burn" newer code to upgrade existing games, adding new features, bug fixes, or enhanced pricing options.  Many hobbyists archived older versions of the game code, to compare features, or possibly to downgrade if a serious bug was discovered.

To assist in verifying collections of EPROM images, the ROMSUM tool was freely offered to pinball hobbyists.  ROMSUM allows users to calculate the checksum for any given file.  ROMSUM will also report the file size and matching EPROM chip size.  Wildcards are supported to run ROMSUM on batches of ROM images.

For some games (Williams System 11{A,B,C}), the checksum was actually calculated every 0x1000 bytes.  For these cases, ROMSUM includes a command-line switch to operate in "verbose" mode and report checksum values for each 0x1000 bytes, in addition to the full checksum value.

EPROM chips do sometimes degrade over time, so performing a checksum is an important step.  Note that the checksum value is not a CRC (Cyclic-Redundancy-Check).  Pinball machines often fought with limited resources, with lower horsepower CPUs and limited EPROM space.  For these reasons, it seems that using a simple checksum was preferable to performing a CRC.

## ROMSUM Version History

- [ROMSUM Version 2.5](http://roms.ufopinball.com/romsum.html) (1996/08/01)

 - This is the final version of the original ROMSUM.exe command-line utility.  It remains available in binary form only, and will run on older versions of MS-DOS/Windows, including newer 32-bit versions of Windows.  Limited to 8.3 style filenames.


- ROMSUM Version 3.0 (2003/09/25)

 - Upgrade the ROMSUM project to use a 32-bit compiler, which adds support for long filenames.  This version requires a 32-bit version of Windows as the minimum-level OS.  At this point, the binary was rebranded as ROMSUM32.exe.


- [ROMSUM Version 3.1](http://roms.ufopinball.com/romsum.html) (2018/01/16)

 - Properly recognize the 32-byte 82S123 PROM used on late-70s/early-80s Bally pinball machines, as well as 2mb ATMEL AT49BV1614 Flash Memory images used on later model Stern Whitestar (modified) games.  I don't believe these are EPROMs exactly, but they are often included in the PinMAME game bundles.

 - Reworked the ROMSUM code slightly, in order to properly compile on Visual Studio 2017 Community Edition.  This way I can offer up an open source version of ROMSUM to the pinball community.
 
 - At present, the code does not compile on Linux using GCC.  I'm not sure how much demand there is for that, but it's on the "someday" list.  Send an email if you have interest.


- ROMSUM Version 4.0 (Dormant)

 - This was planned to be a GUI (Graphical User Interface) version where users could drag-and-drop a ROM file into the window to calculate the checksums.  This version never really got off the ground, but I'll leave this here as a placeholder.


- [ROMSUM Version 5.0](http://roms.ufopinball.com/) (2018/01/16)

 - A new "Pinball ROM Identifier" is an extension on the ROMSUM idea.  This is a web-based tool that allows a user to upload a ROM file that they want to verify, or in some cases may not be able to identify.  The web utility will detect the file size and calculate the checksum of the given file.  The size and checksum will then be compared against the database of known pinball EPROM images.  If a matching game is found, the full details are displayed on the screen.
 
 - Known issue: The web output does not presently handle the case where multiple database matches are found.

[ROMSUM](http://roms.ufopinball.com/) is written by Bill Ung <bill@ufopinball.com>  
Copyright (C) 1996-2018 Bill Ung and [UFO Pinball](http://www.ufopinball.com/)  
Registered trademarks remain the property of their respective owners
