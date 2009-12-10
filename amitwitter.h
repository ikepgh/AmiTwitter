/** $Revision Header *** Header built automatically - do not edit! ***********
 **
 ** � Copyright 2009 IKE <ikepgh@yahoo.com>
 **
 ** File             : amitwitter.h
 ** Created on       : Friday, 06-Nov-09
 ** Created by       : IKE
 ** Current revision : V 0.18
 **
 ** Purpose
 ** -------
 **   AmiTwitter - a Twitter Program
 **
 ** Date        Author                 Comment
 ** =========   ====================   ====================
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
 ** AmiTwitter was compiled with Cubic IDE/gcc 2.95.3-4 on OS 3.x
 **
 ** This code is based heavily on xTwitter written
 ** by Tsukasa Hamano found on http://www.cuspy.org/xtwitter/
 **
 ******************************************************************************/


/******************************************************************************/

#include <time.h>
#include "AmiTwitter_rev.h"

#define TWITTER_BASE_URI                    "http://twitter.com"   //https...

#define TWITTER_API_PATH_DIRECT_MESSAGE     "/direct_messages/new.xml"

#define TWITTER_API_PATH_UPDATE             "/statuses/update.xml"
#define TWITTER_API_PATH_HOME_TIMELINE      "/statuses/home_timeline.xml"
#define TWITTER_API_PATH_USER_TIMELINE      "/statuses/user_timeline.xml"
#define TWITTER_API_PATH_MENTIONS           "/statuses/mentions.xml"
#define TWITTER_API_PATH_PUBLIC_TIMELINE    "/statuses/public_timeline.xml"
#define TWITTER_API_PATH_RETWEETED_BY_ME    "/statuses/retweeted_by_me.xml"
#define TWITTER_API_PATH_RETWEETED_TO_ME    "/statuses/retweeted_to_me.xml"
#define TWITTER_API_PATH_RETWEETS_OF_ME     "/statuses/retweets_of_me.xml"

#define TWITTER_API_PATH_VERIFY_CREDENTIALS "/account/verify_credentials.xml"

/******************************************************************************/

// HTML Introduction/About Message
#define HTML_INTRO \
"<HTML><HEAD><BODY>" \
"<TITLE>Welcome to AmiTwitter</TITLE></HEAD>" \
"<FONT COLOR=#000000><CENTER><B>"VSTRING"  � IKE </B></CENTER><BR><BR>" \
"<HR>"\
"<CENTER>Enter your User Name/Password in the Tools -> Settings dialog window to get started... </CENTER><BR>" \
"<HR>"\
"<CENTER>This program is released under the GNU General Public License</CENTER><BR>" \
"<CENTER>No warranty is expressed or implied. Use at your own risk!</CENTER>" \
"<HR>" \
"<CENTER>The latest source code is always available at:</CENTER><BR>" \
"<CENTER>https://sourceforge.net/projects/amitwitter/</CENTER><BR>" \
"<p>" \
"<CENTER>-- Thanks for your interest, The AmiTwitter Open Source Team</CENTER><BR>" \
"<HR>" \
"<CENTER>This program uses:</CENTER><BR>" \
"<CENTER>MUI � Stefen Stuntz</CENTER><BR>" \
"<CENTER>URLtext � Alfonso Ranieri</CENTER><BR>" \
"<CENTER>HTMLtext � Dirk Holtwick</CENTER><BR>" \
"<CENTER>OpenURL, BetterString  � Open Source Team</CENTER><BR>" \
"<CENTER>libiconv.ixlibrary � GNU port by Bruno Haible </CENTER><BR>" \
"<HR>" \
"<CENTER><IMG SRC = PROGDIR:data/program_images/curl.gif>&nbsp;</CENTER><BR>" \
"<HR>" \
"</FONT></BODY></HTML>"

/******************************************************************************/

