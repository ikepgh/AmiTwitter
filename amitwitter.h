/*
 * AmiTwitter 0.1.1 Copyright (C) 2009 IKE <ikepgh@yahoo.com>
 *
 * Compiled with Cubic IDE/gcc 2.95.3-4 on OS 3.x
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 * This code is based heavily on xTwitter written
 * by Tsukasa Hamano found on http://www.cuspy.org/xtwitter/
 *
 */

/*****************************************************************************/

#include <time.h>

#define TWITTER_BASE_URI "http://twitter.com"   //https...
#define TWITTER_API_PATH_FRIENDS_TIMELINE "/statuses/friends_timeline.xml"
#define TWITTER_API_PATH_UPDATE "/statuses/update.xml"
#define TWITTER_API_PATH_DIRECT_MESSAGE "/direct_messages/new.xml"       //added
#define TWITTER_API_PATH_USER_TIMELINE "/statuses/user_timeline.xml"     //added
#define TWITTER_API_PATH_MENTIONS "/statuses/mentions.xml"               //added
#define TWITTER_API_PATH_PUBLIC_TIMELINE "/statuses/public_timeline.xml" //added

/*****************************************************************************/

// HTML Introduction/About Message
#define HTML_INTRO \
"<HTML><HEAD><BODY>" \
"<TITLE>Welcome to AmiTwitter</TITLE></HEAD>" \
"<FONT COLOR=#000000><CENTER><B>Welcome to AmiTwitter </B></CENTER><BR>" \
"<HR>"\
"<CENTER><I><B>Version 0.1.1 © 2009 IKE </B></I></CENTER><BR>" \
"<HR>" \
"<CENTER>This program is released under the</CENTER><BR>" \
"<CENTER>GNU  General Public License</CENTER><BR>" \
"<HR>" \
"<CENTER>The latest source is always available at:</CENTER><BR>" \
"<CENTER>https://sourceforge.net/projects/amitwitter/</CENTER><BR>" \
"<p>" \
"<CENTER>-- Thanks for your interest, The AmiTwitter Open Source Team</CENTER><BR>" \
"<HR>" \
"<CENTER>This program uses:</CENTER><BR>" \
"<CENTER>MUI © Stefen Stuntz</CENTER><BR>" \
"<CENTER>URLtext © Alfonso Ranieri</CENTER><BR>" \
"<CENTER>HTMLtext © Dirk Holtwick</CENTER><BR>" \
"<CENTER>OpenURL, BetterString  © Open Source Team</CENTER><BR>" \
"<CENTER>libiconv.ixlibrary © GNU, port by Bruno Haible</CENTER><BR>" \
"<HR>" \
"<CENTER><IMG SRC = PROGDIR:data/program_images/curl.gif>&nbsp;</CENTER><BR>" \
"<BR>" \
"<HR>" \
"<CENTER>No Warranty is Expressed or Implied!</CENTER>" \
"<HR>" \
"</FONT></BODY></HTML>"

/*****************************************************************************/

