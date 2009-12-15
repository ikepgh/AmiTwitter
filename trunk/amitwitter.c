/** $Revision Header *** Header built automatically - do not edit! ***********
 **
 ** © Copyright 2009 IKE <ikepgh@yahoo.com>
 **
 ** File             : amitwitter.c
 ** Created on       : Friday, 06-Nov-09
 ** Created by       : IKE
 ** Current revision : V 0.22
 **
 ** Purpose
 ** -------
 **   AmiTwitter - a Twitter Program
 **
 ** Date        Author                 Comment
 ** =========   ====================   ====================
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
 ** AmiTwitter was compiled with Cubic IDE/gcc 2.95.3-4 on OS 3.x
 **
 ** This code is based heavily on xTwitter written
 ** by Tsukasa Hamano found on http://www.cuspy.org/xtwitter/
 **
 ******************************************************************************/

/// Includes ******************************************************************/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <exec/libraries.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#if defined(__AMIGA__) && !defined(__PPC__)
#include <pragmas/dos_pragmas.h>
#include <pragmas/exec_pragmas.h>
#endif
#include <workbench/workbench.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <libraries/gadtools.h>
#if defined(__AMIGA__) && !defined(__PPC__)
#include <clib/gadtools_protos.h>
#endif
#if defined (__MORPHOS__)
#include <proto/exec.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

// MUI
#include <mui/Urltext_mcc.h>
#include <mui/HTMLtext_mcc.h>
#include <mui/BetterString_mcc.h>
#include <mui/TheBar_mcc.h>

// Dependencies
#include <glib.h>
#include <curl/curl.h>
#include <libxml/xmlreader.h>

// Amitwitter
#include "amitwitter.h"
#include "AmiTwitter_rev.h"
#include "language.h"

/*****************************************************************************/

///

/// Definitions ***************************************************************/

// Defines
#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif


// Settings
#define SAVE          47
#define TEST          48
#define CANCEL        49

// Buttons
#define HOME          50
#define PROFILE       51
#define REPLIES       52
#define RELOAD        53

// Search
#define BTN_SEARCH    54
#define CLEARSEARCH   55
#define SEARCH        56
#define CANCELSEARCH  57

// Direct Message
#define DIRECTMESSAGE 58
#define CLEARDM       59
#define DIRMSG        60
#define CANCELDM      61

// Tweet
#define TWEET         62
#define CLEAR         63
#define SENDUPDATE    64
#define CANCELTWEET   65

// User Follow
#define BTN_FOLLOW    66
#define CLEARFOLLOW   67
#define FOLLOW        68
#define UNFOLLOW      69
#define CANCELFOLLOW  70

// User Block
#define CLEARBLOCK    71
#define BLOCK         72
#define UNBLOCK       73
#define CANCELBLOCK   74

// User Notify
#define CLEARNOTIFY   75
#define NOTIFY        76
#define UNNOTIFY      77
#define CANCELNOTIFY  78

// User profile
#define UPDATEPROFILE 79
#define CANCELPROFILE 80

long __stack = 65536;

// Base Structures
/* Both setup by startup-code */
extern struct ExecBase *SysBase;
extern struct Library *DOSBase;

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct Library *MUIMasterBase;

#ifdef __amigaos4__
extern struct ExecIFace *IExec;    /* No need to open them! */
extern struct DOSIFace  *IDOS;

struct GraphicsIFace    *IGraphics      = NULL;
struct IntuitionIFace   *IIntuition     = NULL;
struct MUIMasterIFace   *IMUIMaster     = NULL;
#endif

#ifdef __MORPHOS__
#ifndef __ixemul__

struct Library *SocketBase = NULL;

#include <amitcp/socketbasetags.h>

#define select(args...) WaitSelect( args, NULL)
#define inet_ntoa(x)    Inet_NtoA( x ## .s_addr)
#define ioctl(a,b,c,d)  IoctlSocket( (LONG)a, (ULONG)b, (char*)c)
#define _AMIGASF        1

#else /* __ixemul__ */

#warning compiling with ixemul...

#endif
#endif

/*****************************************************************************/

///

/// Variables *****************************************************************/

Object *app, *STR_user, *STR_pass, *STR_message, *aboutwin, *STR_user_id,
*STR_directmessage, *STR_login, *STR_search, *STR_follow, *STR_block,
*STR_notify, *STR_profile_name, *STR_profile_web, *STR_profile_location,
*STR_profile_bio;

APTR str_user_pref, str_pass_pref, win_preferences, but_save, but_cancel,
but_test, username, password, urltxtlink, urltxtlink2, urltxtlink3, mailtxtlink,
txt_source, scroll_source, scroll_main, donate, win_donate, twittersite,
passforgot, win_tweet, but_cancel_tweet, win_dirmsg, but_cancel_dm, win_search,
window, sendupdate_gad, clear_gad, toolbar, clear_dm_gad, send_gad,
clear_search_gad, search_gad, but_search_cancel, clear_follow_gad, follow_gad,
but_follow_cancel, unfollow_gad, win_follow, clear_block_gad, win_block, block_gad,
unblock_gad, but_block_cancel, win_notify, clear_notify_gad, notify_gad,
unnotify_gad, but_notify_cancel, win_userprofile, update_profile_gad,
but_cancel_profile;

// Direct Messages
const char *screen_name, *text;

// Update Profile
const char *name,  *web,  *location,  *bio;

/*****************************************************************************/

///

/// Menus *********************************************************************/

enum {
    ADD_METHOD=1,
                        
    MEN_FILE, MEN_HOME, MEN_RETWEET, MEN_RETWEETBYME, MEN_RETWEETTOME, MEN_RETWEETOFME,
    MEN_REPLIES, MEN_RELOAD, MEN_SEARCH, MEN_SEARCHUSER,
    MEN_USER, MEN_FOLLOW, MEN_SHOW, MEN_BLOCK, MEN_NOTIFY, MEN_DIRMSG, MEN_DIRMSGSENT, MEN_DIRMSGRCVD,
    MEN_TWEET, MEN_MYTWEET, MEN_FAVS, MEN_QUIT,
    MEN_MISC, MEN_FRIENDS, MEN_FOLLOWERS, MEN_BLOCKING, MEN_RANDOM,
    MEN_TOOLS, MEN_PREFS, MEN_USERPROFILE, MEN_MUIPREFS,
    MEN_HELP, MEN_HELP2, MEN_DONATE, MEN_ABOUT, MEN_ABOUTMUI
};

#define M(type,title_id,flags,men) type, (UBYTE *)(title_id), 0, flags, 0, (APTR)(men)
#define MX(type,title_id,flags,ex,mid) type,   (UBYTE *)(title_id), 0, flags, ex,(APTR)(mid)
#define BAR    NM_ITEM, NM_BARLABEL, NULL, 0, 0, NULL
#define MENU_END  NM_END, NULL, NULL, 0, 0, NULL

// Menu structure
static struct NewMenu MenuData1[]=
{

    M( NM_TITLE,  MSG_FILE,          0,   MEN_FILE        ),
    M( NM_ITEM,   MSG_HOME,          0,   MEN_HOME        ),
    M( NM_ITEM,   MSG_RETWEET,       0,   MEN_RETWEET     ),
    MX( NM_SUB,   MSG_RETWEETBYME,   0,   0, MEN_RETWEETBYME   ),
    MX( NM_SUB,   MSG_RETWEETTOME,   0,   0, MEN_RETWEETTOME   ),
    MX( NM_SUB,   MSG_RETWEETOFME,   0,   0, MEN_RETWEETOFME   ),
    M( NM_ITEM,   MSG_REPLIES,       0,   MEN_REPLIES     ),
    M( NM_ITEM,   MSG_RELOAD,        0,   MEN_RELOAD      ),
//    M( NM_ITEM,   MSG_SEARCH,        0,   MEN_SEARCH      ),
//    MX( NM_SUB,   MSG_SEARCH,        0,   0, MEN_SEARCH        ),
//    MX( NM_SUB,   MSG_SEARCHUSER,    0,   0, MEN_SEARCHUSER    ),
    M( NM_ITEM,   MSG_USER,          0,   MEN_USER        ),
    MX( NM_SUB,   MSG_FOLLOW,        0,   0, MEN_FOLLOW        ),
//    MX( NM_SUB,   MSG_SHOW,          0,   0, MEN_SHOW          ),
    MX( NM_SUB,   MSG_BLOCK,         0,   0, MEN_BLOCK         ),
    MX( NM_SUB,   MSG_NOTIFY,        0,   0, MEN_NOTIFY        ),
    BAR,
    M( NM_ITEM,   MSG_DIRMSG,        0,   MEN_DIRMSG      ),
    MX( NM_SUB,   MSG_DIRMSG,        0,   0, MEN_DIRMSG        ),
//    MX( NM_SUB,   MSG_DIRMSGSENT,    0,   0, MEN_DIRMSGSENT    ),
//    MX( NM_SUB,   MSG_DIRMSGRCVD,    0,   0, MEN_DIRMSGRCVD    ),
    M( NM_ITEM,   MSG_TWEET,         0,   MEN_TWEET       ),
    MX( NM_SUB,   MSG_TWEET,         0,   0, MEN_TWEET         ),
    MX( NM_SUB,   MSG_MYTWEET,       0,   0, MEN_MYTWEET       ),
    MX( NM_SUB,   MSG_FAVS,          0,   0, MEN_FAVS          ),
    BAR,
    M( NM_ITEM,   MSG_QUIT,          0,   MEN_QUIT        ),

    M( NM_TITLE,  MSG_MISC,          0,   MEN_MISC        ),
    M( NM_ITEM,   MSG_FRIENDS,       0,   MEN_FRIENDS     ),
    M( NM_ITEM,   MSG_FOLLOWERS,     0,   MEN_FOLLOWERS   ),
    M( NM_ITEM,   MSG_BLOCKING,      0,   MEN_BLOCKING    ),
    BAR,
    M( NM_ITEM,   MSG_RANDOM,        0,   MEN_RANDOM      ),

    M( NM_TITLE,  MSG_TOOLS,         0,   MEN_TOOLS       ),
    M( NM_ITEM,   MSG_PREFS,         0,   MEN_PREFS       ),
    M( NM_ITEM,   MSG_USERPROFILE,   0,   MEN_USERPROFILE ),
    BAR,          
    M( NM_ITEM,   MSG_MUIPREFS,      0,   MEN_MUIPREFS    ),

    M( NM_TITLE,  MSG_HELP,          0,   MEN_HELP        ),
    M( NM_ITEM,   MSG_HELP2,         0,   MEN_HELP2       ),
    M( NM_ITEM,   MSG_DONATE,        0,   MEN_DONATE      ),
    M( NM_ITEM,   MSG_ABOUT,         0,   MEN_ABOUT       ),
    BAR,
    M( NM_ITEM,   MSG_ABOUTMUI,      0,   MEN_ABOUTMUI    ),
    M( NM_END,    NULL,              0,   0               ),

