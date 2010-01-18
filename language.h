/** $Revision Header *** Header built automatically - do not edit! ***********
 **
 ** © Copyright 2010 IKE <ikepgh@yahoo.com>
 **
 ** File             : amitwitter.h
 ** Created on       : Friday, 06-Nov-09
 ** Created by       : IKE
 ** Current revision : V 0.24
 **
 ** Purpose
 ** -------
 **   AmiTwitter - a Twitter Program
 **
 ** Date        Author                 Comment
 ** =========   ====================   ====================
 ** 09-Jan-10   IKE                    began localization implementation
 ** 18-Dec-09   IKE                    Fast Link prefs, interface cleanup, Users/show added
 ** 15-Dec-09   IKE                    Profile update, Favorites, Blocking/Unblocking and SMS implemented
 ** 14-Dec-09   IKE                    Can now Follow and Unfollow Users via Screen Name!
 ** 13-Dec-09   IKE                    most recent tweet by friends and followers displayed by User ID; began search
 ** 12-Dec-09   IKE                    added TheBar.mcc, other minor enhancements
 ** 04-Dec-09   IKE                    revised interface, new features and error checking
 ** 18-Nov-09   IKE                    login loaded/displayed at startup, error checking, code cleanup
 ** 17-Nov-09   IKE                    Added Hothelp,cleaned up interface and added cross-platform menu's (MorphOS)
 ** 15-Nov-09   - Unknown -            initial MorphOS compile
 ** 06-Nov-09   IKE                    added bumprevision
 **
 ** $Revision Header *********************************************************/
/**
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 2 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **
 ** AmiTwitter was compiled with Cubic IDE/gcc 2.95.3-4 on WinUAE for OS 3.x and
 ** AmiDevCpp for MorphOS
 **
 ** This code is based heavily on xTwitter written
 ** by Tsukasa Hamano found on http://www.cuspy.org/xtwitter/
 **
 ******************************************************************************/


/******************************************************************************/

// Menu Items

#define MSG_FILEX        "File"
#define MSG_TIMELINEX    "Timeline"
#define MSG_RETWEETX     "Retweets"
#define MSG_RETWEETBYMEX "Retweet By Me"
#define MSG_RETWEETTOMEX "Retweet To Me"
#define MSG_RETWEETOFMEX "Retweet Of Me"
#define MSG_REPLIESX     "@Replies"
#define MSG_RELOADX      "Reload"
#define MSG_SEARCHX      "Search"
#define MSG_SEARCHUSERX  "Find Users"
#define MSG_USERSX       "Users"
#define MSG_DIRMSGX      "Direct Message"
#define MSG_DIRMSGSENTX  "Sent"
#define MSG_DIRMSGRCVDX  "Received"
#define MSG_TWEETX       "Tweet"
#define MSG_MYTWEETX     "My Tweets"
#define MSG_FAVSX        "My Favorites"
#define MSG_QUITX        "Quit"

#define MSG_MISCX        "Misc"
#define MSG_FRIENDSX     "My Friends"
#define MSG_FOLLOWERSX   "My Followers"
#define MSG_BLOCKINGX    "Blocking"
#define MSG_RANDOMX      "Random"

#define MSG_TOOLSX       "Tools"
#define MSG_PREFSX       "Settings"
#define MSG_USERPROFILEX "Update Profile"
#define MSG_MUIPREFSX    "MUI Settings"

#define MSG_HELPX        "Help"
#define MSG_FAQX         "FAQs"
#define MSG_DONATEX      "Donate"
#define MSG_ABOUTX       "About AmiTwitter"
#define MSG_ABOUTMUIX    "About MUI"

/******************************************************************************/
