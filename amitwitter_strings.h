/** $Revision Header *** Header built automatically - do not edit! ***********
 **
 ** © Copyright 2010 IKE <ikepgh@yahoo.com>
 **
 ** File             : amitwitter.c
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

/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#define CATCOMP_NUMBERS
#define CATCOMP_ARRAY
#define CATCOMP_BLOCK
#define CATCOMP_CODE
#define CATCOMP_STRINGS

/****************************************************************************/

#ifdef CATCOMP_NUMBERS

#define MSG_FOLLOW 0
#define MSG_BLOCK 1
#define MSG_NOTIFY 2
#define MSG_SHOWUSER 3
#define MSG_TIMELINE 4
#define MSG_TIMELINE2 5
#define MSG_RETWEETS 6
#define MSG_RETWEETS2 7
#define MSG_REPLIES 8
#define MSG_REPLIES2 9
#define MSG_RELOAD 10
#define MSG_RELOAD2 11
#define MSG_SEARCH 12
#define MSG_SEARCH2 13
#define MSG_USERS 14
#define MSG_USERS2 15
#define MSG_DIRMSG 16
#define MSG_DIRMSG2 17
#define MSG_TWEET 18
#define MSG_TWEET2 19
#define MSG_ERROR 20
#define MSG_ERROR2 21
#define MSG_ERROR3 22
#define MSG_ERROR4 23
#define MSG_ERROR5 24
#define MSG_ERROR6 25
#define MSG_ERROR7 26
#define MSG_ERROR8 27
#define MSG_ATTEMPTING 28
#define MSG_STATS 29
#define MSG_STATS2 30
#define MSG_SENT 31
#define MSG_DIRMSGSENT 32
#define MSG_FOLLOWING 33
#define MSG_UNFOLLOWING 34
#define MSG_VALID 35
#define MSG_UPDATED 36
#define MSG_SMSFOLLOW 37
#define MSG_SMSUNFOLLOW 38
#define MSG_BLOCKING 39
#define MSG_UNBLOCKING 40
#define MSG_SETTINGS 41
#define MSG_SETTINGS2 42
#define MSG_USERNAME 43
#define MSG_PASSWORD 44
#define MSG_SAVE 45
#define MSG_TEST2 46
#define MSG_CANCEL 47
#define MSG_HELP 48
#define MSG_REGISTER 49
#define MSG_ESTABLISH 50
#define MSG_FORGOT 51
#define MSG_DONATE 52
#define MSG_DONATE2 53
#define MSG_DONATE3 54
#define MSG_SEND 55
#define MSG_SEND2 56
#define MSG_SEND3 57
#define MSG_SEND4 58
#define MSG_CLEAR 59
#define MSG_UPDATE 60
#define MSG_CANCEL2 61
#define MSG_SEARCH3 62
#define MSG_SEARCH4 63
#define MSG_SEARCH5 64
#define MSG_SEARCH6 65
#define MSG_CLEAR2 66
#define MSG_SEARCH7 67
#define MSG_CANCEL3 68
#define MSG_MANAGE 69
#define MSG_FOLLOW2 70
#define MSG_FOLLOW3 71
#define MSG_FOLLOW4 72
#define MSG_CLEAR3 73
#define MSG_FOLLOW5 74
#define MSG_UNFOLLOOW 75
#define MSG_CANCEL4 76
#define MSG_BLOCKING2 77
#define MSG_BLOCKING3 78
#define MSG_BLOCKING4 79
#define MSG_CLEAR4 80
#define MSG_BLOCKING5 81
#define MSG_UNBLOCK 82
#define MSG_CANCEL5 83
#define MSG_SMS 84
#define MSG_SMS2 85
#define MSG_SMS3 86
#define MSG_CLEAR5 87
#define MSG_RECEIVE 88
#define MSG_STOP 89
#define MSG_CANCEL6 90
#define MSG_SHOWUSER2 91
#define MSG_SHOWUSER3 92
#define MSG_SHOWUSER4 93
#define MSG_CLEAR6 94
#define MSG_SHOWUSER5 95
#define MSG_CANCEL7 96
#define MSG_SENDDIRMSG 97
#define MSG_SENDDIRMSG2 98
#define MSG_RECIPIENT 99
#define MSG_SCREENNAME 100
#define MSG_MESSAGE 101
#define MSG_MESSAGE2 102
#define MSG_CLEAR7 103
#define MSG_SEND5 104
#define MSG_CANCEL8 105
#define MSG_UPDATEPROFILE 106
#define MSG_UPDATEPROFILE2 107
#define MSG_PROFNAME 108
#define MSG_PROFNAME2 109
#define MSG_WEB 110
#define MSG_WEB2 111
#define MSG_LOCATION 112
#define MSG_LOCATION2 113
#define MSG_BIO 114
#define MSG_BIO2 115
#define MSG_UPDATEPROFILE3 116
#define MSG_CANCEL9 117
#define MSG_WHAT 118
#define MSG_FAST 119
#define MSG_FEEDBACK 120
#define MSG_DONATE4 121
#define MSG_SAVE2 122
#define MSG_TEST3 123
#define MSG_DONTSAVE 124
#define MSG_CLEAR8 125
#define MSG_SEND6 126
#define MSG_CANCEL10 127
#define MSG_CLEAR9 128
#define MSG_SEARCH8 129
#define MSG_CANCEL11 130
#define MSG_CLEAR10 131
#define MSG_FOLLOWUSER 132
#define MSG_UNFOLLOWUSER 133
#define MSG_CANCEL12 134
#define MSG_CLEAR12 135
#define MSG_BLOCKUSER 136
#define MSG_UNBLOCKUSER 137
#define MSG_CANCEL13 138
#define MSG_CLEAR13 139
#define MSG_SMSNOTE 140
#define MSG_SMSSTOP 141
#define MSG_CANCEL14 142
#define MSG_CLEAR14 143
#define MSG_SHOWTWEETS 144
#define MSG_CANCEL15 145
#define MSG_CLEAR15 146
#define MSG_SENDDIRMSG3 147
#define MSG_CANCEL16 148
#define MSG_UPDATEPROFILE4 149
#define MSG_CANCEL17 150
#define MSG_SEARCH9 151
#define MSG_OK 152
#define MSG_SEARCH10 153
#define MSG_BLOCK2 154
#define MSG_BLOCK3 155
#define MSG_BLOCK4 156
#define MSG_QUIT 157
#define MSG_QUIT2 158
#define MSG_QUIT3 159
#define MSG_SAVE3 160
#define MSG_OK2 161
#define MSG_SAVED 162
#define MSG_UPDATEPROFILE5 163
#define MSG_OK3 164
#define MSG_UPDATEPROFILE6 165
#define MSG_HTML1 166
#define MSG_HTML2 167
#define MSG_HTML3 168
#define MSG_HTML4 169
#define MSG_HTML5 170
#define MSG_HTML6 171
#define MSG_HTML7 172
#define MSG_HTML8 173
#define MSG_HTML9 174
#define MSG_HTML10 175
#define MSG_HTML11 176
#define MSG_HTML12 177
#define MSG_HTML13 178
#define MSG_HTML14 179
#define MSG_HTML15 180
#define MSG_HTML16 181
#define MSG_HTML17 182
#define MSG_HTML18 183
#define MSG_HTML19 184
#define MSG_HTML20 185
#define MSG_HTML21 186
#define MSG_HTML22 187
#define MSG_HTML23 188
#define MSG_HTML24 189
#define MSG_HTML25 190
#define MSG_HTML26 191
#define MSG_HTML27 192
#define MSG_HTML28 193
#define MSG_HTML29 194
#define MSG_HTML30 195
#define MSG_HTML31 196
#define MSG_HTML32 197
#define MSG_HTML33 198
#define MSG_HTML34 199
#define MSG_HTML35 200
#define MSG_0000 201
#define MSG_0001 202
#define MSG_0002 203
#define MSG_0003 204
#define MSG_0004 205
#define MSG_0005 206
#define MSG_0006 207
#define MSG_0007 208
#define MSG_0008 209
#define MSG_0009 210
#define MSG_0010 211
#define MSG_0011 212
#define MSG_0012 213
#define MSG_0013 214
#define MSG_0014 215
#define MSG_0015 216
#define MSG_0016 217
#define MSG_0017 218
#define MSG_0018 219
#define MSG_0019 220
#define MSG_0020 221
#define MSG_0021 222
#define MSG_0022 223
#define MSG_0023 224
#define MSG_0024 225
#define MSG_0025 226
#define MSG_0026 227
#define MSG_0027 228
#define MSG_0028 229
#define MSG_0029 230
#define MSG_0030 231
#define MSG_0031 232

#endif /* CATCOMP_NUMBERS */

/****************************************************************************/

#ifdef CATCOMP_STRINGS

