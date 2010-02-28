/** $Revision Header *** Header built automatically - do not edit! ***********
 **
 ** © Copyright 2010 IKE <ikepgh@yahoo.com>
 **
 ** File             : amitwitter.h
 ** Created on       : Friday, 06-Nov-09
 ** Created by       : IKE
 ** Current revision : V 0.28
 **
 ** Purpose
 ** -------
 **   AmiTwitter - a Twitter Program
 **
 ** Date        Author                 Comment
 ** =========   ====================   ====================
 ** 28-Feb-10   Cyborg                 Removed all 'const' keywords as none of them was actually a const.
 ** 27-Feb-10   IKE                    glib dependency removed for all OS's; first OS4 compile
 ** 22-Feb-10   IKE                    codesets.library implemented
 ** 04-Feb-10   IKE                    most recent sent/recieved direct message
 ** 28-Jan-10   IKE                    Menu's, tabs & TheBar are now localized
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

#include <time.h>
#include "AmiTwitter_rev.h"

#define TWITTER_BASE_URI                    "http://twitter.com"   
#define TWITTER_BASE_SEARCH_URI             "http://search.twitter.com" 

#define TWITTER_API_PATH_SEARCH             "/search.json?q=morphos"

#define TWITTER_API_PATH_DIRECT_MESSAGE     "/direct_messages/new.xml"
#define TWITTER_API_PATH_DIRECT_MSGSENT     "/direct_messages/sent.xml"
#define TWITTER_API_PATH_DIRECT_MSGRCVD     "/direct_messages.xml"

#define TWITTER_API_PATH_UPDATE             "/statuses/update.xml" 
#define TWITTER_API_PATH_HOME_TIMELINE      "/statuses/home_timeline.xml"
#define TWITTER_API_PATH_USER_TIMELINE      "/statuses/user_timeline.xml"
#define TWITTER_API_PATH_MENTIONS           "/statuses/mentions.xml"
#define TWITTER_API_PATH_PUBLIC_TIMELINE    "/statuses/public_timeline.xml"
#define TWITTER_API_PATH_RETWEETED_BY_ME    "/statuses/retweeted_by_me.xml"
#define TWITTER_API_PATH_RETWEETED_TO_ME    "/statuses/retweeted_to_me.xml"
#define TWITTER_API_PATH_RETWEETS_OF_ME     "/statuses/retweets_of_me.xml"
#define TWITTER_API_PATH_FRIENDS            "/statuses/friends.xml"
#define TWITTER_API_PATH_FOLLOWERS          "/statuses/followers.xml"

#define TWITTER_API_PATH_VERIFY_CREDENTIALS "/account/verify_credentials.xml"
#define TWITTER_API_PATH_UPDATEPROFILE      "/account/update_profile.xml"

#define TWITTER_API_PATH_FOLLOW             "/friendships/create/"
#define TWITTER_API_PATH_UNFOLLOW           "/friendships/destroy/"

#define TWITTER_API_PATH_BLOCK              "/blocks/create/"
#define TWITTER_API_PATH_UNBLOCK            "/blocks/destroy/"
#define TWITTER_API_PATH_BLOCKING           "/blocks/blocking.xml"

#define TWITTER_API_PATH_NOTIFY             "/notifications/follow/"
#define TWITTER_API_PATH_UNNOTIFY           "/notifications/leave/"

#define TWITTER_API_PATH_FAVS               "/favorites.xml"

#define TWITTER_API_PATH_USER_SHOW          "/users/show/"

/******************************************************************************/

// HTML Introduction/About Message
#define HTML_INTRO \
"<HTML><HEAD><BODY>" \
"<TITLE>Welcome to AmiTwitter</TITLE></HEAD>" \
"<FONT COLOR=#000000><CENTER><B>"VSTRING"  © IKE 2009-10</B></CENTER><BR><BR>" \
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
"<CENTER>MUI © Stefen Stuntz</CENTER><BR>" \
"<CENTER>URLtext © Alfonso Ranieri</CENTER><BR>" \
"<CENTER>HTMLtext © Dirk Holtwick</CENTER><BR>" \
"<CENTER>OpenURL, BetterString, TheBar, codesets.library © Open Source Team</CENTER><BR>" \
"<CENTER>libiconv.ixlibrary © GNU port by Bruno Haible </CENTER><BR>" \
"<CENTER>MasonIcons © Martin Merz </CENTER><BR>" \
"<HR>" \
"<CENTER><IMG SRC = PROGDIR:data/program_images/curl.gif>&nbsp;</CENTER><BR>" \
"<HR>" \
"</FONT></BODY></HTML>"

/******************************************************************************/

// Twitter structure
typedef struct {
    char *base_uri;
    char *base_search_uri;
    char *user;
    char *pass;
    char *source;
    char *text;
    char res_dir[PATH_MAX];
    char images_dir[PATH_MAX];
    unsigned long last_home_timeline;
    unsigned long last_user_timeline;   
    unsigned long mentions;             
    unsigned long last_friends_timeline;
    unsigned long last_followers_timeline;
    unsigned long last_public_timeline; 
    unsigned long last_dirmsgsent_timeline;
    unsigned long last_dirmsgrcvd_timeline;
    unsigned long last_favs_timeline;
    unsigned long last_blocking_timeline;
    unsigned long last_usershow_timeline;
    int fetch_interval;
    int show_interval;
    int alignment;
    int debug;
    int error;
}twitter_t;

/******************************************************************************/

// User structure
typedef struct {
	char *id;
    char *name;
    char *screen_name;
    char *location;
    char *description;
    char *profile_image_url;
    char *followers_count;
    char *friends_count;
    char *favourites_count;
    char *statuses_count;
}twitter_user_t;

