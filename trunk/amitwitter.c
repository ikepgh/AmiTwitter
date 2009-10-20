/*
 * AmiTwitter 0.1.1 Copyright (C) 2009 IKE <ikepgh@yahoo.com>
 *
 * Compiled with Cubic IDE/gcc 2.95.3-4
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


/// Includes

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
#include <pragmas/dos_pragmas.h>
#include <pragmas/exec_pragmas.h>
#include <workbench/workbench.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <clib/gadtools_protos.h>
#include <MUI/Urltext_mcc.h>
#include <MUI/HTMLtext_mcc.h>
#include <MUI/BetterString_mcc.h>

// Amitwitter
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <glib.h>
#include <curl/curl.h>
#include <libxml/xmlreader.h>
#include "amitwitter.h"

///

/// Definitions

#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))

#define SENDUPDATE    44
#define CLEAR         45
#define ID_HOME       46
#define ID_CANCEL     54
#define ID_SAVE       55
#define DIRECTMESSAGE 56
#define CLEARDM       57
#define RECENT        58
#define MENTIONS      59
#define RANDOM        60

///

/// Variables

Object *app, *STR_user, *STR_pass, *STR_message, *aboutwin, *STR_friends,
*STR_following, *STR_statuses, *STR_favourites, *STR_user_id, *STR_directmessage;

APTR str_user_pref, str_pass_pref, win_preferences, but_save, but_cancel,
username, password, urltxtlink, urltxtlink2, urltxtlink3, mailtxtlink, txt_source, scroll_source,
scroll_main, donate, win_donate;

// Direct Messages
const char *screen_name, *text;

// Base Structures
struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct Library *MUIMasterBase;

extern struct ExecBase *SysBase;
extern struct Library *DOSBase;

///

/// Menu

enum {
    ADD_METHOD=1,

    MEN_FILE, MEN_DONATE, MEN_QUIT,
    MEN_TOOLS, MEN_PREFS, MEN_MUIPREFS,
    MEN_HELP, MEN_HELP2, MEN_ABOUTMUI, MEN_ABOUT,
};

// Menu structure
static struct NewMenu MenuData1[]=
{
    {NM_TITLE, "File",                         0 , 0, 0,  (APTR)MEN_FILE    },
    {NM_ITEM,  "Donate",                       0 , 0, 0,  (APTR)MEN_DONATE  },
    {NM_ITEM,  "Quit",                        "Q", 0, 0,  (APTR)MEN_QUIT    },
    {NM_TITLE, "Tools",                        0 , 0, 0,  (APTR)MEN_TOOLS   },
    {NM_ITEM,  "Settings",                     0 , 0, 0,  (APTR)MEN_PREFS   },
    {NM_ITEM,  NM_BARLABEL,                    0 , 0, 0,  (APTR)0           },
    {NM_ITEM,  "MUI Settings",                 0 , 0, 0,  (APTR)MEN_MUIPREFS},
    {NM_TITLE, "Help",                         0 , 0, 0,  (APTR)MEN_HELP    },
    {NM_ITEM,  "FAQs",                         0 , 0, 0,  (APTR)MEN_HELP2   },
    {NM_ITEM,  "About AmiTwitter",             0 , 0, 0,  (APTR)MEN_ABOUT   },
    {NM_ITEM,  NM_BARLABEL,                    0 , 0, 0,  (APTR)0           },
    {NM_ITEM,  "About MUI",                    0 , 0, 0,  (APTR)MEN_ABOUTMUI},
    {NM_END,   NULL,                           0 , 0, 0,  (APTR)0           },

};

///

/// Libraries

// Set to running
BOOL running = TRUE;

BOOL Open_Libs(void ) {

    if ( !(IntuitionBase=(struct IntuitionBase *) OpenLibrary("intuition.library",39)) ) {

       return(0);
    }

    if ( !(GfxBase=(struct GfxBase *) OpenLibrary("graphics.library",0)) ) {

       CloseLibrary((struct Library *)IntuitionBase);
       return(0);
    }

    if ( !(MUIMasterBase=OpenLibrary(MUIMASTER_NAME,19)) ) {

       CloseLibrary((struct Library *)GfxBase);
       CloseLibrary((struct Library *)IntuitionBase);
       return(0);
    }
    return(1);
}

// Close libraries function
void Close_Libs(void ) {

  if ( IntuitionBase )
    CloseLibrary((struct Library *)IntuitionBase);

  if ( GfxBase )
    CloseLibrary((struct Library *)GfxBase);

  if ( MUIMasterBase )
    CloseLibrary(MUIMasterBase);

  if ( DOSBase )
    CloseLibrary(DOSBase);

  if ( SysBase )
    CloseLibrary((struct Library *)SysBase);
}

/// 

/// Amitwitter

// New Twitter instance
twitter_t* twitter_new() {

    twitter_t *twitter;
    const char *home;

    home = "PROGDIR:";
 /* home = getenv("HOME");
        if(!home)
        return NULL; */

    twitter = (twitter_t *)malloc(sizeof(twitter_t));
    twitter->base_uri = TWITTER_BASE_URI;
    twitter->user = username;
    twitter->pass = password;  
    twitter->source = "AmiTwitter";
    twitter->last_friends_timeline = 1;
    twitter->last_user_timeline = 1;   //added
    twitter->mentions = 1;             //added
    twitter->last_public_timeline = 1; //added
    twitter->fetch_interval = 60;
    twitter->show_interval = 5;
    twitter->alignment = 2;
    twitter->debug = 0;