    MENU_END
};

/*****************************************************************************/

///

/// TheBar buttons ************************************************************/

/*
Object *appearance, *labelPos, *borderless, *sunny, *raised, *scaled, *update;
*/

enum {

    B_TIMELINE, B_RETWEETS, B_REPLIES,
    B_RELOAD, B_SEARCH, B_FOLLOW,
    B_DIRECTMESSAGE, B_TWEET,

    };

static struct MUIS_TheBar_Button buttons[] =
{
    {0, B_TIMELINE,      "_Timeline",       "Get most recent Tweets (max 20)"         ,0 },
    {1, B_RETWEETS,      "Ret_weets",       "Get most recent Retweets (max 20)"       ,0 },
    {2, B_REPLIES,       "@_Replies",       "Get most recent @Replies (max 20)"       ,0 },
    {3, B_RELOAD,        "Rel_oad",         "Reload current local file"               ,0 },
    {4, B_SEARCH,        "_Search",         "Search"                                  ,0 },
    {5, B_FOLLOW,        "_Users",          "Follow / Unfollow Users"                 ,0 },
    {6, B_DIRECTMESSAGE, "_Dir Msg",        "Send a Direct Message"                   ,0 },
    {7, B_TWEET,         "Tw_eet",          "Send a Tweet"                            ,0 },
    {MUIV_TheBar_End                                                                     },
};

// Buttons
/*
char *appearances[] = {"Images and text","Images","Text",NULL};
char *labelPoss[] = {"Bottom","Top","Right","Left",NULL};
char *borderlessSel[]={"On", "Off",NULL};
char *sunnySel[]={"Yes", "No",NULL};
char *raisedSel[]={"Don't Use", "Use",NULL};
char *scaledSel[]={"Large", "Small",NULL};
*/

STRPTR pics[] =
{
    "timeline",
    "retweets",
    "replies",
    "reload",
    "search",
    "follow",
    "directmessage",
    "tweet",
    NULL
};

/*****************************************************************************/

///

/// Libraries *****************************************************************/

// Set to running
BOOL running = TRUE;

BOOL Open_Libs(void ) {
    
    #ifdef __MORPHOS__

    if( !(SocketBase = OpenLibrary((CONST_STRPTR)"bsdsocket.library", 4)) ) {
        
        fprintf(stderr, "No TCP/IP Stack running!\n\a");
        CloseLibrary((struct Library *)SocketBase);
        return (0);
    }
    #endif
    

    if ((IntuitionBase=(struct IntuitionBase *) OpenLibrary((CONST_STRPTR)"intuition.library",37)) ) {

        #ifdef __amigaos4__
        if (!(IIntuition = (struct IntuitionIFace *) GetInterface((struct Library *)IntuitionBase,
                                                               "main",
                                                               1,
                                                               NULL)))
        {
            CloseLibrary((struct Library *)IntuitionBase);
            return 0;
        }
        #endif
    }
    else
        return(0);


    if ((GfxBase=(struct GfxBase *) OpenLibrary((CONST_STRPTR)"graphics.library",0)) ) {

        #ifdef __amigaos4__
        if (!(IGraphics = (struct GraphicsIFace *) GetInterface((struct Library *)GfxBase,
                                                               "main",
                                                               1,
                                                               NULL)))
        {
            CloseLibrary((struct Library *)GfxBase);

            DropInterface((struct Interface *)IIntuition);
            CloseLibrary((struct Library *)IntuitionBase);
            return 0;
        }
        #endif

    
    }
    else
    {
        #ifdef __amigaos4__
        DropInterface((struct Interface *)IIntuition);
        #endif
        CloseLibrary((struct Library *)IntuitionBase);
        return(0);
    }

    if ((MUIMasterBase=OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN)) ) {

        #ifdef __amigaos4__
        if (!(IMUIMaster = (struct MUIMasterIFace *) GetInterface((struct Library *)MUIMasterBase,
                                                               "main",
                                                               1,
                                                               NULL)))
        {
            CloseLibrary((struct Library *)MUIMasterBase);

            DropInterface((struct Interface *)IGraphics);
            CloseLibrary((struct Library *)GfxBase);

            DropInterface((struct Interface *)IIntuition);
            CloseLibrary((struct Library *)IntuitionBase);
            return 0;
        }
        #endif
     
    }
    else
    {
        #ifdef __amigaos4__
        DropInterface((struct Interface *)IIntuition);
        DropInterface((struct Interface *)IGraphics);
        #endif

        CloseLibrary((struct Library *)GfxBase);
        CloseLibrary((struct Library *)IntuitionBase);

        return(0);
    }

    return(1);
}

// Close libraries function
void Close_Libs(void ) {


    #ifdef __amigaos4__
    DropInterface((struct Interface *)IMUIMaster);
    DropInterface((struct Interface *)IIntuition);
    DropInterface((struct Interface *)IGraphics);
    #endif

    if ( IntuitionBase )
        CloseLibrary((struct Library *)IntuitionBase);

    if ( GfxBase )
        CloseLibrary((struct Library *)GfxBase);

    if ( MUIMasterBase )
        CloseLibrary(MUIMasterBase);

    #ifdef __MORPHOS__
    if ( SocketBase )
        CloseLibrary(SocketBase);        
    #endif
}

/*****************************************************************************/

/// 

/// My functions **************************************************************/

// clear Tweet
void do_clear(void) {
    set(STR_message, MUIA_String_Contents,0);
}

// clear direct message
void do_clear_dm(void) {
    set(STR_directmessage, MUIA_String_Contents,0);
}

// clear search
void do_clear_search(void) {
    set(STR_search, MUIA_String_Contents,0);
}

// clear follow
void do_clear_follow(void) {
    set(STR_follow, MUIA_String_Contents,0);
}

// clear block
void do_clear_block(void) {
    set(STR_block, MUIA_String_Contents,0);
}

// clear notify
void do_clear_notify(void) {
    set(STR_notify, MUIA_String_Contents,0);
}

// about HTML
void about(void) {
    set(txt_source, MUIA_HTMLtext_Contents, (int)HTML_INTRO);
}

// FAQ HTML
void help(void) {
    set(txt_source, MUIA_HTMLtext_Contents, (int)HTML_HELP);
}

// Error
void error(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int)"<B>Hmm...An error occurred!</B><p>Possible reasons:<ul><li>No internet connection</li><li>Twitter site down</li><li>Username/password incorrect</li>");
}

// Error2
void error2(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int)"<B>Hmm...An error occurred!</B><p>Possible reasons:<ul><li>No internet connection</li><li>Twitter site down</li><li>Username/password incorrect</li><li>Did you enter text (and/or correct recipient Screen Name for direct messages)?</li>");
}

// Error3
void error3(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int)"<B>Hmm...An error occurred!</B><p>Is your Username/password set correctly?");
}

// Error4
void error4(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int)"<B>Hmm...An error occurred!</B><p>Possible reasons:<ul><li>No internet connection</li><li>Twitter site down</li><li>Username/password incorrect</li><li>Did you enter the Screen Name correctly?</li><li>Are you already following (or unfollowing) that User?</li>");
}

// Error5
void error5(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int)"<B>Hmm...An error occurred!</B><p>Possible reasons:<ul><li>No internet connection</li><li>Twitter site down</li><li>Username/password incorrect</li><li>Did you enter the Screen Name correctly?</li><li>Are you already blocking (or not blocking) that User?</li>");
}

// Error6
void error6(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int)"<B>Hmm...An error occurred!</B><p>Possible reasons:<ul><li>No internet connection</li><li>Twitter site down</li><li>Username/password incorrect</li><li>Did you enter the Screen Name correctly?</li><li>Are you already receiving (or stopped receiving) SMS notifications for that User?</li>");
}

// Error7
void error7(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int)"<B>Hmm...An error occurred!</B><p>Possible reasons:<ul><li>No internet connection</li><li>Twitter site down</li><li>Username/password incorrect</li><li>You <b>*MUST*</b> specify a 'Name' to make any updates!</li>");
}


// urlTextObject
Object *urlTextObject(struct Library *MUIMasterBase,STRPTR url,STRPTR text,ULONG font) {
    return UrltextObject,
        MUIA_Font,          font,
        MUIA_Urltext_Text,  text,
        MUIA_Urltext_Url,   url,
    End;
}

/*****************************************************************************/

///

/// AmiTwitter ****************************************************************/

// New Twitter instance
twitter_t* twitter_new() {

    twitter_t *twitter;
    const char *home;

    home = "PROGDIR:";

    twitter = (twitter_t *)malloc(sizeof(twitter_t));
    twitter->base_uri = TWITTER_BASE_URI;
    twitter->base_search_uri = TWITTER_BASE_SEARCH_URI;
    twitter->user = username;
    twitter->pass = password;  
    twitter->source = "AmiTwitter";
    twitter->last_home_timeline = 1;
    twitter->last_user_timeline = 1;   
    twitter->mentions = 1;             
    twitter->last_friends_timeline = 1;
    twitter->last_followers_timeline = 1;
    twitter->last_public_timeline = 1; 
    twitter->last_dirmsgsent_timeline = 1;
    twitter->last_favs_timeline = 1;
    twitter->last_blocking_timeline = 1;
    twitter->fetch_interval = 60;
    twitter->show_interval = 5;
    twitter->alignment = 2;
    twitter->debug = 0;
    snprintf(twitter->images_dir, PATH_MAX, "%sdata/temp", home); 

    return twitter;
}

/*****************************************************************************/

// Cleanup
void twitter_free(twitter_t *twitter) {

    free(twitter);
    return;
}

/*****************************************************************************/

// Twitter Config
int twitter_config(twitter_t *twitter) {

    const char *home;

    home = getenv("HOME");
    if(!home) {
        return -1;
    }
    return 0;
}

/*****************************************************************************/

// Curl write
static size_t twitter_curl_write_cb(void *ptr, size_t size, size_t nmemb, void *data) {

    size_t realsize = size * nmemb;
    g_byte_array_append((GByteArray *)data, (guint8*)ptr, realsize);

    return realsize;
}

/*****************************************************************************/

// Get latest Tweets with cURL
int twitter_fetch(twitter_t *twitter, const char *apiuri, GByteArray *buf) {

    CURL *curl;
    CURLcode code;
    long res;
    char userpass[256];

    struct curl_slist *headers = NULL;

    headers = curl_slist_append(headers, "Content-Type: text/xml; charset=utf-8");
    headers = curl_slist_append(headers, "Accept: text/xml; charset=utf-8");

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);

    curl = curl_easy_init();
    if(!curl) {
        fprintf(stderr, "error: curl_easy_init()\n");
        error();
        return -1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, apiuri);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, TRUE); 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, TRUE);   
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, twitter_curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buf);

    code = curl_easy_perform(curl);

    if(code) {
        fprintf(stderr, "error: %s\n", curl_easy_strerror(code));
        error();
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {
        fprintf(stderr, "error respose code: %ld\n", res);
        error();
        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents, (int)"Attempting to get the latest...");
    }
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    return 0;
}

