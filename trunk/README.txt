Short:   AmiTwitter
Uploader:  ikepgh@yahoo.com (IKE) 
Author:  ikepgh@yahoo.com (IKE) 
Type:  comm/misc
Version: 0.1.1
Architecture:  m68k-amigaos
Requires:  MUI, a few .mcc's, OpenURL
Distribution: GPL

This is a  program I wrote to send and view Tweets received on the Twitter network.  Additionally, 
it has the capability to send Direct Messages!  It also displays the 'Following', 'Followers', 'Tweets' 
and 'Favourites' count statistics for the user who sent the most recently received Tweet!  It can 
download your most recent Tweets and display all @Replies to your account.  It also has a 
"Random" function that pulls down the most Recent Tweets to accounts in which you are not subscribed.

Send email/feedback to:  ikepgh@yahoo.com

Look at some of my other projects and ports @
http://www.ezcyberspace.com/programming.shtml

If you like this program, please consider donating to support future development.

Thanks! 
IKE

---

Installation:  Just place the binary wherever you'd like on your hard drive.  The 'data' directory must be in
the same directory as the binary file or the program will not run.  

Requires: MUI, URLtext, HTMLtext & BetterString .mcc's, OpenURL (optional)

Morphos requires: libiconv.ixlibrary from Aminet

Usage:  Enter your Twitter account username/password in the 'Settings' window, be connected to the internet..

---

People who have donated:

---

Known Bugs:  Many at this time...
To do:  

- Error checking (does not do much of it that the user can see...)
- Charcters left countdown
- Periodic refresh option
- Busy .mcc

Feature Requests:

- Command line version
- iconv/character conversion/catalog translations

---

Version History:

0.1.x - Initial Release (Coming soon...)

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

---

Recompiling has quite a few dependencies, hence the file size...but, hey, it works!

gcc -lauto -lcurl -lxml2 -lz -ldl -lglib-2.0 -liconv -lintl -lc -lssl -lcrypto -lpthread -lm -lcurl

Compiled using:  WinUAE/Cubic IDE/gcc 2.95.3-4

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

---

AmiTwitter is © 2009 IKE <ikepgh@yahoo.com>

AmiTwitter was based heavily on the following program:

http://www.cuspy.org/xtwitter/  
 xTwitter is © 2008 Tsukasa Hamano <code@cuspy.org>
