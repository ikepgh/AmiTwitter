Short:   AmiTwitter
Uploader:  ikepgh@ezcyberspace.com (IKE) 
Author:  AmiTwitter Open Source Team
Type:  comm/misc
Version: 0.29 Public Beta Release 4
Architecture:  m68k-amigaos, MorphOS, ppc-amigaos 
Requires:  see below
Distribution: GPL, AmiKit, SourceForge only


INTRODUCTION
------------

Features:

    * Sends Tweets and Direct Messages on the Twitter network
    * Downloads Timeline, Retweets of me/by me/to me, @Replies, Favorites Tweets
    * Downloads Public Timeline (Random Tweets just for fun...)
    * Displays the most recent Tweets that you have sent
    * Displays the most recent Tweet by Friends, Followers and Blocking (User ID only)
    * Follow/Unfollow, Block/Unblock, Receive/Stop SMS messages from Users
    * Update your Twitter website profile
    * Display most recent Tweet for any User (whether following them or not! via User ID)
    * Displays most recently sent and received Direct Message 
    * Fully localized program and FAQs 
    * International Character Support via codesets.library 
    * Sends pictures via TwitPic
    * MorphOS, AmigaOS 3.x/4.1 compatible

Thanks for your interest!
AmiTwitter Open Source Team

Send email/feedback to: ikepgh@ezcyberspace.com

INSTALLATION
-----------

Just place the 'AmiTwitter' directory wherever you'd like on your hard drive. 
Place the enclosed libiconv.ixlibary in your system 'Libs' directory and the
HTMLtext.mcc and HTMLtext.mcp files in your 'MUI/libs' directory, if you don't already have this library.

REQUIREMENTS
-----------

OS3/MorphOS/OS4.1: MUI, URLtext.mcc, BetterString.mcc, HTMLtext.mcc, TheBar.mcc, libiconv.ixlibary (included in Libs directory), 
codesets.library, working TCP stack, gif/png/jpg/bmp datatypes, OpenURL (optional for 3.x/4.1), ixemul.library/ixnet.library 
(needed by OS 3.x version only)

AROS: Unknown at this time.

USAGE
-----

Have an established Twitter account, Enter your Twitter account username/password in the 'Settings'
window, be connected to the internet. Additionally, it can be started from the commandline for a more 
verbose output (should you desire...).  Check the read-me-first.html file in the Docs folder and the FAQ menu item first if you run into issues...

VERSION HISTORY
--------------- 

