[![Stories in Ready](https://badge.waffle.io/KazDragon/paradice9.png?label=ready&title=Ready)](https://waffle.io/KazDragon/paradice9)
What is Paradice9
-----------------

Paradice9 is a terminal server that was developed to aid internet-based tabletop roleplaying sessions.  It has been actively used for approximately two years to support five Warhammer 40K Roleplaying (Dark Heresy/Rogue Trader/etc.) games.

On top of that, its software architecture can also stand as a platform for Telnet/ANSI based computing, for example as a MUD server.

Dependencies
------------
Paradice9 can be built for any platform supporting any modern C++14 compiler.
For example:
  * Ubuntu Linux, using g++-5.2 or above
  * Ubuntu Linux, using Clang 3.4 or above

This is tested automatically on the following platforms:
  * Ubuntu Linux, using Clang 3.4 on Travis-CI: [![Build Status](https://travis-ci.org/KazDragon/paradice9.svg?branch=master)](https://travis-ci.org/KazDragon/paradice9)

It requires the following supporting libraries:
  * The Boost Libraries (www.boost.org)
  * Telnet++ (https://github.com/KazDragon/telnetpp, supplied as a git submodule)
  * Optionally, Crypto++ (www.cryptopp.com)
  * Optionally, CppUnit (sourceforge.net/projects/cppunit)

Building
--------
Use CMake as your system dictates.