// Status structure
typedef struct {
	struct Node Node;
    char *created_at;
    char *id;
    char *text;
    char *source;
    char *truncated;
    char *in_reply_to_status_id;
    char *in_reply_to_user_id;
    char *favorited;
    char *in_reply_to_screen_name;
    char *retweeted_status;
    twitter_user_t *user;
}twitter_status_t;

/******************************************************************************/

// Recipient structure
typedef struct {
    char *created_at;
    char *id;
    char *name;
    char *screen_name;
    char *location;
    char *description;
    char *profile_image_url;
    char *followers_count;
    char *friends_count;
    char *favourites_count;
    char *statuses_count;
    char *recipient;
}twitter_recipient_t;

// Direct Message Recipient structure
typedef struct {
	struct Node Node;
    char *created_at;
    char *id;
    char *sender_id;
    char *text;
    char *recipient_id;
    char *sender_screen_name;
    char *recipient_screen_name;
    twitter_recipient_t *recipient;
}twitter_direct_message_t;

/******************************************************************************/

// Sender structure
typedef struct {
    char *created_at;
    char *id;
    char *name;
    char *screen_name;
    char *location;
    char *description;
    char *profile_image_url;
    char *followers_count;
    char *friends_count;
    char *favourites_count;
    char *statuses_count;
    char *sender;
}twitter_sender_t;

// Direct Message Sender structure
typedef struct {
    struct Node Node;
    char *created_at;
    char *id;
    char *sender_id;
    char *text;
    char *recipient_id;
    char *sender_screen_name;
    char *recipient_screen_name;
    twitter_sender_t *sender;
}twitter_direct_message_rcvd_t;

/******************************************************************************/

typedef struct {
	UBYTE *data;
	ULONG len;
} MyByteArray;

/******************************************************************************/

twitter_t *twitter_new();
void twitter_free(twitter_t *twitter);
int twitter_config(twitter_t *twitter);

int twitter_fetch(twitter_t *twitter, const char *api_uri, MyByteArray *buf);
int twitter_update(twitter_t *twitter, const char *status);
int twitter_follow(twitter_t *twitter, const char *status);
int twitter_unfollow(twitter_t *twitter, const char *status);
int twitter_block(twitter_t *twitter, const char *status);
int twitter_unblock(twitter_t *twitter, const char *status);
int twitter_notify(twitter_t *twitter, const char *status);
int twitter_unnotify(twitter_t *twitter, const char *status);
int twitter_direct_message(twitter_t *twitter, const char *screen_name, const char *text); 
int twitter_updateprofile(twitter_t *twitter, const char *name, const char *web, const char *location, const char *bio);
int twitter_verify_credentials(twitter_t *twitter, const char *screen_name, const char *text);
int twitter_search(twitter_t *twitter, const char *apiuri, MyByteArray *buf);

struct List* twitter_home_timeline(twitter_t *twitter);
struct List* twitter_user_timeline(twitter_t *twitter);
struct List* twitter_mentions(twitter_t *twitter);
struct List* twitter_friends_timeline(twitter_t *twitter);
struct List* twitter_followers_timeline(twitter_t *twitter);
struct List* twitter_blocking_timeline(twitter_t *twitter);
struct List* twitter_public_timeline(twitter_t *twitter);
struct List* twitter_usershow_timeline(twitter_t *twitter);

struct List* twitter_retweeted_by_me(twitter_t *twitter);
struct List* twitter_retweeted_to_me(twitter_t *twitter);
struct List* twitter_retweets_of_me(twitter_t *twitter);

struct List* twitter_dirmsgsent(twitter_t *twitter);
struct List* twitter_dirmsgrcvd(twitter_t *twitter);

struct List* twitter_favs(twitter_t *twitter);

struct List* twitter_parse_statuses_node(xmlTextReaderPtr reader);
struct List* twitter_parse_statuses_node_dirmsg(xmlTextReaderPtr reader);
struct List* twitter_parse_statuses_node_dirmsgrcvd(xmlTextReaderPtr reader);

twitter_user_t* twitter_parse_user_node(xmlTextReaderPtr reader);
twitter_recipient_t* twitter_parse_recipient_node(xmlTextReaderPtr reader);  
twitter_sender_t* twitter_parse_sender_node(xmlTextReaderPtr reader); 

twitter_status_t* twitter_parse_status_node(xmlTextReaderPtr reader);
twitter_direct_message_t* twitter_parse_status_node_dirmsg(xmlTextReaderPtr reader); 
twitter_direct_message_rcvd_t* twitter_parse_status_node_dirmsgrcvd(xmlTextReaderPtr reader); 

void twitter_statuses_free(struct List *statuses);
void twitter_statuses_free_dirmsg(struct List *statuses);
void twitter_statuses_free_dirmsgrcvd(struct List *statuses);

void twitter_status_print(twitter_status_t *status);
void twitter_status_print_dirmsg(twitter_direct_message_t *direct_message);
void twitter_status_print_dirmsgrcvd(twitter_direct_message_rcvd_t *direct_message);

int twitter_fetch_image(twitter_t *twitter, const char *url, char* path);
int twitter_fetch_images(twitter_t *twitter, struct List *statuses);
int twitter_fetch_images_dirmsg(twitter_t *twitter, struct List *statuses);
int twitter_fetch_images_dirmsgrcvd(twitter_t *twitter, struct List *statuses);

#if 0
/* cyborg: Deactivated. See amitwitter.c for more info why. */
int twitter_image_name(twitter_status_t *status, char *name);
int twitter_image_name_dirmsg(twitter_direct_message_t *direct_message, char *name);
int twitter_image_name_dirmsgrcvd(twitter_direct_message_rcvd_t *direct_message, char *name);
#endif

/******************************************************************************/