/*****************************************************************************/

// Send a new Tweet with cURL
int twitter_update(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    struct curl_slist *headers=NULL;
    GByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = g_byte_array_new();
    curl = curl_easy_init();
    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }
    snprintf(api_uri, PATH_MAX, "%s%s",
             twitter->base_uri, TWITTER_API_PATH_UPDATE);
    if(twitter->debug >= 2)
        printf("api_uri: %s\n", api_uri);

    headers = curl_slist_append(headers, "Expect:");

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME, "status",
                 CURLFORM_COPYCONTENTS, status,
                 CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME, "source",
                 CURLFORM_COPYCONTENTS, twitter->source,
                 CURLFORM_END);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, TRUE);   
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, TRUE);   
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, twitter_curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buf);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    code = curl_easy_perform(curl);
    if(code) {
        printf("error: %s\n", curl_easy_strerror(code));
        error2();
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {

        printf("error respose code: %ld\n", res);
        error2();

//      if(twitter->debug > 2){
            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");
//      }
        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents, (int)"Your Tweet was sent!");
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headers);

    return 0;
}

/*****************************************************************************/

// Follow a user with cURL (friendships/create API)
int twitter_follow(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    GByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = g_byte_array_new();
    curl = curl_easy_init();

    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

    get(STR_follow, MUIA_String_Contents, &text);

    snprintf(api_uri, PATH_MAX, "%s%s%s%s",
             twitter->base_uri, TWITTER_API_PATH_FOLLOW, text, ".xml");

//    if(twitter->debug >= 2)
        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, TRUE);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, TRUE);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, twitter_curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buf);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

    code = curl_easy_perform(curl);
    if(code) {
        printf("error: %s\n", curl_easy_strerror(code));
        error2();
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {

        printf("error respose code: %ld\n", res);
        error4();

//      if(twitter->debug > 2){
            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");
//      }
        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents, (int)"You are now <b>Following</b> that User!");
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);

    return 0;
}

/*****************************************************************************/

// Unfollow a user with cURL (friendships/destroy API)
int twitter_unfollow(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    GByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = g_byte_array_new();
    curl = curl_easy_init();

    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

    get(STR_follow, MUIA_String_Contents, &text);

    snprintf(api_uri, PATH_MAX, "%s%s%s%s",
             twitter->base_uri, TWITTER_API_PATH_UNFOLLOW, text, ".xml");

//    if(twitter->debug >= 2)
        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, TRUE);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, TRUE);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, twitter_curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buf);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

    code = curl_easy_perform(curl);
    if(code) {
        printf("error: %s\n", curl_easy_strerror(code));
        error2();
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {

        printf("error respose code: %ld\n", res);
        error4();

//      if(twitter->debug > 2){
            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");
//      }
        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents, (int)"You are now <b>Unfollowing</b> that User!");
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);

    return 0;
}

/*****************************************************************************/

// Block a user with cURL (blocks/create API)
int twitter_block(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    GByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = g_byte_array_new();
    curl = curl_easy_init();

    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

    get(STR_block, MUIA_String_Contents, &text);

    snprintf(api_uri, PATH_MAX, "%s%s%s%s",
             twitter->base_uri, TWITTER_API_PATH_BLOCK, text, ".xml");

//    if(twitter->debug >= 2)
        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, TRUE);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, TRUE);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, twitter_curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buf);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

    code = curl_easy_perform(curl);
    if(code) {
        printf("error: %s\n", curl_easy_strerror(code));
        error2();
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {

        printf("error respose code: %ld\n", res);
        error5();

//      if(twitter->debug > 2){
            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");
//      }
        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents, (int)"You are now <b>Blocking</b> that User!");
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);

    return 0;
}

/*****************************************************************************/

// Unblocks a user with cURL (blocks/destroy API)
int twitter_unblock(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    GByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = g_byte_array_new();
    curl = curl_easy_init();

    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

    get(STR_block, MUIA_String_Contents, &text);

    snprintf(api_uri, PATH_MAX, "%s%s%s%s",
             twitter->base_uri, TWITTER_API_PATH_UNBLOCK, text, ".xml");

//    if(twitter->debug >= 2)
        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, TRUE);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, TRUE);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, twitter_curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buf);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

    code = curl_easy_perform(curl);
    if(code) {
        printf("error: %s\n", curl_easy_strerror(code));
        error2();
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {

        printf("error respose code: %ld\n", res);
        error5();

//      if(twitter->debug > 2){
            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");
//      }
        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents, (int)"You are now <b>Unblocking</b> that User!");
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);

    return 0;
}

/*****************************************************************************/

// Notifications (follow) a user with cURL (notifications/follow API)
int twitter_notify(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    GByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = g_byte_array_new();
    curl = curl_easy_init();

    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

    get(STR_notify, MUIA_String_Contents, &text);

    snprintf(api_uri, PATH_MAX, "%s%s%s%s",
             twitter->base_uri, TWITTER_API_PATH_NOTIFY, text, ".xml");

//    if(twitter->debug >= 2)
        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, TRUE);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, TRUE);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, twitter_curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buf);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

    code = curl_easy_perform(curl);
    if(code) {
        printf("error: %s\n", curl_easy_strerror(code));
        error2();
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {

        printf("error respose code: %ld\n", res);
        error6();

//      if(twitter->debug > 2){
            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");
//      }
        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents, (int)"You are now receiving Notifications (SMS following) that User!");
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);

    return 0;
}

/*****************************************************************************/

// Notifications (leave) Unfollows a user with cURL (notifications/leave API)
int twitter_unnotify(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    GByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = g_byte_array_new();
    curl = curl_easy_init();

    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

    get(STR_notify, MUIA_String_Contents, &text);

    snprintf(api_uri, PATH_MAX, "%s%s%s%s",
             twitter->base_uri, TWITTER_API_PATH_UNNOTIFY, text, ".xml");

//    if(twitter->debug >= 2)
        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, TRUE);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, TRUE);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, twitter_curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buf);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

    code = curl_easy_perform(curl);
    if(code) {
        printf("error: %s\n", curl_easy_strerror(code));
        error2();
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {

        printf("error respose code: %ld\n", res);
        error6();

//      if(twitter->debug > 2){
            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");
//      }
        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents, (int)"You are no longer receiving notifications (SMS following) that User!");

    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);

    return 0;
}

/*****************************************************************************/

// Update User Profile (account/update_profile API)
int twitter_updateprofile(twitter_t *twitter, const char *name, const char *web, const char *location, const char *bio) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    struct curl_slist *headers=NULL;

    GByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = g_byte_array_new();
    curl = curl_easy_init();

    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

    get(STR_profile_name, MUIA_String_Contents, &name);
    get(STR_profile_web, MUIA_String_Contents, &web);
    get(STR_profile_location, MUIA_String_Contents, &location);
    get(STR_profile_bio, MUIA_String_Contents, &bio);

    snprintf(api_uri, PATH_MAX, "%s%s",
             twitter->base_uri, TWITTER_API_PATH_UPDATEPROFILE);

    headers = curl_slist_append(headers, "Expect:"); 

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME, "name",
                 CURLFORM_COPYCONTENTS, name,
                 CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME, "url",
                 CURLFORM_COPYCONTENTS, web,
                 CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME, "location",
                 CURLFORM_COPYCONTENTS, location,
                 CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME, "description",
                 CURLFORM_COPYCONTENTS, bio,
                 CURLFORM_END);

//    if(twitter->debug >= 2)
        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, TRUE);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, TRUE);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, twitter_curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buf);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    code = curl_easy_perform(curl);
    if(code) {
        printf("error: %s\n", curl_easy_strerror(code));
        error2();
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {

        printf("error respose code: %ld\n", res);
        error7();

//      if(twitter->debug > 2){
            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");
//      }
        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents, (int)"<B>Congratulations!</B> You have updated your profile!");

    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headers);

    return 0;
}
/*****************************************************************************/

// Send a Direct Message with cURL
int twitter_direct_message(twitter_t *twitter, const char *screen_name, const char *text) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    struct curl_slist *headers=NULL;
    GByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = g_byte_array_new();
    curl = curl_easy_init();
    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }
    snprintf(api_uri, PATH_MAX, "%s%s", twitter->base_uri, TWITTER_API_PATH_DIRECT_MESSAGE);
    if(twitter->debug >= 2)
        printf("api_uri: %s\n", api_uri);

    headers = curl_slist_append(headers, "Expect:");

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME, "screen_name",
                 CURLFORM_COPYCONTENTS, screen_name,
                 CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME, "text",
                 CURLFORM_COPYCONTENTS, text, 
                 CURLFORM_END);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, TRUE);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, TRUE);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, twitter_curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buf);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    code = curl_easy_perform(curl);
    if(code) {
        printf("error: %s\n", curl_easy_strerror(code));
        error2();
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {
        printf("error respose code: %ld\n", res);

        error2();

//      if(twitter->debug > 2){
            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");
//      }
        return res;
    }
    else {

        set (txt_source, MUIA_HTMLtext_Contents, (int)"Your Direct Message was sent!");
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headers);

    return 0;
}

/*****************************************************************************/

// Verify Credentials with cURL
int twitter_verify_credentials(twitter_t *twitter, const char *screen_name, const char *text) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    GByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = g_byte_array_new();
    curl = curl_easy_init();
    if(!curl) {
        printf("error: curl_easy_init()\n");
        error3();
        return -1;
    }
    snprintf(api_uri, PATH_MAX, "%s%s", twitter->base_uri, TWITTER_API_PATH_VERIFY_CREDENTIALS);
    if(twitter->debug >= 2)
        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, TRUE);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, TRUE);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, twitter_curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buf);

    code = curl_easy_perform(curl);
    if(code) {
        printf("error: %s\n", curl_easy_strerror(code));
        error3();
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);

     // Pulls xml data
    // curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &res);

    if(res != 200) {
        printf("error respose code: %ld\n", res);

        error3();

//      if(twitter->debug > 2){
            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");
//      }
        return res;
    }
    else {

       set (txt_source, MUIA_HTMLtext_Contents, (int)"<B>Congratulations!</B> Your username/password is valid!");
    }

    curl_easy_cleanup(curl);

    return 0;
}

/*****************************************************************************/

// statuses/home_timeline API
GList* twitter_home_timeline(twitter_t *twitter) {

    int ret;
    GList *timeline = NULL;
    GByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_HOME_TIMELINE,
             twitter->last_home_timeline);
//  if(twitter->debug > 1)
        printf("api_uri: %s\n", api_uri);

    buf = g_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);
/*  if(ret){
        printf("ERROR: twitter_fetch()\n");
        return NULL;
    } */

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);


    g_byte_array_free (buf, TRUE);
