# About #
This page describes the release targets and the purpose of each.  Additionally, in brackets you can find the primary target of the release (e.g. Application is for the Paradice9 application, Paradice is for the underlying framework, Bug Fix Release is for the odd-numbered releases between feature releases) and also whether the scheduled release has been completed or are in progress.

## Release 1.0 {Paradice + Application/Complete 2nd December 2010} ##
Initial release of Paradice9 with the Munin user interface.  The Munin User Interface, which was tracked by [Issue #29](https://code.google.com/p/paradice9/issues/detail?id=#29), is the primary feature for Paradice.  It is a component-based user interface that renders the screen a user sees server side and then makes use of ANSI control codes to send the changes to the client.  This technology enables a much higher degree of control over the presentation than the usual line-mode style of this kind of application.

## Release 1.1 {Bug Fix Release/Complete 3rd October 2011} ##
This release saw a large stabilisation effort as regular usage of the application showed opportunities for all kinds of improvements.

## Release 1.2 {Application/Abandoned} ##
Feature requests for Paradice9, with [Issue #107](https://code.google.com/p/paradice9/issues/detail?id=#107) (Encounter Control) being the primary feature for the release.

Update: Our RP group dissolved and the both the need for the feature and the implementation of the feature itself have stagnated.  Therefore, I am pushing the issues for 1.2 back to the backlog and will go back to work on the codebase.

## Release 1.3 {Bug Fix Release/Complete} ##
Two performance-related issues are scheduled here.  The first is an effort to reduce the number of memory allocations, since profiling has revealed that allocations and deallocations are the largest consumers of processing time during the application's life.  This is tracked in [Issue #115](https://code.google.com/p/paradice9/issues/detail?id=#115).  The second ticket is for other performance considerations.  In particular, usage of Valgrind has yielded some interesting performance hotspots that can be quickly cleaned up.  This is tracked in [Issue #121](https://code.google.com/p/paradice9/issues/detail?id=#121).

These issues have been mostly worked on in spare time and are solved.  As such, Release 1.3 will be for actual bugs discovered after the release of 1.2 and before the release of 1.4.

## Release 1.4 {Paradice} ##
The flagship feature for this release is to re-work the GUI to have value semantics.  Following Bjarne Stroustrup's and Sean Parent's presentations during GoingNative 2013, I became convinced that this is the right way to go around it.  It also promises easier integration with threads -- an area where the GUI has problems.  This change is tracked in [Issue #133](https://code.google.com/p/paradice9/issues/detail?id=#133).

This will require upgrading to C++11, which itself is quite the task.  That is tracked in [Issue #92](https://code.google.com/p/paradice9/issues/detail?id=#92).

## Release 1.6 {Paradice} ##
Incorporate multi-threading capabilities across the application and library.  This is tracked in [Issue #67](https://code.google.com/p/paradice9/issues/detail?id=#67).

## Release 1.8 {Application} ##
Addition of the 'Room' concept to allow multiple gaming sessions.  This is tracked in [Issue #123](https://code.google.com/p/paradice9/issues/detail?id=#123).

## Release 2.0 {Paradice} ##
Provide features that enable Paradice applications to service a larger range of clients.  [Issue #99](https://code.google.com/p/paradice9/issues/detail?id=#99) tracks a feature to provide terminal type detection.  [Issue #100](https://code.google.com/p/paradice9/issues/detail?id=#100) tracks a feature to provide a user interface that is far less ANSI-heavy, but provides hooks for other markups and protocols that are used by the applications targeted by Paradice (e.g. [MSDP](http://tintin.sourceforge.net/msdp/), [MSSP](http://tintin.sourceforge.net/mssp/)).  Could also include [GMCP](http://www.aardwolf.com/wiki/index.php/Clients/GMCP)/[ATCP](http://www.ironrealms.com/rapture/manual/files/FeatATCP-txt.html) and Aardwolf's ["Protocol 102"](http://www.aardwolf.com/blog/2008/07/10/telnet-negotiation-control-mud-client-interaction/).

## Future ##
After the release of Paradice v2.0, the intent is to announce and open the framework to public scrutiny, possibly add additional development partners, and to branch out into new applications for Paradice.