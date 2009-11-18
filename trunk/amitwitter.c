/** $Revision Header *** Header built automatically - do not edit! ***********
 **
 ** © Copyright 2009 IKE <ikepgh@yahoo.com>
 **
 ** File             : amitwitter.c
 ** Created on       : Friday, 06-Nov-09
 ** Created by       : IKE
 ** Current revision : V 0.17
 **
 ** Purpose
 ** -------
 **   AmiTwitter - a Twitter Program
 **
 ** Date        Author                 Comment
 ** =========   ====================   ====================
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
//#include <mui/Busy_mcc.h>

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

#define SENDUPDATE    44
#define CLEAR         45
#define HOME          46
#define ID_CANCEL     54
#define ID_SAVE       55
#define DIRECTMESSAGE 56
#define CLEARDM       57
#define PROFILE       58
#define REPLIES       59
#define RANDOM        60

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

//#warning compiling with ixemul...

#endif
#endif

/*****************************************************************************/

///

/// Variables *****************************************************************/

Object *app, *STR_user, *STR_pass, *STR_message, *aboutwin, *STR_friends,
*STR_following, *STR_statuses, *STR_favourites, *STR_user_id,
*STR_directmessage, *STR_name, *STR_location, *STR_login, *busy;

APTR str_user_pref, str_pass_pref, win_preferences, but_save, but_cancel,
username, password, urltxtlink, urltxtlink2, urltxtlink3, mailtxtlink, txt_source,
scroll_source, scroll_main, donate, win_donate, twittersite;

// Direct Messages
const char *screen_name, *text;

/*****************************************************************************/

///

/// Menus *********************************************************************/

enum {
    ADD_METHOD=1,

    MEN_FILE, MEN_HOME, MEN_PROFILE, MEN_REPLIES, MEN_RANDOM, MEN_DONATE, MEN_QUIT,
    MEN_TOOLS, MEN_PREFS, MEN_MUIPREFS,
    MEN_HELP, MEN_HELP2, MEN_ABOUT, MEN_ABOUTMUI
};

#define M(type,title_id,flags,men) type, (UBYTE *)(title_id), 0, flags, 0, (APTR)(men)
#define BAR    NM_ITEM, NM_BARLABEL, NULL, 0, 0, NULL
#define MENU_END  NM_END, NULL, NULL, 0, 0, NULL

// Menu structure
static struct NewMenu MenuData1[]=
{

    M( NM_TITLE,  MSG_FILE,      0,   MEN_FILE    ),
    M( NM_ITEM,   MSG_HOME,      0,   MEN_HOME    ),
    M( NM_ITEM,   MSG_PROFILE,   0,   MEN_PROFILE ),
    M( NM_ITEM,   MSG_REPLIES,   0,   MEN_REPLIES ),
    M( NM_ITEM,   MSG_RANDOM,    0,   MEN_RANDOM  ),
    M( NM_ITEM,   MSG_DONATE,    0,   MEN_DONATE  ),
    M( NM_ITEM,   MSG_QUIT,      0,   MEN_QUIT    ),

    M( NM_TITLE,  MSG_TOOLS,     0,   MEN_TOOLS   ),
    M( NM_ITEM,   MSG_PREFS,     0,   MEN_PREFS   ),
    BAR,          
    M( NM_ITEM,   MSG_MUIPREFS,  0,   MEN_MUIPREFS),

    M( NM_TITLE,  MSG_HELP,      0,   MEN_HELP    ),
    M( NM_ITEM,   MSG_HELP2,     0,   MEN_HELP2   ),
    M( NM_ITEM,   MSG_ABOUT,     0,   MEN_ABOUT   ),
    BAR,
    M( NM_ITEM,   MSG_ABOUTMUI,  0,   MEN_ABOUTMUI),
    M( NM_END,    NULL,          0,   0           ),