//  xmlMemoryDump();

    if(timeline){
        status = timeline->data;

        twitter->last_home_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// statuses/user_timeline API
GList* twitter_user_timeline(twitter_t *twitter) {

    int ret;
    GList *timeline = NULL;
    GByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_USER_TIMELINE,
             twitter->last_user_timeline);
//  if(twitter->debug > 1)
        printf("api_uri: %s\n", api_uri);

    buf = g_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);
/*  if(ret){
        printf("ERROR: twitter_fetch()\n");
        return NULL;
    } */

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);


    g_byte_array_free (buf, TRUE);
//  xmlMemoryDump();

    if(timeline){
        status = timeline->data;

        twitter->last_user_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// statuses/mentions API
GList* twitter_mentions(twitter_t *twitter) {

    int ret;
    GList *timeline = NULL;
    GByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_MENTIONS,
             twitter->mentions);
//  if(twitter->debug > 1)
        printf("api_uri: %s\n", api_uri);

    buf = g_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);
/*  if(ret){
        printf("ERROR: twitter_fetch()\n");
        return NULL;
    } */

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);


    g_byte_array_free (buf, TRUE);
//  xmlMemoryDump();

    if(timeline){
        status = timeline->data;

        twitter->mentions = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// statuses/public_timeline API
GList* twitter_public_timeline(twitter_t *twitter) {

    int ret;
    GList *timeline = NULL;
    GByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_PUBLIC_TIMELINE,
             twitter->last_public_timeline);
//  if(twitter->debug > 1)
        printf("api_uri: %s\n", api_uri);

    buf = g_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);
/*  if(ret){
        printf("ERROR: twitter_fetch()\n");
        return NULL;
    } */

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);


    g_byte_array_free (buf, TRUE);
//  xmlMemoryDump();

    if(timeline){
        status = timeline->data;

        twitter->last_home_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// statuses/friends API
GList* twitter_friends_timeline(twitter_t *twitter) {

    int ret;
    GList *timeline = NULL;
    GByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_FRIENDS,
             twitter->last_friends_timeline);
//  if(twitter->debug > 1)
        printf("api_uri: %s\n", api_uri);

    buf = g_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);
/*  if(ret){
        printf("ERROR: twitter_fetch()\n");
        return NULL;
    } */

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);


    g_byte_array_free (buf, TRUE);
//  xmlMemoryDump();

    if(timeline){
        status = timeline->data;

        twitter->last_friends_timeline = atol(status->id);       
    }
    return timeline;
}

/*****************************************************************************/

// statuses/followers API
GList* twitter_followers_timeline(twitter_t *twitter) {

    int ret;
    GList *timeline = NULL;
    GByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_FOLLOWERS,
             twitter->last_followers_timeline);
//  if(twitter->debug > 1)
        printf("api_uri: %s\n", api_uri);

    buf = g_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);
/*  if(ret){
        printf("ERROR: twitter_fetch()\n");
        return NULL;
    } */

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);


    g_byte_array_free (buf, TRUE);
//  xmlMemoryDump();

    if(timeline){
        status = timeline->data;

        twitter->last_followers_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// blocks/blocking API
GList* twitter_blocking_timeline(twitter_t *twitter) {

    int ret;
    GList *timeline = NULL;
    GByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_BLOCKING,
             twitter->last_blocking_timeline);
//  if(twitter->debug > 1)
        printf("api_uri: %s\n", api_uri);

    buf = g_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);
/*  if(ret){
        printf("ERROR: twitter_fetch()\n");
        return NULL;
    } */

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);


    g_byte_array_free (buf, TRUE);
//  xmlMemoryDump();

    if(timeline){
        status = timeline->data;

        twitter->last_blocking_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// statuses/twitter_retweeted_by_me API
GList* twitter_retweeted_by_me(twitter_t *twitter) {

    int ret;
    GList *timeline = NULL;
    GByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_RETWEETED_BY_ME,
             twitter->last_public_timeline);
//  if(twitter->debug > 1)
        printf("api_uri: %s\n", api_uri);

    buf = g_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);
/*  if(ret){
        printf("ERROR: twitter_fetch()\n");
        return NULL;
    } */

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);


    g_byte_array_free (buf, TRUE);
//  xmlMemoryDump();

    if(timeline){
        status = timeline->data;

        twitter->last_home_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// statuses/twitter_retweeted_to_me API
GList* twitter_retweeted_to_me(twitter_t *twitter) {

    int ret;
    GList *timeline = NULL;
    GByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_RETWEETED_TO_ME,
             twitter->last_public_timeline);
//  if(twitter->debug > 1)
        printf("api_uri: %s\n", api_uri);

    buf = g_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);
/*  if(ret){
        printf("ERROR: twitter_fetch()\n");
        return NULL;
    } */

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);


    g_byte_array_free (buf, TRUE);
//  xmlMemoryDump();

    if(timeline){
        status = timeline->data;

        twitter->last_home_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// statuses/twitter_retweets_of_me API
GList* twitter_retweets_of_me(twitter_t *twitter) {

    int ret;
    GList *timeline = NULL;
    GByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_RETWEETS_OF_ME,
             twitter->last_public_timeline);
//  if(twitter->debug > 1)
        printf("api_uri: %s\n", api_uri);

    buf = g_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);
/*  if(ret){
        printf("ERROR: twitter_fetch()\n");
        return NULL;
    } */

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);


    g_byte_array_free (buf, TRUE);
//  xmlMemoryDump();

    if(timeline){
        status = timeline->data;

        twitter->last_home_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Direct Messages/sent API *** NOT WORKING YET ***
GList* twitter_dirmsgsent(twitter_t *twitter) {

    int ret;
    GList *timeline = NULL;
    GByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_DIRECT_MSGSENT,
             twitter->last_dirmsgsent_timeline);
//  if(twitter->debug > 1)
        printf("api_uri: %s\n", api_uri);

    buf = g_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);
/*  if(ret){
        printf("ERROR: twitter_fetch()\n");
        return NULL;
    } */

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);


    g_byte_array_free (buf, TRUE);
//  xmlMemoryDump();

    if(timeline){
        status = timeline->data;

        twitter->last_dirmsgsent_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Favorites API
GList* twitter_favs(twitter_t *twitter) {

    int ret;
    GList *timeline = NULL;
    GByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_FAVS,
             twitter->last_favs_timeline);
//  if(twitter->debug > 1)
        printf("api_uri: %s\n", api_uri);

    buf = g_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);
/*  if(ret){
        printf("ERROR: twitter_fetch()\n");
        return NULL;
    } */

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);


    g_byte_array_free (buf, TRUE);