//  snprintf(twitter->res_dir, PATH_MAX, "%s.amitwitter", home);
//  snprintf(twitter->images_dir, PATH_MAX, "%s.amitwitter/images", home);
    snprintf(twitter->images_dir, PATH_MAX, "%sPROGDIR:data/temp", home);
    return twitter;
}

// Cleanup
void twitter_free(twitter_t *twitter) {

/*  if(twitter->user) {
        free((char*)(twitter->user));
    }
    if(twitter->pass) {
        free((char*)(twitter->pass));
    }  */
    free(twitter);
    return;
}

// Twitter Config
int twitter_config(twitter_t *twitter) {

    const char *home;
    char config_dir[PATH_MAX];
    char config_file[PATH_MAX];
    FILE *fp;
    struct stat st;
    char line[256];
    char *key;
    char *value;

    home = getenv("HOME");
    if(!home) {
        return -1;
    }
//  snprintf(config_dir, PATH_MAX, "%s.amitwitter/", home);
//  snprintf(config_file, PATH_MAX, "%s.amitwitter/config", home);

    fp = fopen(config_file, "r");
    if(!fp) {
        fprintf(stderr, "config open error\n");
        if(stat(config_dir, &st)) {
            mkdir(config_dir, 0755);
        }
        if(stat(config_file, &st)) {
            fp = fopen(config_file, "w");
            fprintf(fp, "user=\npass=\n");
            fclose(fp);
        }
        return -1;
    }
    while ((fgets(line, 256 - 1, fp)) != NULL) {
        key=line;
        if(strlen(line) < 1) continue;
        if(line[0] == '#') continue;
        value = strchr(line, '=');
        if(!value) continue;
        *value = '\0';
        value++;
        value[strlen(value) - 1] = '\0';
        if(!strcmp(key, "debug")) {
            twitter->debug = atoi(value);
        }
        if(!strcmp(key, "user")) {
            twitter->user = strdup(value);
        }
        if(!strcmp(key, "pass")) {
            twitter->pass = strdup(value);
        }
        if(!strcmp(key, "fetch_interval")) {
            twitter->fetch_interval = atoi(value);
            if(twitter->fetch_interval < 0) {
                fprintf(stderr, "config read error:\n");
            }
        }
        if(!strcmp(key, "show_interval")) {
            twitter->show_interval = atoi(value);
            if(twitter->show_interval < 0) {
                fprintf(stderr, "config read error:\n");
            }
        }
        if(!strcmp(key, "alignment")) {
            if(!strcmp(value, "top_left")) {
                twitter->alignment = 0;
            }else if(!strcmp(value, "top_right")) {
                twitter->alignment = 1;
            }else if(!strcmp(value, "bottom_left")) {
                twitter->alignment = 2;
            }else if(!strcmp(value, "bottom_right")) {
                twitter->alignment = 3;
            }else{
                twitter->alignment = 3;
            }
        }
    }
    fclose(fp);
    return 0;
}