0.29 - Public Beta Release 4 - added TwitPic support; Reworked: main interface,
tweet formatting display and a few misc style guide issues by popular demand; Iconify now has the
logo displayed instead of default MUI tool; code cleanup
0.28 - Public Beta Release 3 - glib dependency removed for all OS's thanks to Cyborg!; first OS 4.1 version compiled; updated swedish,
italian catalogs, FAQs, program icons & numerous bug fixes: image name truncation, Locale functions, safety NULL checks for free() and
replaced free() with xmlFree() where appropriate, fixed numerous declarations
0.27 - Public Beta Release 2 - codesets.library implemented!  
0.26 - Added Tweet/Direct Message char countdown; most recent sent/received direct message; updated and reduced logo size; 
reordered Users tabs; added .png images
0.25 - Menu's, tabs & TheBar are now localized 
0.24 - Began localization implementation
0.23 - Public Beta Release 1 - Users controls are now tabbed; added Users/Show - can search by User Name and it will display 
the last Tweet by that User (whether you are following them or not); fixed minor display bug (missing </small> tag)
0.22 - Major implementations: Blocking/Unblocking; Receive/Stop receiving SMS messages; ability to update your profile;
view Tweets that you have marked as Favorites, also can view users currently being Blocked by you (via User ID) 
0.21 - Can now Follow and Unfollow Users via Screen Name!
0.20 - Added Friends, Followers most recent tweet (currently only by User ID); began search implementation
0.19 - Fixed resize issue; cleaned up error checking; renamed friends_timeline to the new home_timeline; added TheBar.mcc 
buttons and MasonIcons
0.18 - Revised display of name/location/following/followers/tweets stats; reworked interface; added user/pass Test routine,
added retweet by, to and of me,  reloads current local file, more error checking
0.17 - Fixed login, now loaded on startup (and displayed); added rudimentary error checking, serious code cleanup 
0.16 - Added hothelp, improved interface, added cross-platform Menus
0.15 - Preliminary compile on MorphOS, fixed AmiTwitter icons/image
0.14 - Added proper bumprevision to sources
0.13 - Added stack, CTR-C from CLI, other minor enhancements
0.12 - Added Name, Location of last Tweet received to main window (removed now); added drop down menu items for buttons/shortcuts,
cleaned up direct message interface to be more useable (cycle chain); added the Source that a Tweet was sent from to the
results (disabled now); fixed some memory leaks
0.11 - Fixed bug that caused some malformed image names to not display the image; implemented users/show for the 
id logged in, returns 20 most recent Tweets you have sent; implemented @Replies and implemented "Random" (most 
recent Tweets from unprotected accounts in which you do not have to be subscribed...kinda neat)
0.10 - Added current 'Following', 'Followers', 'Tweets' and 'Favorites (disabled now)' counts; sends Direct Messages!
0.09 - Added 'Donate' window dialog, FAQ help text, fixed download directory locations
0.08 - Fixed download logic/display; added date/time a Tweet was sent
0.07 - Images are now displayed!
0.06 - Downloads recent Tweet data into a window now; working on images...
0.05 - Fixed endless update loop (now only pulls Tweets once when user selects 'Timeline'(Home))
0.04 - Downloads images to a directory
0.03 - Pulls current Tweets from account and dumps to stdout
0.02 - Added 'Settings' window dialog, URLtext links
0.01 - Got it sending Tweets!
0.00 - libtwitter compiled as an .o file

RECOMPILING
-----------

Recompiling has quite a few dependencies, hence the file size...but, hey, it
works!  Compiled on OS 3.x (WinUAE/Cubic IDE/gcc 2.95.3-4) and MorphOS (AmiDevCpp) with the following dependencies:

curl 7.14.0,  libxml2-2.6.22,  zlib-1.2.3,  libdl,  iconv-1.12,
gettext-0.11.2 (libintl.a),  openssl 0.9.7m (libssl.a & libcrypto.a),  pth-bin
(pthread.a 2.0.7), libm, libc and libcodesets (MorphOS only).

Here's the string used on 3.x:

gcc -lauto -lcurl -lxml2 -lz -ldl -liconv -lintl -lc -lssl -lcrypto -lpthread -lm -lcurl

Morphos - see above and: -lcodesets

On OS 4.1 It's linked dynamically against the following shared libraries
as available in the SOBJS: drawer of AmigaOS 4.1 Update 1:
- libcurl-7.16.so
- libxml2.so
- libdl.so
- libssl-0.9.8.so
- libpthread.so
- libz.so

- There seems to be an issue with the shared libraries on Update 1.
If you get a requester claiming that "libz.so.1" couldn't be opened,
just go to your SOBJS: in a Shell and enter:

    Makelink SOFT libz.so.1 libz.so

LICENSE
-------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

THANKS
------

Special thanks to: 
- Cyborg for joining the project, contributing to the codebase
- Sprocki for his testing of the MorphOS version and very helpful suggestions!
- ShInKurO and James Carroll for contributing to the code
- Mason & Patrick Jung for the buttons/icon sets
- All the catalog translators and beta testers

Donators: Roschmyr, (your name could be here...)

ABOUT/AUTHORS
-------------

AmiTwitter is © 2009-10 IKE <ikepgh@ezcyberspace.com>

Look at some of my other projects and ports @
http://www.ezcyberspace.com/programming.shtml


AmiTwitter is based on the following program:

http://www.cuspy.org/xtwitter/  
 xTwitter is © 2008 Tsukasa Hamano <code@cuspy.org>
