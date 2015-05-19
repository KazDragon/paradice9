## Update February 2013 ##
I have started a blog about Paradice.  The intent is to show some of my thoughts about the design of the server, to show how it is put together, and to track progress of the current work.  You can find the blog at http://paradice9.blogspot.com.

## About ##
Paradice9 is a C++ codebase that implements a server that can be connected to by terminal clients in order to enhance a distributed tabletop role-play session.

![http://1.bp.blogspot.com/-4fe-EYrX_Hc/UQJ3wBphZfI/AAAAAAAAALw/cwD_O-NjACs/s1600/paradice9.png](http://1.bp.blogspot.com/-4fe-EYrX_Hc/UQJ3wBphZfI/AAAAAAAAALw/cwD_O-NjACs/s1600/paradice9.png)

## Status ##
The project was hacked together in a lunchtime or two, and underwent its initial outing on the 5th of November 2009.  Development has continued in a more formal manner from there, and is now our tool of choice for over-the-internet role playing.

On the 2nd of December 2010, version 1.0 went live.  This contains a major upgrade to the user interface, changing it from a line-mode spew of text that it was into a more sophisticated component-based GUI (see above).

The most recent release is version 1.1, which went live on the 3rd of October 2012.  It provides a series of minor feature enhancements and a host of bug fixes since version 1.0.

A lot of work has been done for version 1.2, including many of the bug fixes and optimisations scheduled for 1.3, but it has been abandoned since the gaming group using the product has not met in some time and therefore the need for the 1.2-specific features has gone.  Instead, work progresses for 1.4, which contains a rework of the codebase for more modern features, which will make it easier to work with for future releases.

## Philosophy ##
Paradice9 is to be developed using modern C++ in a standard-library/Boost coding style.  No platform-specific code is to be used.  Abstraction libraries must be used instead.

For algorithmic components, Test-Driven development via Unit Testing is the preferred means of development.  For GUI components, system testing must be performed in its stead.

Paradice9 has been tested successfully against the following clients:
  * PuTTY
  * TeraTerm Pro
  * Tintin++
  * Ubuntu Telnet

Some other clients have problems, so don't use them:
  * MUSHClient - no support for ANSI codes beyond colour; has own command bar.
  * CMUD - Does not support SCO character set; has own command bar.
  * Mudlet - Does not support SCO character set; has own command bar.
  * TinyFugue - Is fundamentally linemode.

There is a long-term goal to support these clients through use of a client detection algorithm.

## Required Libraries ##

_For Development_

[Boost](http://www.boost.org/)
The project is currently compiled and tested under Boost 1.46, although later versions should just work.  Components used include:
```
    Boost.Asio
    Boost.Bind
    Boost.ConceptCheck
    Boost.CallTraits
    Boost.Filesystem
    Boost.Foreach
    Boost.Format
    Boost.Function
    Boost.Iterator
    Boost.Lambda
    Boost.Optional
    Boost.ProgramOptions
    Boost.Serialization
    Boost.Signals
    Boost.SmartPtr
    Boost.Spirit
    Boost.StringAlgo
    Boost.TypeTraits
    Boost.Variant
```

Note: the requirement was recently changed from 1.43 to 1.46 in order to accommodate boost::random\_device for a better random number seed.  In [issue #129](https://code.google.com/p/paradice9/issues/detail?id=#129), this will increase again to 1.54.  Probably best just to grab the latest.

[Crypto++](http://www.cryptopp.com/)

_For Testing_

[CppUnit](http://sourceforge.net/apps/mediawiki/cppunit/index.php?title=Main_Page)

## Architecture ##
The source code is distributed in several components:

### Odin ###
The Norse god of Wisdom and War, Odin gave away an eye in return for knowledge.  The Odin library was partially developed during my (currently stalled) Vision project, and is a collection of select pieces of source that I have built in private over the years.  It is also the target library for more generically usable source components, including an asynchronous communications layer and a fully-featured Telnet layer built on top of it.

### Munin and Hugin ###
Along with its twin Hugin, the raven Munin circled the world each day, bringing back news of events taking place to its master, Odin.  In Paradice, Munin is a generic user-interface API, with the primary implementation being a windowing system implemented by the exchange of text-based ANSI codes.  Hugin is the implementation of specific components required for the Paradice9 application.

### Paradice ###
This is the library for application-specific developments.

### Paradice9 ###
This is a lightweight driver application that just parses command line arguments, then hooks user input into the core library.