// Curl write
static size_t twitter_curl_write_cb(void *ptr, size_t size, size_t nmemb, void *data) {

    size_t realsize = size * nmemb;
    g_byte_array_append((GByteArray *)data, (guint8*)ptr, realsize);
    return realsize;
}

// Get latest Tweets with cURL
int twitter_fetch(twitter_t *twitter, const char *apiuri, GByteArray *buf) {

    CURL *curl;
    CURLcode code;
    long res;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);

    curl = curl_easy_init();
    if(!curl) {
        fprintf(stderr, "error: curl_easy_init()\n");
        return -1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, apiuri);
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
        return res;
    }

    curl_easy_cleanup(curl);

    return 0;
}


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
//      if(twitter->debug > 2){
            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");
//      }
        return res;
    }
    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headers);

    set(txt_source, MUIA_HTMLtext_Contents, status);  //added

    return 0;
}


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
//      if(twitter->debug > 2){
            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");
//      }
        return res;
    }
    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headers);

    return 0;
}

// statuses/triends_timeline API
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

// XML parse statuses node
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

// XML parse statuses node
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

// XML parse user node
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
            }else if(!xmlStrcmp(name, (xmlChar *)"screen_name")) {
                ret = xmlTextReaderRead(reader);
                user->screen_name = (const char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"profile_image_url")) {
                ret = xmlTextReaderRead(reader);
                user->profile_image_url =
                    (const char *)xmlTextReaderValue(reader);
            // added the next four...
            }else if(!xmlStrcmp(name, (xmlChar *)"followers_count")) {
                ret = xmlTextReaderRead(reader);
                user->followers_count = (const char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"friends_count")) {
                ret = xmlTextReaderRead(reader);
                user->friends_count = (const char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"statuses_count")) {
                ret = xmlTextReaderRead(reader);
                user->statuses_count = (const char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"favourites_count")) {
                ret = xmlTextReaderRead(reader);
                user->favourites_count = (const char *)xmlTextReaderValue(reader);
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

// Display in HTMLtext .mcc
void twitter_status_print(twitter_status_t *status) {

FILE *outfile;
        
outfile = freopen("PROGDIR:data/temp/twitter.html", "a+", stdout);

printf("<IMG SRC=PROGDIR:data/temp/%s><p> <b>%s </b> %s <p><small>%s</small>", status->user->id, status->user->screen_name, status->text, status->created_at);
              
printf("<p>");

// printf("[%s]%s: %s\n", status->id, status->user->screen_name, status->text);

set (STR_following, MUIA_String_Contents, (int)status->user->friends_count);
set (STR_friends, MUIA_String_Contents, (int)status->user->followers_count);
set (STR_statuses, MUIA_String_Contents, (int)status->user->statuses_count);
set (STR_favourites, MUIA_String_Contents, (int)status->user->favourites_count);

fclose(stdout);

}

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
            free((void*)(status->user->screen_name));
            free((void*)(status->user->profile_image_url));
            free((void*)(status->user));
        }
        free(status);
    }while((l = g_list_next(l)));
    g_list_free(statuses);

}

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
/*  ret = mkdir(twitter->images_dir, 0755);

      if(ret && errno != EEXIST){
        fprintf(stderr, "can't create directory.\n");
        return -1;
    } */

    do{
        status = statuses->data;
        twitter_image_name(status, name);
        url = status->user->profile_image_url;
        snprintf(path, PATH_MAX, "%s/%s", twitter->images_dir, name);
        ret = stat(path, &st);
    //  if(ret) {
        //  if(twitter->debug)
                printf("fetch_image: %s\n", url);
           //   printf("path is: %\n", path);
            twitter_fetch_image(twitter, url, path);
    //  }
    }while((statuses = g_list_previous(statuses)));

    return 0;
}