#define MSG_FOLLOW_STR "Follow/Unfollow"
#define MSG_BLOCK_STR "Block/Unblock"
#define MSG_NOTIFY_STR "Notify/Unnotify"
#define MSG_SHOWUSER_STR "Show User"
#define MSG_TIMELINE_STR "_Timeline"
#define MSG_TIMELINE2_STR "Get most recent Tweets (max 20)"
#define MSG_RETWEETS_STR "Ret_weets"
#define MSG_RETWEETS2_STR "Get most recent Retweets to me (max 20)"
#define MSG_REPLIES_STR "@_Replies"
#define MSG_REPLIES2_STR "Get most recent @Replies (max 20)"
#define MSG_RELOAD_STR "Rel_oad"
#define MSG_RELOAD2_STR "Reload current local file"
#define MSG_SEARCH_STR "_Search"
#define MSG_SEARCH2_STR "Search"
#define MSG_USERS_STR "_Users"
#define MSG_USERS2_STR "Manage Users"
#define MSG_DIRMSG_STR "_Dir Msg"
#define MSG_DIRMSG2_STR "Send a Direct Message"
#define MSG_TWEET_STR "Tw_eet"
#define MSG_TWEET2_STR "Send a Tweet"
#define MSG_ERROR_STR "<B>Hmm...An error occurred!</B><p>Possible reasons:<ul><li>No internet connection</li><li>Twitter site down</li><li>Username/password incorrect</li>"
#define MSG_ERROR2_STR "<B>Hmm...An error occurred!</B><p>Possible reasons:<ul><li>No internet connection</li><li>Twitter site down</li><li>Username/password incorrect</li><li>Did you enter text (and/or correct recipient Screen Name for direct messages)?</li>"
#define MSG_ERROR3_STR "<B>Hmm...An error occurred!</B><p>Is your Username/password set correctly?"
#define MSG_ERROR4_STR "<B>Hmm...An error occurred!</B><p>Possible reasons:<ul><li>No internet connection</li><li>Twitter site down</li><li>Username/password incorrect</li><li>Did you enter the Screen Name correctly?</li><li>Are you already following (or unfollowing) that User?</li>"
#define MSG_ERROR5_STR "<B>Hmm...An error occurred!</B><p>Possible reasons:<ul><li>No internet connection</li><li>Twitter site down</li><li>Username/password incorrect</li><li>Did you enter the Screen Name correctly?</li><li>Are you already blocking (or not blocking) that User?</li>"
#define MSG_ERROR6_STR "<B>Hmm...An error occurred!</B><p>Possible reasons:<ul><li>No internet connection</li><li>Twitter site down</li><li>Username/password incorrect</li><li>Did you enter the Screen Name correctly?</li><li>Are you already receiving (or stopped receiving) SMS notifications for that User?</li>"
#define MSG_ERROR7_STR "<B>Hmm...An error occurred!</B><p>Possible reasons:<ul><li>No internet connection</li><li>Twitter site down</li><li>Username/password incorrect</li><li>You <b>*MUST*</b> specify a 'Name' to make any updates!</li>"
#define MSG_ERROR8_STR "<B>Hmm...An error occurred!</B><p>Possible reasons:<ul><li>No internet connection</li><li>Twitter site down</li><li>Username/password incorrect</li><li>Did you enter the Screen Name correctly?</li>"
#define MSG_ATTEMPTING_STR "Attempting to get the latest..."
#define MSG_STATS_STR "<small>Name: %s Location: %s Following: %s Followers: %s Tweets: %s</small>"
#define MSG_STATS2_STR "<b>User ID:</b> %s<br><b>Last Tweet:</b> %s<br> <b>Date:</b><small>  %s</small></b><br><p>"
#define MSG_SENT_STR "Your Tweet was sent!"
#define MSG_DIRMSGSENT_STR "Your Direct Message was sent!"
#define MSG_FOLLOWING_STR "You are now <b>Following</b> that User!"
#define MSG_UNFOLLOWING_STR "You are now <b>Unfollowing</b> that User!"
#define MSG_VALID_STR "<B>Congratulations!</B> Your username/password is valid!"
#define MSG_UPDATED_STR "<B>Congratulations!</B> You have updated your profile!"
#define MSG_SMSFOLLOW_STR "You are now receiving Notifications (SMS following) that User!"
#define MSG_SMSUNFOLLOW_STR "You are no longer receiving notifications (SMS following) that User!"
#define MSG_BLOCKING_STR "You are now <b>Blocking</b> that User!"
#define MSG_UNBLOCKING_STR "You are now <b>Unblocking</b> that User!"
#define MSG_SETTINGS_STR "Settings"
#define MSG_SETTINGS2_STR "Account Information Settings"
#define MSG_USERNAME_STR "User Name:"
#define MSG_PASSWORD_STR "Password:"
#define MSG_SAVE_STR "_Save"
#define MSG_TEST2_STR "_Test"
#define MSG_CANCEL_STR "_Cancel"
#define MSG_HELP_STR "Help!"
#define MSG_REGISTER_STR "\33c\nDon't already have a User Name/Password?\n  You must register at the Twitter website first!\n"
#define MSG_ESTABLISH_STR "Establish An Account!"
#define MSG_FORGOT_STR "Forgot Your Password?"
#define MSG_DONATE_STR "Donate"
#define MSG_DONATE2_STR "\33c\nIf you find AmiTwitter useful, please consider donating.\n  You must have an active internet connection:\n"
#define MSG_DONATE3_STR "Donate!"
#define MSG_SEND_STR "Send a Tweet"
#define MSG_SEND2_STR "Send a Tweet"
#define MSG_SEND3_STR "Tweet:"
#define MSG_SEND4_STR "Enter your Tweet and click Update to send (max 140 characters)"
#define MSG_CLEAR_STR "C_lear"
#define MSG_UPDATE_STR "_Update"
#define MSG_CANCEL2_STR "_Cancel"
#define MSG_SEARCH3_STR "Search"
#define MSG_SEARCH4_STR "Search"
#define MSG_SEARCH5_STR "Search:"
#define MSG_SEARCH6_STR "Enter your search string and click Search (max 140 characters)"
#define MSG_CLEAR2_STR "C_lear"
#define MSG_SEARCH7_STR "_Search"
#define MSG_CANCEL3_STR "_Cancel"
#define MSG_MANAGE_STR "Manage Users"
#define MSG_FOLLOW2_STR "Change the Following Status for a User"
#define MSG_FOLLOW3_STR "User:"
#define MSG_FOLLOW4_STR "Enter the Screen Name you want to follow or unfollow..."
#define MSG_CLEAR3_STR "C_lear"
#define MSG_FOLLOW5_STR "_Follow"
#define MSG_UNFOLLOOW_STR "_Unfollow"
#define MSG_CANCEL4_STR "_Cancel"
#define MSG_BLOCKING2_STR "Change the Blocking Status for a User"
#define MSG_BLOCKING3_STR "User:"
#define MSG_BLOCKING4_STR "Enter the Screen Name you want to block or unblock..."
#define MSG_CLEAR4_STR "C_lear"
#define MSG_BLOCKING5_STR "_Block"
#define MSG_UNBLOCK_STR "U_nblock"
#define MSG_CANCEL5_STR "_Cancel"
#define MSG_SMS_STR "Change the SMS Notification Status for a User"
#define MSG_SMS2_STR "User:"
#define MSG_SMS3_STR "Enter the Screen Name you want to either receive\n or stop receiving SMS notifications for..."
#define MSG_CLEAR5_STR "C_lear"
#define MSG_RECEIVE_STR "_Receive"
#define MSG_STOP_STR "_Stop"
#define MSG_CANCEL6_STR "_Cancel"
#define MSG_SHOWUSER2_STR "Show a User"
#define MSG_SHOWUSER3_STR "User:"
#define MSG_SHOWUSER4_STR "\33cEnter the Screen Name of the User you want to display.\nYou do not need to be following them to view their\n most recent Tweet!"
#define MSG_CLEAR6_STR "C_lear"
#define MSG_SHOWUSER5_STR "_Show User"
#define MSG_CANCEL7_STR "_Cancel"
#define MSG_SENDDIRMSG_STR "Send a Direct Message"
#define MSG_SENDDIRMSG2_STR "Send a Direct Message"
#define MSG_RECIPIENT_STR "Recipient:"
#define MSG_SCREENNAME_STR "Enter a Screen Name only"
#define MSG_MESSAGE_STR "Message:"
#define MSG_MESSAGE2_STR "Enter a message (max 140 characters)"
#define MSG_CLEAR7_STR "C_lear"
#define MSG_SEND5_STR "_Send"
#define MSG_CANCEL8_STR "_Cancel"
#define MSG_UPDATEPROFILE_STR "Update Your Profile"
#define MSG_UPDATEPROFILE2_STR "Update Your Twitter Website Profile"
#define MSG_PROFNAME_STR "Name:"
#define MSG_PROFNAME2_STR "\33cThis field is currently mandatory to make\n any updates to your profile. This is what\n is displayed on your Twitter site\nIt can be anything and has no effect on your\n actual account Screen Name or User ID whatsoever...\nfor a lot of users this is their 'real' name\n (Max 20 characters)"
#define MSG_WEB_STR "Web:"
#define MSG_WEB2_STR "\33cEnter your website URL\n It will be prepended with 'http://' if not present\n(Max 100 characters)"
#define MSG_LOCATION_STR "Location:"
#define MSG_LOCATION2_STR "\33cEnter your location. The contents\nare not normalized or geocoded in any way\n(Max 30 characters)"
#define MSG_BIO_STR "Bio:"
#define MSG_BIO2_STR "\33cEnter something about yourself or your description\n(Max 160 characters)"
#define MSG_UPDATEPROFILE3_STR "_Update Profile"
#define MSG_CANCEL9_STR "_Cancel"
#define MSG_WHAT_STR "What's Happening?"
#define MSG_FAST_STR "Fast Links"
#define MSG_FEEDBACK_STR "Feedback"
#define MSG_DONATE4_STR "Donate!"
#define MSG_SAVE2_STR "Save Settings"
#define MSG_TEST3_STR "Test User Name / Password"
#define MSG_DONTSAVE_STR "Don't Save Current Changes"
#define MSG_CLEAR8_STR "Clear Tweet"
#define MSG_SEND6_STR "Send Tweet"
#define MSG_CANCEL10_STR "Cancel Tweet"
#define MSG_CLEAR9_STR "Clear Search"
#define MSG_SEARCH8_STR "Search"
#define MSG_CANCEL11_STR "Cancel Search"
#define MSG_CLEAR10_STR "Clear User Name"
#define MSG_FOLLOWUSER_STR "Follow a User"
#define MSG_UNFOLLOWUSER_STR "Unfollow a User"
#define MSG_CANCEL12_STR "Cancel Request"
#define MSG_CLEAR12_STR "Clear User Name"
#define MSG_BLOCKUSER_STR "Block a User (Use with care!)"
#define MSG_UNBLOCKUSER_STR "Unblock a currently blocked User"
#define MSG_CANCEL13_STR "Cancel Request"
#define MSG_CLEAR13_STR "Clear User Name"
#define MSG_SMSNOTE_STR "\33cPlease Note: You must first set up your cell phone\non the Twitter website (under 'Settings/Mobile')\nif you have not done so already, to begin receiving\n SMS notifications with AmiTwitter..."
#define MSG_SMSSTOP_STR "Stop receiving SMS notifications from this user"
#define MSG_CANCEL14_STR "Cancel Request"
#define MSG_CLEAR14_STR "Clear User Name"
#define MSG_SHOWTWEETS_STR "Show latest Tweet for this User"
#define MSG_CANCEL15_STR "Cancel Request"
#define MSG_CLEAR15_STR "Clear Direct Message"
#define MSG_SENDDIRMSG3_STR "Send Direct Message"
#define MSG_CANCEL16_STR "Cancel Direct Message"
#define MSG_UPDATEPROFILE4_STR "Update Your Profile!"
#define MSG_CANCEL17_STR "Cancel Update"
#define MSG_SEARCH9_STR "Search"
#define MSG_OK_STR "*OK"
#define MSG_SEARCH10_STR "Search not implemented yet..."
#define MSG_BLOCK2_STR "Block?"
#define MSG_BLOCK3_STR "_Block|_Cancel"
#define MSG_BLOCK4_STR "\33cAre you sure you want to block this User?\n\nIf you block someone, they wont be able to follow\nyou or send you any messages. If your account is\npublic, they'll still be able to view it, but they\nwont show up on your followers list, and you wont be\non their following list.\n\nIf it's a spammer you're blocking, then thanks!"
#define MSG_QUIT_STR "Quit"
#define MSG_QUIT2_STR "_Quit|_Cancel"
#define MSG_QUIT3_STR "\33cAre you sure you want\nto quit AmiTwitter?"
#define MSG_SAVE3_STR "Save"
#define MSG_OK2_STR "*OK"
#define MSG_SAVED_STR "Settings Saved to Envarc!"
#define MSG_UPDATEPROFILE5_STR "Update Profile"
#define MSG_OK3_STR "*OK"
#define MSG_UPDATEPROFILE6_STR "\33cPlease Note:\n\n Currently, You must *ALWAYS* specify a 'Name',\nthe other fields are optional, but if left blank they will\noverwrite the profile that is currently on your\nTwitter site.  (i.e., you should fill in all the\ninformation if you want it displayed on your Twitter\nsite! If you don't want it displayed, leave it blank\n(except for 'Name' of course)...\n\nI hope to make this a bit more user friendly in the future!\nPlease see the bubble help for more info for each field!"
#define MSG_HTML1_STR "<TITLE>Welcome to AmiTwitter</TITLE></HEAD>"
#define MSG_HTML2_STR "<CENTER>Enter your User Name/Password in the Tools -> Settings dialog window to get started... </CENTER><BR>"
#define MSG_HTML3_STR "<CENTER>This program is released under the GNU General Public License</CENTER><BR>"
#define MSG_HTML4_STR "<CENTER>No warranty is expressed or implied. Use at your own risk!</CENTER>"
#define MSG_HTML5_STR "<CENTER>The latest source code is always available at:</CENTER><BR>"
#define MSG_HTML6_STR "<CENTER>https://sourceforge.net/projects/amitwitter/</CENTER><BR>"
#define MSG_HTML7_STR "<CENTER>-- Thanks for your interest, The AmiTwitter Open Source Team</CENTER><BR>"
#define MSG_HTML8_STR "<CENTER>This program uses:</CENTER><BR>"
#define MSG_HTML9_STR "<TITLE>Welcome to AmiTwitter</TITLE></HEAD>"
#define MSG_HTML10_STR "<CENTER>What does AmiTwitter stand for?</CENTER><BR>"
#define MSG_HTML11_STR "<CENTER>Answer: <b>A</b>miga-<b>M</b>orphOS-<b>I</b>KE-Twitter </CENTER><BR>"
#define MSG_HTML12_STR "<CENTER>What are the requirements for AmiTwitter?</CENTER><BR>"
#define MSG_HTML13_STR "<CENTER>Answer: MUI, HTMLtext, URLtext, TheBar, BetterString, libiconv.ixlibrary.  OpenURL is optional. </CENTER><BR>"
#define MSG_HTML14_STR "<CENTER>AmiTwitter does not download or send Tweets?</CENTER><BR>"
#define MSG_HTML15_STR "<CENTER>Answer:  Are you connected to the internet, have you entered your Twitter User Name/Password in the Tools -> Settings window and saved them? Are you already following some people for <I>Timeline</I> to work) and have you sent some Tweets (for <I>My Tweets</I> to work)?</CENTER><BR>"
#define MSG_HTML16_STR "<CENTER>Direct Messages don't seem to work?</CENTER><BR>"
#define MSG_HTML17_STR "<CENTER>Answer: Ensure you enter the recipient's Screen Name (i.e. User Name) correctly, it does not currently accept user id numbers.</CENTER><BR>"
#define MSG_HTML18_STR "<CENTER>I like AmiTwitter, how do I donate?</CENTER><BR>"
#define MSG_HTML19_STR "<CENTER>Answer: Follow the 'Donate!' link to the PayPal website that opens in your favorite web browser</CENTER><BR>"
#define MSG_HTML20_STR "<CENTER>Nothing happens when I click on the links at the very bottom of the main program window?</CENTER><BR>"
#define MSG_HTML21_STR "<CENTER>Answer: Ensure OpenURL is properly installed/configured.</CENTER><BR>"
#define MSG_HTML22_STR "<CENTER>All I see is the message, 'Attempting to get the latest...'?</CENTER><BR>"
#define MSG_HTML23_STR "<CENTER>Answer: Most probable reason is you don't have anything in that particular timeline.  For instance, you have never 'Favorited' any items on the Twitter site, and therefore they won't show up under 'My Favorites' in AmiTwitter either.</CENTER><BR>"
#define MSG_HTML24_STR "<CENTER>Why is the file size so large?</CENTER><BR>"
#define MSG_HTML25_STR "<CENTER>Answer: Because AmiTwitter relies on numerous libs to work, namely: cURL, SSL, xml2, glib, etc....</CENTER><BR>"
#define MSG_HTML26_STR "<CENTER>Why did you write AmiTwitter?</CENTER><BR>"
#define MSG_HTML27_STR "<CENTER>Answer: To prove it could be done!</CENTER><BR>"
#define MSG_HTML28_STR "<CENTER>What is AmiTwitter based on?</CENTER><BR>"
#define MSG_HTML29_STR "<CENTER>Answer: xTwitter by Tsukasa Hamano </CENTER><BR>"
#define MSG_HTML30_STR "<CENTER>Links in the main Tweets window don't work?</CENTER><BR>"
#define MSG_HTML31_STR "<CENTER>Answer: This is not a webbrowser...Also...it is a limitation of HTMLtext.mcc currently being used (The plan is to switch to HTMLview.mcc in the future...)</CENTER><BR>"
#define MSG_HTML32_STR "<CENTER>What are the  future plans for AmiTwitter?</CENTER><BR>"
#define MSG_HTML33_STR "<CENTER>Answer: First, learn more of the Twitter API, but it depends a lot on the feedback I recieve.  Also, ports to other systems (i.e OS 4.x and AROS) and an active source code base with some developers joining the project at SourceForge would be nice...<BR>"
#define MSG_HTML34_STR "<CENTER>I don't know how to code, what can I do?</CENTER><BR>"
#define MSG_HTML35_STR "<CENTER>Consider donating.<BR>"
#define MSG_0000_STR "File"
#define MSG_0001_STR "Timeline"
#define MSG_0002_STR "Retweets"
#define MSG_0003_STR "Retweet By Me"
#define MSG_0004_STR "Retweet To Me"
#define MSG_0005_STR "Retweet Of Me"
#define MSG_0006_STR "@Replies"
#define MSG_0007_STR "Reload"
#define MSG_0008_STR "Search"
#define MSG_0009_STR "Find Users"
#define MSG_0010_STR "Users"
#define MSG_0011_STR "Direct Message"
#define MSG_0012_STR "Sent"
#define MSG_0013_STR "Received"
#define MSG_0014_STR "Tweet"
#define MSG_0015_STR "My Tweets"
#define MSG_0016_STR "My Favorites"
#define MSG_0017_STR "Quit"
#define MSG_0018_STR "Misc"
#define MSG_0019_STR "My Friends"
#define MSG_0020_STR "My Followers"
#define MSG_0021_STR "Blocking"
#define MSG_0022_STR "Random"
#define MSG_0023_STR "Tools"
#define MSG_0024_STR "Settings"
#define MSG_0025_STR "Update Profile"
#define MSG_0026_STR "MUI Settings"
#define MSG_0027_STR "Help"
#define MSG_0028_STR "FAQs"
#define MSG_0029_STR "Donate"
#define MSG_0030_STR "About AmiTwitter"
#define MSG_0031_STR "About MUI"