//  xmlMemoryDump();

    if(timeline){
        status = timeline->data;

        twitter->last_favs_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// XML parse Statuses node
GList* twitter_parse_statuses_node(xmlTextReaderPtr reader) {

    int ret;
    xmlElementType type;
    xmlChar *name;
    GList* statuses = NULL;
    twitter_status_t *status;

    do {
        ret = xmlTextReaderRead(reader);
        type = xmlTextReaderNodeType(reader);
        if(type == XML_READER_TYPE_ELEMENT) {
            name = xmlTextReaderName(reader);
            if(!xmlStrcmp(name, (xmlChar *)"status" /*statuses" || "direct-messages"*/)) {
            //added es" || "direct-messages"
                status = twitter_parse_status_node(reader);
                if(status) {
                    statuses = g_list_append(statuses, status);
                }
            }
            xmlFree(name);
        }
    }while(ret == 1);
    return statuses;
}

/*****************************************************************************/

// XML parse Status node
twitter_status_t* twitter_parse_status_node(xmlTextReaderPtr reader) {
    int ret;
    xmlElementType type;
    xmlChar *name;
    twitter_status_t *status;
    status = (twitter_status_t *)malloc(sizeof(twitter_status_t));
    memset(status, 0, sizeof(twitter_status_t));

    do{
        ret = xmlTextReaderRead(reader);
        type = xmlTextReaderNodeType(reader);
        if (type == XML_READER_TYPE_ELEMENT) {
            name = xmlTextReaderName(reader);
            if (!xmlStrcmp(name, (xmlChar *)"created_at")) {
                ret = xmlTextReaderRead(reader);
                status->created_at = (const char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"id")) {
                ret = xmlTextReaderRead(reader);
                status->id = (const char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"text")) {
                ret = xmlTextReaderRead(reader);
                status->text = (const char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"source")) {
                ret = xmlTextReaderRead(reader);
                status->source = (const char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"truncated")) {
                ret = xmlTextReaderRead(reader);
                status->truncated = (const char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"in_reply_to_status_id")) {
                ret = xmlTextReaderRead(reader);
                status->in_reply_to_status_id = (const char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"in_reply_to_user_id")) {
                ret = xmlTextReaderRead(reader);
                status->in_reply_to_user_id = (const char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"favorited")) {
                ret = xmlTextReaderRead(reader);
                status->favorited = (const char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"in_reply_to_screen_name")) {
                ret = xmlTextReaderRead(reader);
                status->in_reply_to_screen_name = (const char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"retweeted_status")) {
                ret = xmlTextReaderRead(reader);
                status->retweeted_status = (const char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"user")) {
                status->user = twitter_parse_user_node(reader);
            }
            xmlFree(name);
        } else if (type == XML_READER_TYPE_END_ELEMENT) {
            name = xmlTextReaderName(reader);
            if (!xmlStrcmp(name, (xmlChar *)"status" /*|| "direct_message"*/)) {
                xmlFree(name);
                break;
            }
            xmlFree(name);
        }
    }while(ret == 1);
    return status;
}

/*****************************************************************************/

// XML parse User node
twitter_user_t* twitter_parse_user_node(xmlTextReaderPtr reader) {
    int ret;
    xmlElementType type;
    xmlChar *name;
    twitter_user_t *user;

    user = (twitter_user_t *)malloc(sizeof(twitter_user_t));
    memset(user, 0, sizeof(twitter_user_t));
    do{
        ret = xmlTextReaderRead(reader);
        type = xmlTextReaderNodeType(reader);
        if (type == XML_READER_TYPE_ELEMENT) {
            name = xmlTextReaderName(reader);
            if(!xmlStrcmp(name, (xmlChar *)"id")) {
                ret = xmlTextReaderRead(reader);
                user->id = (const char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"name")) {
                ret = xmlTextReaderRead(reader);
                user->name = (const char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"screen_name")) {
                ret = xmlTextReaderRead(reader);
                user->screen_name = (const char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"location")) {
                ret = xmlTextReaderRead(reader);
                user->location = (const char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"description")) {
                ret = xmlTextReaderRead(reader);
                user->description = (const char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"profile_image_url")) {
                ret = xmlTextReaderRead(reader);
                user->profile_image_url = (const char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"followers_count")) {
                ret = xmlTextReaderRead(reader);
                user->followers_count = (const char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"friends_count")) {
                ret = xmlTextReaderRead(reader);
                user->friends_count = (const char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"favourites_count")) {
                ret = xmlTextReaderRead(reader);
                user->favourites_count = (const char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"statuses_count")) {
                ret = xmlTextReaderRead(reader);
                user->statuses_count = (const char *)xmlTextReaderValue(reader);
            }
            xmlFree(name);
        }else if(type == XML_READER_TYPE_END_ELEMENT) {
            name = xmlTextReaderName(reader);
            if(!xmlStrcmp(name, (xmlChar *)"user" /*|| "sender"*/)) {
                xmlFree(name);
                break;
            }
            xmlFree(name);
        }
    }while(ret == 1);
    return user;
}

/*****************************************************************************/

// Display in HTMLtext .mcc (timelines)
void twitter_status_print(twitter_status_t *status) {
                                                                                                                                  
    FILE *outfile;
        
    outfile = freopen("PROGDIR:data/temp/twitter.html", "a+", stdout);

    printf("<IMG SRC=PROGDIR:data/temp/%s><p> <b>%s </b> %s <p><small>%s </small><br>",status->user->id, status->user->screen_name, status->text, status->created_at);
    printf("<small>Name: %s Location: %s Following: %s Followers: %s Tweets: %s",status->user->name, status->user->location, status->user->friends_count, status->user->followers_count,  status->user->statuses_count);
    printf("<p>");

    fclose(stdout);
}

/*****************************************************************************/

// Display in HTMLtext .mcc (friends and followers)
void twitter_status_print_friendsfollowers(twitter_status_t *status) {

    FILE *outfile;

    outfile = freopen("PROGDIR:data/temp/twitter.html", "a+", stdout);
    printf("<b>User ID:</b> %s<br><b>Last Tweet:</b> %s<br> <b>Date:</b><small>  %s</small></b><br><p>",  status->id, status->text, status->created_at);
    printf("<p>");

    fclose(stdout);
}

/*****************************************************************************/

// Free statuses
void twitter_statuses_free(GList *statuses) {
    GList *l = statuses;
    twitter_status_t *status;
    if(!statuses){
        return;
    }
    do{
        status = l->data;
        if(!status) {
            continue;
        }
        free((void*)(status->created_at));
        free((void*)(status->id));
        free((void*)(status->text));
        free((void*)(status->source));
        free((void*)(status->truncated));
        free((void*)(status->in_reply_to_status_id));
        free((void*)(status->in_reply_to_user_id));
        free((void*)(status->favorited));
        free((void*)(status->in_reply_to_screen_name));
        free((void*)(status->retweeted_status));

        if(status->user){
            free((void*)(status->user->id));
            free((void*)(status->user->name));
            free((void*)(status->user->screen_name));
            free((void*)(status->user->location));
            free((void*)(status->user->description));
            free((void*)(status->user->profile_image_url));
            free((void*)(status->user->followers_count));
            free((void*)(status->user->friends_count));
            free((void*)(status->user->favourites_count));
            free((void*)(status->user->statuses_count));
            free((void*)(status->user));
        }
        free(status);
    }while((l = g_list_next(l)));
    g_list_free(statuses);
}

/*****************************************************************************/

// Image name
int twitter_image_name(twitter_status_t *status, char *name) {
    size_t i;
    i = strlen(status->user->profile_image_url);
    while(i--)
        if(status->user->profile_image_url[i] == '/')       
            break;
    while(i--)
        if(status->user->profile_image_url[i] == '/')        
            break;
    i++;
    strncpy(name, status->user->profile_image_url + i, PATH_MAX - 1);
    i = strlen(name);
    while(i--)
        if(name[i] == '/')    
         name[i] = '_';

      // added to truncate image name for displaying in HTMLtext .mcc
         name[i] = strncpy(name, status->user->id, PATH_MAX);
         
    return 0;
}

/*****************************************************************************/

// Fetch images
int twitter_fetch_images(twitter_t *twitter, GList *statuses) {
    int ret;
    twitter_status_t *status;
    const char *url;
    char name[PATH_MAX];
    char path[PATH_MAX];
    struct stat st;

    statuses = g_list_last(statuses);
    if(!statuses) {
        return 0;
    }

    do{
        status = statuses->data;
        twitter_image_name(status, name);
        url = status->user->profile_image_url;
        snprintf(path, PATH_MAX, "%s/%s", twitter->images_dir, name);
        ret = stat(path, &st);
    //  if(ret) {
        //  if(twitter->debug)
            printf("fetch_image: %s\n", url);
            twitter_fetch_image(twitter, url, path);
    //  }
    }while((statuses = g_list_previous(statuses)));

    return 0;
}

/*****************************************************************************/

// Twitter cURL File
static size_t twitter_curl_file_cb(void *ptr, size_t size, size_t nmemb, void *data) {

    size_t realsize = size * nmemb;
    fwrite(ptr, size, nmemb, (FILE*)data);

    return realsize;
}

/*****************************************************************************/

// cURL Fetch image
int twitter_fetch_image(twitter_t *twitter, const char *url, char* path) {

    CURL *curl;
    CURLcode code;
    long res;
    FILE *fp;
    int i;
    char *esc;
    char escaped_url[PATH_MAX];

    fp = fopen(path, "w");
    if(!fp) {
        fprintf(stderr, "error: can't openfile %s\n", path);
        error();
        return -1;
    }

    curl = curl_easy_init();
    if(!curl) {
        fprintf(stderr, "error: curl_easy_init()\n");
        error();
        return -1;
    }

    /* url escape */
    i = strlen(url);
    while(i-- && url[i] != '/');

    /* esc = curl_easy_escape(curl, url + i + 1, 0); */
    /* changed to line below for curl 7.14.0 compatability */
    esc = curl_escape(url + i + 1, 0);

    strncpy(escaped_url, url, PATH_MAX - 1);
    strncpy(escaped_url + i + 1, esc, PATH_MAX - i);
//  curl_free(esc);

    curl_easy_setopt(curl, CURLOPT_URL, escaped_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, twitter_curl_file_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)fp);

    code = curl_easy_perform(curl);
    if(code) {
        fprintf(stderr, "error: %s\n", curl_easy_strerror(code));
        error();
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {
        fprintf(stderr, "error respose code: %ld\n", res);
        error();
        return res;
    }
    fclose(fp);
    return 0;
}

/*****************************************************************************/

// UFT8
int utf8pos(const char *str, int width) {
    int i=0;
    unsigned char c;
    while(str[i] && width > 1){
        c = (unsigned char)str[i];
        width--;
        if(c < 0x80){
            i++;
            continue;
        }
        width--;
        while(c & 0x80){
            c<<=1;
            i++;
        }
    }
    return i;
}

/*****************************************************************************/

// Show timeline
void amitwitter_show_timeline(twitter_t *twitter, GList *statuses) {
    twitter_status_t *status;
    statuses = g_list_last(statuses);
    if(!statuses) {
        return;
    }
    do{
        status = statuses->data;

     // if(twitter->debug)
            twitter_status_print(status);

    }while((statuses = g_list_previous(statuses)));

        set (txt_source, MUIA_HTMLtext_URL, (int)"PROGDIR:data/temp/twitter.html");
}

/*****************************************************************************/

// Show timeline friends/followers
void amitwitter_show_timeline_friendsfollowers(twitter_t *twitter, GList *statuses) {
    twitter_status_t *status;
    statuses = g_list_last(statuses);
    if(!statuses) {
        return;
    }
    do{
        status = statuses->data;

     // if(twitter->debug)
            twitter_status_print_friendsfollowers(status);

    }while((statuses = g_list_previous(statuses)));

        set (txt_source, MUIA_HTMLtext_URL, (int)"PROGDIR:data/temp/twitter.html");
}

/*****************************************************************************/

// Get most recent Tweets (statuses/home_timeline API)
void amitwitter_loop() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

/*
    if(!twitter->debug) {
        timeline = twitter_home_timeline(twitter);
        twitter_statuses_free(timeline);
    } */

// original 'while' loop.  my loop below gets content once....

 /* while(1) {
        timeline = twitter_home_timeline(twitter);
        if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        twitter_fetch_images(twitter, timeline);
        amitwitter_show_timeline(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
        sleep(twitter->fetch_interval);
    } */

// my 'for' loop, pulls data once

    for(i=1; i<2; i++) {
        timeline = twitter_home_timeline(twitter);
//      if(twitter->debug >= 2)

            printf("timeline num: %d\n", g_list_length(timeline));

        twitter_fetch_images(twitter, timeline);
        amitwitter_show_timeline(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    }

    twitter_free(twitter);
}

/*****************************************************************************/

// Show users most recent messages (statuses/user_timeline API)
void amitwitter_user_loop() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_user_timeline(twitter);
//      if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        twitter_fetch_images(twitter, timeline);
        amitwitter_show_timeline(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    }

    twitter_free(twitter);
}

/*****************************************************************************/

// Shows all mentions @Replies (statuses/mentions API)
void amitwitter_mentions_loop() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_mentions(twitter);
//      if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        twitter_fetch_images(twitter, timeline);
        amitwitter_show_timeline(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    }

     twitter_free(twitter);
}

/*****************************************************************************/

// Shows retweeted by me (statuses/retweeted_by_me API)
void amitwitter_retweeted_by_me() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_retweeted_by_me(twitter);
//      if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        twitter_fetch_images(twitter, timeline);
        amitwitter_show_timeline(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    }

    twitter_free(twitter);
}

/*****************************************************************************/

// Shows retweeted to me (statuses/retweeted_to_me API)
void amitwitter_retweeted_to_me() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_retweeted_to_me(twitter);
//      if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        twitter_fetch_images(twitter, timeline);
        amitwitter_show_timeline(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    }

    twitter_free(twitter);
}

/*****************************************************************************/

// Shows retweets of me (statuses/retweets_of_me API)
void amitwitter_retweets_of_me() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_retweets_of_me(twitter);
//      if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        twitter_fetch_images(twitter, timeline);
        amitwitter_show_timeline(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    }

    twitter_free(twitter);
}

/*****************************************************************************/

// Shows Direct Messages sent API   *** NOT WORKING YET ***
void amitwitter_dirmsgsent() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_dirmsgsent(twitter);
//      if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        twitter_fetch_images(twitter, timeline);
        amitwitter_show_timeline(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    }

    twitter_free(twitter);
}

/*****************************************************************************/

// Shows Favorites API  
void amitwitter_favs() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_favs(twitter);
//      if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        twitter_fetch_images(twitter, timeline);
        amitwitter_show_timeline(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    }

    twitter_free(twitter);
}

/*****************************************************************************/

// Shows friends (statuses/friends API)
void amitwitter_friends_loop() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_friends_timeline(twitter);
//      if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        amitwitter_show_timeline_friendsfollowers(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    }

    twitter_free(twitter);
}

/*****************************************************************************/