// Twitter cURL File
static size_t twitter_curl_file_cb(void *ptr, size_t size, size_t nmemb, void *data) {

    size_t realsize = size * nmemb;
    fwrite(ptr, size, nmemb, (FILE*)data);
    return realsize;
}

// cURL Fetch image
int twitter_fetch_image(twitter_t *twitter, const char *url, char* path) {

    CURL *curl;
    CURLcode code;
    long res;
    FILE *fp;
    int i;
    char *esc;
    char escaped_url[PATH_MAX];

//  printf("twitter_fetch_image\n");
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

    /*  esc = curl_easy_escape(curl, url + i + 1, 0);  /* changed to line below for curl 7.14.0 capatability */
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

// XML unescape only &lt; and &gt;  notice: destructive conversion.
static void xmlunescape(const char *str) {

    char *p;
    while((p = strstr(str, "&lt;")) != NULL) {
        *p++ = '<';
        while(*p = *(p + 3)) p++;
        *p = '\0';
    }
    while((p = strstr(str, "&gt;")) != NULL) {
        *p++ = '>';
        while(*p = *(p + 3)) p++;
        *p = '\0';
    }
}

// XML escape only &, > and <;
static void xmlescape(char *dest, const char *src, size_t n) {

    int i = 0;
    int j = 0;
    size_t len = strlen(src);
    do{
        if(j + 6 > n){
            dest[j] = '\0';
            break;
        }else if(src[i] == '&'){
            dest[j++] = '&';
            dest[j++] = 'a';
            dest[j++] = 'm';
            dest[j++] = 'p';
            dest[j++] = ';';
        }else if(src[i] == '>'){
            dest[j++] = '&';
            dest[j++] = 'g';
            dest[j++] = 't';
            dest[j++] = ';';
        }else if(src[i] == '<'){
            dest[j++] = '&';
            dest[j++] = 'l';
            dest[j++] = 't';
            dest[j++] = ';';
        }else{
            dest[j] = src[i];
            j++;
        }
    }while(src[i++]);
}

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

// Get most recent Tweets (statuses/friends_timeline API)
void amitwitter_loop() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    get(str_user_pref, MUIA_String_Contents, &username);
    get(str_pass_pref, MUIA_String_Contents, &password);

    twitter = twitter_new();
    twitter_config(twitter);

/*
    if(!twitter->debug) {
        timeline = twitter_friends_timeline(twitter);
        twitter_statuses_free(timeline);
    } */

// original 'while' loop.  my loop below gets content once....

/*    while(1) {
        timeline = twitter_friends_timeline(twitter);
        if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        twitter_fetch_images(twitter, timeline);
        amitwitter_show_timeline(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
        sleep(twitter->fetch_interval);

    }
*/

// my 'for' loop, pulls data once

    for(i=1; i<2; i++) {
        timeline = twitter_friends_timeline(twitter);
//      if(twitter->debug >= 2)
            printf("timeline num: %d\n", g_list_length(timeline));

        twitter_fetch_images(twitter, timeline);
        amitwitter_show_timeline(twitter, timeline);
        twitter_statuses_free(timeline);
        timeline = NULL;
//      sleep(twitter->fetch_interval);

    if(i==1) break;
    } 

// twitter_free(twitter);
}

// Show users most recent messages (statuses/user_timeline API)
void amitwitter_user_loop() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    get(str_user_pref, MUIA_String_Contents, &username);
    get(str_pass_pref, MUIA_String_Contents, &password);

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

