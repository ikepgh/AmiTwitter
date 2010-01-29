Short:   AmiTwitter
Uploader:  ikepgh@yahoo.com (IKE) 
Author:  AmiTwitter Open Source Team
Type:  comm/misc
Version: 0.2x Public Beta Release 1
Architecture:  m68k-amigaos, MorphOS
Requires:  see below
Distribution: GPL, AmiKit, SourceForge only


INTRODUCTION
------------

Features:

    * Sends Tweets and Direct Messages on the Twitter network
    * Downloads Following Tweets Timeline (and images)
    * Downloads @Replies; Retweets of me, by me and to me
    * Downloads Public Timeline (Random Tweets just for fun...)
    * Displays the most recent Tweets that you have sent
    * Displays the most recent Tweet by all friends & followers (User ID)
    * Follow/Unfollow, Block/Unblock specified Users
    * Receive/Stop receiving SMS messages from Users
    * Update your profile; view Favorites and view Users currently Blocking
    * Display most recent Tweet for any User (whether following them or not)
    * MorphOS, OS 3.x compatible


Thanks for your interest!
AmiTwitter Open Source Team

Send email/feedback to: ikepgh@ezcyberspace.com

INSTALLATION
-----------

Just place the binary wherever you'd like on your hard drive.  The 'data' directory 
(and its subdirectories) must be in the same directory as the binary file or the program 
will not run correctly.

REQUIREMENTS
-----------

OS3/MorphOS: MUI, URLtext.mcc, BetterString.mcc, HTMLtext.mcc, TheBar.mcc, 
libiconv.ixlibary (included in Libs directory), working TCP stack, gif/png/jpg/bmp datatypes, OpenURL (optional), ixemul.library/ixnet.library (needed by OS 3.x version only), 

OS4/AROS: Unknown at this time...Please note:  the OS 3.x version crashes on OS 4.x...

USAGE
-----

Have an established Twitter account, Enter your Twitter account username/password in the 'Settings'
window, be connected to the internet. Additionally, it can be started from the commandline for a more 
verbose output (should you desire...).  Check the read-me-first.html file in the Docs folder and the FAQ menu item first if you run into issues...

VERSION HISTORY
--------------- 

0.26 - Added Tweet/Direct Message char countdown;
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

curl 7.14.0,  libxml2-2.6.22,  zlib-1.2.3,  libdl,  glib-2.4.4,  iconv-1.12,
gettext-0.11.2 (libintl.a),  openssl 0.9.7m (libssl.a & libcrypto.a),  pth-bin
(pthread.a 2.0.7), libm  &  libc.

Here's the string used on 3.x/MorphOS:

gcc -lauto -lcurl -lxml2 -lz -ldl -lglib-2.0 -liconv -lintl -lc -lssl -lcrypto -lpthread -lm -lcurl

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
- ShInKurO for joining the project, contributing to the codebase
- Sprocki for his testing of the MorphOS version and very helpful suggestions!
- James Carroll for contributing to the code

Donators: Roschmyr

ABOUT/AUTHORS
-------------

AmiTwitter is © 2009 IKE <ikepgh@ezcyberspace.com>

Look at some of my other projects and ports @
http://www.ezcyberspace.com/programming.shtml


AmiTwitter is based on the following program:

http://www.cuspy.org/xtwitter/  
 xTwitter is © 2008 Tsukasa Hamano <code@cuspy.org>