// Shows followers (statuses/followers API)
void amitwitter_followers_loop() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_followers_timeline(twitter);
//      if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        amitwitter_show_timeline_friendsfollowers(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    }

    twitter_free(twitter);
}

/*****************************************************************************/

// Shows blocking (blocks/blocking API)
void amitwitter_blocking_loop() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_blocking_timeline(twitter);
//      if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        amitwitter_show_timeline_friendsfollowers(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    }

    twitter_free(twitter);
}

/*****************************************************************************/

// Shows most recent random messages (statuses/public_timeline API)
void amitwitter_public_loop() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_public_timeline(twitter);
//      if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        twitter_fetch_images(twitter, timeline);
        amitwitter_show_timeline(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    }

    twitter_free(twitter);
}

/*****************************************************************************/

// Send a Tweet
void amitwitter_update(const char *text) {

    twitter_t *twitter = NULL;

    fprintf(stdout, "updating...");
    fflush(stdout);

    get(STR_message, MUIA_String_Contents, &text);

    printf("Message is %u characters long.\n", strlen(text));

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_update(twitter, text);
    twitter_free(twitter);

    fprintf(stdout, "done\n");
}

/*****************************************************************************/

// Follow a User (friendships/create API)
void amitwitter_follow(const char *text) {

    twitter_t *twitter = NULL;

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_follow(twitter, text);
    twitter_free(twitter);
}

/*****************************************************************************/

// Unfollow a User (friendships/destroy API)
void amitwitter_unfollow(const char *text) {

    twitter_t *twitter = NULL;

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_unfollow(twitter, text);
    twitter_free(twitter);
}

/*****************************************************************************/

// Block a User (blocks/create API)
void amitwitter_block(const char *text) {

    twitter_t *twitter = NULL;

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_block(twitter, text);
    twitter_free(twitter);
}

/*****************************************************************************/

// Unblock a User (blocks/destroy API)
void amitwitter_unblock(const char *text) {

    twitter_t *twitter = NULL;

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_unblock(twitter, text);
    twitter_free(twitter);
}

/*****************************************************************************/

// Notifications follow a User with SMS (notifications/follow API)
void amitwitter_notify(const char *text) {

    twitter_t *twitter = NULL;

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_notify(twitter, text);
    twitter_free(twitter);
}

/*****************************************************************************/

// Notifications unfollow a User with SMS (notifications/leave API)
void amitwitter_unnotify(const char *text) {

    twitter_t *twitter = NULL;

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_unnotify(twitter, text);
    twitter_free(twitter);
}

/*****************************************************************************/

// Direct Messages
void amitwitter_direct_message(const char *screen_name, const char *text) {

    twitter_t *twitter = NULL;

    fprintf(stdout, "sending...");
    fflush(stdout);
   
    get(STR_user_id, MUIA_String_Contents, &screen_name);
    get(STR_directmessage, MUIA_String_Contents, &text);

    printf("Message is %u characters long.\n", strlen(text));

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_direct_message(twitter, screen_name, text);
    twitter_free(twitter);

    fprintf(stdout, "done\n");
}

/*****************************************************************************/

// Update profile
void amitwitter_updateprofile(const char *name, const char *web, const char *location, const char *bio) {

    twitter_t *twitter = NULL;

    fprintf(stdout, "updating profile...");
    fflush(stdout);

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_updateprofile(twitter, name, web, location, bio);
    twitter_free(twitter);

    fprintf(stdout, "done\n");
}

/*****************************************************************************/

// Verify Credentials
void amitwitter_verify_credentials(const char *screen_name, const char *text) {

    twitter_t *twitter = NULL;

    get(STR_user_id, MUIA_String_Contents, &screen_name);
    get(STR_directmessage, MUIA_String_Contents, &text);

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_verify_credentials(twitter, screen_name, text);
    twitter_free(twitter);

}

/*****************************************************************************/

///

/// Main Program **************************************************************/