// twitter_free(twitter);
}

// Shows all mentions @Replies (statuses/mentions API)
void amitwitter_mentions_loop() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    get(str_user_pref, MUIA_String_Contents, &username);
    get(str_pass_pref, MUIA_String_Contents, &password);

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

// twitter_free(twitter);
}

// Shows most recent random messages (statuses/public_timeline API)
void amitwitter_public_loop() {

    int i;

    twitter_t *twitter = NULL;

    GList* timeline = NULL;

    get(str_user_pref, MUIA_String_Contents, &username);
    get(str_pass_pref, MUIA_String_Contents, &password);

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

// twitter_free(twitter);
}

// Send a Tweet
void amitwitter_update(const char *text) {

    twitter_t *twitter = NULL;

    fprintf(stdout, "updating...");
    fflush(stdout);

    get(STR_message, MUIA_String_Contents, &text);

    printf("Message was %u characters long.\n", strlen(text));

    get(str_user_pref, MUIA_String_Contents, &username);
    get(str_pass_pref, MUIA_String_Contents, &password);

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_update(twitter, text);
//  twitter_free(twitter);

    fprintf(stdout, "done\n");

}

// Direct Messages
void amitwitter_direct_message(const char *screen_name, const char *text) {

    twitter_t *twitter = NULL;

    fprintf(stdout, "sending...");
    fflush(stdout);

    get(STR_user_id, MUIA_String_Contents, &screen_name);
    get(STR_directmessage, MUIA_String_Contents, &text);

    printf("Message was %u characters long.\n", strlen(text));

    get(str_user_pref, MUIA_String_Contents, &username);
    get(str_pass_pref, MUIA_String_Contents, &password);

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_direct_message(twitter, screen_name, text);
//  twitter_free(twitter);

    fprintf(stdout, "done\n");

}

// Update stdin
void amitwitter_update_stdin() {

    twitter_t *twitter = NULL;
    int i;
    char text[1024];
    fgets(text, 1024, stdin);
    amitwitter_update(text);
}


///

/// My functions

// 'clear' 
void do_clear(void ) {
    set(STR_message, MUIA_String_Contents,0);
}

// 'clear direct message' 
void do_clear_dm(void) {
    set(STR_directmessage, MUIA_String_Contents,0);

}

// 'about' 
void about(void ) {
    set(txt_source, MUIA_HTMLtext_Contents, (int)HTML_INTRO);
}

// 'help' 
void help(void ) {
    set(txt_source, MUIA_HTMLtext_Contents, (int)HTML_HELP);
}


// "urlTextObject"
Object *urlTextObject(struct Library *MUIMasterBase,STRPTR url,STRPTR text,ULONG font) {
    return UrltextObject,
        MUIA_Font,          font,
        MUIA_Urltext_Text,  text,
        MUIA_Urltext_Url,   url,
    End;
}

///

/// Main Program