#endif /* CATCOMP_STRINGS */

/****************************************************************************/

#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG   cca_ID;
    STRPTR cca_Str;
};

static const struct CatCompArrayType CatCompArray[] =
{
    {MSG_FOLLOW,(STRPTR)MSG_FOLLOW_STR},
    {MSG_BLOCK,(STRPTR)MSG_BLOCK_STR},
    {MSG_NOTIFY,(STRPTR)MSG_NOTIFY_STR},
    {MSG_SHOWUSER,(STRPTR)MSG_SHOWUSER_STR},
    {MSG_TIMELINE,(STRPTR)MSG_TIMELINE_STR},
    {MSG_TIMELINE2,(STRPTR)MSG_TIMELINE2_STR},
    {MSG_RETWEETS,(STRPTR)MSG_RETWEETS_STR},
    {MSG_RETWEETS2,(STRPTR)MSG_RETWEETS2_STR},
    {MSG_REPLIES,(STRPTR)MSG_REPLIES_STR},
    {MSG_REPLIES2,(STRPTR)MSG_REPLIES2_STR},
    {MSG_RELOAD,(STRPTR)MSG_RELOAD_STR},
    {MSG_RELOAD2,(STRPTR)MSG_RELOAD2_STR},
    {MSG_SEARCH,(STRPTR)MSG_SEARCH_STR},
    {MSG_SEARCH2,(STRPTR)MSG_SEARCH2_STR},
    {MSG_USERS,(STRPTR)MSG_USERS_STR},
    {MSG_USERS2,(STRPTR)MSG_USERS2_STR},
    {MSG_DIRMSG,(STRPTR)MSG_DIRMSG_STR},
    {MSG_DIRMSG2,(STRPTR)MSG_DIRMSG2_STR},
    {MSG_TWEET,(STRPTR)MSG_TWEET_STR},
    {MSG_TWEET2,(STRPTR)MSG_TWEET2_STR},
    {MSG_ERROR,(STRPTR)MSG_ERROR_STR},
    {MSG_ERROR2,(STRPTR)MSG_ERROR2_STR},
    {MSG_ERROR3,(STRPTR)MSG_ERROR3_STR},
    {MSG_ERROR4,(STRPTR)MSG_ERROR4_STR},
    {MSG_ERROR5,(STRPTR)MSG_ERROR5_STR},
    {MSG_ERROR6,(STRPTR)MSG_ERROR6_STR},
    {MSG_ERROR7,(STRPTR)MSG_ERROR7_STR},
    {MSG_ERROR8,(STRPTR)MSG_ERROR8_STR},
    {MSG_ATTEMPTING,(STRPTR)MSG_ATTEMPTING_STR},
    {MSG_STATS,(STRPTR)MSG_STATS_STR},
    {MSG_STATS2,(STRPTR)MSG_STATS2_STR},
    {MSG_SENT,(STRPTR)MSG_SENT_STR},
    {MSG_DIRMSGSENT,(STRPTR)MSG_DIRMSGSENT_STR},
    {MSG_FOLLOWING,(STRPTR)MSG_FOLLOWING_STR},
    {MSG_UNFOLLOWING,(STRPTR)MSG_UNFOLLOWING_STR},
    {MSG_VALID,(STRPTR)MSG_VALID_STR},
    {MSG_UPDATED,(STRPTR)MSG_UPDATED_STR},
    {MSG_SMSFOLLOW,(STRPTR)MSG_SMSFOLLOW_STR},
    {MSG_SMSUNFOLLOW,(STRPTR)MSG_SMSUNFOLLOW_STR},
    {MSG_BLOCKING,(STRPTR)MSG_BLOCKING_STR},
    {MSG_UNBLOCKING,(STRPTR)MSG_UNBLOCKING_STR},
    {MSG_SETTINGS,(STRPTR)MSG_SETTINGS_STR},
    {MSG_SETTINGS2,(STRPTR)MSG_SETTINGS2_STR},
    {MSG_USERNAME,(STRPTR)MSG_USERNAME_STR},
    {MSG_PASSWORD,(STRPTR)MSG_PASSWORD_STR},
    {MSG_SAVE,(STRPTR)MSG_SAVE_STR},
    {MSG_TEST2,(STRPTR)MSG_TEST2_STR},
    {MSG_CANCEL,(STRPTR)MSG_CANCEL_STR},
    {MSG_HELP,(STRPTR)MSG_HELP_STR},
    {MSG_REGISTER,(STRPTR)MSG_REGISTER_STR},
    {MSG_ESTABLISH,(STRPTR)MSG_ESTABLISH_STR},
    {MSG_FORGOT,(STRPTR)MSG_FORGOT_STR},
    {MSG_DONATE,(STRPTR)MSG_DONATE_STR},
    {MSG_DONATE2,(STRPTR)MSG_DONATE2_STR},
    {MSG_DONATE3,(STRPTR)MSG_DONATE3_STR},
    {MSG_SEND,(STRPTR)MSG_SEND_STR},
    {MSG_SEND2,(STRPTR)MSG_SEND2_STR},
    {MSG_SEND3,(STRPTR)MSG_SEND3_STR},
    {MSG_SEND4,(STRPTR)MSG_SEND4_STR},
    {MSG_CLEAR,(STRPTR)MSG_CLEAR_STR},
    {MSG_UPDATE,(STRPTR)MSG_UPDATE_STR},
    {MSG_CANCEL2,(STRPTR)MSG_CANCEL2_STR},
    {MSG_SEARCH3,(STRPTR)MSG_SEARCH3_STR},
    {MSG_SEARCH4,(STRPTR)MSG_SEARCH4_STR},
    {MSG_SEARCH5,(STRPTR)MSG_SEARCH5_STR},
    {MSG_SEARCH6,(STRPTR)MSG_SEARCH6_STR},
    {MSG_CLEAR2,(STRPTR)MSG_CLEAR2_STR},
    {MSG_SEARCH7,(STRPTR)MSG_SEARCH7_STR},
    {MSG_CANCEL3,(STRPTR)MSG_CANCEL3_STR},
    {MSG_MANAGE,(STRPTR)MSG_MANAGE_STR},
    {MSG_FOLLOW2,(STRPTR)MSG_FOLLOW2_STR},
    {MSG_FOLLOW3,(STRPTR)MSG_FOLLOW3_STR},
    {MSG_FOLLOW4,(STRPTR)MSG_FOLLOW4_STR},
    {MSG_CLEAR3,(STRPTR)MSG_CLEAR3_STR},
    {MSG_FOLLOW5,(STRPTR)MSG_FOLLOW5_STR},
    {MSG_UNFOLLOOW,(STRPTR)MSG_UNFOLLOOW_STR},
    {MSG_CANCEL4,(STRPTR)MSG_CANCEL4_STR},
    {MSG_BLOCKING2,(STRPTR)MSG_BLOCKING2_STR},
    {MSG_BLOCKING3,(STRPTR)MSG_BLOCKING3_STR},
    {MSG_BLOCKING4,(STRPTR)MSG_BLOCKING4_STR},
    {MSG_CLEAR4,(STRPTR)MSG_CLEAR4_STR},
    {MSG_BLOCKING5,(STRPTR)MSG_BLOCKING5_STR},
    {MSG_UNBLOCK,(STRPTR)MSG_UNBLOCK_STR},
    {MSG_CANCEL5,(STRPTR)MSG_CANCEL5_STR},
    {MSG_SMS,(STRPTR)MSG_SMS_STR},
    {MSG_SMS2,(STRPTR)MSG_SMS2_STR},
    {MSG_SMS3,(STRPTR)MSG_SMS3_STR},
    {MSG_CLEAR5,(STRPTR)MSG_CLEAR5_STR},
    {MSG_RECEIVE,(STRPTR)MSG_RECEIVE_STR},
    {MSG_STOP,(STRPTR)MSG_STOP_STR},
    {MSG_CANCEL6,(STRPTR)MSG_CANCEL6_STR},
    {MSG_SHOWUSER2,(STRPTR)MSG_SHOWUSER2_STR},
    {MSG_SHOWUSER3,(STRPTR)MSG_SHOWUSER3_STR},
    {MSG_SHOWUSER4,(STRPTR)MSG_SHOWUSER4_STR},
    {MSG_CLEAR6,(STRPTR)MSG_CLEAR6_STR},
    {MSG_SHOWUSER5,(STRPTR)MSG_SHOWUSER5_STR},
    {MSG_CANCEL7,(STRPTR)MSG_CANCEL7_STR},
    {MSG_SENDDIRMSG,(STRPTR)MSG_SENDDIRMSG_STR},
    {MSG_SENDDIRMSG2,(STRPTR)MSG_SENDDIRMSG2_STR},
    {MSG_RECIPIENT,(STRPTR)MSG_RECIPIENT_STR},
    {MSG_SCREENNAME,(STRPTR)MSG_SCREENNAME_STR},
    {MSG_MESSAGE,(STRPTR)MSG_MESSAGE_STR},
    {MSG_MESSAGE2,(STRPTR)MSG_MESSAGE2_STR},
    {MSG_CLEAR7,(STRPTR)MSG_CLEAR7_STR},
    {MSG_SEND5,(STRPTR)MSG_SEND5_STR},
    {MSG_CANCEL8,(STRPTR)MSG_CANCEL8_STR},
    {MSG_UPDATEPROFILE,(STRPTR)MSG_UPDATEPROFILE_STR},
    {MSG_UPDATEPROFILE2,(STRPTR)MSG_UPDATEPROFILE2_STR},
    {MSG_PROFNAME,(STRPTR)MSG_PROFNAME_STR},
    {MSG_PROFNAME2,(STRPTR)MSG_PROFNAME2_STR},
    {MSG_WEB,(STRPTR)MSG_WEB_STR},
    {MSG_WEB2,(STRPTR)MSG_WEB2_STR},
    {MSG_LOCATION,(STRPTR)MSG_LOCATION_STR},
    {MSG_LOCATION2,(STRPTR)MSG_LOCATION2_STR},
    {MSG_BIO,(STRPTR)MSG_BIO_STR},
    {MSG_BIO2,(STRPTR)MSG_BIO2_STR},
    {MSG_UPDATEPROFILE3,(STRPTR)MSG_UPDATEPROFILE3_STR},
    {MSG_CANCEL9,(STRPTR)MSG_CANCEL9_STR},
    {MSG_WHAT,(STRPTR)MSG_WHAT_STR},
    {MSG_FAST,(STRPTR)MSG_FAST_STR},
    {MSG_FEEDBACK,(STRPTR)MSG_FEEDBACK_STR},
    {MSG_DONATE4,(STRPTR)MSG_DONATE4_STR},
    {MSG_SAVE2,(STRPTR)MSG_SAVE2_STR},
    {MSG_TEST3,(STRPTR)MSG_TEST3_STR},
    {MSG_DONTSAVE,(STRPTR)MSG_DONTSAVE_STR},
    {MSG_CLEAR8,(STRPTR)MSG_CLEAR8_STR},
    {MSG_SEND6,(STRPTR)MSG_SEND6_STR},
    {MSG_CANCEL10,(STRPTR)MSG_CANCEL10_STR},
    {MSG_CLEAR9,(STRPTR)MSG_CLEAR9_STR},
    {MSG_SEARCH8,(STRPTR)MSG_SEARCH8_STR},
    {MSG_CANCEL11,(STRPTR)MSG_CANCEL11_STR},
    {MSG_CLEAR10,(STRPTR)MSG_CLEAR10_STR},
    {MSG_FOLLOWUSER,(STRPTR)MSG_FOLLOWUSER_STR},
    {MSG_UNFOLLOWUSER,(STRPTR)MSG_UNFOLLOWUSER_STR},
    {MSG_CANCEL12,(STRPTR)MSG_CANCEL12_STR},
    {MSG_CLEAR12,(STRPTR)MSG_CLEAR12_STR},
    {MSG_BLOCKUSER,(STRPTR)MSG_BLOCKUSER_STR},
    {MSG_UNBLOCKUSER,(STRPTR)MSG_UNBLOCKUSER_STR},
    {MSG_CANCEL13,(STRPTR)MSG_CANCEL13_STR},
    {MSG_CLEAR13,(STRPTR)MSG_CLEAR13_STR},
    {MSG_SMSNOTE,(STRPTR)MSG_SMSNOTE_STR},
    {MSG_SMSSTOP,(STRPTR)MSG_SMSSTOP_STR},
    {MSG_CANCEL14,(STRPTR)MSG_CANCEL14_STR},
    {MSG_CLEAR14,(STRPTR)MSG_CLEAR14_STR},
    {MSG_SHOWTWEETS,(STRPTR)MSG_SHOWTWEETS_STR},
    {MSG_CANCEL15,(STRPTR)MSG_CANCEL15_STR},
    {MSG_CLEAR15,(STRPTR)MSG_CLEAR15_STR},
    {MSG_SENDDIRMSG3,(STRPTR)MSG_SENDDIRMSG3_STR},
    {MSG_CANCEL16,(STRPTR)MSG_CANCEL16_STR},
    {MSG_UPDATEPROFILE4,(STRPTR)MSG_UPDATEPROFILE4_STR},
    {MSG_CANCEL17,(STRPTR)MSG_CANCEL17_STR},
    {MSG_SEARCH9,(STRPTR)MSG_SEARCH9_STR},
    {MSG_OK,(STRPTR)MSG_OK_STR},
    {MSG_SEARCH10,(STRPTR)MSG_SEARCH10_STR},
    {MSG_BLOCK2,(STRPTR)MSG_BLOCK2_STR},
    {MSG_BLOCK3,(STRPTR)MSG_BLOCK3_STR},
    {MSG_BLOCK4,(STRPTR)MSG_BLOCK4_STR},
    {MSG_QUIT,(STRPTR)MSG_QUIT_STR},
    {MSG_QUIT2,(STRPTR)MSG_QUIT2_STR},
    {MSG_QUIT3,(STRPTR)MSG_QUIT3_STR},
    {MSG_SAVE3,(STRPTR)MSG_SAVE3_STR},
    {MSG_OK2,(STRPTR)MSG_OK2_STR},
    {MSG_SAVED,(STRPTR)MSG_SAVED_STR},
    {MSG_UPDATEPROFILE5,(STRPTR)MSG_UPDATEPROFILE5_STR},
    {MSG_OK3,(STRPTR)MSG_OK3_STR},
    {MSG_UPDATEPROFILE6,(STRPTR)MSG_UPDATEPROFILE6_STR},
    {MSG_HTML1,(STRPTR)MSG_HTML1_STR},
    {MSG_HTML2,(STRPTR)MSG_HTML2_STR},
    {MSG_HTML3,(STRPTR)MSG_HTML3_STR},
    {MSG_HTML4,(STRPTR)MSG_HTML4_STR},
    {MSG_HTML5,(STRPTR)MSG_HTML5_STR},
    {MSG_HTML6,(STRPTR)MSG_HTML6_STR},
    {MSG_HTML7,(STRPTR)MSG_HTML7_STR},
    {MSG_HTML8,(STRPTR)MSG_HTML8_STR},
    {MSG_HTML9,(STRPTR)MSG_HTML9_STR},
    {MSG_HTML10,(STRPTR)MSG_HTML10_STR},
    {MSG_HTML11,(STRPTR)MSG_HTML11_STR},
    {MSG_HTML12,(STRPTR)MSG_HTML12_STR},
    {MSG_HTML13,(STRPTR)MSG_HTML13_STR},
    {MSG_HTML14,(STRPTR)MSG_HTML14_STR},
    {MSG_HTML15,(STRPTR)MSG_HTML15_STR},
    {MSG_HTML16,(STRPTR)MSG_HTML16_STR},
    {MSG_HTML17,(STRPTR)MSG_HTML17_STR},
    {MSG_HTML18,(STRPTR)MSG_HTML18_STR},
    {MSG_HTML19,(STRPTR)MSG_HTML19_STR},
    {MSG_HTML20,(STRPTR)MSG_HTML20_STR},
    {MSG_HTML21,(STRPTR)MSG_HTML21_STR},
    {MSG_HTML22,(STRPTR)MSG_HTML22_STR},
    {MSG_HTML23,(STRPTR)MSG_HTML23_STR},
    {MSG_HTML24,(STRPTR)MSG_HTML24_STR},
    {MSG_HTML25,(STRPTR)MSG_HTML25_STR},
    {MSG_HTML26,(STRPTR)MSG_HTML26_STR},
    {MSG_HTML27,(STRPTR)MSG_HTML27_STR},
    {MSG_HTML28,(STRPTR)MSG_HTML28_STR},
    {MSG_HTML29,(STRPTR)MSG_HTML29_STR},
    {MSG_HTML30,(STRPTR)MSG_HTML30_STR},
    {MSG_HTML31,(STRPTR)MSG_HTML31_STR},
    {MSG_HTML32,(STRPTR)MSG_HTML32_STR},
    {MSG_HTML33,(STRPTR)MSG_HTML33_STR},
    {MSG_HTML34,(STRPTR)MSG_HTML34_STR},
    {MSG_HTML35,(STRPTR)MSG_HTML35_STR},
    {MSG_0000,(STRPTR)MSG_0000_STR},
    {MSG_0001,(STRPTR)MSG_0001_STR},
    {MSG_0002,(STRPTR)MSG_0002_STR},
    {MSG_0003,(STRPTR)MSG_0003_STR},
    {MSG_0004,(STRPTR)MSG_0004_STR},
    {MSG_0005,(STRPTR)MSG_0005_STR},
    {MSG_0006,(STRPTR)MSG_0006_STR},
    {MSG_0007,(STRPTR)MSG_0007_STR},
    {MSG_0008,(STRPTR)MSG_0008_STR},
    {MSG_0009,(STRPTR)MSG_0009_STR},
    {MSG_0010,(STRPTR)MSG_0010_STR},
    {MSG_0011,(STRPTR)MSG_0011_STR},
    {MSG_0012,(STRPTR)MSG_0012_STR},
    {MSG_0013,(STRPTR)MSG_0013_STR},
    {MSG_0014,(STRPTR)MSG_0014_STR},
    {MSG_0015,(STRPTR)MSG_0015_STR},
    {MSG_0016,(STRPTR)MSG_0016_STR},
    {MSG_0017,(STRPTR)MSG_0017_STR},
    {MSG_0018,(STRPTR)MSG_0018_STR},
    {MSG_0019,(STRPTR)MSG_0019_STR},
    {MSG_0020,(STRPTR)MSG_0020_STR},
    {MSG_0021,(STRPTR)MSG_0021_STR},
    {MSG_0022,(STRPTR)MSG_0022_STR},
    {MSG_0023,(STRPTR)MSG_0023_STR},
    {MSG_0024,(STRPTR)MSG_0024_STR},
    {MSG_0025,(STRPTR)MSG_0025_STR},
    {MSG_0026,(STRPTR)MSG_0026_STR},
    {MSG_0027,(STRPTR)MSG_0027_STR},
    {MSG_0028,(STRPTR)MSG_0028_STR},
    {MSG_0029,(STRPTR)MSG_0029_STR},
    {MSG_0030,(STRPTR)MSG_0030_STR},
    {MSG_0031,(STRPTR)MSG_0031_STR},
};