// HTML Help/FAQ Message
#define HTML_HELP \
"<HTML><HEAD><BODY>" \
"<TITLE>Welcome to AmiTwitter</TITLE></HEAD>" \
"<FONT COLOR=#000000><CENTER><B>FAQs </B></CENTER><BR>" \
"<HR>" \
"<CENTER>What does AmiTwitter stand for?</CENTER><BR>" \
"<CENTER>Answer: <b>A</b>miga-<b>M</b>orphOS-<b>I</b>KE-Twitter </CENTER><BR>" \
"<HR>" \
"<CENTER>What are the requirements for AmiTwitter?</CENTER><BR>" \
"<CENTER>Answer: MUI, HTMLtext, URLtext, BetterString, libiconv.ixlibrary.  OpenURL is optional. </CENTER><BR>" \
"<HR>"\
"<CENTER>AmiTwitter does not download or send Tweets?</CENTER><BR>" \
"<CENTER>Answer:  Are you connected to the internet, have you entered your Twitter User Name/Password in the Tools -> Settings window and saved them? Are you already following some people for <I>Timeline</I> to work) and have you sent some Tweets (for <I>My Tweets</I> to work)?</CENTER><BR>" \
"<HR>" \
"<CENTER>Direct Messages don't seem to work?</CENTER><BR>" \
"<CENTER>Answer: Ensure you enter the recipient's Screen Name (i.e. User Name) correctly, it does not currently accept user id numbers.</CENTER><BR>" \
"<HR>" \
"<CENTER>I like AmiTwitter, how do I donate?</CENTER><BR>" \
"<CENTER>Answer: Follow the 'Donate!' link to the PayPal website that opens in your favorite web browser</CENTER><BR>" \
"<HR>" \
"<CENTER>Nothing happens when I click on the links at the very bottom of the main program window?</CENTER><BR>" \
"<CENTER>Answer: Ensure OpenURL is properly installed/configured.</CENTER><BR>" \
"<HR>" \
"<CENTER>Why is the file size so large?</CENTER><BR>" \
"<CENTER>Answer: Because AmiTwitter relies on numerous libs to work, namely: cURL, SSL, xml2, glib, etc....</CENTER><BR>" \
"<HR>" \
"<CENTER>Why did you write AmiTwitter?</CENTER><BR>" \
"<CENTER>Answer: To prove it could be done!</CENTER><BR>" \
"<HR>" \
"<CENTER>What is AmiTwitter based on?</CENTER><BR>" \
"<CENTER>Answer: xTwitter by Tsukasa Hamano </CENTER><BR>" \
"<HR>" \
"<CENTER>Links in the main Tweets window don't work?</CENTER><BR>" \
"<CENTER>Answer: This is not a webbrowser...Also...it is a limitation of HTMLtext.mcc currently being used (The plan is to switch to HTMLview.mcc in the future...)</CENTER><BR>" \
"<HR>" \
"<CENTER>What are the  future plans for AmiTwitter?</CENTER><BR>" \
"<CENTER>Answer: First, learn more of the Twitter API, but it depends a lot on the feedback I recieve.  Also, ports to other systems (i.e OS 4.x and AROS) and an active source code base with some developers joining the project at SourceForge would be nice...<BR>" \
"<HR>" \
"<CENTER>I don't know how to code, what can I do?</CENTER><BR>" \
"<CENTER>Consider donating.<BR>" \
"<HR>" \
"<BR>" \
"</FONT></BODY></HTML>"

/******************************************************************************/

// Twitter structure
typedef struct {
    const char *base_uri;
    const char *user;
    const char *pass;
    const char *source;
    const char *text;                   
    char res_dir[PATH_MAX];
    char images_dir[PATH_MAX];
    unsigned long last_home_timeline;
    unsigned long last_user_timeline;   
    unsigned long mentions;             
    unsigned long last_public_timeline; 
    int fetch_interval;
    int show_interval;
    int alignment;
    int debug;
    int error;
  
}twitter_t;

/******************************************************************************/

// User structure
typedef struct {
    const char *id;
    const char *name;              
    const char *screen_name;
    const char *location;          
    const char *description;
    const char *profile_image_url;
    const char *followers_count;   
    const char *friends_count;     
    const char *favourites_count;
    const char *statuses_count;    
}twitter_user_t;

/******************************************************************************/

// Status structure
typedef struct {
    const char *created_at;
    const char *id;
    const char *text;
    const char *source;
    const char *truncated;              
    const char *in_reply_to_status_id;   
    const char *in_reply_to_user_id;     
    const char *favorited;               
    const char *in_reply_to_screen_name; 
    const char *retweeted_status;        
    const twitter_user_t *user;
}twitter_status_t;

/******************************************************************************/

twitter_t *twitter_new();
void twitter_free(twitter_t *twitter);
int twitter_config(twitter_t *twitter);

int twitter_fetch(twitter_t *twitter, const char *api_uri, GByteArray *buf);
int twitter_update(twitter_t *twitter, const char *status);
int twitter_direct_message(twitter_t *twitter, const char *screen_name, const char *text); 
int twitter_verify_credentials(twitter_t *twitter, const char *screen_name, const char *text);

GList* twitter_home_timeline(twitter_t *twitter);
GList* twitter_user_timeline(twitter_t *twitter);   
GList* twitter_mentions(twitter_t *twitter);
GList* twitter_public_timeline(twitter_t *twitter); 

GList* twitter_retweeted_by_me(twitter_t *twitter);
GList* twitter_retweeted_to_me(twitter_t *twitter);
GList* twitter_retweets_of_me(twitter_t *twitter);

GList* twitter_parse_statuses_node(xmlTextReaderPtr reader);
twitter_user_t* twitter_parse_user_node(xmlTextReaderPtr reader);
twitter_status_t* twitter_parse_status_node(xmlTextReaderPtr reader);

void twitter_statuses_free(GList *statuses);
void twitter_status_print(twitter_status_t *status);

int twitter_fetch_images(twitter_t *twitter, GList *statuses);
int twitter_fetch_image(twitter_t *twitter, const char *url, char* path);

/******************************************************************************/
