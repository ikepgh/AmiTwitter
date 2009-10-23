Short:   AmiTwitter
Uploader:  ikepgh@yahoo.com (IKE) 
Author:  AmiTwitter Open Source Team
Type:  comm/misc
Version: x.x.x
Architecture:  m68k-amigaos, others coming soon.....
Requires:  MUI, .mcc's, libiconv.ixlibary
Distribution: GPL


INTRODUCTION
------------

This is a  program to send and view Tweets received on the Twitter network.  It has the
capability to send Direct Messages.  It can display the 'Following', 'Followers', 'Tweets'
and 'Favourites' count statistics for the user who sent the most recently received Tweet!  It can 
download your most recent Tweets and display all @Replies to your account.  It also has a 
"Random" function that downloads the most Recent Tweets to open public accounts in
which you are not subscribed...just for fun...and very entertaining.  ;-)

Thanks! 

AmiTwitter Open Source Team

Send email/feedback to: ikepgh@yahoo.com

INSTALLATION
-----------

Just place the binary wherever you'd like on your hard drive.  The 'data' directory must be in
the same directory as the binary file or the program will not run.  

REQUIREMENTS
-----------

OS 3: MUI, URLtext.mcc, BetterString.mcc, HTMLtext.mcc,
libiconv.ixlibary (libs directory), OpenURL (optional)

OS 4: 

MorphOS:

USAGE
-----

Have an established Twitter account, Enter your Twitter account username/password in the 'Settings'
window, be connected to the internet.

---

BUGS
----

Many at this time...

TO DO
-----

- Move code base from HTMLtext to HTMLview
- Error checking (does not do much of it...user/password missing dialog, etc.)
- Make code portable for all versions of the Amiga-like OS's
- Charcters left countdown
- Periodic refresh option?
- Busy notification (.mcc)

FEATURE REQUESTS
----------------

- Command line version
- character conversion (iconv)/catalog translations

---

VERSION HISTORY
---------------

x.x.x - Initial Release (Coming soon...)

0.1.1 - Fixed bug that caused some malformed image names to not display the image; implemented users/show for the 
id logged in, returns 20 most recent Tweets you have sent; implemented @Replies and implemented "Random" (most 
recent Tweets from unprotected accounts in which you do not have to be subscribed...kinda neat)
0.1.0 - Added current 'Following', 'Followers', 'Tweets' and 'Favourites' counts; sends Direct Messages!
0.0.9 - Added 'Donate' window dialog, FAQ help text, fixed download directory locations
0.0.8 - Fixed download logic/display; added date/time a Tweet was sent
0.0.7 - Images are now displayed!
0.0.6 - Downloads recent Tweet data into a window now; working on images...
0.0.5 - Fixed endless update loop (now only pulls Tweets once when user selects 'Home')
0.0.4 - Downloads images to a directory
0.0.3 - Pulls current Tweets from account and dumps to stdout
0.0.2 - Added 'Settings' window dialog, URLtext links
0.0.1 - Got it sending Tweets!
0.0.0 - libtwitter compiled as an .o file

RECOMPILING
-----------

Recompiling has quite a few dependencies, hence the file size...but, hey, it
works!  Compiled on OS 3.x (WinUAE/Cubic IDE/gcc 2.95.3-4) with the following dependencies:

curl 7.14.0,  libxml2-2.6.22,  zlib-1.2.3,  libdl,  glib-2.4.4,  iconv-1.12,
gettext-0.11.2 (libintl.a),  openssl 0.9.7m (libssl.a & libcrypto.a),  pth-bin
(pthread.a 2.0.7), libm  &  libc.

Here's the string used on 3.x:

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

ABOUT/AUTHORS
-------------

AmiTwitter is © 2009 IKE <ikepgh@yahoo.com>

Look at some of my other projects and ports @
http://www.ezcyberspace.com/programming.shtml

Special thanks to ShInKurO for joining the project, contributing to the codebase
and tons of patience.

AmiTwitter was based heavily on the following program:

http://www.cuspy.org/xtwitter/  
 xTwitter is © 2008 Tsukasa Hamano <code@cuspy.org>