#endif /* CATCOMP_ARRAY */

/****************************************************************************/

#ifdef CATCOMP_BLOCK

static const char CatCompBlock[] =
{
    "\x00\x00\x00\x00\x00\x10"
    MSG_FOLLOW_STR "\x00"
    "\x00\x00\x00\x01\x00\x0E"
    MSG_BLOCK_STR "\x00"
    "\x00\x00\x00\x02\x00\x10"
    MSG_NOTIFY_STR "\x00"
    "\x00\x00\x00\x03\x00\x0A"
    MSG_SHOWUSER_STR "\x00"
    "\x00\x00\x00\x04\x00\x0A"
    MSG_TIMELINE_STR "\x00"
    "\x00\x00\x00\x05\x00\x20"
    MSG_TIMELINE2_STR "\x00"
    "\x00\x00\x00\x06\x00\x0A"
    MSG_RETWEETS_STR "\x00"
    "\x00\x00\x00\x07\x00\x28"
    MSG_RETWEETS2_STR "\x00"
    "\x00\x00\x00\x08\x00\x0A"
    MSG_REPLIES_STR "\x00"
    "\x00\x00\x00\x09\x00\x22"
    MSG_REPLIES2_STR "\x00"
    "\x00\x00\x00\x0A\x00\x08"
    MSG_RELOAD_STR "\x00"
    "\x00\x00\x00\x0B\x00\x1A"
    MSG_RELOAD2_STR "\x00"
    "\x00\x00\x00\x0C\x00\x08"
    MSG_SEARCH_STR "\x00"
    "\x00\x00\x00\x0D\x00\x08"
    MSG_SEARCH2_STR "\x00\x00"
    "\x00\x00\x00\x0E\x00\x08"
    MSG_USERS_STR "\x00\x00"
    "\x00\x00\x00\x0F\x00\x0E"
    MSG_USERS2_STR "\x00\x00"
    "\x00\x00\x00\x10\x00\x0A"
    MSG_DIRMSG_STR "\x00\x00"
    "\x00\x00\x00\x11\x00\x16"
    MSG_DIRMSG2_STR "\x00"
    "\x00\x00\x00\x12\x00\x08"
    MSG_TWEET_STR "\x00\x00"
    "\x00\x00\x00\x13\x00\x0E"
    MSG_TWEET2_STR "\x00\x00"
    "\x00\x00\x00\x14\x00\x96"
    MSG_ERROR_STR "\x00\x00"
    "\x00\x00\x00\x15\x00\xEC"
    MSG_ERROR2_STR "\x00"
    "\x00\x00\x00\x16\x00\x4C"
    MSG_ERROR3_STR "\x00\x00"
    "\x00\x00\x00\x17\x01\x04"
    MSG_ERROR4_STR "\x00"
    "\x00\x00\x00\x18\x01\x04"
    MSG_ERROR5_STR "\x00"
    "\x00\x00\x00\x19\x01\x20"
    MSG_ERROR6_STR "\x00"
    "\x00\x00\x00\x1A\x00\xD6"
    MSG_ERROR7_STR "\x00\x00"
    "\x00\x00\x00\x1B\x00\xC6"
    MSG_ERROR8_STR "\x00"
    "\x00\x00\x00\x1C\x00\x20"
    MSG_ATTEMPTING_STR "\x00"
    "\x00\x00\x00\x1D\x00\x4C"
    MSG_STATS_STR "\x00"
    "\x00\x00\x00\x1E\x00\x5C"
    MSG_STATS2_STR "\x00\x00"
    "\x00\x00\x00\x1F\x00\x16"
    MSG_SENT_STR "\x00\x00"
    "\x00\x00\x00\x20\x00\x1E"
    MSG_DIRMSGSENT_STR "\x00"
    "\x00\x00\x00\x21\x00\x28"
    MSG_FOLLOWING_STR "\x00"
    "\x00\x00\x00\x22\x00\x2A"
    MSG_UNFOLLOWING_STR "\x00"
    "\x00\x00\x00\x23\x00\x3A"
    MSG_VALID_STR "\x00\x00"
    "\x00\x00\x00\x24\x00\x38"
    MSG_UPDATED_STR "\x00\x00"
    "\x00\x00\x00\x25\x00\x40"
    MSG_SMSFOLLOW_STR "\x00\x00"
    "\x00\x00\x00\x26\x00\x46"
    MSG_SMSUNFOLLOW_STR "\x00\x00"
    "\x00\x00\x00\x27\x00\x28"
    MSG_BLOCKING_STR "\x00\x00"
    "\x00\x00\x00\x28\x00\x2A"
    MSG_UNBLOCKING_STR "\x00\x00"
    "\x00\x00\x00\x29\x00\x0A"
    MSG_SETTINGS_STR "\x00\x00"
    "\x00\x00\x00\x2A\x00\x1E"
    MSG_SETTINGS2_STR "\x00\x00"
    "\x00\x00\x00\x2B\x00\x0C"
    MSG_USERNAME_STR "\x00\x00"
    "\x00\x00\x00\x2C\x00\x0A"
    MSG_PASSWORD_STR "\x00"
    "\x00\x00\x00\x2D\x00\x06"
    MSG_SAVE_STR "\x00"
    "\x00\x00\x00\x2E\x00\x06"
    MSG_TEST2_STR "\x00"
    "\x00\x00\x00\x2F\x00\x08"
    MSG_CANCEL_STR "\x00"
    "\x00\x00\x00\x30\x00\x06"
    MSG_HELP_STR "\x00"
    "\x00\x00\x00\x31\x00\x60"
    MSG_REGISTER_STR "\x00\x00"
    "\x00\x00\x00\x32\x00\x16"
    MSG_ESTABLISH_STR "\x00"
    "\x00\x00\x00\x33\x00\x16"
    MSG_FORGOT_STR "\x00"
    "\x00\x00\x00\x34\x00\x08"
    MSG_DONATE_STR "\x00\x00"
    "\x00\x00\x00\x35\x00\x6C"
    MSG_DONATE2_STR "\x00"
    "\x00\x00\x00\x36\x00\x08"
    MSG_DONATE3_STR "\x00"
    "\x00\x00\x00\x37\x00\x0E"
    MSG_SEND_STR "\x00\x00"
    "\x00\x00\x00\x38\x00\x0E"
    MSG_SEND2_STR "\x00\x00"
    "\x00\x00\x00\x39\x00\x08"
    MSG_SEND3_STR "\x00\x00"
    "\x00\x00\x00\x3A\x00\x40"
    MSG_SEND4_STR "\x00\x00"
    "\x00\x00\x00\x3B\x00\x08"
    MSG_CLEAR_STR "\x00\x00"
    "\x00\x00\x00\x3C\x00\x08"
    MSG_UPDATE_STR "\x00"
    "\x00\x00\x00\x3D\x00\x08"
    MSG_CANCEL2_STR "\x00"
    "\x00\x00\x00\x3E\x00\x08"
    MSG_SEARCH3_STR "\x00\x00"
    "\x00\x00\x00\x3F\x00\x08"
    MSG_SEARCH4_STR "\x00\x00"
    "\x00\x00\x00\x40\x00\x08"
    MSG_SEARCH5_STR "\x00"
    "\x00\x00\x00\x41\x00\x40"
    MSG_SEARCH6_STR "\x00\x00"
    "\x00\x00\x00\x42\x00\x08"
    MSG_CLEAR2_STR "\x00\x00"
    "\x00\x00\x00\x43\x00\x08"
    MSG_SEARCH7_STR "\x00"
    "\x00\x00\x00\x44\x00\x08"
    MSG_CANCEL3_STR "\x00"
    "\x00\x00\x00\x45\x00\x0E"
    MSG_MANAGE_STR "\x00\x00"
    "\x00\x00\x00\x46\x00\x28"
    MSG_FOLLOW2_STR "\x00\x00"
    "\x00\x00\x00\x47\x00\x06"
    MSG_FOLLOW3_STR "\x00"
    "\x00\x00\x00\x48\x00\x38"
    MSG_FOLLOW4_STR "\x00"
    "\x00\x00\x00\x49\x00\x08"
    MSG_CLEAR3_STR "\x00\x00"
    "\x00\x00\x00\x4A\x00\x08"
    MSG_FOLLOW5_STR "\x00"
    "\x00\x00\x00\x4B\x00\x0A"
    MSG_UNFOLLOOW_STR "\x00"
    "\x00\x00\x00\x4C\x00\x08"
    MSG_CANCEL4_STR "\x00"
    "\x00\x00\x00\x4D\x00\x26"
    MSG_BLOCKING2_STR "\x00"
    "\x00\x00\x00\x4E\x00\x06"
    MSG_BLOCKING3_STR "\x00"
    "\x00\x00\x00\x4F\x00\x36"
    MSG_BLOCKING4_STR "\x00"
    "\x00\x00\x00\x50\x00\x08"
    MSG_CLEAR4_STR "\x00\x00"
    "\x00\x00\x00\x51\x00\x08"
    MSG_BLOCKING5_STR "\x00\x00"
    "\x00\x00\x00\x52\x00\x0A"
    MSG_UNBLOCK_STR "\x00\x00"
    "\x00\x00\x00\x53\x00\x08"
    MSG_CANCEL5_STR "\x00"
    "\x00\x00\x00\x54\x00\x2E"
    MSG_SMS_STR "\x00"
    "\x00\x00\x00\x55\x00\x06"
    MSG_SMS2_STR "\x00"
    "\x00\x00\x00\x56\x00\x5E"
    MSG_SMS3_STR "\x00\x00"
    "\x00\x00\x00\x57\x00\x08"
    MSG_CLEAR5_STR "\x00\x00"
    "\x00\x00\x00\x58\x00\x0A"
    MSG_RECEIVE_STR "\x00\x00"
    "\x00\x00\x00\x59\x00\x06"
    MSG_STOP_STR "\x00"
    "\x00\x00\x00\x5A\x00\x08"
    MSG_CANCEL6_STR "\x00"
    "\x00\x00\x00\x5B\x00\x0C"
    MSG_SHOWUSER2_STR "\x00"
    "\x00\x00\x00\x5C\x00\x06"
    MSG_SHOWUSER3_STR "\x00"
    "\x00\x00\x00\x5D\x00\x80"
    MSG_SHOWUSER4_STR "\x00"
    "\x00\x00\x00\x5E\x00\x08"
    MSG_CLEAR6_STR "\x00\x00"
    "\x00\x00\x00\x5F\x00\x0C"
    MSG_SHOWUSER5_STR "\x00\x00"
    "\x00\x00\x00\x60\x00\x08"
    MSG_CANCEL7_STR "\x00"
    "\x00\x00\x00\x61\x00\x16"
    MSG_SENDDIRMSG_STR "\x00"
    "\x00\x00\x00\x62\x00\x16"
    MSG_SENDDIRMSG2_STR "\x00"
    "\x00\x00\x00\x63\x00\x0C"
    MSG_RECIPIENT_STR "\x00\x00"
    "\x00\x00\x00\x64\x00\x1A"
    MSG_SCREENNAME_STR "\x00\x00"
    "\x00\x00\x00\x65\x00\x0A"
    MSG_MESSAGE_STR "\x00\x00"
    "\x00\x00\x00\x66\x00\x26"
    MSG_MESSAGE2_STR "\x00\x00"
    "\x00\x00\x00\x67\x00\x08"
    MSG_CLEAR7_STR "\x00\x00"
    "\x00\x00\x00\x68\x00\x06"
    MSG_SEND5_STR "\x00"
    "\x00\x00\x00\x69\x00\x08"
    MSG_CANCEL8_STR "\x00"
    "\x00\x00\x00\x6A\x00\x14"
    MSG_UPDATEPROFILE_STR "\x00"
    "\x00\x00\x00\x6B\x00\x24"
    MSG_UPDATEPROFILE2_STR "\x00"
    "\x00\x00\x00\x6C\x00\x06"
    MSG_PROFNAME_STR "\x00"
    "\x00\x00\x00\x6D\x01\x1E"
    MSG_PROFNAME2_STR "\x00"
    "\x00\x00\x00\x6E\x00\x06"
    MSG_WEB_STR "\x00\x00"
    "\x00\x00\x00\x6F\x00\x62"
    MSG_WEB2_STR "\x00"
    "\x00\x00\x00\x70\x00\x0A"
    MSG_LOCATION_STR "\x00"
    "\x00\x00\x00\x71\x00\x62"
    MSG_LOCATION2_STR "\x00"
    "\x00\x00\x00\x72\x00\x06"
    MSG_BIO_STR "\x00\x00"
    "\x00\x00\x00\x73\x00\x4A"
    MSG_BIO2_STR "\x00"
    "\x00\x00\x00\x74\x00\x10"
    MSG_UPDATEPROFILE3_STR "\x00"
    "\x00\x00\x00\x75\x00\x08"
    MSG_CANCEL9_STR "\x00"
    "\x00\x00\x00\x76\x00\x12"
    MSG_WHAT_STR "\x00"
    "\x00\x00\x00\x77\x00\x0C"
    MSG_FAST_STR "\x00\x00"
    "\x00\x00\x00\x78\x00\x0A"
    MSG_FEEDBACK_STR "\x00\x00"
    "\x00\x00\x00\x79\x00\x08"
    MSG_DONATE4_STR "\x00"
    "\x00\x00\x00\x7A\x00\x0E"
    MSG_SAVE2_STR "\x00"
    "\x00\x00\x00\x7B\x00\x1A"
    MSG_TEST3_STR "\x00"
    "\x00\x00\x00\x7C\x00\x1C"
    MSG_DONTSAVE_STR "\x00\x00"
    "\x00\x00\x00\x7D\x00\x0C"
    MSG_CLEAR8_STR "\x00"
    "\x00\x00\x00\x7E\x00\x0C"
    MSG_SEND6_STR "\x00\x00"
    "\x00\x00\x00\x7F\x00\x0E"
    MSG_CANCEL10_STR "\x00\x00"
    "\x00\x00\x00\x80\x00\x0E"
    MSG_CLEAR9_STR "\x00\x00"
    "\x00\x00\x00\x81\x00\x08"
    MSG_SEARCH8_STR "\x00\x00"
    "\x00\x00\x00\x82\x00\x0E"
    MSG_CANCEL11_STR "\x00"
    "\x00\x00\x00\x83\x00\x10"
    MSG_CLEAR10_STR "\x00"
    "\x00\x00\x00\x84\x00\x0E"
    MSG_FOLLOWUSER_STR "\x00"
    "\x00\x00\x00\x85\x00\x10"
    MSG_UNFOLLOWUSER_STR "\x00"
    "\x00\x00\x00\x86\x00\x10"
    MSG_CANCEL12_STR "\x00\x00"
    "\x00\x00\x00\x87\x00\x10"
    MSG_CLEAR12_STR "\x00"
    "\x00\x00\x00\x88\x00\x1E"
    MSG_BLOCKUSER_STR "\x00"
    "\x00\x00\x00\x89\x00\x22"
    MSG_UNBLOCKUSER_STR "\x00\x00"
    "\x00\x00\x00\x8A\x00\x10"
    MSG_CANCEL13_STR "\x00\x00"
    "\x00\x00\x00\x8B\x00\x10"
    MSG_CLEAR13_STR "\x00"
    "\x00\x00\x00\x8C\x00\xC0"
    MSG_SMSNOTE_STR "\x00"
    "\x00\x00\x00\x8D\x00\x30"
    MSG_SMSSTOP_STR "\x00"
    "\x00\x00\x00\x8E\x00\x10"
    MSG_CANCEL14_STR "\x00\x00"
    "\x00\x00\x00\x8F\x00\x10"
    MSG_CLEAR14_STR "\x00"
    "\x00\x00\x00\x90\x00\x20"
    MSG_SHOWTWEETS_STR "\x00"
    "\x00\x00\x00\x91\x00\x10"
    MSG_CANCEL15_STR "\x00\x00"
    "\x00\x00\x00\x92\x00\x16"
    MSG_CLEAR15_STR "\x00\x00"
    "\x00\x00\x00\x93\x00\x14"
    MSG_SENDDIRMSG3_STR "\x00"
    "\x00\x00\x00\x94\x00\x16"
    MSG_CANCEL16_STR "\x00"
    "\x00\x00\x00\x95\x00\x16"
    MSG_UPDATEPROFILE4_STR "\x00\x00"
    "\x00\x00\x00\x96\x00\x0E"
    MSG_CANCEL17_STR "\x00"
    "\x00\x00\x00\x97\x00\x08"
    MSG_SEARCH9_STR "\x00\x00"
    "\x00\x00\x00\x98\x00\x04"
    MSG_OK_STR "\x00"
    "\x00\x00\x00\x99\x00\x1E"
    MSG_SEARCH10_STR "\x00"
    "\x00\x00\x00\x9A\x00\x08"
    MSG_BLOCK2_STR "\x00\x00"
    "\x00\x00\x00\x9B\x00\x10"
    MSG_BLOCK3_STR "\x00\x00"
    "\x00\x00\x00\x9C\x01\x42"
    MSG_BLOCK4_STR "\x00"
    "\x00\x00\x00\x9D\x00\x06"
    MSG_QUIT_STR "\x00\x00"
    "\x00\x00\x00\x9E\x00\x0E"
    MSG_QUIT2_STR "\x00"
    "\x00\x00\x00\x9F\x00\x2C"
    MSG_QUIT3_STR "\x00"
    "\x00\x00\x00\xA0\x00\x06"
    MSG_SAVE3_STR "\x00\x00"
    "\x00\x00\x00\xA1\x00\x04"
    MSG_OK2_STR "\x00"
    "\x00\x00\x00\xA2\x00\x1A"
    MSG_SAVED_STR "\x00"
    "\x00\x00\x00\xA3\x00\x10"
    MSG_UPDATEPROFILE5_STR "\x00\x00"
    "\x00\x00\x00\xA4\x00\x04"
    MSG_OK3_STR "\x00"
    "\x00\x00\x00\xA5\x01\xDE"
    MSG_UPDATEPROFILE6_STR "\x00\x00"
    "\x00\x00\x00\xA6\x00\x2C"
    MSG_HTML1_STR "\x00"
    "\x00\x00\x00\xA7\x00\x6E"
    MSG_HTML2_STR "\x00\x00"
    "\x00\x00\x00\xA8\x00\x54"
    MSG_HTML3_STR "\x00\x00"
    "\x00\x00\x00\xA9\x00\x4C"
    MSG_HTML4_STR "\x00"
    "\x00\x00\x00\xAA\x00\x44"
    MSG_HTML5_STR "\x00"
    "\x00\x00\x00\xAB\x00\x42"
    MSG_HTML6_STR "\x00"
    "\x00\x00\x00\xAC\x00\x52"
    MSG_HTML7_STR "\x00"
    "\x00\x00\x00\xAD\x00\x28"
    MSG_HTML8_STR "\x00"
    "\x00\x00\x00\xAE\x00\x2C"
    MSG_HTML9_STR "\x00"
    "\x00\x00\x00\xAF\x00\x36"
    MSG_HTML10_STR "\x00\x00"
    "\x00\x00\x00\xB0\x00\x4E"
    MSG_HTML11_STR "\x00\x00"
    "\x00\x00\x00\xB1\x00\x40"
    MSG_HTML12_STR "\x00\x00"
    "\x00\x00\x00\xB2\x00\x76"
    MSG_HTML13_STR "\x00"
    "\x00\x00\x00\xB3\x00\x42"
    MSG_HTML14_STR "\x00"
    "\x00\x00\x00\xB4\x01\x24"
    MSG_HTML15_STR "\x00\x00"
    "\x00\x00\x00\xB5\x00\x3A"
    MSG_HTML16_STR "\x00\x00"
    "\x00\x00\x00\xB6\x00\x94"
    MSG_HTML17_STR "\x00"
    "\x00\x00\x00\xB7\x00\x3A"
    MSG_HTML18_STR "\x00\x00"
    "\x00\x00\x00\xB8\x00\x76"
    MSG_HTML19_STR "\x00\x00"
    "\x00\x00\x00\xB9\x00\x6E"
    MSG_HTML20_STR "\x00"
    "\x00\x00\x00\xBA\x00\x4E"
    MSG_HTML21_STR "\x00"
    "\x00\x00\x00\xBB\x00\x52"
    MSG_HTML22_STR "\x00"
    "\x00\x00\x00\xBC\x01\x00"
    MSG_HTML23_STR "\x00\x00"
    "\x00\x00\x00\xBD\x00\x34"
    MSG_HTML24_STR "\x00"
    "\x00\x00\x00\xBE\x00\x78"
    MSG_HTML25_STR "\x00"
    "\x00\x00\x00\xBF\x00\x34"
    MSG_HTML26_STR "\x00\x00"
    "\x00\x00\x00\xC0\x00\x38"
    MSG_HTML27_STR "\x00"
    "\x00\x00\x00\xC1\x00\x32"
    MSG_HTML28_STR "\x00"
    "\x00\x00\x00\xC2\x00\x3A"
    MSG_HTML29_STR "\x00\x00"
    "\x00\x00\x00\xC3\x00\x42"
    MSG_HTML30_STR "\x00\x00"
    "\x00\x00\x00\xC4\x00\xB0"
    MSG_HTML31_STR "\x00"
    "\x00\x00\x00\xC5\x00\x40"
    MSG_HTML32_STR "\x00"
    "\x00\x00\x00\xC6\x01\x06"
    MSG_HTML33_STR "\x00"
    "\x00\x00\x00\xC7\x00\x3E"
    MSG_HTML34_STR "\x00"
    "\x00\x00\x00\xC8\x00\x20"
    MSG_HTML35_STR "\x00\x00"
    "\x00\x00\x00\xC9\x00\x06"
    MSG_0000_STR "\x00\x00"
    "\x00\x00\x00\xCA\x00\x0A"
    MSG_0001_STR "\x00\x00"
    "\x00\x00\x00\xCB\x00\x0A"
    MSG_0002_STR "\x00\x00"
    "\x00\x00\x00\xCC\x00\x0E"
    MSG_0003_STR "\x00"
    "\x00\x00\x00\xCD\x00\x0E"
    MSG_0004_STR "\x00"
    "\x00\x00\x00\xCE\x00\x0E"
    MSG_0005_STR "\x00"
    "\x00\x00\x00\xCF\x00\x0A"
    MSG_0006_STR "\x00\x00"
    "\x00\x00\x00\xD0\x00\x08"
    MSG_0007_STR "\x00\x00"
    "\x00\x00\x00\xD1\x00\x08"
    MSG_0008_STR "\x00\x00"
    "\x00\x00\x00\xD2\x00\x0C"
    MSG_0009_STR "\x00\x00"
    "\x00\x00\x00\xD3\x00\x06"
    MSG_0010_STR "\x00"
    "\x00\x00\x00\xD4\x00\x10"
    MSG_0011_STR "\x00\x00"
    "\x00\x00\x00\xD5\x00\x06"
    MSG_0012_STR "\x00\x00"
    "\x00\x00\x00\xD6\x00\x0A"
    MSG_0013_STR "\x00\x00"
    "\x00\x00\x00\xD7\x00\x06"
    MSG_0014_STR "\x00"
    "\x00\x00\x00\xD8\x00\x0A"
    MSG_0015_STR "\x00"
    "\x00\x00\x00\xD9\x00\x0E"
    MSG_0016_STR "\x00\x00"
    "\x00\x00\x00\xDA\x00\x06"
    MSG_0017_STR "\x00\x00"
    "\x00\x00\x00\xDB\x00\x06"
    MSG_0018_STR "\x00\x00"
    "\x00\x00\x00\xDC\x00\x0C"
    MSG_0019_STR "\x00\x00"
    "\x00\x00\x00\xDD\x00\x0E"
    MSG_0020_STR "\x00\x00"
    "\x00\x00\x00\xDE\x00\x0A"
    MSG_0021_STR "\x00\x00"
    "\x00\x00\x00\xDF\x00\x08"
    MSG_0022_STR "\x00\x00"
    "\x00\x00\x00\xE0\x00\x06"
    MSG_0023_STR "\x00"
    "\x00\x00\x00\xE1\x00\x0A"
    MSG_0024_STR "\x00\x00"
    "\x00\x00\x00\xE2\x00\x10"
    MSG_0025_STR "\x00\x00"
    "\x00\x00\x00\xE3\x00\x0E"
    MSG_0026_STR "\x00\x00"
    "\x00\x00\x00\xE4\x00\x06"
    MSG_0027_STR "\x00\x00"
    "\x00\x00\x00\xE5\x00\x06"
    MSG_0028_STR "\x00\x00"
    "\x00\x00\x00\xE6\x00\x08"
    MSG_0029_STR "\x00\x00"
    "\x00\x00\x00\xE7\x00\x12"
    MSG_0030_STR "\x00\x00"
    "\x00\x00\x00\xE8\x00\x0A"
    MSG_0031_STR "\x00"
};

#endif /* CATCOMP_BLOCK */

/****************************************************************************/

struct LocaleInfo
{
    APTR li_LocaleBase;
    APTR li_Catalog;
};

#ifdef CATCOMP_CODE

STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
LONG   *l;
UWORD  *w;
STRPTR  builtIn;

    l = (LONG *)CatCompBlock;

    while (*l != stringNum)
    {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    builtIn = (STRPTR)((ULONG)l + 6);

#undef LocaleBase
#define LocaleBase li->li_LocaleBase

    if (LocaleBase)
        return(STRPTR)(GetCatalogStr(li->li_Catalog,stringNum,builtIn)); //added (STRPTR)
#undef LocaleBase

    return(builtIn);
}

#endif /* CATCOMP_CODE */