int main(int argc, char *argv[]) {

APTR app, window, sendupdate_gad, clear_gad, home_gad, clear_dm_gad, send_gad,
recent_gad, mentions_gad, public_gad;

  if ( ! Open_Libs() ) {

    printf("Cannot open libs\n");
    return(0);
  }

  app = ApplicationObject,
      MUIA_Application_Title  , "AmiTwitter",
      MUIA_Application_Version , "$VER: AmiTwitter 0.1.1",
      MUIA_Application_Copyright , "©2009, IKE",
      MUIA_Application_Author  , "IKE",
      MUIA_Application_Description, "A Twitter program",
      MUIA_Application_Base  , "AmiTwitter",


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
                  Child, but_save   = KeyButton("Save",'s'),
                  Child, but_cancel = KeyButton("Cancel",'c'),
              End,
          End,
      End,


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
                  Child, donate = urlTextObject(MUIMasterBase,"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=8981293h","Click to Donate!",MUIV_Font_Normal),
                  Child, HSpace(0),
                  Child, HSpace(0),
                  End,
                  Child, HSpace(0),
                  End,
              End,
          End,
      End,


      // The Main Program Window
      SubWindow, window = WindowObject,
          MUIA_Window_Title, "AmiTwitter",
          MUIA_Window_ID , MAKE_ID('W','T','M','F'),
          MUIA_Window_Menustrip,      MUI_MakeObject(MUIO_MenustripNM,MenuData1,0),

          WindowContents, VGroup,

              Child, HGroup, GroupFrame, MUIA_Group_SameSize, FALSE,
                  Child, ImageObject,
                       MUIA_Frame, MUIV_Frame_None,
                       MUIA_Image_Spec, "5:data/program_images/amitwitter.jpg",
                       MUIA_Image_FreeVert, TRUE,
                       MUIA_Image_FreeHoriz, TRUE,
                       MUIA_FixWidth, 170,
                       MUIA_FixHeight, 77,
                       MUIA_ShortHelp, "AmiTwitter can send up to 140 characters",
                  End,

                  Child, TextObject, TextFrame,
                       MUIA_Background, MUII_TextBack,
                       MUIA_Text_Contents, "\33c\nWhat Are You Doing?\n",
                  End,

              End,

              Child, ColGroup(2),
                  Child, Label2("Tweet:"),
                //Child, STR_message = String("",141), 
                  Child, STR_message = BetterStringObject, StringFrame, MUIA_CycleChain, TRUE,
                  End,
              End,

              Child, HGroup, GroupFrame, MUIA_Group_SameSize, TRUE, MUIA_CycleChain, TRUE,
                  Child, home_gad = KeyButton("Home", 'h'),
                  Child, recent_gad = KeyButton("My Tweets", 'm'),
                  Child, mentions_gad = KeyButton("@Replies", 'r'),
                  Child, public_gad = KeyButton("Random",'d'),
                  Child, clear_gad = KeyButton("Clear Tweet",'c'),
                  Child, sendupdate_gad = KeyButton("Update",'u'),
              End, 

              Child, HGroup, GroupFrame, MUIA_Group_SameSize, TRUE,
                  Child, ColGroup(2),
                       Child, Label2("Current Tweet Stats..."),
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
                       Child, Label2("Favourites:"),
                       Child, STR_favourites = BetterStringObject, 
                       MUIA_BetterString_NoInput, MUIA_BetterString_Columns,8, TRUE,
                       End,
                  End,
              End,

              Child, scroll_main = ScrollgroupObject,
                  MUIA_Scrollgroup_UseWinBorder, TRUE,
                  MUIA_Scrollgroup_Contents, txt_source = HTMLtextObject, TextFrame,
                  MUIA_HTMLtext_Contents, HTML_INTRO,
                  End,
              End,

              Child, ColGroup(2),
                  Child, Label2("Direct Message:"),
                //Child, STR_directmessage = String("",141),
                  Child, STR_directmessage = BetterStringObject, StringFrame, MUIA_CycleChain, TRUE, End,
              End,

              Child, HGroup, GroupFrame, MUIA_Group_SameSize, TRUE,

              Child, ColGroup(2),
                  Child, Label2("Recipient:"),
                //Child, STR_user_id = String("",141),
                  Child, STR_user_id = BetterStringObject, StringFrame, MUIA_CycleChain, TRUE, End,
                  MUIA_ShortHelp, "Enter a screen name only",
              End,
     
              Child, ColGroup(2),      
                  Child, clear_dm_gad = KeyButton("Clear Message", 'l'),
                  Child, send_gad = KeyButton("Send Message", 's'), MUIA_CycleChain, 5,
                  End,
              End,
  
              Child, VGroup, GroupFrame, MUIA_Background, MUII_GroupBack,
                  Child, HGroup,
                  Child, HSpace(0),
                  Child, ColGroup(4),
                  Child, urltxtlink =  urlTextObject(MUIMasterBase,"http://twitter.com/ikepgh","Follow ikepgh on Twitter",MUIV_Font_Normal),
                  Child, urltxtlink2 = urlTextObject(MUIMasterBase,"https://sourceforge.net/projects/amitwitter/","AmiTwitter SourceForge Project Page",MUIV_Font_Normal),
                  Child, mailtxtlink = urlTextObject(MUIMasterBase,"mailto:ikepgh@yahoo.com","Email ikepgh",MUIV_Font_Normal),
                  Child, urltxtlink3 = urlTextObject(MUIMasterBase,"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=8981293h","Click to Donate!",MUIV_Font_Normal),
                  End,
                  Child, HSpace(0),
                  End,
              End,
          End,
	  End,
  End;

  if (!app) {

    printf("Cannot create application!\n");
    return(0);
  }

///

/// DoMethods

  // Load Preferences
  DoMethod(app,MUIM_Application_Load,MUIV_Application_Load_ENV);
  DoMethod(app,MUIM_Application_Load,MUIV_Application_Load_ENVARC);

  // Quit
  DoMethod(window,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    app,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

  // Buttons
  DoMethod(home_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,ID_HOME);

  DoMethod(clear_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEAR);

  DoMethod(sendupdate_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,SENDUPDATE);

  DoMethod(send_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,DIRECTMESSAGE);

  DoMethod(clear_dm_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEARDM);

  DoMethod(recent_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,RECENT);

  DoMethod(mentions_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,MENTIONS);

  DoMethod(public_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,RANDOM);

  // Return keys
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

///

/// Switch 

  // Open the main window
  set(window,MUIA_Window_Open,TRUE);

    // Continue running while waiting for user input
    while (running) {

         ULONG sigs = 0;
         LONG result;


        switch (DoMethod(app,MUIM_Application_Input,&sigs)) {

                // Quit program
                case MUIV_Application_ReturnID_Quit:
                     running = FALSE;
                     break;

                // Pull Tweets  
                case ID_HOME:
                    remove("PROGDIR:data/temp/twitter.html");
                    set (txt_source, MUIA_HTMLtext_Contents, (int)"Downloading the latest Tweets...");
                    amitwitter_loop();
                    break;

                // Recent
                case RECENT:
                     remove("PROGDIR:data/temp/twitter.html");
                     set (txt_source, MUIA_HTMLtext_Contents, (int)"Downloading your recently sent Tweets...");
                     amitwitter_user_loop();
                    break;

                // Mentions @Recipient
                case MENTIONS:
                     remove("PROGDIR:data/temp/twitter.html");
                     set (txt_source, MUIA_HTMLtext_Contents, (int)"Downloading your @Replies...");
                     amitwitter_mentions_loop();
                    break;

                // Random
                case RANDOM:
                     remove("PROGDIR:data/temp/twitter.html");
                     set (txt_source, MUIA_HTMLtext_Contents, (int)"Downloading the most recent Public Tweets (random fun)...");
                     amitwitter_public_loop();
                    break;

                // Send Tweets
                case SENDUPDATE:
                     amitwitter_update(optarg);
                     set (txt_source, MUIA_HTMLtext_Contents, (int)"Your Tweet was sent!");
                     break;

                case DIRECTMESSAGE:
                     amitwitter_direct_message(screen_name, text);
                     set (txt_source, MUIA_HTMLtext_Contents, (int)"Your Direct Message was sent!");
                     break;

                // Clear tweet string gadget
                case CLEAR:
                     do_clear();
                     break;

                // Clear direct message string gadget
                case CLEARDM:
                     do_clear_dm();
                     break;

                // Show 'about'
                case MEN_ABOUT:                   
                     about();
                     break;

                // Show 'help'
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
        if (running && sigs) Wait(sigs);

    }

  // Kill the main window
  set(window,MUIA_Window_Open,FALSE);

  // Cleanup
  MUI_DisposeObject(app);

  // Close libraries
  Close_Libs();

  return (0);
}

///