int main(int argc, char *argv[]) {

  APTR app;

  if ( ! Open_Libs() ) {

    printf("Cannot open libs\n");
    return(0);
  }

  app = ApplicationObject,
      MUIA_Application_Title  , "AmiTwitter",
      MUIA_Application_Version , "$VER: "VSTRING"",
      MUIA_Application_Copyright , "©2009, IKE",
      MUIA_Application_Author  , "IKE",
      MUIA_Application_Description, "A Twitter program",
      MUIA_Application_Base  , "AmiTwitter",

/*****************************************************************************/

      // The Preferences Window
      SubWindow, win_preferences = WindowObject,
          MUIA_Window_Title, "Settings",
          MUIA_Window_ID, MAKE_ID('P','R','E','F'),

          WindowContents, VGroup,

              Child, VGroup,

                  Child, ColGroup(2), GroupFrameT("Account Information"),
                       Child, Label2("User Name:"),
                       Child, str_user_pref = StringObject, StringFrame, MUIA_ObjectID, 1, End,
                       Child, Label2("Password:"),
                       Child, str_pass_pref = StringObject, StringFrame,  MUIA_ObjectID, 2, MUIA_String_Secret, TRUE, End,
                  End,

              End,

              Child, HGroup, MUIA_Group_SameSize,  TRUE,
                  Child, but_save   = MUI_MakeObject(MUIO_Button,"_Save"),   
                  Child, but_test   = MUI_MakeObject(MUIO_Button,"_Test"),
                  Child, but_cancel = MUI_MakeObject(MUIO_Button,"_Cancel"), 
              End,

              Child, VGroup, GroupFrameT("Help!"),

                  MUIA_Background, MUII_GroupBack,
                  Child, TextObject, TextFrame,
                       MUIA_Background, MUII_TextBack,
                       MUIA_Text_Contents, "\33c\nDon't already have a User Name/Password?\n  You must register at the Twitter website first!\n",
                  End,

                  Child, HGroup,
                  Child, HSpace(0),
                  Child, ColGroup(0),
                  Child, twittersite = urlTextObject(MUIMasterBase,"http://twitter.com/","Establish An Account!",MUIV_Font_Normal),
                  Child, passforgot =  urlTextObject(MUIMasterBase,"http://twitter.com/account/resend_password ","Forgot Your Password?",MUIV_Font_Normal),
                  Child, HSpace(0),
                  Child, HSpace(0),
                  End,
                  Child, HSpace(0),
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // The Donate Window
      SubWindow, win_donate = WindowObject,
          MUIA_Window_Title, "Donate",
          MUIA_Window_ID, MAKE_ID('D','N','A','T'),

          WindowContents, VGroup,

              Child, VGroup, GroupFrame,

                  MUIA_Background, MUII_GroupBack,
                  Child, TextObject, TextFrame,
                       MUIA_Background, MUII_TextBack,
                       MUIA_Text_Contents, "\33c\nIf you find AmiTwitter useful, please consider donating.\n  You must have an active internet connection:\n",
                  End,

                  Child, HGroup,
                  Child, HSpace(0),
                  Child, ColGroup(0),
                  Child, donate = urlTextObject(MUIMasterBase,"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=8981293h","Donate!",MUIV_Font_Normal),
                  Child, HSpace(0),
                  Child, HSpace(0),
                  End,
                  Child, HSpace(0),
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // The Tweet Window
      SubWindow, win_tweet = WindowObject,
          MUIA_Window_Title, "Send a Tweet",
          MUIA_Window_ID, MAKE_ID('T','W','E','T'),

          WindowContents, VGroup,

              Child, HGroup, GroupFrameT("Send a Tweet"),

                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2("Tweet:"),
                           Child, HGroup,
                           Child, STR_message = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, "Enter your Tweet and click Update to send (max 140 characters)",
                           End,
                      End,
                      Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,
                           Child, clear_gad = MUI_MakeObject(MUIO_Button,"C_lear"),
                           Child, sendupdate_gad = MUI_MakeObject(MUIO_Button,"_Update"),
                           Child, but_cancel_tweet = MUI_MakeObject(MUIO_Button, "_Cancel"),
                      End,
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // The Search Window
      SubWindow, win_search = WindowObject,
          MUIA_Window_Title, "Search",
          MUIA_Window_ID, MAKE_ID('S','R','C','H'),

          WindowContents, VGroup,

              Child, HGroup, GroupFrameT("Search"),

                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2("Search:"),
                           Child, HGroup,
                           Child, STR_search = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, "Enter your search string and click Search (max 140 characters)",
                           End,
                      End,
                      Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,
                           Child, clear_search_gad = MUI_MakeObject(MUIO_Button,"C_lear"),
                           Child, search_gad = MUI_MakeObject(MUIO_Button,"_Search"),
                           Child, but_search_cancel = MUI_MakeObject(MUIO_Button, "_Cancel"),
                      End,
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // The Follow/Unfollow Window
      SubWindow, win_follow = WindowObject,
          MUIA_Window_Title, "Follow/Unfollow a User",
          MUIA_Window_ID, MAKE_ID('F','L','L','W'),

          WindowContents, VGroup,

              Child, HGroup, GroupFrameT("Change the Status for a User"),

                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2("User:"),
                           Child, HGroup,
                           Child, STR_follow = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, "Enter the Screen Name you want to follow or unfollow...",
                           End,
                      End,
                      Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,
                           Child, clear_follow_gad = MUI_MakeObject(MUIO_Button,"C_lear"),
                           Child, follow_gad =  MUI_MakeObject(MUIO_Button,"_Follow"),
                           Child, unfollow_gad = MUI_MakeObject(MUIO_Button,"_Unfollow"),                          
                           Child, but_follow_cancel = MUI_MakeObject(MUIO_Button, "_Cancel"),
                      End,
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // The Block/Unblock Window
      SubWindow, win_block = WindowObject,
          MUIA_Window_Title, "Block/Unblock a User",
          MUIA_Window_ID, MAKE_ID('B','L','C','K'),

          WindowContents, VGroup,

              Child, HGroup, GroupFrameT("Change the Status for a User"),

                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2("User:"),
                           Child, HGroup,
                           Child, STR_block = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, "Enter the Screen Name you want to block or unblock...",
                           End,
                      End,
                      Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,
                           Child, clear_block_gad = MUI_MakeObject(MUIO_Button,"C_lear"),
                           Child, block_gad = MUI_MakeObject(MUIO_Button,"_Block"),
                           Child, unblock_gad = MUI_MakeObject(MUIO_Button,"U_nblock"),
                           Child, but_block_cancel = MUI_MakeObject(MUIO_Button, "_Cancel"),
                      End,
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // The Notify/Unnotify SMS Window
      SubWindow, win_notify = WindowObject,
          MUIA_Window_Title, "Receive/Stop SMS Notifications for a User",
          MUIA_Window_ID, MAKE_ID('N','T','F','Y'),

          WindowContents, VGroup,

              Child, HGroup, GroupFrameT("Change the Status for a User"),

                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2("User:"),
                           Child, HGroup,
                           Child, STR_notify = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, "Enter the Screen Name you want to either receive\n or stop receiving SMS notifications for...",
                           End,
                      End,
                      Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,
                           Child, clear_notify_gad = MUI_MakeObject(MUIO_Button,"C_lear"),
                           Child, notify_gad = MUI_MakeObject(MUIO_Button,"_Receive"),
                           Child, unnotify_gad = MUI_MakeObject(MUIO_Button,"_Stop"),
                           Child, but_notify_cancel = MUI_MakeObject(MUIO_Button, "_Cancel"),
                      End,
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // The Direct Message Window
      SubWindow, win_dirmsg = WindowObject,
          MUIA_Window_Title, "Send a Direct Message",
          MUIA_Window_ID, MAKE_ID('D','I','R','M'),

          WindowContents, VGroup,

              Child, HGroup, GroupFrameT("Send a Direct Message"),
                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2("Recipient:"),
                           Child, STR_user_id = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, "Enter a Screen Name only",
                      End,
  
                      Child, HGroup,
                           Child, Label2("Message:"),
                           Child, STR_directmessage = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, "Enter a message (max 140 characters)",
                      End,
 
                     Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,
                           Child, clear_dm_gad = MUI_MakeObject(MUIO_Button,"C_lear"),
                           Child, send_gad = MUI_MakeObject(MUIO_Button,"_Send"),
                           Child, but_cancel_dm = MUI_MakeObject(MUIO_Button, "_Cancel"),
                     End,
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // User Update Profile Window
      SubWindow, win_userprofile = WindowObject,
          MUIA_Window_Title, "Update Your Profile",
          MUIA_Window_ID, MAKE_ID('P','R','F','L'),

          WindowContents, VGroup,

              Child, HGroup, GroupFrameT("Update Your Profile"),
                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2("Name:"),
                           Child, STR_profile_name = BetterStringObject, StringFrame, MUIA_ObjectID, 3,
                           MUIA_String_MaxLen, 21, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, "\33cThis field is currently mandatory to make\n any updates to your profile. This is what\n is displayed on your Twitter site\nIt can be anything and has no effect on your\n actual account Screen Name or User ID whatsoever...\nfor a lot of users this is their 'real' name\n (Max 20 characters)",
                      End,

                      Child, HGroup,
                           Child, Label2("Web:"),
                           Child, STR_profile_web = BetterStringObject, StringFrame, MUIA_ObjectID, 4,
                           MUIA_String_MaxLen, 101, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, "\33cEnter your website URL\n It will be prepended with 'http://' if not present\n(Max 100 characters)",
                      End,

                      Child, HGroup,
                           Child, Label2("Location:"),
                           Child, STR_profile_location = BetterStringObject, StringFrame, MUIA_ObjectID, 5,
                           MUIA_String_MaxLen, 31, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, "\33cEnter your location. The contents\nare not normalized or geocoded in any way\n(Max 30 characters)",
                      End,

                      Child, HGroup,
                           Child, Label2("Bio:"),
                           Child, STR_profile_bio = BetterStringObject, StringFrame, MUIA_ObjectID, 6,
                           MUIA_String_MaxLen, 161, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, "\33cEnter something about yourself or your description\n(Max 160 characters)",
                      End,

                     Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,                     
                           Child, update_profile_gad = MUI_MakeObject(MUIO_Button,"_Update Profile"),
                           Child, but_cancel_profile = MUI_MakeObject(MUIO_Button, "_Cancel"),
                     End,
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // The Main Program Window
      SubWindow, window = WindowObject,
          MUIA_Window_Title, "AmiTwitter",
          MUIA_Window_ID , MAKE_ID('W','T','M','F'),
          MUIA_Window_Menustrip,      MUI_MakeObject(MUIO_MenustripNM,MenuData1,0),

          WindowContents, VGroup,

              Child, HGroup, GroupFrame, MUIA_Group_SameSize, FALSE,
                  Child, ImageObject,
                       MUIA_Frame, MUIV_Frame_None,
                       MUIA_Image_Spec, "5:data/program_images/amitwitter.png",
                       MUIA_Image_FreeVert, TRUE,
                       MUIA_Image_FreeHoriz, TRUE,
                       MUIA_FixWidth, 157,
                       MUIA_FixHeight, 66,
                       MUIA_ShortHelp, "AmiTwitter © IKE",
                  End,

                  Child, RectangleObject, MUIA_Weight, 20, End,
                  Child, Label2("What's Happening?"),

                  Child, STR_login = BetterStringObject,
                       MUIA_BetterString_NoInput,
                       MUIA_BetterString_Columns,8, TRUE,
                  End,
              End,             

              Child, VGroup, GroupFrameT("Fast Links"),

                 Child, toolbar = TheBarObject,
                       GroupFrame, 
                       MUIA_Group_Horiz,       TRUE,
                       MUIA_TheBar_EnableKeys, TRUE,
                 /*    MUIA_TheBar_Borderless, TRUE,
                       MUIA_TheBar_Sunny,      TRUE,
                       MUIA_TheBar_Raised,     TRUE,
                       MUIA_TheBar_Scaled,     FALSE,  */
                       MUIA_TheBar_Buttons,    buttons,
                       MUIA_TheBar_PicsDrawer, "PROGDIR:data/program_images",
                       MUIA_TheBar_Pics,        pics,
                       End,
                 End,

              Child, scroll_main = ScrollgroupObject,
                  MUIA_Scrollgroup_UseWinBorder, TRUE,
                  MUIA_Scrollgroup_Contents, txt_source = HTMLtextObject, TextFrame,
                  MUIA_HTMLtext_Contents, HTML_INTRO,
                  End,
              End,
  
              Child, VGroup, GroupFrame, MUIA_Background, MUII_GroupBack,
                  Child, HGroup,
                       Child, HSpace(0),
                       Child, ColGroup(4),
                           Child, urltxtlink  = urlTextObject(MUIMasterBase,"http://twitter.com","Twitter",MUIV_Font_Normal),
                           Child, urltxtlink2 = urlTextObject(MUIMasterBase,"https://sourceforge.net/projects/amitwitter/","AmiTwitter SourceForge",MUIV_Font_Normal),
                           Child, mailtxtlink = urlTextObject(MUIMasterBase,"mailto:ikepgh@yahoo.com","Feedback",MUIV_Font_Normal),
                           Child, urltxtlink3 = urlTextObject(MUIMasterBase,"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=8981293h","Donate!",MUIV_Font_Normal),
                       End,
                       Child, HSpace(0),
                  End,
              End,
          End,
      End,
  End;

/*****************************************************************************/

  if (!app) {

    printf("Cannot create application!\n");
    return(0);
  }

///

/// DoMethods *****************************************************************/

  // Load Preferences
  DoMethod(app,MUIM_Application_Load,MUIV_Application_Load_ENV);
  DoMethod(app,MUIM_Application_Load,MUIV_Application_Load_ENVARC);


  // Quit
  DoMethod(window,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    app,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);


  // Main Buttons
  DoMethod(buttons[B_TIMELINE].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,HOME);

  DoMethod(buttons[B_RETWEETS].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,MEN_RETWEETTOME);

  DoMethod(buttons[B_REPLIES].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,REPLIES);

  DoMethod(buttons[B_RELOAD].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,RELOAD);

  DoMethod(buttons[B_SEARCH].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,BTN_SEARCH);

  DoMethod(buttons[B_FOLLOW].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,BTN_FOLLOW);

  DoMethod(buttons[B_DIRECTMESSAGE].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,DIRMSG);

  DoMethod(buttons[B_TWEET].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,TWEET);


  // Return keys
  DoMethod(STR_message,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
    app,2,MUIM_Application_ReturnID,SENDUPDATE);

  DoMethod(STR_directmessage,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
    app,2,MUIM_Application_ReturnID,DIRECTMESSAGE);

  DoMethod(STR_search,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
    app,2,MUIM_Application_ReturnID,SEARCH);


  // Prefs subwindow
  DoMethod(but_save,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,SAVE);

  DoMethod(but_test,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,TEST);

  DoMethod(but_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCEL);

  DoMethod(win_preferences,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_preferences,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Donate subwindow
  DoMethod(win_donate,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_donate,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Send a Tweet subwindow
  DoMethod(clear_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEAR);
    set(clear_gad, MUIA_ShortHelp, (ULONG)"Clear Tweet");

  DoMethod(sendupdate_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,SENDUPDATE);
    set(sendupdate_gad, MUIA_ShortHelp, (ULONG)"Send Tweet");

  DoMethod(but_cancel_tweet,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELTWEET);
    set(but_cancel_tweet, MUIA_ShortHelp, (ULONG)"Cancel Tweet");

  DoMethod(win_tweet,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_tweet,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Search subwindow
  DoMethod(clear_search_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEARSEARCH);
    set(clear_search_gad, MUIA_ShortHelp, (ULONG)"Clear Search");

  DoMethod(search_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,SEARCH);
    set(search_gad, MUIA_ShortHelp, (ULONG)"Search");

  DoMethod(but_search_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELSEARCH);
    set(but_search_cancel, MUIA_ShortHelp, (ULONG)"Cancel Search");

  DoMethod(win_search,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_search,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Follow subwindow
  DoMethod(clear_follow_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEARFOLLOW);
    set(clear_follow_gad, MUIA_ShortHelp, (ULONG)"Clear User Name");

  DoMethod(follow_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,FOLLOW);
    set(follow_gad, MUIA_ShortHelp, (ULONG)"Follow a User");

  DoMethod(unfollow_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,UNFOLLOW);
    set(unfollow_gad, MUIA_ShortHelp, (ULONG)"Unfollow a User");

  DoMethod(but_follow_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELFOLLOW);
    set(but_follow_cancel, MUIA_ShortHelp, (ULONG)"Cancel Request");

  DoMethod(win_follow,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_follow,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Block subwindow
  DoMethod(clear_block_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEARBLOCK);
    set(clear_block_gad, MUIA_ShortHelp, (ULONG)"Clear User Name");

  DoMethod(block_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,BLOCK);
    set(block_gad, MUIA_ShortHelp, (ULONG)"Block a User (Use with care!)");

  DoMethod(unblock_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,UNBLOCK);
    set(unblock_gad, MUIA_ShortHelp, (ULONG)"Unblock a currently blocked User");

  DoMethod(but_block_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELBLOCK);
    set(but_block_cancel, MUIA_ShortHelp, (ULONG)"Cancel Request");

  DoMethod(win_block,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_block,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Notifications SMS subwindow
  DoMethod(clear_notify_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEARNOTIFY);
    set(clear_notify_gad, MUIA_ShortHelp, (ULONG)"Clear User Name");

  DoMethod(notify_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,NOTIFY);
    set(notify_gad, MUIA_ShortHelp, (ULONG)"Begin receiving SMS notifications from this user");

  DoMethod(unnotify_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,UNNOTIFY);
    set(unnotify_gad, MUIA_ShortHelp, (ULONG)"Stop receiving SMS notifications from this user");

  DoMethod(but_notify_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELNOTIFY);
    set(but_notify_cancel, MUIA_ShortHelp, (ULONG)"Cancel Request");

  DoMethod(win_notify,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_notify,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Direct Message subwindow
  DoMethod(clear_dm_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEARDM);
    set(clear_dm_gad, MUIA_ShortHelp, (ULONG)"Clear Direct Message");

  DoMethod(send_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,DIRECTMESSAGE);
    set(send_gad, MUIA_ShortHelp, (ULONG)"Send Direct Message");

  DoMethod(but_cancel_dm,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELDM);
    set(but_cancel_dm, MUIA_ShortHelp, (ULONG)"Cancel Direct Message");

  DoMethod(win_dirmsg,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_dirmsg,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Update Profile subwindow
  DoMethod(update_profile_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,UPDATEPROFILE);
    set(update_profile_gad, MUIA_ShortHelp, (ULONG)"Update your profile!");

  DoMethod(but_cancel_profile,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELPROFILE);
    set(but_cancel_profile, MUIA_ShortHelp, (ULONG)"Cancel Update");

  DoMethod(win_userprofile,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_userprofile,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Get User Name/Password
  get(str_user_pref, MUIA_String_Contents, &username);
  get(str_pass_pref, MUIA_String_Contents, &password);


  // Set login string
  set (STR_login, MUIA_String_Contents, (int)username);

/*****************************************************************************/

///

/// Switch ********************************************************************/

  // Open the main window
  set(window,MUIA_Window_Open,TRUE);

    // Continue running while waiting for user input
    while (running) {

         ULONG sigs = 0;
         LONG result;

        /*  if (TAG_USER) {

            ULONG appareanceV, labelPosV, borderlessV, sunnyV, raisedV, scaledV;

            get(appearance,MUIA_Cycle_Active,&appareanceV);
            get(labelPos,MUIA_Cycle_Active,&labelPosV);
            get(borderless,MUIA_Cycle_Active,&borderlessV);
            get(sunny,MUIA_Cycle_Active,&sunnyV);
            get(raised,MUIA_Cycle_Active,&raisedV);
            get(scaled,MUIA_Cycle_Active,&scaledV);

            SetAttrs(toolbar,
                MUIA_TheBar_ViewMode,   appareanceV,
                MUIA_TheBar_LabelPos,   labelPosV,
                MUIA_TheBar_Borderless, borderlessV,
                MUIA_TheBar_Sunny,      sunnyV,
                MUIA_TheBar_Raised,     raisedV,
                MUIA_TheBar_Scaled,     scaledV,
                TAG_DONE);
        }  */

        switch (DoMethod(app,MUIM_Application_NewInput,&sigs)) {

              // Quit program
                case MUIV_Application_ReturnID_Quit:
                     running = FALSE;
                     break;

                // Timeline
                case HOME:
                case MEN_HOME:
                    remove("PROGDIR:data/temp/twitter.html");
                    amitwitter_loop();
                    break;

                // My Tweets
                case PROFILE:
                case MEN_MYTWEET:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_user_loop();
                     break;

                // Favorites
                case MEN_FAVS:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_favs();
                     break;

                // @Replies
                case REPLIES:
                case MEN_REPLIES:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_mentions_loop();
                     break;

                // Reload
                case RELOAD:
                case MEN_RELOAD:
                     set (txt_source, MUIA_HTMLtext_URL, (int)"PROGDIR:data/temp/twitter.html");
                     break;

                // Direct Message window
                case DIRMSG:
                case MEN_DIRMSG:
                     set(win_dirmsg, MUIA_Window_Open, TRUE);
                     break;

                case CLEARDM:
                     do_clear_dm();
                     break;

                case DIRECTMESSAGE:
                     amitwitter_direct_message(screen_name, text);
                     break;

                case CANCELDM:
                     set(win_dirmsg, MUIA_Window_Open, FALSE);
                     break;

                case MEN_DIRMSGSENT:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_dirmsgsent();
                     break;

                // Update Profile
                case MEN_USERPROFILE:
                     MUI_RequestA(app,window,0,"Update Profile","*OK","\33cPlease Note:\n\n Currently, You must *ALWAYS* specify a 'Name',\nthe other fields are optional, but if left blank they will\noverwrite the profile that is currently on your\nTwitter site.  (i.e., you should fill in all the\ninformation if you want it displayed on your Twitter\nsite! If you don't want it displayed, leave it blank\n(except for 'Name' of course)...\n\nI hope to make this a bit more user friendly in the future!\nPlease see the bubble help for more info for each field!", NULL);
                     set(win_userprofile, MUIA_Window_Open, TRUE);
                     break;

                case UPDATEPROFILE:
                     amitwitter_updateprofile(name, web, location, bio);
                     DoMethod(app,MUIM_Application_Save,MUIV_Application_Save_ENV);
                     DoMethod(app,MUIM_Application_Save,MUIV_Application_Save_ENVARC);
                     break;

                case CANCELPROFILE:
                     set(win_userprofile, MUIA_Window_Open, FALSE);
                     break;

                // Tweet window
                case TWEET:
                case MEN_TWEET:
                     set(win_tweet, MUIA_Window_Open, TRUE);
                     break;

                case CLEAR:
                     do_clear();
                     break;

                case SENDUPDATE:
                     amitwitter_update(optarg);
                     break;

                case CANCELTWEET:
                     set(win_tweet, MUIA_Window_Open, FALSE);
                     break;

                // Search window
                case MEN_SEARCH:
                case BTN_SEARCH:
                     set(win_search, MUIA_Window_Open, TRUE);
                     break;

                case CLEARSEARCH:
                     do_clear_search();
                     break;

                case SEARCH:
                     MUI_RequestA(app,window,0,"Search","*OK","Search not implemented yet...",NULL);
                     set(win_search, MUIA_Window_Open, FALSE);
                     break;

                case CANCELSEARCH:
                     set(win_search, MUIA_Window_Open, FALSE);
                     break;

                // Follow window
                case MEN_FOLLOW:
                case BTN_FOLLOW:
                     set(win_follow, MUIA_Window_Open, TRUE);
                     break;

                case CLEARFOLLOW:
                     do_clear_follow();
                     break;

                case FOLLOW:
                     amitwitter_follow(optarg);
                     break;

                case UNFOLLOW:
                     amitwitter_unfollow(optarg);
                     break;

                case CANCELFOLLOW:
                     set(win_follow, MUIA_Window_Open, FALSE);
                     break;

                // Block window
                case MEN_BLOCK:
                     set(win_block, MUIA_Window_Open, TRUE);
                     break;

                case CLEARBLOCK:
                     do_clear_block();
                     break;

                case BLOCK:
                     result=MUI_RequestA(app,0,0,"Block?","_Block|_Cancel","\33cAre you sure you want to block this User?\n\nIf you block someone, they wont be able to follow\nyou or send you any messages. If your account is\npublic, they'll still be able to view it, but they\nwont show up on your followers list, and you wont be\non their following list.\n\nIf it's a spammer you're blocking, then thanks!",0);

                        if(result==1)
                            amitwitter_block(optarg);
                        else
                            break;
                     break;

                case UNBLOCK:
                     amitwitter_unblock(optarg);
                     break;

                case CANCELBLOCK:
                     set(win_block, MUIA_Window_Open, FALSE);
                     break;

                // Notifications SMS window
                case MEN_NOTIFY:
                     MUI_RequestA(app,window,0,"SMS Notifications","*OK","\33cPlease Note: You must first set up your cell phone\non the Twitter website (under 'Settings/Mobile')\nif you have not done so already\nto begin receiving or to stop receiving SMS notifications\n with AmiTwitter...",NULL);
                     set(win_notify, MUIA_Window_Open, TRUE);
                     break;

                case CLEARNOTIFY:
                     do_clear_notify();
                     break;

                case NOTIFY:
                     amitwitter_notify(optarg);
                     break;

                case UNNOTIFY:
                     amitwitter_unnotify(optarg);
                     break;

                case CANCELNOTIFY:
                     set(win_notify, MUIA_Window_Open, FALSE);
                     break;

                // Quit the program
                case MEN_QUIT:
                     result=MUI_RequestA(app,0,0,"Quit","_Quit|_Cancel","\33cAre you sure you want\nto quit AmiTwitter?",0);

                        if(result==1)
                           running=FALSE;
                     break;

                // Retweets
                case MEN_RETWEETBYME:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_retweeted_by_me();
                     break;

                case MEN_RETWEETTOME:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_retweeted_to_me();
                     break;

                case MEN_RETWEETOFME:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_retweets_of_me();
                     break;

                // Friends
                case MEN_FRIENDS:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_friends_loop();
                     break;

                // Followers
                case MEN_FOLLOWERS:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_followers_loop();
                     break;

                // Blocking
                case MEN_BLOCKING:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_blocking_loop();
                     break;

                // Random
                case MEN_RANDOM:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_public_loop();
                     break;

                // Settings window
                case MEN_PREFS:
                     set(win_preferences, MUIA_Window_Open, TRUE);
                     break;

                case SAVE:
                     DoMethod(app,MUIM_Application_Save,MUIV_Application_Save_ENV);
                     DoMethod(app,MUIM_Application_Save,MUIV_Application_Save_ENVARC);

                     set (STR_login, MUIA_String_Contents, (int)username);

                     MUI_RequestA(app,0,0,"Save","*OK","Settings Saved",NULL);

                     set(win_preferences, MUIA_Window_Open, FALSE);
                     break;

                case TEST:
                     amitwitter_verify_credentials(screen_name, text);
                     break;

                case CANCEL:
                     set(win_preferences, MUIA_Window_Open, FALSE);
                     break;

                // MUI prefs
                case MEN_MUIPREFS:
                     DoMethod(app, MUIM_Application_OpenConfigWindow, 0);
                     break;

                // Show FAQ HTML
                case MEN_HELP2:
                     help();
                     break;

                // Donate window
                case MEN_DONATE:
                     set(win_donate, MUIA_Window_Open, TRUE);
                     break;

                // Show about HTML
                case MEN_ABOUT:                   
                     about();
                     break;

                // MUI about
                case MEN_ABOUTMUI:
        
                     if(!aboutwin) {

                        aboutwin = AboutmuiObject, MUIA_Aboutmui_Application, app, End;
                     }

                     if(aboutwin) {

                        set(aboutwin, MUIA_Window_Open, TRUE);
                     }
                     break;

        }

        // Continue running and wait for user input
        if (running && sigs) //Wait(sigs);
           {
              sigs = Wait(sigs | SIGBREAKF_CTRL_C);
               if (sigs & SIGBREAKF_CTRL_C) break;
           }
    }

/*****************************************************************************/

  // Kill the main window
  set(window,MUIA_Window_Open,FALSE);

  // Cleanup
  MUI_DisposeObject(app);

  // Close libraries
  Close_Libs();

  return (0);
}

///