// HTML Help/FAQ Message
#define HTML_HELP \
"<HTML><HEAD><BODY>" \
"<TITLE>Welcome to AmiTwitter</TITLE></HEAD>" \
"<FONT COLOR=#000000><CENTER><B>FAQs </B></CENTER><BR>" \
"<HR>" \
"<CENTER>What are the requirements for AmiTwitter?</CENTER><BR>" \
"<CENTER>Answer: MUI, HTMLtext, URLtext, BetterString.  'libiconv.ixlibrary' is required to be in your 'Libs' directory.  OpenURL is optional. </CENTER><BR>" \
"<HR>"\
"<CENTER>AmiTwitter does not download or send Tweets?</CENTER><BR>" \
"<CENTER>Answer:  Are you connected to the internet, have you entered your Twitter username/password in the 'Tools' ->'Settings' window and saved them?</CENTER><BR>" \
"<HR>" \
"<CENTER>Direct Messages don't seem to work?</CENTER><BR>" \
"<CENTER>Answer: Ensure you enter the recipient's 'Screen Name' (i.e. username) correctly, it does not currently accept user id numbers.</CENTER><BR>" \
"<HR>" \
"<CENTER>I like AmiTwitter, how do I donate?</CENTER><BR>" \
"<CENTER>Answer: Follow the 'Click to Donate!' link to the PayPal website that opens in your favorite web browser</CENTER><BR>" \
"<HR>" \
"<CENTER>Nothing happens when I click on the links at the very bottom of the main program window?</CENTER><BR>" \
"<CENTER>Answer: Ensure OpenURL is properly installed/configured.</CENTER><BR>" \
"<HR>" \
"<CENTER>Why is the file size so large?</CENTER><BR>" \
"<CENTER>Answer: Because AmiTwitter relies on numerous libs to work: cURL, SSL, xml2, glib, etc....</CENTER><BR>" \
"<HR>" \
"<CENTER>Why did you write AmiTwitter?</CENTER><BR>" \
"<CENTER>Answer: To prove it could be done on AmigaOS 3.x!</CENTER><BR>" \
"<HR>" \
"<CENTER>What is AmiTwitter based on?</CENTER><BR>" \
"<CENTER>Answer: xTwitter by Tsukasa Hamano </CENTER><BR>" \
"<HR>" \
"<CENTER>Links in the main Tweets window don't work?</CENTER><BR>" \
"<CENTER>Answer: This is not a webbrowser...Also...it is a limitation of the .mcc's currently available for 3.x and useable with gcc (i.e. HTMLtext)</CENTER><BR>" \
"<HR>" \
"<CENTER>What are the  future plans for AmiTwitter?</CENTER><BR>" \
"<CENTER>Answer: First, learn more of the Twitter API, but it depends on the feedback I recieve.  Also, ports to other systems (i.e MorphOS and OS 4.x) and an active source code base with some developers joining the project at SourceForge would be nice...<BR>" \
"<HR>" \
"<CENTER>I don't know how to code, what can I do?</CENTER><BR>" \
"<CENTER>Consider donating.<BR>" \
"<HR>" \
"<BR>" \
"</FONT></BODY></HTML>"

/*****************************************************************************/

// Twitter structure
typedef struct {
    const char *base_uri;
    const char *user;
    const char *pass;
    const char *source;
    const char *text;                   //added
    char res_dir[PATH_MAX];
    char images_dir[PATH_MAX];
    unsigned long last_friends_timeline;
    unsigned long last_user_timeline;   //added
    unsigned long mentions;             //added
    unsigned long last_public_timeline; //added
    int fetch_interval;
    int show_interval;
    int alignment;
    int debug;
    int error;
  
}twitter_t;

/*****************************************************************************/

// User structure
typedef struct {
    const char *id;
    const char *name;              //added
    const char *screen_name;
    const char *location;          //added
    const char *profile_image_url;
    const char *followers_count;   //added
    const char *friends_count;     //added
    const char *favourites_count;  //added
    const char *statuses_count;    //added
}twitter_user_t;

/*****************************************************************************/

// Status structure
typedef struct {
    const char *created_at;
    const char *id;
    const char *text;
    const char *source;
    const twitter_user_t *user;
}twitter_status_t;

/*****************************************************************************/

twitter_t *twitter_new();
void twitter_free(twitter_t *twitter);
int twitter_config(twitter_t *twitter);

int twitter_fetch(twitter_t *twitter, const char *api_uri, GByteArray *buf);
int twitter_update(twitter_t *twitter, const char *status);
int twitter_direct_message(twitter_t *twitter, const char *screen_name, const char *text); //added
GList* twitter_friends_timeline(twitter_t *twitter);
GList* twitter_user_timeline(twitter_t *twitter);   //added
GList* twitter_public_timeline(twitter_t *twitter); //added
GList* twitter_mentions(twitter_t *twitter);        //added
GList* twitter_parse_statuses_node(xmlTextReaderPtr reader);
twitter_status_t* twitter_parse_status_node(xmlTextReaderPtr reader);
twitter_user_t* twitter_parse_user_node(xmlTextReaderPtr reader);

void twitter_statuses_free(GList *statuses);
void twitter_status_print(twitter_status_t *status);

int twitter_fetch_images(twitter_t *twitter, GList *statuses);
int twitter_fetch_image(twitter_t *twitter, const char *url, char* path);

/*****************************************************************************/