    MENU_END
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

/// Amitwitter ****************************************************************/

// New Twitter instance
twitter_t* twitter_new() {

    twitter_t *twitter;
    const char *home;

    home = "PROGDIR:";

    twitter = (twitter_t *)malloc(sizeof(twitter_t));
    twitter->base_uri = TWITTER_BASE_URI;
    twitter->user = username;
    twitter->pass = password;  
    twitter->source = "AmiTwitter";
    twitter->last_friends_timeline = 1;
    twitter->last_user_timeline = 1;   
    twitter->mentions = 1;             
    twitter->last_public_timeline = 1; 
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
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {
        fprintf(stderr, "error respose code: %ld\n", res);

        set (txt_source, MUIA_HTMLtext_Contents, (int)"Hmm...An error occurred! Are you connected to the internet?  Is your username/password set correctly?  Is the Twitter site down?");
        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents, (int)"Getting the latest...");
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
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {
        printf("error respose code: %ld\n", res);

        set (txt_source, MUIA_HTMLtext_Contents, (int)"Hmm...An error occurred! Are you connected to the internet?  Is your username/password set correctly?  Did you enter some text?");

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
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {
        printf("error respose code: %ld\n", res);

        set (txt_source, MUIA_HTMLtext_Contents, (int)"Hmm...An error occurred! Are you connected to the internet?  Is your username/password set correctly?  Did you enter the recipients screen name correctly?  Did you enter some text?");

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

// statuses/friends_timeline API
GList* twitter_friends_timeline(twitter_t *twitter) {

    int ret;
    GList *timeline = NULL;
    GByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_FRIENDS_TIMELINE,
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

        twitter->last_friends_timeline = atol(status->id);
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
            if(!xmlStrcmp(name, (xmlChar *)"status")) {
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
            }else if (!xmlStrcmp(name, (xmlChar *)"user")) {
                status->user = twitter_parse_user_node(reader);
            }
            xmlFree(name);
        } else if (type == XML_READER_TYPE_END_ELEMENT) {
            name = xmlTextReaderName(reader);
            if (!xmlStrcmp(name, (xmlChar *)"status")) {
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
            }else if(!xmlStrcmp(name, (xmlChar *)"profile_image_url")) {
                ret = xmlTextReaderRead(reader);
                user->profile_image_url =
                    (const char *)xmlTextReaderValue(reader);
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
            if(!xmlStrcmp(name, (xmlChar *)"user")) {
                xmlFree(name);
                break;
            }
            xmlFree(name);
        }
    }while(ret == 1);
    return user;
}

/*****************************************************************************/

// Display in HTMLtext .mcc
void twitter_status_print(twitter_status_t *status) {
                                                                                                                                  
    FILE *outfile;
        
    outfile = freopen("PROGDIR:data/temp/twitter.html", "a+", stdout);

    printf("<IMG SRC=PROGDIR:data/temp/%s><p> <b>%s </b> %s <p><small>%s from %s</small> ",status->user->id, status->user->screen_name, status->text, status->created_at, status->source);
    printf("<p>");

 // printf("[%s]%s: %s\n", status->id, status->user->screen_name, status->text);

    set (STR_name, MUIA_String_Contents, (int)status->user->name);
    set (STR_location, MUIA_String_Contents, (int)status->user->location);
    set (STR_following, MUIA_String_Contents, (int)status->user->friends_count);
    set (STR_friends, MUIA_String_Contents, (int)status->user->followers_count);
    set (STR_statuses, MUIA_String_Contents, (int)status->user->statuses_count);
    set (STR_favourites, MUIA_String_Contents, (int)status->user->favourites_count);

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

        if(status->user){
            free((void*)(status->user->id));
            free((void*)(status->user->name));
            free((void*)(status->user->screen_name));
            free((void*)(status->user->location));
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
        return -1;
    }

    curl = curl_easy_init();
    if(!curl) {
        fprintf(stderr, "error: curl_easy_init()\n");
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
        return -1;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {
        fprintf(stderr, "error respose code: %ld\n", res);
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

// Get most recent Tweets (statuses/friends_timeline API)
void amitwitter_loop() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

/*
    if(!twitter->debug) {
        timeline = twitter_friends_timeline(twitter);
        twitter_statuses_free(timeline);
    } */

// original 'while' loop.  my loop below gets content once....

 /* while(1) {
        timeline = twitter_friends_timeline(twitter);
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
        timeline = twitter_friends_timeline(twitter);
//      if(twitter->debug >= 2)

//      set(busy, MUIA_Busy_Speed, 20);
//      DoMethod(busy, MUIM_Busy_Move);

            printf("timeline num: %d\n", g_list_length(timeline));

        twitter_fetch_images(twitter, timeline);
        amitwitter_show_timeline(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    }

    twitter_free(twitter);

//  set(busy, MUIA_Busy_Speed, MUIV_Busy_Speed_Off);
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

    printf("Message was %u characters long.\n", strlen(text));

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_update(twitter, text);
    twitter_free(twitter);

    fprintf(stdout, "done\n");
}

/*****************************************************************************/

// Direct Messages
void amitwitter_direct_message(const char *screen_name, const char *text) {

    twitter_t *twitter = NULL;

    fprintf(stdout, "sending...");
    fflush(stdout);
   
    get(STR_user_id, MUIA_String_Contents, &screen_name);
    get(STR_directmessage, MUIA_String_Contents, &text);

    printf("Message was %u characters long.\n", strlen(text));

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_direct_message(twitter, screen_name, text);
    twitter_free(twitter);

    fprintf(stdout, "done\n");
}

/*****************************************************************************/

///

/// My functions **************************************************************/

// clear Tweet
void do_clear(void ) {
    set(STR_message, MUIA_String_Contents,0);
}

// clear direct message
void do_clear_dm(void) {
    set(STR_directmessage, MUIA_String_Contents,0);
}

// about HTML
void about(void ) {
    set(txt_source, MUIA_HTMLtext_Contents, (int)HTML_INTRO);
}

// FAQ HTML
void help(void ) {
    set(txt_source, MUIA_HTMLtext_Contents, (int)HTML_HELP);
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

/// Main Program **************************************************************/

int main(int argc, char *argv[]) {

APTR app, window, sendupdate_gad, clear_gad, home_gad, clear_dm_gad, send_gad,
recent_gad, mentions_gad, public_gad;

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
                  Child, twittersite = urlTextObject(MUIMasterBase,"http://twitter.com/","Establish an Account",MUIV_Font_Normal),
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
                       MUIA_ShortHelp, "AmiTwitter can send up to 140 characters",
                  End,

                  Child, TextObject, TextFrame,
                       MUIA_Background, MUII_TextBack,
                       MUIA_Text_Contents, "\33c\0338\33b\nWhat Are You Doing?\n",
                  End,

              End,

              Child, HGroup, GroupFrameT("Send a Tweet"),

                  Child, ColGroup(4), 
                       Child, Label2("Tweet:"),
                       Child, STR_message = BetterStringObject, StringFrame,
                       MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                       MUIA_ShortHelp, "Enter your Tweet here and click 'Update' to send a Tweet",

                  Child, HGroup, MUIA_Group_SameSize, TRUE, MUIA_CycleChain, TRUE,
                       Child, clear_gad = MUI_MakeObject(MUIO_Button,"_Clear"),
                       Child, sendupdate_gad = MUI_MakeObject(MUIO_Button,"_Update"),
                       End,
                  End,
              End,

              Child, VGroup, GroupFrameT("User Status"),

                  Child, HGroup, MUIA_Group_SameSize, TRUE, MUIA_CycleChain, TRUE,
                       Child, home_gad = MUI_MakeObject(MUIO_Button,"_Timeline"),
                       Child, recent_gad = MUI_MakeObject(MUIO_Button,"_My Tweets"),
                       Child, mentions_gad =  MUI_MakeObject(MUIO_Button,"@_Replies"),
                       Child, public_gad = MUI_MakeObject(MUIO_Button,"R_andom"),
                  End,

                  Child, HGroup,

                       Child, ColGroup(2),
                           Child, Label2("Name:"),
                           Child, STR_name = BetterStringObject,
                           MUIA_BetterString_NoInput, MUIA_BetterString_Columns,8, TRUE,
                           End,
                       End,

                       Child, ColGroup(2),
                           Child, Label2("Location:"),
                           Child, STR_location = BetterStringObject,
                           MUIA_BetterString_NoInput, MUIA_BetterString_Columns,8, TRUE,
                           End,
                       End,
                  End,

                  Child, HGroup,
                       Child, ColGroup(2),
                           Child, Label2("Current Tweet Stats..."),
                           MUIA_ShortHelp, "Displays statistics for most recent Tweet",
                       End,

                       Child, ColGroup(2),
                           Child, Label2("Following:"),
                           Child, STR_following = BetterStringObject,
                           MUIA_BetterString_NoInput, MUIA_BetterString_Columns,8, TRUE,
                           End,
                       End,

                       Child, ColGroup(2),
                           Child, Label2("Followers:"),
                           Child, STR_friends = BetterStringObject,
                           MUIA_BetterString_NoInput, MUIA_BetterString_Columns,8, TRUE,
                           End,
                       End,

                       Child, ColGroup(2),
                           Child, Label2("Tweets:"),
                           Child, STR_statuses = BetterStringObject,
                           MUIA_BetterString_NoInput, MUIA_BetterString_Columns,8, TRUE,
                           End,
                       End,

                       Child, ColGroup(2),
                           Child, Label2("Favorites:"),
                           Child, STR_favourites = BetterStringObject,
                           MUIA_BetterString_NoInput, MUIA_BetterString_Columns,8, TRUE,
                           End,
                       End,
                  End,
              End,

              Child, scroll_main = ScrollgroupObject,
                  MUIA_Scrollgroup_UseWinBorder, TRUE,
                  MUIA_Scrollgroup_Contents, txt_source = HTMLtextObject, TextFrame,
                  MUIA_HTMLtext_Contents, HTML_INTRO,
                  End,
              End,

              Child, VGroup, GroupFrameT("Send a Direct Message"),
         
                  Child, HGroup,

                       Child, Label2("      Send:"),
                       Child, STR_user_id = BetterStringObject, StringFrame, MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                       MUIA_ShortHelp, "Enter a screen name only",
                       Child, Label2("a direct message."),
                       Child, RectangleObject, MUIA_Weight, 100, End,
                  End,
     
                  Child, HGroup,
                       Child, ColGroup(4),
                           Child, Label2("Message:"),
                           Child, STR_directmessage = BetterStringObject, StringFrame, MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, "Enter your message here and click 'Send' to send a Direct Message",

                       Child, HGroup, MUIA_Group_SameSize, TRUE, MUIA_CycleChain, TRUE,
                           Child, clear_dm_gad = MUI_MakeObject(MUIO_Button,"C_lear"),
                           Child, send_gad = MUI_MakeObject(MUIO_Button,"_Send"),
                           End,
                       End,
                  End,
              End,
  
              Child, VGroup, GroupFrame, MUIA_Background, MUII_GroupBack,
                  Child, HGroup,
                       Child, ColGroup(5), //6
                       Child, urltxtlink  = urlTextObject(MUIMasterBase,"http://twitter.com","Twitter",MUIV_Font_Normal),
                       Child, urltxtlink2 = urlTextObject(MUIMasterBase,"https://sourceforge.net/projects/amitwitter/","AmiTwitter SourceForge",MUIV_Font_Normal),
                       Child, mailtxtlink = urlTextObject(MUIMasterBase,"mailto:ikepgh@yahoo.com","Feedback",MUIV_Font_Normal),
                       Child, urltxtlink3 = urlTextObject(MUIMasterBase,"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=8981293h","Donate!",MUIV_Font_Normal),
                       End,

                       Child, Label2("Logged In As:"),
                       Child, STR_login = BetterStringObject,
                       MUIA_BetterString_NoInput, MUIA_BetterString_Columns,8, TRUE,
                       End,

                    // Child, busy = BusyObject, MUIA_Busy_Speed,
                    // MUIV_Busy_Speed_Off,
				    // End,
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
  DoMethod(home_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,HOME);

  set(home_gad, MUIA_ShortHelp, (ULONG)"Get most recent Tweets (max 20)");

  DoMethod(recent_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,PROFILE);

  set(recent_gad, MUIA_ShortHelp, (ULONG)"Get most recently sent Tweets (max 20)");

  DoMethod(mentions_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,REPLIES);

  set(mentions_gad, MUIA_ShortHelp, (ULONG)"Get most recent @Replies (max 20)");

  DoMethod(public_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,RANDOM);

  set(public_gad, MUIA_ShortHelp, (ULONG)"Get Random Public Tweets! (max 20)");

  DoMethod(clear_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEAR);

  set(clear_gad, MUIA_ShortHelp, (ULONG)"Clear Tweet text");

  DoMethod(sendupdate_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,SENDUPDATE);

  set(sendupdate_gad, MUIA_ShortHelp, (ULONG)"Send a Tweet");

  // Direct Message Buttons
  DoMethod(clear_dm_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEARDM);

  set(clear_dm_gad, MUIA_ShortHelp, (ULONG)"Clear Direct Message text");

  DoMethod(send_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,DIRECTMESSAGE);

  set(send_gad, MUIA_ShortHelp, (ULONG)"Send a Direct Message");

  // Return key
  DoMethod(STR_message,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
    app,2,MUIM_Application_ReturnID,SENDUPDATE);

  DoMethod(STR_directmessage,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
    app,2,MUIM_Application_ReturnID,DIRECTMESSAGE);

  // Prefs
  DoMethod(but_save,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,ID_SAVE);

  DoMethod(but_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,ID_CANCEL);

  DoMethod(win_preferences,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_preferences,3,MUIM_Set,MUIA_Window_Open,FALSE);

  // Donate
  DoMethod(win_donate,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_donate,3,MUIM_Set,MUIA_Window_Open,FALSE);

  // Get User Name/Password
  get(str_user_pref, MUIA_String_Contents, &username);
  get(str_pass_pref, MUIA_String_Contents, &password);

  // Set login string
  set (STR_login, MUIA_String_Contents, (int)username);


/*****************************************************************************/

///

/// Switch *******************************************************************/

  // Open the main window
  set(window,MUIA_Window_Open,TRUE);

    // Continue running while waiting for user input
    while (running) {

         ULONG sigs = 0;
         LONG result;


        switch (DoMethod(app,MUIM_Application_NewInput,&sigs)) {

                // Quit program
                case MUIV_Application_ReturnID_Quit:
                     running = FALSE;
                     break;

                // Pull Tweets  
                case HOME:
                case MEN_HOME:
                    remove("PROGDIR:data/temp/twitter.html");
                    amitwitter_loop();
                    break;

                // Recent
                case PROFILE:
                case MEN_PROFILE:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_user_loop();
                    break;

                // Mentions @Recipient
                case REPLIES:
                case MEN_REPLIES:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_mentions_loop();
                    break;

                // Random
                case RANDOM:
                case MEN_RANDOM:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_public_loop();
                    break;

                // Clear tweet string 
                case CLEAR:
                     do_clear();
                     break;

                // Send Tweets
                case SENDUPDATE:
                     amitwitter_update(optarg);
                     break;

                // Clear direct message string 
                case CLEARDM:
                     do_clear_dm();
                     break;

                // Send a Direct Message
                case DIRECTMESSAGE:
                     amitwitter_direct_message(screen_name, text);
                     break;
              
                // Show 'about' HTML
                case MEN_ABOUT:                   
                     about();
                     break;

                // Show 'help' HTML
                case MEN_HELP2:
                     help();
                     break;

                // Quit the program 
                case MEN_QUIT:
                     result=MUI_RequestA(app,0,0,"Quit","_Quit|_Cancel","\33cAre you sure you want\nto quit AmiTwitter?",0);

                        if(result==1)
                           running=FALSE;
                     break;

                // Open MUI prefs
                case MEN_MUIPREFS:                    
                     DoMethod(app, MUIM_Application_OpenConfigWindow, 0);
                     break;

                // Open MUI about 
                case MEN_ABOUTMUI:
        
                     if(!aboutwin) {

                        aboutwin = AboutmuiObject, MUIA_Aboutmui_Application, app, End;
                     }

                     if(aboutwin) {

                        set(aboutwin, MUIA_Window_Open, TRUE);
                     }
                     break;

                // Prefs window
                case MEN_PREFS:
                     set(win_preferences, MUIA_Window_Open, TRUE);
                     break;

                case ID_SAVE:
                     DoMethod(app,MUIM_Application_Save,MUIV_Application_Save_ENV);
                     DoMethod(app,MUIM_Application_Save,MUIV_Application_Save_ENVARC);

                     set (STR_login, MUIA_String_Contents, (int)username);

                     MUI_RequestA(app,0,0,"Save","*OK","Settings Saved",NULL);  
                
                     set(win_preferences, MUIA_Window_Open, FALSE);
                     break;

                case ID_CANCEL:
                     set(win_preferences, MUIA_Window_Open, FALSE);
                     break;

                // Donate window
                case MEN_DONATE:
                     set(win_donate, MUIA_Window_Open, TRUE);
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
