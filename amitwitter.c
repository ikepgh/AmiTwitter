/** $Revision Header *** Header built automatically - do not edit! ***********
 **
 ** � Copyright 2010 IKE <ikepgh@yahoo.com>
 **
 ** File             : amitwitter.c
 ** Created on       : Friday, 06-Nov-09
 ** Created by       : IKE
 ** Current revision : V 0.29
 **
 ** Purpose
 ** -------
 **   AmiTwitter - a Twitter Program
 **
 ** Date        Author                 Comment
 ** =========   ====================   ====================
 ** 09-Apr-10   Cyborg                 Revised Open_Libs() to a bit more sane style. Close_Libs() is now always called
									   if something went wrong, so make sure you have proper NULL checks in there.
 ** 07-Apr-10   - Unknown -            added TwitPic support; Reworked: main interface, tweet formatting and misc style
 ** 28-Fev-10   Cyborg                 - ILocale was not opened for OS4
									   - Fixed localization
									   - Fixed image name truncation
									   - Added a zillion of saftey checks before free(). Prevents lots of crashes, but
										 there are very likely even more needed. ALWAYS check if there is actually
										 something to free!
									   - Removed a lot of 'const' keywords for non-const variables.
									   - Replaced free() with xmlFree() where appropriate.
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
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <libraries/asl.h>
#include <libraries/gadtools.h>
#include <libraries/locale.h>
#ifndef __amigaos4__
# include <clib/exec_protos.h>
# include <clib/locale_protos.h>
#else
#include <proto/locale.h>
#endif
#include <proto/exec.h>
#include <proto/asl.h>
#include <proto/icon.h>
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
#include <curl/curl.h>
#include <libxml/xmlreader.h>
#include <proto/codesets.h>
#include <libraries/codesets.h>

// Amitwitter
#include "amitwitter.h"
#include "AmiTwitter_rev.h"

#define CATCOMP_ARRAY
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK

#include "amitwitter_strings.h"

/*****************************************************************************/

///

/// Definitions and Base Structures *******************************************/

// Defines
#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

// Buttons
#define TIMELINE      58
#define RETWEETS      59
#define REPLIES       60
#define RELOAD        61

// Search
#define BTN_SEARCH    62
#define CLEARSEARCH   63
#define SEARCH        64
#define CANCELSEARCH  65

// User Follow
#define USERS         66
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

// User Show
#define CLEARSHOW     79
#define USERSHOW      80
#define CANCELSHOW    81

// Direct Message
#define DIRECTMESSAGE 82
#define CLEARDM       83
#define DIRMSG        84
#define CANCELDM      85

// Tweet
#define TWEET         86
#define CLEAR         87
#define SENDUPDATE    88
#define SENDUPDATE2   95
#define CANCELTWEET   89

// Settings
#define SAVE          90
#define TEST          91
#define CANCEL        92

// Update profile
#define UPDATEPROFILE 93
#define CANCELPROFILE 94

// Stack
long __stack = 65536;

// Base Structures
extern struct ExecBase *SysBase; 
extern struct Library *DOSBase; 

struct GfxBase *GfxBase             = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct Library *MUIMasterBase       = NULL;
struct Library *CodesetsBase        = NULL;
struct Library *IconBase            = NULL;
struct Library *AslBase             = NULL;

struct Library *LocaleBase          = NULL;
struct LocaleInfo li;
struct NewMenu *nm;

#ifdef __amigaos4__
extern struct ExecIFace *IExec;
extern struct DOSIFace  *IDOS;
struct GraphicsIFace    *IGraphics      = NULL;
struct IntuitionIFace   *IIntuition     = NULL;
struct MUIMasterIFace   *IMUIMaster     = NULL;
struct CodesetsIFace	*ICodesets		= NULL;
struct LocaleIFace      *ILocale        = NULL;
struct AslIFace			*IAsl			= NULL;
struct IconIFace		*IIcon			= NULL;
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

/// Prototypes ****************************************************************/

STRPTR GetString(struct LocaleInfo *li, LONG stringNum);

/****************************************************************************/

///

/// Variables *****************************************************************/

Object *app, *STR_user, *STR_pass, *STR_message, *aboutwin, *STR_user_id,
*STR_directmessage, *STR_login, *STR_search, *STR_follow, *STR_block,
*STR_notify, *STR_profile_name, *STR_profile_web, *STR_profile_location,
*STR_profile_bio, *STR_show, *STR_twitLength, *STR_dirmsgLength,
*STR_twitpictweet;

APTR str_user_pref, str_pass_pref, win_preferences, but_save, but_cancel,
but_test, username, password, urltxtlink, urltxtlink2, urltxtlink3, mailtxtlink,
txt_source, scroll_source, scroll_main, donate, win_donate, twittersite,
passforgot, win_tweet, but_cancel_tweet, win_dirmsg, but_cancel_dm, win_search,
window, sendupdate_gad, clear_gad, toolbar, clear_dm_gad, send_gad,
clear_search_gad, search_gad, but_search_cancel, clear_follow_gad, follow_gad,
but_follow_cancel, unfollow_gad, win_follow, clear_block_gad, block_gad,
unblock_gad, but_block_cancel, clear_notify_gad, notify_gad,
unnotify_gad, but_notify_cancel, win_userprofile, update_profile_gad,
but_cancel_profile, clear_show_gad, show_gad, but_show_cancel, cy1, pop1;

// Direct Messages
char *screen_name, *text;

// Update Profile
char *name, *web, *location, *bio;

char *media;

// International Character Codesets
#define BUF_SIZE 102400

struct codeset *srcCodeset, *dstCodeset;

struct DiskObject *dobj = 0;

char *buf, *destbuf;
ULONG destlen;
FILE *f;

STRPTR charset[] = { "ISO-8859-1 + Euro", "ISO-8859-1", "ISO-8859-2", "ISO-8859-3",
                      "ISO-8859-4", "ISO-8859-5", "ISO-8859-7", "ISO-8859-9", "ISO-8859-10",
                      "ISO-8859-15", "ISO-8859-16", "Amiga-1251", "AmigaPL", "CP1250", "CP1251",
                      "CP1252", "IBM866", "KOI8-R", /*"UTF-16", "UTF-32", "UTF-8", */
                      "windows-1250", "windows-1252", NULL };

ULONG charsetV;
STRPTR ch;

/*****************************************************************************/

///

/// Menus *********************************************************************/

enum {
    ADD_METHOD=1,
                        
    MEN_TIMELINE, MEN_RETWEETBYME, MEN_RETWEETTOME, MEN_RETWEETOFME,
    MEN_REPLIES, MEN_RELOAD, MEN_SEARCH, MEN_SEARCHUSER, MEN_USERS,
    MEN_DIRMSG, MEN_DIRMSGSENT, MEN_DIRMSGRCVD, MEN_TWEET, MEN_MYTWEET, MEN_FAVS, MEN_QUIT,
    MEN_FRIENDS, MEN_FOLLOWERS, MEN_BLOCKING, MEN_RANDOM,
    MEN_PREFS, MEN_USERPROFILE, MEN_MUIPREFS,
    MEN_FAQ, MEN_DONATE, MEN_ABOUT, MEN_ABOUTMUI
};

static struct NewMenu Menu[]= {

    { NM_TITLE,  (STRPTR)MSG_0000,       0,   0, 0, (APTR)0               },
    { NM_ITEM,   (STRPTR)MSG_0001,      "T",  0, 0, (APTR)MEN_TIMELINE    },
    { NM_ITEM,   (STRPTR)MSG_0002,       0,   0, 0, (APTR)0               },
    { NM_SUB,    (STRPTR)MSG_0003,      "Y",  0, 0, (APTR)MEN_RETWEETBYME },
    { NM_SUB,    (STRPTR)MSG_0004,      "W",  0, 0, (APTR)MEN_RETWEETTOME },
    { NM_SUB,    (STRPTR)MSG_0005,      "X",  0, 0, (APTR)MEN_RETWEETOFME },
    { NM_ITEM,   (STRPTR)MSG_0006,      "R",  0, 0, (APTR)MEN_REPLIES     },
    { NM_ITEM,   (STRPTR)MSG_0007,      "O",  0, 0, (APTR)MEN_RELOAD      },
//  { NM_ITEM,   (STRPTR)MSG_0008,       0,   0, 0, (APTR)0               },
//  { NM_SUB,    (STRPTR)MSG_0008,      "S",  0, 0, (APTR)MEN_SEARCH      },
//  { NM_SUB,    (STRPTR)MSG_0009,       0,   0, 0, (APTR)MEN_SEARCHUSER  },
    { NM_ITEM,   (STRPTR)MSG_0010,      "U",  0, 0, (APTR)MEN_USERS       },
    { NM_ITEM,   (STRPTR)NM_BARLABEL,        0,   0, 0, (APTR)0               },
    { NM_ITEM,   (STRPTR)MSG_0011,       0,   0, 0, (APTR)0               },
    { NM_SUB,    (STRPTR)MSG_0011,      "D",  0, 0, (APTR)MEN_DIRMSG      },
    { NM_SUB,    (STRPTR)MSG_0012,      "G",  0, 0, (APTR)MEN_DIRMSGSENT  },
    { NM_SUB,    (STRPTR)MSG_0013,      "E",  0, 0, (APTR)MEN_DIRMSGRCVD  },
    { NM_ITEM,   (STRPTR)MSG_0014,       0,   0, 0, (APTR)0               },
    { NM_SUB,    (STRPTR)MSG_0015,      "M",  0, 0, (APTR)MEN_MYTWEET     },
    { NM_SUB,    (STRPTR)MSG_0016,      "F",  0, 0, (APTR)MEN_FAVS        },
    { NM_ITEM,   (STRPTR)MSG_TWITPIC2,  "P",  0, 0, (APTR)MEN_TWEET       },
    { NM_ITEM,   (STRPTR)NM_BARLABEL,        0,   0, 0, (APTR)0               },
    { NM_ITEM,   (STRPTR)MSG_0017,      "Q",  0, 0, (APTR)MEN_QUIT        },

    { NM_TITLE,  (STRPTR)MSG_0018,       0,   0, 0, (APTR)0               },
    { NM_ITEM,   (STRPTR)MSG_0019,      "I",  0, 0, (APTR)MEN_FRIENDS     },
    { NM_ITEM,   (STRPTR)MSG_0020,      "L",  0, 0, (APTR)MEN_FOLLOWERS   },
    { NM_ITEM,   (STRPTR)MSG_0021,      "B",  0, 0, (APTR)MEN_BLOCKING    },
    { NM_ITEM,   (STRPTR)NM_BARLABEL,        0,   0, 0, (APTR)0               },
    { NM_ITEM,   (STRPTR)MSG_0022,      "N",  0, 0, (APTR)MEN_RANDOM      },

    { NM_TITLE,  (STRPTR)MSG_0023,       0,   0, 0, (APTR)0               },
    { NM_ITEM,   (STRPTR)MSG_0024,      "1",  0, 0, (APTR)MEN_PREFS       },
    { NM_ITEM,   (STRPTR)MSG_0025,      "2",  0, 0, (APTR)MEN_USERPROFILE },
    { NM_ITEM,   (STRPTR)NM_BARLABEL,        0,   0, 0, (APTR)0               },
    { NM_ITEM,   (STRPTR)MSG_0026,      "3",  0, 0, (APTR)MEN_MUIPREFS    },

    { NM_TITLE,  (STRPTR)MSG_0027,       0,   0, 0, (APTR)0               },
    { NM_ITEM,   (STRPTR)MSG_0028,      "?",  0, 0, (APTR)MEN_FAQ         },
    { NM_ITEM,   (STRPTR)MSG_0029,      "$",  0, 0, (APTR)MEN_DONATE      },
    { NM_ITEM,   (STRPTR)MSG_0030,      "A",  0, 0, (APTR)MEN_ABOUT       },
    { NM_ITEM,   (STRPTR)NM_BARLABEL,        0,   0, 0, (APTR)0               },
    { NM_ITEM,   (STRPTR)MSG_0031,      "Z",  0, 0, (APTR)MEN_ABOUTMUI    },
    { NM_END,    (STRPTR)NULL,               0,   0, 0, (APTR)0               },

};

/*****************************************************************************/

///

/// TheBar buttons ************************************************************/

enum {

    B_TIMELINE, B_RETWEETS, B_REPLIES,
    B_RELOAD, B_SEARCH, B_FOLLOW,
    B_DIRECTMESSAGE, B_TWEET,

    };

static struct MUIS_TheBar_Button buttons[] =
{
    {0, B_TIMELINE,      (STRPTR)MSG_TIMELINE,   (STRPTR)MSG_TIMELINE2  ,0, 0, NULL, NULL },
    {1, B_RETWEETS,      (STRPTR)MSG_RETWEETS,   (STRPTR)MSG_RETWEETS2  ,0, 0, NULL, NULL },
    {2, B_REPLIES,       (STRPTR)MSG_REPLIES ,   (STRPTR)MSG_REPLIES2   ,0, 0, NULL, NULL },
    {3, B_RELOAD,        (STRPTR)MSG_RELOAD  ,   (STRPTR)MSG_RELOAD2    ,0, 0, NULL, NULL },
    {4, B_SEARCH,        (STRPTR)MSG_SEARCH  ,   (STRPTR)MSG_SEARCH2    ,0, 0, NULL, NULL },
    {5, B_FOLLOW,        (STRPTR)MSG_USERS   ,   (STRPTR)MSG_USERS2     ,0, 0, NULL, NULL },
    {6, B_DIRECTMESSAGE, (STRPTR)MSG_DIRMSG  ,   (STRPTR)MSG_DIRMSG2    ,0, 0, NULL, NULL },
    {7, B_TWEET,         (STRPTR)MSG_TWITPIC ,   (STRPTR)MSG_TWITPIC2   ,0, 0, NULL, NULL },
    {MUIV_TheBar_End,   0,NULL,NULL,0, 0, NULL, NULL                                      },
};

STRPTR pics[] =
{
    "timeline",
    "retweets",
    "replies",
    "reload",
    "search",
    "follow",
    "directmessage",
    "twitpic",
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

	if(!(IntuitionBase=(struct IntuitionBase *) OpenLibrary((CONST_STRPTR)"intuition.library",37))
	#ifdef __amigaos4__
		|| !(IIntuition = (struct IntuitionIFace *) GetInterface((struct Library *)IntuitionBase, "main", 1, NULL))
	#endif
		)
	{
		fprintf(stderr, "Can't open intuition.library!\n");
		return 0;
	}

	if(!(GfxBase=(struct GfxBase *) OpenLibrary((CONST_STRPTR)"graphics.library",0))
	#ifdef __amigaos4__
		 || !(IGraphics = (struct GraphicsIFace *) GetInterface((struct Library *)GfxBase, "main", 1, NULL))
	#endif
		)
	{
		fprintf(stderr, "Can't open graphics.library!\n");
		return 0;
    }

	if(!(MUIMasterBase=OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN))
	#ifdef __amigaos4__
		|| !(IMUIMaster = (struct MUIMasterIFace *) GetInterface((struct Library *)MUIMasterBase, "main", 1, NULL))
	#endif
		)
	{
		fprintf(stderr, "Can't open muimaster.library!\n");
		return 0;
    }

	if(!(CodesetsBase = OpenLibrary(CODESETSNAME, CODESETSVER))
	#ifdef __amigaos4__
		|| !(ICodesets = (struct CodesetsIFace *) GetInterface((struct Library *)CodesetsBase, "main", 1, NULL))
	#endif
		)
	{
		fprintf(stderr, "Can't open codesets.library!\n");
		return 0;
	}


	if(!(IconBase = OpenLibrary("icon.library",0))
	#ifdef __amigaos4__
		|| !(IIcon = (struct IconIFace *) GetInterface((struct Library *)IconBase, "main", 1, NULL))
	#endif
		)
	{
		fprintf(stderr, "Can't open icon.library!\n");
		return 0;
	}

	if(!(AslBase = OpenLibrary("asl.library",0))
	#ifdef __amigaos4__
		|| !(IAsl = (struct AslIFace *) GetInterface((struct Library *)AslBase, "main", 1, NULL))
	#endif
		)
	{
		fprintf(stderr, "Can't open asl.library!\n");
		return 0;
	}


	if((LocaleBase = OpenLibrary("locale.library",38))
	#ifdef __amigaos4__
		&& (ILocale = (struct LocaleIFace *) GetInterface((struct Library *)LocaleBase, "main", 1, NULL))
	#endif
		)
	{
        li.li_LocaleBase = LocaleBase;

		if((li.li_Catalog = (APTR)OpenCatalogA(NULL,"amitwitter.catalog",NULL)) != NULL)
		{

        }
    }
	else
	{
		fprintf(stderr, "Can't open locale.library!\n");
		return 0;
	}

	/* If we came this far, everything is OK */
	return 1;
}

// Close libraries function
void Close_Libs(void ) {

    #ifdef __amigaos4__
	if(IAsl)
		DropInterface((struct Interface *)IAsl);

	if(IIcon)
		DropInterface((struct Interface *)IIcon);

	if(ICodesets)
		DropInterface((struct Interface *)ICodesets);

	if(IMUIMaster)
		DropInterface((struct Interface *)IMUIMaster);

	if(IIntuition)
		DropInterface((struct Interface *)IIntuition);

	if(IGraphics)
		DropInterface((struct Interface *)IGraphics);

	if(ILocale)
		DropInterface((struct Interface *)ILocale);
    #endif

	if(IntuitionBase)
        CloseLibrary((struct Library *)IntuitionBase);

	if(GfxBase)
        CloseLibrary((struct Library *)GfxBase);

	if(MUIMasterBase)
        CloseLibrary(MUIMasterBase);

	if(LocaleBase)
	{
		if(li.li_Catalog)
			CloseCatalog(li.li_Catalog);

        CloseLibrary(LocaleBase);
    }
	if(IconBase)
        CloseLibrary(IconBase);

	if(AslBase)
        CloseLibrary(AslBase);

	if(CodesetsBase)
        CloseLibrary(CodesetsBase);


    #ifdef __MORPHOS__
	if(SocketBase)
        CloseLibrary(SocketBase);        
    #endif
}

/*****************************************************************************/

///

/// My functions **************************************************************/

// About AmiTwitter HTML
void about(void) {
    set(txt_source, MUIA_HTMLtext_Contents, (int) HTML_INTRO);
}

// FAQ HTML
void help(void) {
    set(txt_source, MUIA_HTMLtext_URL, (int)"PROGDIR:data/html/faq.html");
}

// Error
void error(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int) GetString(&li, MSG_ERROR));
}

// Error2
void error2(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int) GetString(&li,MSG_ERROR2));
}

// Error3
void error3(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int) GetString(&li, MSG_ERROR3));
}

// Error4
void error4(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int) GetString(&li, MSG_ERROR4));
}

// Error5
void error5(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int) GetString(&li, MSG_ERROR5));
}

// Error6
void error6(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int) GetString(&li, MSG_ERROR6));
}

// Error7
void error7(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int) GetString(&li, MSG_ERROR7));
}

// Error8
void error8(void) {
     set (txt_source, MUIA_HTMLtext_Contents, (int) GetString(&li, MSG_ERROR8));
}

// urlTextObject
Object *urlTextObject(struct Library *MUIMasterBase,STRPTR url,STRPTR text,ULONG font) {
    return UrltextObject,
        MUIA_Font,          font,
        MUIA_Urltext_Text,  text,
        MUIA_Urltext_Url,   url,
    End;
}

// localization functions

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

    if (LocaleBase)
		return (STRPTR)GetCatalogStr(li->li_Catalog,stringNum,builtIn);

    return(builtIn);
}

void localizeNewMenu(struct NewMenu *nm) {

    for ( ; nm->nm_Type!=NM_END; nm++)
        if (nm->nm_Label!=NM_BARLABEL)
			nm->nm_Label = GetString(&li, (ULONG)nm->nm_Label);
}

void localizeTheBar(struct MUIS_TheBar_Button *button) {

    for ( ; button->img!=MUIV_TheBar_End; button++)
    {
		if (button->text) button->text = GetString(&li, (ULONG)button->text);
		if (button->help) button->help = GetString(&li, (ULONG)button->help);
    }
}

// International Character Conversion
void charconv() {

    srcCodeset = CodesetsFind("UTF-8", CSA_FallbackToDefault, FALSE, TAG_DONE);

    if (srcCodeset)
    {
        get(cy1, MUIA_Cycle_Active, &charsetV);

        ch = charset[charsetV];

        dstCodeset = CodesetsFind(ch, CSA_FallbackToDefault, FALSE, TAG_DONE);

        if (dstCodeset) {

         // fprintf(stderr,"Default codeset for your system is %s\n", dstCodeset->name);

            buf = AllocMem(BUF_SIZE, MEMF_CLEAR);
            if (buf) {

                f = fopen("PROGDIR:data/temp/twitter.html", "r+");
                if (f) {

                    fread(buf, BUF_SIZE-1, 1, f);
                    fclose(f);
                    destbuf = CodesetsConvertStr(CSA_SourceCodeset, (Tag)srcCodeset,
                                                 CSA_DestCodeset, (Tag)dstCodeset,
                                                 CSA_Source, (Tag)buf,
                                                 CSA_DestLenPtr, (Tag)&destlen,
                                                 TAG_DONE);
                    if (destbuf) {

                    //  fprintf(stderr, "Result length: %u\n", destlen);
                        remove("PROGDIR:data/temp/twitter.html");
                        f = fopen("PROGDIR:data/temp/twitter.html", "a+");
                        fwrite(destbuf, destlen, 1, f);
                        fclose(f);

                        CodesetsFreeA(destbuf, NULL);
                    }
                    else
                        fprintf(stderr, "Failed to convert text!\n");
                }
                FreeMem(buf, BUF_SIZE);
            }
            else
                fprintf(stderr, "Failed to allocate %d bytes for buffer\n", BUF_SIZE);
        }
        else

    //  fprintf(stderr, "Unable to query default codeset\n");
        fprintf(stderr, "Unknown destination codeset %s\n", ch);
    }
    else
       fprintf(stderr, "Unknown source codeset %s\n", "UTF-8");
}

ULONG my_list_length(struct List *list)
{
	struct Node *node = NULL;
	ULONG num = 0;

	for(node = list->lh_Head;
		node->ln_Succ;
		node = node->ln_Succ)
	{
		num++;
	}

	return num;
}

MyByteArray *my_byte_array_new(void)
{
	return AllocVec(sizeof(MyByteArray), MEMF_ANY|MEMF_CLEAR);
}

void my_byte_array_free(MyByteArray *mba, BOOL free_segment)
{
	if(mba)
	{
		if(free_segment && mba->data)
			FreeVec(mba->data);

		FreeVec(mba);
	}
}

MyByteArray	*my_byte_array_append(MyByteArray *mba, UBYTE *ptr, ULONG size)
{
	UBYTE *temp = NULL;
	ULONG newsize = 0;

	if(!mba || !ptr || !size)
		return mba;

	newsize = mba->len + size;
	temp = AllocVec(newsize, MEMF_ANY|MEMF_CLEAR);

	if(mba->data && mba->len > 0)
		memcpy(temp, mba->data, mba->len);

	memcpy(temp+mba->len, ptr, size);

	FreeVec(mba->data);
	mba->data = temp;
	mba->len = newsize;

	return mba;
}


/*****************************************************************************/

///

/// AmiTwitter Setup and Cleanup **********************************************/

// New AmiTwitter instance
twitter_t* twitter_new() {

    twitter_t *twitter;
	char *home;

    home = "PROGDIR:";

    twitter = (twitter_t *)malloc(sizeof(twitter_t));
    twitter->base_uri = TWITTER_BASE_URI;
    twitter->base_search_uri = TWITTER_BASE_SEARCH_URI;
    twitter->user = username;
    twitter->pass = password;  
    twitter->last_home_timeline = 1;
    twitter->last_user_timeline = 1;   
    twitter->mentions = 1;             
    twitter->last_friends_timeline = 1;
    twitter->last_followers_timeline = 1;
    twitter->last_public_timeline = 1; 
    twitter->last_dirmsgsent_timeline = 1;
    twitter->last_dirmsgrcvd_timeline = 1;
    twitter->last_favs_timeline = 1;
    twitter->last_blocking_timeline = 1;
    twitter->last_usershow_timeline = 1;
    twitter->fetch_interval = 60;
    twitter->show_interval = 5;
    twitter->alignment = 2;
    twitter->debug = 0;
    snprintf(twitter->images_dir, PATH_MAX, "%sdata/temp", home); 

    return twitter;
}

/*****************************************************************************/

// AmiTwitter Config
int twitter_config(twitter_t *twitter) {

	char *home;

    home = getenv("HOME");
    if(!home) {
        return -1;
    }
    return 0;
}

/*****************************************************************************/

// AmiTwitter Cleanup
void twitter_free(twitter_t *twitter) {

	if(twitter)
		free(twitter);
    return;
}

/*****************************************************************************/

// Free User
void twitter_statuses_free(struct List *statuses)
{
	struct Node *node = NULL, *next = NULL;

	twitter_status_t *status;

	if(!statuses)
	{
        return;
    }

	for(node = statuses->lh_Head;
		node->ln_Succ;
		node = next)
	{
		next = node->ln_Succ;

		status = (twitter_status_t *)node;
        if(!status) {
            continue;
        }
		Remove(node);

		if(status->created_at)
			xmlFree(status->created_at);
		if(status->id)
			xmlFree(status->id);
		if(status->text)
			xmlFree(status->text);
		if(status->source)
			xmlFree(status->source);
		if(status->truncated)
			xmlFree(status->truncated);
		if(status->in_reply_to_status_id)
			xmlFree(status->in_reply_to_status_id);
		if(status->in_reply_to_user_id)
			xmlFree(status->in_reply_to_user_id);
		if(status->favorited)
			xmlFree(status->favorited);
		if(status->in_reply_to_screen_name)
			xmlFree(status->in_reply_to_screen_name);
		if(status->retweeted_status)
			xmlFree(status->retweeted_status);

		if(status->user)
		{
			if(status->user->id)
				xmlFree(status->user->id);
			if(status->user->name)
				xmlFree(status->user->name);
			if(status->user->screen_name)
				xmlFree(status->user->screen_name);
			if(status->user->location)
				xmlFree(status->user->location);
			if(status->user->description)
				xmlFree(status->user->description);
			if(status->user->profile_image_url)
				xmlFree(status->user->profile_image_url);
			if(status->user->followers_count)
				xmlFree(status->user->followers_count);
			if(status->user->friends_count)
				xmlFree(status->user->friends_count);
			if(status->user->favourites_count)
				xmlFree(status->user->favourites_count);
			if(status->user->statuses_count)
				xmlFree(status->user->statuses_count);

			free((void*)(status->user));
        }
        free(status);
	}
	FreeVec(statuses);
}

/*****************************************************************************/

// Free Recipient
void twitter_statuses_free_dirmsg(struct List *statuses) {
	struct Node *node = NULL, *next = NULL;
    twitter_direct_message_t *direct_message;
    if(!statuses){
        return;
    }

	for(node = statuses->lh_Head;
		node->ln_Succ;
		node = next)
	{
		next = node->ln_Succ;

		direct_message = (twitter_direct_message_t *)node;
        if(!direct_message) {
            continue;
        }

		Remove(node);

		if(direct_message->id)
			xmlFree(direct_message->id);
		if(direct_message->sender_id)
			xmlFree(direct_message->sender_id);
		if(direct_message->text)
			xmlFree(direct_message->text);
		if(direct_message->recipient_id)
			xmlFree(direct_message->recipient_id);
		if(direct_message->created_at)
			xmlFree(direct_message->created_at);
		if(direct_message->sender_screen_name)
			xmlFree(direct_message->sender_screen_name);
		if(direct_message->recipient_screen_name)
			xmlFree(direct_message->recipient_screen_name);

		if(direct_message->recipient)
		{
			if(direct_message->recipient->id)
				xmlFree(direct_message->recipient->id);
			if(direct_message->recipient->created_at)
				xmlFree(direct_message->recipient->created_at);
			if(direct_message->recipient->name)
				xmlFree(direct_message->recipient->name);
			if(direct_message->recipient->screen_name)
				xmlFree(direct_message->recipient->screen_name);
			if(direct_message->recipient->location)
				xmlFree(direct_message->recipient->location);
			if(direct_message->recipient->description)
				xmlFree(direct_message->recipient->description);
			if(direct_message->recipient->profile_image_url)
				xmlFree(direct_message->recipient->profile_image_url);
			if(direct_message->recipient->followers_count)
				xmlFree(direct_message->recipient->followers_count);
			if(direct_message->recipient->friends_count)
				xmlFree(direct_message->recipient->friends_count);
			if(direct_message->recipient->favourites_count)
				xmlFree(direct_message->recipient->favourites_count);
			if(direct_message->recipient->statuses_count)
				xmlFree(direct_message->recipient->statuses_count);

			free((void*)(direct_message->recipient));
        }
        free(direct_message);
	}
	FreeVec(statuses);
}

/*****************************************************************************/

// Free Sender
void twitter_statuses_free_dirmsgrcvd(struct List *statuses) {
	struct Node *node = NULL, *next = NULL;
    twitter_direct_message_rcvd_t *direct_message;
    if(!statuses){
        return;
    }

	for(node = statuses->lh_Head;
		node->ln_Succ;
		node = next)
	{
		next = node->ln_Succ;

		direct_message = (twitter_direct_message_rcvd_t *)node;

        if(!direct_message) {
            continue;
        }

		Remove(node);

		if(direct_message->id)
			xmlFree(direct_message->id);
		if(direct_message->sender_id)
			xmlFree(direct_message->sender_id);
		if(direct_message->text)
			xmlFree(direct_message->text);
		if(direct_message->recipient_id)
			xmlFree(direct_message->recipient_id);
		if(direct_message->created_at)
			xmlFree(direct_message->created_at);
		if(direct_message->sender_screen_name)
			xmlFree(direct_message->sender_screen_name);
		if(direct_message->recipient_screen_name)
			xmlFree(direct_message->recipient_screen_name);

		if(direct_message->sender)
		{
			if(direct_message->sender->id)
				xmlFree(direct_message->sender->id);
			if(direct_message->sender->created_at)
				xmlFree(direct_message->sender->created_at);
			if(direct_message->sender->name)
				xmlFree(direct_message->sender->name);
			if(direct_message->sender->screen_name)
				xmlFree(direct_message->sender->screen_name);
			if(direct_message->sender->location)
				xmlFree(direct_message->sender->location);
			if(direct_message->sender->description)
				xmlFree(direct_message->sender->description);
			if(direct_message->sender->profile_image_url)
				xmlFree(direct_message->sender->profile_image_url);
			if(direct_message->sender->followers_count)
				xmlFree(direct_message->sender->followers_count);
			if(direct_message->sender->friends_count)
				xmlFree(direct_message->sender->friends_count);
			if(direct_message->sender->favourites_count)
				xmlFree(direct_message->sender->favourites_count);
			if(direct_message->sender->statuses_count)
				xmlFree(direct_message->sender->statuses_count);

            free((void*)(direct_message->sender));
        }
        free(direct_message);
	}
	FreeVec(statuses);
}

/*****************************************************************************/

///

/// AmiTwitter Curl functions *************************************************/

// cURL Write
static size_t twitter_curl_write_cb(void *ptr, size_t size, size_t nmemb, void *data) {

    size_t realsize = size * nmemb;
	my_byte_array_append((MyByteArray *)data, (UBYTE*)ptr, realsize);

    return realsize;
}

/*****************************************************************************/

// cURL File
static size_t twitter_curl_file_cb(void *ptr, size_t size, size_t nmemb, void *data) {

    size_t realsize = size * nmemb;
    fwrite(ptr, size, nmemb, (FILE*)data);

    return realsize;
}

/*****************************************************************************/

// Fetch with cURL
int twitter_fetch(twitter_t *twitter, const char *apiuri, MyByteArray *buf) {

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

        set (txt_source, MUIA_HTMLtext_Contents, (int) GetString(&li, MSG_ATTEMPTING));
    }
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    return 0;
}

/*****************************************************************************/

// Fetch Image with cURL
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

// Fetch Images
int twitter_fetch_images(twitter_t *twitter, struct List *statuses) {
    int ret;
    twitter_status_t *status;
	char *url;
    char name[PATH_MAX];
    char path[PATH_MAX];
    struct stat st;
	struct Node *node = NULL;

    if(!statuses) {
        return 0;
    }

	for(node = statuses->lh_TailPred;
		node->ln_Pred;
		node = node->ln_Pred)
	{
		status = (twitter_status_t *)node;

		strncpy(name, status->user->id, PATH_MAX);
        url = status->user->profile_image_url;
        snprintf(path, PATH_MAX, "%s/%s", twitter->images_dir, name);
        ret = stat(path, &st);
        printf("fetch_image: %s\n", url);
        twitter_fetch_image(twitter, url, path);

	}

    return 0;
}

/*****************************************************************************/

// Fetch Recipient Images
int twitter_fetch_images_dirmsg(twitter_t *twitter, struct List *statuses) {
    int ret;
    twitter_direct_message_t *direct_message;
	char *url;
    char name[PATH_MAX];
    char path[PATH_MAX];
    struct stat st;
	struct Node *node = NULL;

    if(!statuses) {
        return 0;
    }

	for(node = statuses->lh_TailPred;
		node->ln_Pred;
		node = node->ln_Pred)
	{
		direct_message = (twitter_direct_message_t *)node;

        strncpy(name, direct_message->recipient->id, PATH_MAX);
        url = direct_message->recipient->profile_image_url;
        snprintf(path, PATH_MAX, "%s/%s", twitter->images_dir, name);
        ret = stat(path, &st);
        printf("fetch_image: %s\n", url);
        twitter_fetch_image(twitter, url, path);

	}

    return 0;
}

/*****************************************************************************/

// Fetch Sender Images
int twitter_fetch_images_dirmsgrcvd(twitter_t *twitter, struct List *statuses) {
    int ret;
    twitter_direct_message_rcvd_t *direct_message;
	char *url;
    char name[PATH_MAX];
    char path[PATH_MAX];
    struct stat st;
	struct Node *node = NULL;

    if(!statuses) {
        return 0;
    }

	for(node = statuses->lh_TailPred;
		node->ln_Pred;
		node = node->ln_Pred)
	{
		direct_message = (twitter_direct_message_rcvd_t *)node;

		strncpy(name, direct_message->sender->id, PATH_MAX);
        url = direct_message->sender->profile_image_url;
        snprintf(path, PATH_MAX, "%s/%s", twitter->images_dir, name);
        ret = stat(path, &st);
        printf("fetch_image: %s\n", url);
        twitter_fetch_image(twitter, url, path);

	}

    return 0;
}

/*****************************************************************************/

///

/// AmiTwitter XML Parser *****************************************************/

// XML parse Statuses node
struct List* twitter_parse_statuses_node(xmlTextReaderPtr reader) {

    int ret;
    xmlElementType type;
    xmlChar *name;
	struct List* statuses = AllocVec(sizeof(struct List), MEMF_ANY|MEMF_CLEAR);
    twitter_status_t *status;

	if(!statuses)
		return NULL;
	NewList(statuses);

    do {
        ret = xmlTextReaderRead(reader);
        type = xmlTextReaderNodeType(reader);
        if(type == XML_READER_TYPE_ELEMENT) {
            name = xmlTextReaderName(reader);
            if(!xmlStrcmp(name, (xmlChar *)"status")) {
                status = twitter_parse_status_node(reader);
                if(status) {
					AddTail(statuses, (struct Node *)status);
                }
            }
            xmlFree(name);
        }
    }while(ret == 1);
    return statuses;
}

/*****************************************************************************/

// Parse Direct Messages Recipient
struct List* twitter_parse_statuses_node_dirmsg(xmlTextReaderPtr reader) {

    int ret;
    xmlElementType type;
    xmlChar *name;
    struct List* statuses = AllocVec(sizeof(struct List), MEMF_ANY|MEMF_CLEAR);
    twitter_direct_message_t *direct_message;

	if(!statuses)
		return NULL;
	NewList(statuses);

    do {
        ret = xmlTextReaderRead(reader);
        type = xmlTextReaderNodeType(reader);
        if(type == XML_READER_TYPE_ELEMENT) {
            name = xmlTextReaderName(reader);
            if(!xmlStrcmp(name, (xmlChar *)"direct-messages")) {
                direct_message = twitter_parse_status_node_dirmsg(reader);
                if(direct_message) {
					AddTail(statuses, (struct Node *)direct_message);
                }
            }
            xmlFree(name);
        }
    }while(ret == 1);
    return statuses;
}

/*****************************************************************************/

// Parse Direct Messages Sender
struct List* twitter_parse_statuses_node_dirmsgrcvd(xmlTextReaderPtr reader) {

    int ret;
    xmlElementType type;
    xmlChar *name;
	struct List* statuses = AllocVec(sizeof(struct List), MEMF_ANY|MEMF_CLEAR);
    twitter_direct_message_rcvd_t *direct_message;

	if(!statuses)
		return NULL;
	NewList(statuses);

    do {
        ret = xmlTextReaderRead(reader);
        type = xmlTextReaderNodeType(reader);
        if(type == XML_READER_TYPE_ELEMENT) {
            name = xmlTextReaderName(reader);
            if(!xmlStrcmp(name, (xmlChar *)"direct-messages")) {
                direct_message = twitter_parse_status_node_dirmsgrcvd(reader);
                if(direct_message) {
                    AddTail(statuses, (struct Node *)direct_message);
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
                status->created_at = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"id")) {
                ret = xmlTextReaderRead(reader);
                status->id = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"text")) {
                ret = xmlTextReaderRead(reader);
                status->text = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"source")) {
                ret = xmlTextReaderRead(reader);
                status->source = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"truncated")) {
                ret = xmlTextReaderRead(reader);
                status->truncated = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"in_reply_to_status_id")) {
                ret = xmlTextReaderRead(reader);
                status->in_reply_to_status_id = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"in_reply_to_user_id")) {
                ret = xmlTextReaderRead(reader);
                status->in_reply_to_user_id = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"favorited")) {
                ret = xmlTextReaderRead(reader);
                status->favorited = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"in_reply_to_screen_name")) {
                ret = xmlTextReaderRead(reader);
                status->in_reply_to_screen_name = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"retweeted_status")) {
                ret = xmlTextReaderRead(reader);
                status->retweeted_status = (char *)xmlTextReaderValue(reader);
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

// Parse Direct Messages - Recipient
twitter_direct_message_t* twitter_parse_status_node_dirmsg(xmlTextReaderPtr reader) {
    int ret;
    xmlElementType type;
    xmlChar *name;
    twitter_direct_message_t *direct_message;
    direct_message = (twitter_direct_message_t *)malloc(sizeof(twitter_direct_message_t));
    memset(direct_message, 0, sizeof(twitter_direct_message_t));

    do{
        ret = xmlTextReaderRead(reader);
        type = xmlTextReaderNodeType(reader);
        if (type == XML_READER_TYPE_ELEMENT) {
            name = xmlTextReaderName(reader);
            if (!xmlStrcmp(name, (xmlChar *)"created_at")) {
                ret = xmlTextReaderRead(reader);
                direct_message->created_at = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"id")) {
                ret = xmlTextReaderRead(reader);
                direct_message->id = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"sender_id")) {
                ret = xmlTextReaderRead(reader);
                direct_message->sender_id = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"text")) {
                ret = xmlTextReaderRead(reader);
                direct_message->text = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"recipient_id")) {
                ret = xmlTextReaderRead(reader);
                direct_message->recipient_id = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"sender_screen_name")) {
                ret = xmlTextReaderRead(reader);
                direct_message->sender_screen_name = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"recipient_screen_name")) {
                ret = xmlTextReaderRead(reader);
                direct_message->recipient_screen_name = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"recipient")) {
                direct_message->recipient = twitter_parse_recipient_node(reader);
            }
            xmlFree(name);
        } else if (type == XML_READER_TYPE_END_ELEMENT) {
            name = xmlTextReaderName(reader);
            if (!xmlStrcmp(name, (xmlChar *)"direct_message")) {
                xmlFree(name);
                break;
            }
            xmlFree(name);
        }
    }while(ret == 1);
    return direct_message;
}

/*****************************************************************************/

// Parse Direct Messages - Sender
twitter_direct_message_rcvd_t* twitter_parse_status_node_dirmsgrcvd(xmlTextReaderPtr reader) {
    int ret;
    xmlElementType type;
    xmlChar *name;
    twitter_direct_message_rcvd_t *direct_message;
    direct_message = (twitter_direct_message_rcvd_t *)malloc(sizeof(twitter_direct_message_rcvd_t));
    memset(direct_message, 0, sizeof(twitter_direct_message_rcvd_t));

    do{
        ret = xmlTextReaderRead(reader);
        type = xmlTextReaderNodeType(reader);
        if (type == XML_READER_TYPE_ELEMENT) {
            name = xmlTextReaderName(reader);
            if (!xmlStrcmp(name, (xmlChar *)"created_at")) {
                ret = xmlTextReaderRead(reader);
                direct_message->created_at = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"id")) {
                ret = xmlTextReaderRead(reader);
                direct_message->id = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"sender_id")) {
                ret = xmlTextReaderRead(reader);
                direct_message->sender_id = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"text")) {
                ret = xmlTextReaderRead(reader);
                direct_message->text = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"recipient_id")) {
                ret = xmlTextReaderRead(reader);
                direct_message->recipient_id = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"sender_screen_name")) {
                ret = xmlTextReaderRead(reader);
                direct_message->sender_screen_name = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"recipient_screen_name")) {
                ret = xmlTextReaderRead(reader);
                direct_message->recipient_screen_name = (char *)xmlTextReaderValue(reader);
            }else if (!xmlStrcmp(name, (xmlChar *)"sender")) {
                direct_message->sender = twitter_parse_sender_node(reader);
            }
            xmlFree(name);
        } else if (type == XML_READER_TYPE_END_ELEMENT) {
            name = xmlTextReaderName(reader);
            if (!xmlStrcmp(name, (xmlChar *)"direct_message")) {
                xmlFree(name);
                break;
            }
            xmlFree(name);
        }
    }while(ret == 1);
    return direct_message;
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
                user->id = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"name")) {
                ret = xmlTextReaderRead(reader);
                user->name = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"screen_name")) {
                ret = xmlTextReaderRead(reader);
                user->screen_name = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"location")) {
                ret = xmlTextReaderRead(reader);
                user->location = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"description")) {
                ret = xmlTextReaderRead(reader);
                user->description = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"profile_image_url")) {
                ret = xmlTextReaderRead(reader);
                user->profile_image_url = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"followers_count")) {
                ret = xmlTextReaderRead(reader);
                user->followers_count = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"friends_count")) {
                ret = xmlTextReaderRead(reader);
                user->friends_count = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"favourites_count")) {
                ret = xmlTextReaderRead(reader);
                user->favourites_count = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"statuses_count")) {
                ret = xmlTextReaderRead(reader);
                user->statuses_count = (char *)xmlTextReaderValue(reader);
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

// Recipient node
twitter_recipient_t* twitter_parse_recipient_node(xmlTextReaderPtr reader) {
    int ret;
    xmlElementType type;
    xmlChar *name;
    twitter_recipient_t *recipient;

    recipient = (twitter_recipient_t *)malloc(sizeof(twitter_recipient_t));
    memset(recipient, 0, sizeof(twitter_recipient_t));
    do{
        ret = xmlTextReaderRead(reader);
        type = xmlTextReaderNodeType(reader);
        if (type == XML_READER_TYPE_ELEMENT) {
            name = xmlTextReaderName(reader);
            if(!xmlStrcmp(name, (xmlChar *)"id")) {
                ret = xmlTextReaderRead(reader);
                recipient->id = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"created_at")) {
                ret = xmlTextReaderRead(reader);
                recipient->created_at = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"name")) {
                ret = xmlTextReaderRead(reader);
                recipient->name = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"screen_name")) {
                ret = xmlTextReaderRead(reader);
                recipient->screen_name = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"location")) {
                ret = xmlTextReaderRead(reader);
                recipient->location = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"description")) {
                ret = xmlTextReaderRead(reader);
                recipient->description = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"profile_image_url")) {
                ret = xmlTextReaderRead(reader);
                recipient->profile_image_url = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"followers_count")) {
                ret = xmlTextReaderRead(reader);
                recipient->followers_count = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"friends_count")) {
                ret = xmlTextReaderRead(reader);
                recipient->friends_count = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"favourites_count")) {
                ret = xmlTextReaderRead(reader);
                recipient->favourites_count = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"statuses_count")) {
                ret = xmlTextReaderRead(reader);
                recipient->statuses_count = (char *)xmlTextReaderValue(reader);
            }
            xmlFree(name);
        }else if(type == XML_READER_TYPE_END_ELEMENT) {
            name = xmlTextReaderName(reader);
            if(!xmlStrcmp(name, (xmlChar *)"recipient")) {
                xmlFree(name);
                break;
            }
            xmlFree(name);
        }
    }while(ret == 1);
    return recipient;
}

/*****************************************************************************/

// Sender node
twitter_sender_t* twitter_parse_sender_node(xmlTextReaderPtr reader) {
    int ret;
    xmlElementType type;
    xmlChar *name;
    twitter_sender_t *sender;

    sender = (twitter_sender_t *)malloc(sizeof(twitter_sender_t));
    memset(sender, 0, sizeof(twitter_sender_t));
    do{
        ret = xmlTextReaderRead(reader);
        type = xmlTextReaderNodeType(reader);
        if (type == XML_READER_TYPE_ELEMENT) {
            name = xmlTextReaderName(reader);
            if(!xmlStrcmp(name, (xmlChar *)"id")) {
                ret = xmlTextReaderRead(reader);
                sender->id = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"created_at")) {
                ret = xmlTextReaderRead(reader);
                sender->created_at = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"name")) {
                ret = xmlTextReaderRead(reader);
                sender->name = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"screen_name")) {
                ret = xmlTextReaderRead(reader);
                sender->screen_name = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"location")) {
                ret = xmlTextReaderRead(reader);
                sender->location = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"description")) {
                ret = xmlTextReaderRead(reader);
                sender->description = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"profile_image_url")) {
                ret = xmlTextReaderRead(reader);
                sender->profile_image_url = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"followers_count")) {
                ret = xmlTextReaderRead(reader);
                sender->followers_count = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"friends_count")) {
                ret = xmlTextReaderRead(reader);
                sender->friends_count = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"favourites_count")) {
                ret = xmlTextReaderRead(reader);
                sender->favourites_count = (char *)xmlTextReaderValue(reader);
            }else if(!xmlStrcmp(name, (xmlChar *)"statuses_count")) {
                ret = xmlTextReaderRead(reader);
                sender->statuses_count = (char *)xmlTextReaderValue(reader);
            }
            xmlFree(name);
        }else if(type == XML_READER_TYPE_END_ELEMENT) {
            name = xmlTextReaderName(reader);
            if(!xmlStrcmp(name, (xmlChar *)"sender")) {
                xmlFree(name);
                break;
            }
            xmlFree(name);
        }
    }while(ret == 1);
    return sender;
}

/*****************************************************************************/

///

/// Display in HTMLtext *******************************************************/

// Display in HTMLtext .mcc (timelines)
void twitter_status_print(twitter_status_t *status) {

    FILE *outfile;

    outfile = freopen("PROGDIR:data/temp/twitter.html", "a+", stdout);

/*  printf("<IMG SRC=PROGDIR:data/temp/%s><p> <b>%s </b> %s <p><small>%s </small><br>",status->user->id, status->user->screen_name, status->text, status->created_at);
	printf(GetString(&li, MSG_STATS), status->user->name, status->user->location ? status->user->location : "n/a", status->user->friends_count, status->user->followers_count,  status->user->statuses_count);
    printf("<p>"); */

    printf("<IMG SRC=PROGDIR:data/temp/%s ALIGN=TOP> <b>%s </b> <p> %s <p><small>%s </small><br>",status->user->id, status->user->screen_name, status->text, status->created_at);
//	printf(GetString(&li, MSG_STATS), status->user->name, status->user->location ? status->user->location : "n/a", status->user->friends_count, status->user->followers_count,  status->user->statuses_count);
    printf("<p>");

    fclose(stdout);
}

/*****************************************************************************/

// Display in HTMLtext .mcc (friends/followers/Users)
void twitter_status_print_friendsfollowers(twitter_status_t *status) {

    FILE *outfile;

    outfile = freopen("PROGDIR:data/temp/twitter.html", "a+", stdout);
    printf(GetString(&li, MSG_STATS2), status->id, status->text, status->created_at);
    printf("<p>");

    fclose(stdout);
}

/*****************************************************************************/

// Display in HTMLtext .mcc (direct_messages/sent)
void twitter_status_print_dirmsg(twitter_direct_message_t *direct_message) {

    FILE *outfile;

    outfile = freopen("PROGDIR:data/temp/twitter.html", "a+", stdout);
    printf("<IMG SRC=PROGDIR:data/temp/%s><p> <b>%s </b> %s  ",direct_message->recipient->id, direct_message->recipient_screen_name, direct_message->text);
    printf("<p>");

    fclose(stdout);
}

/*****************************************************************************/

// Display in HTMLtext .mcc (direct_message)
void twitter_status_print_dirmsgrcvd(twitter_direct_message_rcvd_t *direct_message) {

    FILE *outfile;

    outfile = freopen("PROGDIR:data/temp/twitter.html", "a+", stdout);                                                                                                            
    printf("<IMG SRC=PROGDIR:data/temp/%s><p> <b>%s </b> %s ", direct_message->sender->id, direct_message->sender_screen_name, direct_message->text);
    printf("<p>");

    fclose(stdout);
}

/*****************************************************************************/

///

/// statuses/public_timeline

/*****************************************************************************/

// Show timelines (public, home, user, mentions, all the retweets)
void amitwitter_show_timeline(twitter_t *twitter, struct List *statuses) {

    twitter_status_t *status;
	struct Node *node = NULL;

    if(!statuses) {
        return;
    }

	for(node = statuses->lh_TailPred;
		node->ln_Pred;
		node = node->ln_Pred)
	{
		status = (twitter_status_t*)node;

            twitter_status_print(status);

	}

    charconv();
         
    set (txt_source, MUIA_HTMLtext_URL, (int)"PROGDIR:data/temp/twitter.html");

}

/*****************************************************************************/

// statuses/public_timeline API
struct List* twitter_public_timeline(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
	MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_PUBLIC_TIMELINE,
             twitter->last_public_timeline);

        printf("api_uri: %s\n", api_uri);

	buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

	timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);

	my_byte_array_free (buf, TRUE);

	if(timeline){
		status = (twitter_status_t *)timeline->lh_TailPred;

		if(status && status->id) twitter->last_home_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Shows most recent random messages (statuses/public_timeline)
void amitwitter_public_loop() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_public_timeline(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			twitter_fetch_images(twitter, timeline);
			amitwitter_show_timeline(twitter, timeline);
			twitter_statuses_free(timeline);
		}
        timeline = NULL;

    if(i==1) break;
    }
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// statuses/home_timeline

// statuses/home_timeline API
struct List* twitter_home_timeline(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
	MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_HOME_TIMELINE,
             twitter->last_home_timeline);

        printf("api_uri: %s\n", api_uri);

	buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);

    my_byte_array_free (buf, TRUE);

    if(timeline){
		status = (twitter_status_t *)timeline->lh_TailPred;

		if(status && status->id) twitter->last_home_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Get most recent Tweets (statuses/home_timeline)
void amitwitter_loop() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {   
        timeline = twitter_home_timeline(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			twitter_fetch_images(twitter, timeline);
			amitwitter_show_timeline(twitter, timeline);
			twitter_statuses_free(timeline);
		}
        timeline = NULL;

    if(i==1) break;
    }
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// statuses/user_timeline

// statuses/user_timeline API
struct List* twitter_user_timeline(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
    MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_USER_TIMELINE,
             twitter->last_user_timeline);

        printf("api_uri: %s\n", api_uri);

    buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);

    my_byte_array_free (buf, TRUE);

    if(timeline){
		status = (twitter_status_t *)timeline->lh_TailPred;

		if(status && status->id) twitter->last_user_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Show users most recent messages (statuses/user_timeline)
void amitwitter_user_loop() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_user_timeline(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			twitter_fetch_images(twitter, timeline);
			amitwitter_show_timeline(twitter, timeline);
			twitter_statuses_free(timeline);
		}
        timeline = NULL;

    if(i==1) break;
    }
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// statuses/mentions

// statuses/mentions API
struct List* twitter_mentions(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
    MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_MENTIONS,
             twitter->mentions);

        printf("api_uri: %s\n", api_uri);

    buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);

    my_byte_array_free (buf, TRUE);

    if(timeline){
		status = (twitter_status_t *)timeline->lh_TailPred;

		if(status && status->id) twitter->mentions = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Shows all mentions @Replies (statuses/mentions)
void amitwitter_mentions_loop() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_mentions(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			twitter_fetch_images(twitter, timeline);
			amitwitter_show_timeline(twitter, timeline);
			twitter_statuses_free(timeline);
		}
        timeline = NULL;

    if(i==1) break;
    }
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// statuses/retweeted_by_me

// statuses/twitter_retweeted_by_me API
struct List* twitter_retweeted_by_me(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
    MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_RETWEETED_BY_ME,
             twitter->last_public_timeline);

        printf("api_uri: %s\n", api_uri);

    buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);

    my_byte_array_free (buf, TRUE);

    if(timeline){
		status = (twitter_status_t *)timeline->lh_TailPred;

		if(status && status->id) twitter->last_home_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Shows retweeted by me (statuses/retweeted_by_me)
void amitwitter_retweeted_by_me() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_retweeted_by_me(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			twitter_fetch_images(twitter, timeline);
			amitwitter_show_timeline(twitter, timeline);
			twitter_statuses_free(timeline);
		}
        timeline = NULL;

    if(i==1) break;
    }
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// statuses/retweeted_to_me

// statuses/twitter_retweeted_to_me API
struct List* twitter_retweeted_to_me(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
    MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_RETWEETED_TO_ME,
             twitter->last_public_timeline);

        printf("api_uri: %s\n", api_uri);

    buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);

    my_byte_array_free (buf, TRUE);

    if(timeline){
		status = (twitter_status_t *)timeline->lh_TailPred;

		if(status && status->id) twitter->last_home_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Shows retweeted to me (statuses/retweeted_to_me)
void amitwitter_retweeted_to_me() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_retweeted_to_me(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			twitter_fetch_images(twitter, timeline);
			amitwitter_show_timeline(twitter, timeline);
			twitter_statuses_free(timeline);
		}
        timeline = NULL;

    if(i==1) break;
    }
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// statuses/retweets_of_me

// statuses/twitter_retweets_of_me API
struct List* twitter_retweets_of_me(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
    MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_RETWEETS_OF_ME,
             twitter->last_public_timeline);

        printf("api_uri: %s\n", api_uri);

    buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);

    my_byte_array_free (buf, TRUE);

    if(timeline){
		status = (twitter_status_t *)timeline->lh_TailPred;

        if(status && status->id) twitter->last_home_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Shows retweets of me (statuses/retweets_of_me)
void amitwitter_retweets_of_me() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_retweets_of_me(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			twitter_fetch_images(twitter, timeline);
			amitwitter_show_timeline(twitter, timeline);
			twitter_statuses_free(timeline);
		}
        timeline = NULL;

    if(i==1) break;
    }
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// statuses/update

// Send a new Tweet with cURL  statuses/update API
int twitter_update(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    struct curl_slist *headers=NULL;
    MyByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = my_byte_array_new();
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

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
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

            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");

        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents, (int) GetString(&li, MSG_SENT));
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headers);

    return 0;
}

/*****************************************************************************/

// Send a Tweet (statuses/update)
void amitwitter_update(const char *text) {

    twitter_t *twitter = NULL;

 // fprintf(stdout, "updating...");
    fflush(stdout);

    get(STR_message, MUIA_String_Contents, &text);

 // printf("Message is %u characters long.\n", strlen(text));

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_update(twitter, text);
    twitter_free(twitter);

 // fprintf(stdout, "done\n");
}

/*****************************************************************************/

///

/// users/show

// Show timeline friends/followers/Users
void amitwitter_show_timeline_friendsfollowers(twitter_t *twitter, struct List *statuses) {
    twitter_status_t *status;
	struct Node *node = NULL;


    if(!statuses) {
        return;
    }
	
	for(node = statuses->lh_TailPred;
		node->ln_Pred;
		node = node->ln_Pred)
	{
		status = (twitter_status_t *)node;

            twitter_status_print_friendsfollowers(status);

	}

    charconv();

    set (txt_source, MUIA_HTMLtext_URL, (int)"PROGDIR:data/temp/twitter.html");
}

/*****************************************************************************/

// Show a user with cURL (users/show API)
struct List* twitter_usershow_timeline(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
    MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;


    get(STR_show, MUIA_String_Contents, &text);

	snprintf(api_uri, PATH_MAX, "%s%s%s%s",
			 twitter->base_uri, TWITTER_API_PATH_USER_SHOW, text, ".xml"); 

        printf("api_uri: %s\n", api_uri);

    buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);

    my_byte_array_free (buf, TRUE);

    if(timeline){
		status = (twitter_status_t *)timeline->lh_TailPred;

		if(status && status->id) twitter->last_usershow_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Show a User (users/show)
void amitwitter_usershow() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_usershow_timeline(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			amitwitter_show_timeline_friendsfollowers(twitter, timeline);
			twitter_statuses_free(timeline);
		}
        timeline = NULL;

    if(i==1) break;
    }
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// statuses/friends

// statuses/friends API
struct List* twitter_friends_timeline(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
    MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_FRIENDS,
             twitter->last_friends_timeline);

        printf("api_uri: %s\n", api_uri);

    buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);

    my_byte_array_free (buf, TRUE);

    if(timeline){
		status = (twitter_status_t *)timeline->lh_TailPred;

        if(status && status->id) twitter->last_friends_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Shows friends (statuses/friends API)
void amitwitter_friends_loop() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_friends_timeline(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			amitwitter_show_timeline_friendsfollowers(twitter, timeline);
			twitter_statuses_free(timeline);
		}
        timeline = NULL;

    if(i==1) break;
    }
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// statuses/followers

// statuses/followers API
struct List* twitter_followers_timeline(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
    MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_FOLLOWERS,
             twitter->last_followers_timeline);

        printf("api_uri: %s\n", api_uri);

    buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);

    my_byte_array_free (buf, TRUE);

    if(timeline){
		status = (twitter_status_t *)timeline->lh_TailPred;

        if(status && status->id) twitter->last_followers_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Shows followers (statuses/followers API)
void amitwitter_followers_loop() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_followers_timeline(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			amitwitter_show_timeline_friendsfollowers(twitter, timeline);
			twitter_statuses_free(timeline);
		}
        timeline = NULL;

    if(i==1) break;
    }
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// direct_messages

/*****************************************************************************/

// Show Recieved
void amitwitter_show_timeline_dirmsgrcvd(twitter_t *twitter, struct List *statuses) {
    twitter_direct_message_rcvd_t *direct_message;
	struct Node *node = NULL;


    if(!statuses) {
        return;
    }
	
	for(node = statuses->lh_TailPred;
		node->ln_Pred;
		node = node->ln_Pred)
	{
		direct_message = (twitter_direct_message_rcvd_t *)node;

            twitter_status_print_dirmsgrcvd(direct_message);

	}

    charconv();

    set (txt_source, MUIA_HTMLtext_URL, (int)"PROGDIR:data/temp/twitter.html");
}

/*****************************************************************************/

// direct_messages/recieved API
struct List* twitter_dirmsgrcvd(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
    MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_direct_message_rcvd_t *direct_message;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_DIRECT_MSGRCVD,
             twitter->last_dirmsgrcvd_timeline);

        printf("api_uri: %s\n", api_uri);

    buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node_dirmsgrcvd(reader);
    xmlFreeTextReader(reader);

    my_byte_array_free (buf, TRUE);

    if(timeline){
		direct_message = (twitter_direct_message_rcvd_t *)timeline->lh_TailPred;

		if(direct_message && direct_message->id) twitter->last_dirmsgrcvd_timeline = atol(direct_message->id);
    }
    return timeline;
}

/*****************************************************************************/

// Shows Direct Messages (direct_messages/recieved)
void amitwitter_dirmsgrcvd() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_dirmsgrcvd(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			twitter_fetch_images_dirmsgrcvd(twitter, timeline);
			amitwitter_show_timeline_dirmsgrcvd(twitter, timeline);
		    twitter_statuses_free_dirmsgrcvd(timeline);
		}

        timeline = NULL;

    if(i==1) break;
    }
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// direct_messages/sent

// Show Sent
void amitwitter_show_timeline_dirmsg(twitter_t *twitter, struct List *statuses) {
    twitter_direct_message_t *direct_message;
	struct Node *node = NULL;

    if(!statuses) {
        return;
    }
	
	for(node = statuses->lh_TailPred;
		node->ln_Pred;
		node = node->ln_Pred)
	{
		direct_message = (twitter_direct_message_t *)node;

            twitter_status_print_dirmsg(direct_message);

	}

    charconv();

    set (txt_source, MUIA_HTMLtext_URL, (int)"PROGDIR:data/temp/twitter.html");
}

/*****************************************************************************/

// direct_messages/sent API    
struct List* twitter_dirmsgsent(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
    MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_direct_message_t *direct_message;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_DIRECT_MSGSENT,
             twitter->last_dirmsgsent_timeline);

        printf("api_uri: %s\n", api_uri);

    buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node_dirmsg(reader);
    xmlFreeTextReader(reader);

    my_byte_array_free (buf, TRUE);

    if(timeline){
		direct_message = (twitter_direct_message_t *)timeline->lh_TailPred;

		if(direct_message && direct_message->id) twitter->last_dirmsgsent_timeline = atol(direct_message->id);
    }
    return timeline;
}

/*****************************************************************************/

// Shows Direct Messages (direct_messages/sent)     
void amitwitter_dirmsgsent() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_dirmsgsent(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			twitter_fetch_images_dirmsg(twitter, timeline);
			amitwitter_show_timeline_dirmsg(twitter, timeline);
			twitter_statuses_free_dirmsg(timeline);
		}
        timeline = NULL;

    if(i==1) break;
    }
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// direct_messages/new

// direct_messages/new API Send a Direct Message with cURL
int twitter_direct_message(twitter_t *twitter, const char *screen_name, const char *text) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    struct curl_slist *headers=NULL;
    MyByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = my_byte_array_new();
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

            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");

        return res;
    }
    else {

        set (txt_source, MUIA_HTMLtext_Contents, (int) GetString(&li, MSG_DIRMSGSENT));
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headers);

    return 0;
}

/*****************************************************************************/

// Direct Messages (direct_messages/new)
void amitwitter_direct_message(const char *screen_name, const char *text) {

    twitter_t *twitter = NULL;

 // fprintf(stdout, "sending...");
    fflush(stdout);

    get(STR_user_id, MUIA_String_Contents, &screen_name);
    get(STR_directmessage, MUIA_String_Contents, &text);

 // printf("Message is %u characters long.\n", strlen(text));

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_direct_message(twitter, screen_name, text);
    twitter_free(twitter);

 // fprintf(stdout, "done\n");
}

/*****************************************************************************/

///

/// friendships/create

// Follow a user with cURL (friendships/create API)
int twitter_follow(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    MyByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = my_byte_array_new();
    curl = curl_easy_init();

    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

    get(STR_follow, MUIA_String_Contents, &text);

    snprintf(api_uri, PATH_MAX, "%s%s%s%s",
             twitter->base_uri, TWITTER_API_PATH_FOLLOW, text, ".xml");

        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
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

            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");

        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents,  (int) GetString(&li, MSG_FOLLOWING));
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);

    return 0;
}

/*****************************************************************************/

// Follow a User (friendships/create)
void amitwitter_follow(const char *text) {

    twitter_t *twitter = NULL;

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_follow(twitter, text);
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// friendships/destroy

// Unfollow a user with cURL (friendships/destroy API)
int twitter_unfollow(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    MyByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = my_byte_array_new();
    curl = curl_easy_init();

    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

    get(STR_follow, MUIA_String_Contents, &text);

    snprintf(api_uri, PATH_MAX, "%s%s%s%s",
             twitter->base_uri, TWITTER_API_PATH_UNFOLLOW, text, ".xml");

        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
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

            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");

        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents,  (int) GetString(&li, MSG_UNFOLLOWING));
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);

    return 0;
}

/*****************************************************************************/

// Unfollow a User (friendships/destroy)
void amitwitter_unfollow(const char *text) {

    twitter_t *twitter = NULL;

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_unfollow(twitter, text);
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// account/verify_credentials

// account/verify_credentials API Verify Credentials with cURL
int twitter_verify_credentials(twitter_t *twitter, const char *screen_name, const char *text) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    MyByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = my_byte_array_new();
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

    if(res != 200) {
        printf("error respose code: %ld\n", res);

        error3();

            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");

        return res;
    }
    else {

       set (txt_source, MUIA_HTMLtext_Contents,  (int) GetString(&li, MSG_VALID));
    }

    curl_easy_cleanup(curl);

    return 0;
}

/*****************************************************************************/

// Verify Credentials  (account/verify_credentials)
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

/// account/update_profile

// Update Profile (account/update_profile API)
int twitter_updateprofile(twitter_t *twitter, const char *name, const char *web, const char *location, const char *bio) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    struct curl_slist *headers=NULL;

    MyByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = my_byte_array_new();
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

        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
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

            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");

        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents,  (int) GetString(&li, MSG_UPDATED));
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headers);

    return 0;
}
/*****************************************************************************/

// Update profile (account/update_profile)
void amitwitter_updateprofile(const char *name, const char *web, const char *location, const char *bio) {

    twitter_t *twitter = NULL;

 // fprintf(stdout, "updating profile...");
    fflush(stdout);

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_updateprofile(twitter, name, web, location, bio);
    twitter_free(twitter);

 // fprintf(stdout, "done\n");
}

/*****************************************************************************/

///

/// favorites

// Favorites API
struct List* twitter_favs(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
    MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_FAVS,
             twitter->last_favs_timeline);

        printf("api_uri: %s\n", api_uri);

    buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);

    my_byte_array_free (buf, TRUE);

    if(timeline){
		status = (twitter_status_t *)timeline->lh_TailPred;

        if(status && status->id) twitter->last_favs_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Shows Favorites (favorites API)
void amitwitter_favs() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_favs(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			twitter_fetch_images(twitter, timeline);
			amitwitter_show_timeline(twitter, timeline);
			twitter_statuses_free(timeline);
		}
        timeline = NULL;

    if(i==1) break;
    }

    twitter_free(twitter);
}

/*****************************************************************************/

///

/// notifications/follow

// Notifications (SMS follow) a user with cURL (notifications/follow API)
int twitter_notify(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    MyByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = my_byte_array_new();
    curl = curl_easy_init();

    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

    get(STR_notify, MUIA_String_Contents, &text);

    snprintf(api_uri, PATH_MAX, "%s%s%s%s",
             twitter->base_uri, TWITTER_API_PATH_NOTIFY, text, ".xml");

        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
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

            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");

        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents,  (int) GetString(&li, MSG_SMSFOLLOW));
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);

    return 0;
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

///

/// notifications/leave

// Notifications (SMS leave) Unfollows a user with cURL (notifications/leave API)
int twitter_unnotify(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    MyByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = my_byte_array_new();
    curl = curl_easy_init();

    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

    get(STR_notify, MUIA_String_Contents, &text);

    snprintf(api_uri, PATH_MAX, "%s%s%s%s",
             twitter->base_uri, TWITTER_API_PATH_UNNOTIFY, text, ".xml");

        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
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

            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");

        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents,  (int) GetString(&li, MSG_SMSUNFOLLOW));
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);

    return 0;
}

/*****************************************************************************/

// Notifications unfollow a User with SMS (notifications/leave)
void amitwitter_unnotify(const char *text) {

    twitter_t *twitter = NULL;

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_unnotify(twitter, text);
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// blocks/create

// Block a user with cURL (blocks/create API)
int twitter_block(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    MyByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = my_byte_array_new();
    curl = curl_easy_init();

    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

    get(STR_block, MUIA_String_Contents, &text);

    snprintf(api_uri, PATH_MAX, "%s%s%s%s",
             twitter->base_uri, TWITTER_API_PATH_BLOCK, text, ".xml");

        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
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

            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");

        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents,  (int) GetString(&li, MSG_BLOCKING));
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);

    return 0;
}

/*****************************************************************************/

// Block a User (blocks/create)
void amitwitter_block(const char *text) {

    twitter_t *twitter = NULL;

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_block(twitter, text);
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// blocks/destroy

// Unblocks a user with cURL (blocks/destroy API)
int twitter_unblock(twitter_t *twitter, const char *status) {

    CURL *curl;
    CURLcode code;
    long res;
    char api_uri[PATH_MAX];
    struct curl_httppost *formpost=NULL;
    MyByteArray *buf;
    char userpass[256];

    snprintf(userpass, 256, "%s:%s", twitter->user, twitter->pass);
    buf = my_byte_array_new();
    curl = curl_easy_init();

    if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

    get(STR_block, MUIA_String_Contents, &text);

    snprintf(api_uri, PATH_MAX, "%s%s%s%s",
             twitter->base_uri, TWITTER_API_PATH_UNBLOCK, text, ".xml");


        printf("api_uri: %s\n", api_uri);

    curl_easy_setopt(curl, CURLOPT_URL, api_uri);
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

            fwrite(buf->data, 1, buf->len, stderr);
            fprintf(stderr, "\n");

        return res;
    }
     else {

        set (txt_source, MUIA_HTMLtext_Contents,  (int) GetString(&li, MSG_UNBLOCKING));
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);

    return 0;
}

/*****************************************************************************/

// Unblock a User (blocks/destroy)
void amitwitter_unblock(const char *text) {

    twitter_t *twitter = NULL;

    twitter = twitter_new();
    twitter_config(twitter);
    twitter_unblock(twitter, text);
    twitter_free(twitter);
}

/*****************************************************************************/

///

/// blocks/blocking

// blocks/blocking API
struct List* twitter_blocking_timeline(twitter_t *twitter) {

    int ret;
	struct List *timeline = NULL;
    MyByteArray *buf;
    xmlTextReaderPtr reader;
    char api_uri[PATH_MAX];
    twitter_status_t *status;

    snprintf(api_uri, PATH_MAX, "%s%s?since_id=%lu",
             twitter->base_uri, TWITTER_API_PATH_BLOCKING,
             twitter->last_blocking_timeline);

        printf("api_uri: %s\n", api_uri);

    buf = my_byte_array_new();

    ret = twitter_fetch(twitter, api_uri, buf);

    reader = xmlReaderForMemory((const char *)buf->data, buf->len,
                                NULL, NULL, 0);

    timeline = twitter_parse_statuses_node(reader);
    xmlFreeTextReader(reader);

    my_byte_array_free (buf, TRUE);

    if(timeline){
		status = (twitter_status_t *)timeline->lh_TailPred;

        if(status && status->id) twitter->last_blocking_timeline = atol(status->id);
    }
    return timeline;
}

/*****************************************************************************/

// Shows blocking (blocks/blocking)
void amitwitter_blocking_loop() {

    int i;

    twitter_t *twitter = NULL;

	struct List* timeline = NULL;

    twitter = twitter_new();
    twitter_config(twitter);

    for(i=1; i<2; i++) {
        timeline = twitter_blocking_timeline(twitter);

		if(timeline)
		{
			printf("timeline num: %ld\n", my_list_length(timeline));

			amitwitter_show_timeline_friendsfollowers(twitter, timeline);
			twitter_statuses_free(timeline);
		}
        timeline = NULL;

    if(i==1) break;
    }

    twitter_free(twitter);
}

/*****************************************************************************/

///

/// upload TwitPic

// Upload a picture via TwitPic
int twitter_uploadtwitpic() {

  CURL *curl;
  CURLcode code;
  long res;
  const char *media;
  const char *message;

  struct curl_httppost *formpost=NULL;
  struct curl_httppost *lastptr=NULL;
  struct curl_slist *headerlist=NULL;
  static const char buf[] = "Expect:";

  get(pop1, MUIA_String_Contents, &media);
  get(STR_twitpictweet, MUIA_String_Contents, &message);

  curl_global_init(CURL_GLOBAL_ALL);

  curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "media",
               CURLFORM_FILE, media,
               CURLFORM_END);

  curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "username",
               CURLFORM_COPYCONTENTS, username,
               CURLFORM_END);


  curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "password",
               CURLFORM_COPYCONTENTS, password,
               CURLFORM_END);

  curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "message",
               CURLFORM_COPYCONTENTS, message, 
               CURLFORM_END);

  curl = curl_easy_init();

  if(!curl) {
        printf("error: curl_easy_init()\n");
        error2();
        return -1;
    }

  headerlist = curl_slist_append(headerlist, buf);
  if(curl) {

    curl_easy_setopt(curl, CURLOPT_URL, "http://www.twitpic.com/api/uploadAndPost");

 // if ( (argc == 2) && (!strcmp(argv[1], "noexpectheader")) )

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

    code = curl_easy_perform(curl);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
    if(res != 200) {

        printf("error respose code: %ld\n", res);
        error();

            fprintf(stderr, "\n");

        return res;
    }
    else {

        set (txt_source, MUIA_HTMLtext_Contents, (int) GetString(&li, MSG_PICTUREUPLOADED));
    }
    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all (headerlist); 
  }
  return 0;
}

/*****************************************************************************/

///

/// Main Program **************************************************************/

int main(int argc, char *argv[]) {

  APTR app;

  static char *Pages[6];

  char *twitContents, *dirmsgContents;
  char twitLength[512], dirmsgLength[512];
  enum { TYPING_TWIT, TYPING_DIRMSG=1 };

  if ( ! Open_Libs() ) {

	Close_Libs();
    return(0);
  }

  localizeNewMenu(Menu);
  localizeTheBar(buttons);

  Pages[0] = GetString(&li, MSG_SHOWUSER),
  Pages[1] = GetString(&li, MSG_FOLLOW),
  Pages[2] = GetString(&li, MSG_BLOCK),
  Pages[3] = GetString(&li, MSG_NOTIFY),
  Pages[4] = NULL,

  app = ApplicationObject,
      MUIA_Application_Title  , "AmiTwitter",
      MUIA_Application_Version , "$VER: "VSTRING"",
      MUIA_Application_Copyright , "�2009-10, IKE",
      MUIA_Application_Author  , "IKE",
      MUIA_Application_Description, "A Twitter program",
      MUIA_Application_Base  , "AmiTwitter",

      MUIA_Application_DiskObject, dobj = GetDiskObject("PROGDIR:AmiTwitter"),

/*****************************************************************************/

      // The Preferences Window
      SubWindow, win_preferences = WindowObject,
          MUIA_Window_Title, GetString(&li, MSG_SETTINGS),
          MUIA_Window_ID, MAKE_ID('P','R','E','F'),

          WindowContents, VGroup,  MUIA_Register_Frame, TRUE,
              Child, VGroup,

                  Child, ColGroup(2), GroupFrameT(GetString(&li, MSG_SETTINGS2)),
                       Child, Label2(GetString(&li, MSG_USERNAME)),
                       Child, str_user_pref = StringObject, StringFrame, MUIA_ObjectID, 1, End,
                       Child, Label2(GetString(&li,MSG_PASSWORD)),
                       Child, str_pass_pref = StringObject, StringFrame, MUIA_ObjectID, 2, MUIA_String_Secret, TRUE, End,
                  End,

              End,       

              Child, ColGroup(2), GroupFrame,

				  Child, Label1("Charset:"),
				  Child, cy1  = CycleObject, MUIA_Cycle_Entries, charset, MUIA_ExportID, 7, End,
              End,

              Child, HGroup, MUIA_Group_SameSize,  TRUE,
                  Child, but_save   = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_SAVE)),
                  Child, but_test   = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_TEST2)),
                  Child, but_cancel = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CANCEL)),
              End,

              Child, VGroup, GroupFrameT(GetString(&li, MSG_0027)),

                  MUIA_Background, MUII_GroupBack,
                  Child, TextObject, TextFrame,
                       MUIA_Background, MUII_TextBack,
                       MUIA_Text_Contents, GetString(&li, MSG_REGISTER),
                  End,

                  Child, VGroup,

                       Child, HGroup,
                            Child, ImageObject,
                                 MUIA_Frame, MUIV_Frame_None,
                                 MUIA_Image_Spec, "5:data/program_images/twitter.jpg",
                                 MUIA_Image_FreeVert, TRUE,
                                 MUIA_Image_FreeHoriz, TRUE,
                                 MUIA_FixWidth, 20,
                                 MUIA_FixHeight, 20,
                            End,
                       End,

                       Child, HGroup,
                            Child, HSpace(0),
                            Child, ColGroup(0),
                            Child, twittersite = urlTextObject(MUIMasterBase,"http://twitter.com/", GetString(&li, MSG_ESTABLISH), MUIV_Font_Normal),
                            Child, passforgot =  urlTextObject(MUIMasterBase, "http://twitter.com/account/resend_password", GetString(&li, MSG_FORGOT), MUIV_Font_Normal),
                            Child, HSpace(0),
                            Child, HSpace(0),
                            End,
                            Child, HSpace(0),
                       End,
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // The Donate Window
      SubWindow, win_donate = WindowObject,
          MUIA_Window_Title, GetString(&li, MSG_DONATE),
          MUIA_Window_ID, MAKE_ID('D','N','A','T'),

          WindowContents, VGroup,

              Child, VGroup, GroupFrame,

                  MUIA_Background, MUII_GroupBack,
                  Child, TextObject, TextFrame,
                       MUIA_Background, MUII_TextBack,
                       MUIA_Text_Contents, GetString(&li, MSG_DONATE2),
                  End,

                  Child, HGroup,
                  Child, HSpace(0),
                  Child, ColGroup(0),
                  Child, donate = urlTextObject(MUIMasterBase,"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=8981293h", GetString(&li, MSG_DONATE), MUIV_Font_Normal),
                  Child, HSpace(0),
                  Child, HSpace(0),
                  End,
                  Child, HSpace(0),
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // The TwitPic Window
      SubWindow, win_tweet = WindowObject,
          MUIA_Window_Title, GetString(&li, MSG_UPLOADVIATWITPIC), 
          MUIA_Window_ID, MAKE_ID('T','W','E','T'),

          WindowContents, VGroup,

              Child, HGroup, GroupFrameT(GetString(&li, MSG_UPLOADAPICTURE)), 

                  Child, ImageObject,
                      MUIA_Frame, MUIV_Frame_None,
                      MUIA_Image_Spec, "5:data/program_images/twitpic.gif",
                      MUIA_Image_FreeVert, TRUE,
                      MUIA_Image_FreeHoriz, TRUE,
                      MUIA_FixWidth, 70,
                      MUIA_FixHeight, 65,
                      MUIA_ShortHelp, (GetString(&li, MSG_TWITPICACCEPTS)), 
                  End,

                  Child, VGroup,
                      Child, HGroup,

				          Child, pop1 = PopaslObject,
						  MUIA_Popstring_String, KeyString(0,256,'f'), MUIA_CycleChain, TRUE,
						  MUIA_Popstring_Button, PopButton(MUII_PopFile),
                          MUIA_ShortHelp, (GetString(&li, MSG_PATHTOIMAGE)), 
						  ASLFR_TitleText,       (GetString(&li, MSG_SELECTAPICTURE)), 
                          ASLFR_DoPatterns,       TRUE,
                          ASLFR_InitialDrawer,    (ULONG)"RAM:",
                          ASLFR_InitialPattern,   (ULONG) "#?.(gif|jpg|png)",
                          ASLFR_InitialHeight,    300,
                          ASLFR_InitialWidth,     256,
                          ASLFR_InitialLeftEdge,  400,
                          ASLFR_InitialTopEdge,   300,
					      End,
                      End,

                      Child, STR_twitpictweet = BetterStringObject, StringFrame, MUIA_CycleChain, TRUE,
                      MUIA_ShortHelp, (GetString(&li, MSG_TEXTMESSAGEOPTIONAL)), End, 

                      Child, HGroup,
   
                          Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,
                              Child, clear_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CLEAR)),
                              Child, sendupdate_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_UPLOAD)), 
                              Child, but_cancel_tweet = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CANCEL)),
                              End,
                          End,
                      End,
                  End,
              End,
          End,

/*****************************************************************************/

      // The Search Window
      SubWindow, win_search = WindowObject,
          MUIA_Window_Title,  GetString(&li, MSG_SEARCH2),
          MUIA_Window_ID, MAKE_ID('S','R','C','H'),

          WindowContents, VGroup,

              Child, HGroup, GroupFrameT( GetString(&li, MSG_SEARCH2)),

                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2( GetString(&li, MSG_SEARCH5)),
                           Child, HGroup,
                           Child, STR_search = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp,  GetString(&li, MSG_SEARCH6),
                           End,
                      End,
                      Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,
                           Child, clear_search_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CLEAR)),
                           Child, search_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_SEARCH)),
                           Child, but_search_cancel = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CANCEL)),
                      End,
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // The User Window
      SubWindow, win_follow = WindowObject,
          MUIA_Window_Title,  GetString(&li, MSG_MANAGE),
          MUIA_Window_ID, MAKE_ID('U','S','E','R'),

          WindowContents, VGroup,
            Child, RegisterGroup(Pages),
              Child, HGroup, GroupFrameT(GetString(&li, MSG_SHOWUSER2)),

                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2(GetString(&li, MSG_FOLLOW3)),
                           Child, HGroup,
                           Child, STR_show = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, GetString(&li, MSG_SHOWUSER4),
                           End,
                      End,
                      Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,
                           Child, clear_show_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CLEAR)),
                           Child, show_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_SHOWUSER5)),
                           Child, but_show_cancel = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CANCEL)),
                      End,
                  End,
              End,

              Child, HGroup, GroupFrameT( GetString(&li, MSG_FOLLOW2)),

                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2( GetString(&li, MSG_FOLLOW3)),
                           Child, HGroup,
                           Child, STR_follow = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp,  GetString(&li, MSG_FOLLOW4),
                           End,
                      End,
                      Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,
                           Child, clear_follow_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CLEAR)),
                           Child, follow_gad =  MUI_MakeObject(MUIO_Button, GetString(&li, MSG_FOLLOW5)),
                           Child, unfollow_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_UNFOLLOOW)),
                           Child, but_follow_cancel = MUI_MakeObject(MUIO_Button,  GetString(&li, MSG_CANCEL)),
                      End,
                  End,
              End,

              Child, HGroup, GroupFrameT( GetString(&li, MSG_BLOCKING2)),

                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2(GetString(&li, MSG_FOLLOW3)),
                           Child, HGroup,
                           Child, STR_block = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, GetString(&li, MSG_BLOCKING4),
                           End,
                      End,
                      Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,
                           Child, clear_block_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CLEAR)),
                           Child, block_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_BLOCKING5)),
                           Child, unblock_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_UNBLOCK)),
                           Child, but_block_cancel = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CANCEL)),
                      End,
                  End,
              End,

              Child, HGroup, GroupFrameT(GetString(&li, MSG_SMS)),

                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2(GetString(&li, MSG_FOLLOW3)),
                           Child, HGroup,
                           Child, STR_notify = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, GetString(&li, MSG_SMS3),
                           End,
                      End,
                      Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,
                           Child, clear_notify_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CLEAR)),
                           Child, notify_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_RECEIVE)),
                           Child, unnotify_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_STOP)),
                           Child, but_notify_cancel = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CANCEL)),
                      End,
                  End,
              End,

            End,

          End,
      End,

/*****************************************************************************/

      // The Direct Message Window
      SubWindow, win_dirmsg = WindowObject,
          MUIA_Window_Title, GetString(&li, MSG_DIRMSG2),
          MUIA_Window_ID, MAKE_ID('D','I','R','M'),

          WindowContents, VGroup,

              Child, HGroup, GroupFrameT(GetString(&li, MSG_DIRMSG2)),
                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2(GetString(&li, MSG_RECIPIENT)),
                           Child, STR_user_id = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, GetString(&li, MSG_SCREENNAME),
                      End,

                      Child, HGroup,
                           Child, HSpace(2),
                           Child, STR_dirmsgLength = BetterStringObject, MUIA_String_Contents, "140",
                           MUIA_BetterString_NoInput, MUIA_BetterString_Columns, 4,
                           MUIA_String_MaxLen, 4, MUIA_CycleChain, FALSE, End,
                      End,
  
                      Child, HGroup,
                           Child, Label2(GetString(&li, MSG_MESSAGE)),
                           Child, STR_directmessage = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, GetString(&li, MSG_MESSAGE2),
                      End,
 
                     Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,
                           Child, clear_dm_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CLEAR)),
                           Child, send_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_SEND5)),
                           Child, but_cancel_dm = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CANCEL)),
                     End,
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // Update Profile Window
      SubWindow, win_userprofile = WindowObject,
          MUIA_Window_Title, GetString(&li, MSG_UPDATEPROFILE),
          MUIA_Window_ID, MAKE_ID('P','R','F','L'),

          WindowContents, VGroup,

              Child, HGroup, GroupFrameT(GetString(&li, MSG_UPDATEPROFILE2)),
                  Child, VGroup,
                      Child, HGroup,
                           Child, Label2(GetString(&li, MSG_PROFNAME)),
                           Child, STR_profile_name = BetterStringObject, StringFrame, MUIA_ObjectID, 3,
                           MUIA_String_MaxLen, 21, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, GetString(&li, MSG_PROFNAME2),
                      End,

                      Child, HGroup,
                           Child, Label2(GetString(&li, MSG_WEB)),
                           Child, STR_profile_web = BetterStringObject, StringFrame, MUIA_ObjectID, 4,
                           MUIA_String_MaxLen, 101, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, GetString(&li, MSG_WEB2),
                      End,

                      Child, HGroup,
                           Child, Label2(GetString(&li, MSG_LOCATION)),
                           Child, STR_profile_location = BetterStringObject, StringFrame, MUIA_ObjectID, 5,
                           MUIA_String_MaxLen, 31, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, GetString(&li, MSG_LOCATION2),
                      End,

                      Child, HGroup,
                           Child, Label2(GetString(&li, MSG_BIO)),
                           Child, STR_profile_bio = BetterStringObject, StringFrame, MUIA_ObjectID, 6,
                           MUIA_String_MaxLen, 161, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, GetString(&li, MSG_BIO2),
                      End,

                     Child, HGroup, MUIA_Group_SameSize, FALSE, MUIA_CycleChain, TRUE,                     
                           Child, update_profile_gad = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_UPDATEPROFILE3)),
                           Child, but_cancel_profile = MUI_MakeObject(MUIO_Button, GetString(&li, MSG_CANCEL)),
                     End,
                  End,
              End,
          End,
      End,

/*****************************************************************************/

      // The Main Program Window
      SubWindow, window = WindowObject,
          MUIA_Window_Title, ""VERS"", 
          MUIA_Window_ID , MAKE_ID('W','T','M','F'),
          MUIA_Window_Menustrip,      MUI_MakeObject(MUIO_MenustripNM,Menu,0),

          WindowContents, VGroup,

              Child, VGroup, GroupFrame,
                  Child, HGroup,

                      Child, RectangleObject, MUIA_Weight, 35, End,
                      Child, Label2(GetString(&li, MSG_WHAT)),

                      Child, STR_login = BetterStringObject, MUIA_BetterString_NoInput, MUIA_BetterString_Columns,8, TRUE,
                  End,
              End,

              Child, VGroup,
                  Child, HGroup,
                           Child, HSpace(2),
                           Child, STR_twitLength = BetterStringObject, MUIA_String_Contents, "140",
                           MUIA_BetterString_NoInput, MUIA_BetterString_Columns, 4,
                           MUIA_String_MaxLen, 4, MUIA_CycleChain, FALSE, End,
                      End,
                      Child, HGroup,
                           Child, Label2(GetString(&li, MSG_SEND3)),
                           Child, HGroup,
                           Child, STR_message = BetterStringObject, StringFrame,
                           MUIA_String_MaxLen, 141, MUIA_CycleChain, TRUE, End,
                           MUIA_ShortHelp, GetString(&li, MSG_ENTERYOURTWEET), 
                           End,
                      End,
                  End,
              End,

              Child, VGroup, GroupFrame, 

                 Child, toolbar = TheBarObject,
                       GroupFrame, MUIA_Group_SameSize, FALSE,
                       MUIA_Group_Horiz,       TRUE,
                       MUIA_TheBar_EnableKeys, TRUE, 
                       MUIA_TheBar_Borderless, TRUE,
                       MUIA_TheBar_Sunny,      TRUE,
                       MUIA_TheBar_Raised,     TRUE,
                       MUIA_TheBar_Scaled,     FALSE,
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
                           Child, urltxtlink2 = urlTextObject(MUIMasterBase,"https://sourceforge.net/projects/amitwitter/", "AmiTwitter SourceForge", MUIV_Font_Normal),
                           Child, mailtxtlink = urlTextObject(MUIMasterBase,"mailto:ikepgh@ezcyberspace.com", GetString(&li, MSG_FEEDBACK), MUIV_Font_Normal),
                           Child, urltxtlink3 = urlTextObject(MUIMasterBase,"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=8981293h", GetString(&li, MSG_DONATE), MUIV_Font_Normal),
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
    app,2,MUIM_Application_ReturnID,TIMELINE);

  DoMethod(buttons[B_RETWEETS].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,RETWEETS);

  DoMethod(buttons[B_REPLIES].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,REPLIES);

  DoMethod(buttons[B_RELOAD].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,RELOAD);

  DoMethod(buttons[B_SEARCH].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,BTN_SEARCH);

  DoMethod(buttons[B_FOLLOW].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,USERS);

  DoMethod(buttons[B_DIRECTMESSAGE].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,DIRMSG);

  DoMethod(buttons[B_TWEET].obj,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,TWEET);


  // Return keys
  DoMethod(STR_message,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
    app,2,MUIM_Application_ReturnID,SENDUPDATE2);

  DoMethod(STR_directmessage,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
    app,2,MUIM_Application_ReturnID,DIRECTMESSAGE);

  DoMethod(STR_search,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
    app,2,MUIM_Application_ReturnID,SEARCH);

  DoMethod(STR_show,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
    app,2,MUIM_Application_ReturnID,USERSHOW);


  // Prefs subwindow
  DoMethod(but_save,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,SAVE);
    set(but_save, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_SAVE2));

  DoMethod(but_test,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,TEST);
    set(but_test, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_TEST3));

  DoMethod(but_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCEL);
    set(but_cancel, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_DONTSAVE));

  DoMethod(win_preferences,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_preferences,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Donate subwindow
  DoMethod(win_donate,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_donate,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Send a TwitPic subwindow
  DoMethod(clear_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEAR);
    set(clear_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CLEARTWITPIC));

  DoMethod(sendupdate_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,SENDUPDATE);
    set(sendupdate_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_UPLOADTWITPIC));

  DoMethod(but_cancel_tweet,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELTWEET);
    set(but_cancel_tweet, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CANCELTWITPIC));

  DoMethod(win_tweet,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_tweet,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Search subwindow
  DoMethod(clear_search_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEARSEARCH);
    set(clear_search_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CLEAR9));

  DoMethod(search_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,SEARCH);
    set(search_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_SEARCH2));

  DoMethod(but_search_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELSEARCH);
    set(but_search_cancel, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CANCEL11));

  DoMethod(win_search,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_search,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Manage Users subwindow
  DoMethod(clear_follow_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEARFOLLOW);
    set(clear_follow_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CLEAR10));

  DoMethod(follow_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,FOLLOW);
    set(follow_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_FOLLOWUSER));

  DoMethod(unfollow_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,UNFOLLOW);
    set(unfollow_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_UNFOLLOWUSER));

  DoMethod(but_follow_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELFOLLOW);
    set(but_follow_cancel, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CANCEL12));

  DoMethod(win_follow,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_follow,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Block 
  DoMethod(clear_block_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEARBLOCK);
    set(clear_block_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CLEAR10));

  DoMethod(block_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,BLOCK);
    set(block_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_BLOCKUSER));

  DoMethod(unblock_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,UNBLOCK);
    set(unblock_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_UNBLOCKUSER));

  DoMethod(but_block_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELBLOCK);
    set(but_block_cancel, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CANCEL12));


  // Notifications SMS 
  DoMethod(clear_notify_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEARNOTIFY);
    set(clear_notify_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CLEAR10));

  DoMethod(notify_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,NOTIFY);
    set(notify_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_SMSNOTE));

  DoMethod(unnotify_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,UNNOTIFY);
    set(unnotify_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_SMSSTOP));

  DoMethod(but_notify_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELNOTIFY);
    set(but_notify_cancel, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CANCEL12));


  // Show Users
  DoMethod(clear_show_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEARSHOW);
    set(clear_show_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CLEAR10));

  DoMethod(show_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,USERSHOW);
    set(show_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_SHOWTWEETS));

  DoMethod(but_show_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELSHOW);
    set(but_show_cancel, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CANCEL12));


  // Direct Message subwindow
  DoMethod(clear_dm_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CLEARDM);
    set(clear_dm_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CLEAR15));

  DoMethod(send_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,DIRECTMESSAGE);
    set(send_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_DIRMSG2));

  DoMethod(but_cancel_dm,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELDM);
    set(but_cancel_dm, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CANCEL16));

  DoMethod(win_dirmsg,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_dirmsg,3,MUIM_Set,MUIA_Window_Open,FALSE);


  // Update Profile subwindow
  DoMethod(update_profile_gad,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,UPDATEPROFILE);
    set(update_profile_gad, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_UPDATEPROFILE4));

  DoMethod(but_cancel_profile,MUIM_Notify,MUIA_Pressed,FALSE,
    app,2,MUIM_Application_ReturnID,CANCELPROFILE);
    set(but_cancel_profile, MUIA_ShortHelp, (ULONG) GetString(&li, MSG_CANCEL17));

  DoMethod(win_userprofile,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    win_userprofile,3,MUIM_Set,MUIA_Window_Open,FALSE);

  // Tweet / Direct Message character countdown
  DoMethod((Object*)STR_message,MUIM_Notify,MUIA_String_Contents,
  MUIV_EveryTime,(Object*)app, 2, MUIM_Application_ReturnID,TYPING_TWIT);

  DoMethod((Object*)STR_directmessage,MUIM_Notify,MUIA_String_Contents,
  MUIV_EveryTime,(Object*)app, 2, MUIM_Application_ReturnID,TYPING_DIRMSG);

  // Codesets conversion
  DoMethod(cy1, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, app, 3,
  MUIM_Set, MUIM_Application_ReturnID, MUIV_TriggerValue);

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

        switch (DoMethod(app,MUIM_Application_NewInput,&sigs)) {

              // Quit program
                case MUIV_Application_ReturnID_Quit:
                     running = FALSE;
                     break;

                // Tweet character countdown
                case TYPING_TWIT:
                     get(STR_message, MUIA_String_Contents, &twitContents);

                     if(twitContents) {

                          sprintf(twitLength,"%d", 140-strlen(twitContents) ); 

                          set(STR_twitLength, MUIA_String_Contents, (int)twitLength);

                          if( strlen(twitContents) > 140) {

                               twitContents[140]='\0'; //chop off all characters after character 140

                               set(STR_message, MUIA_String_Contents, (int)twitContents);
                          }
                     }
                     break;

                // Dir Msg character countdown
                case TYPING_DIRMSG:
                     get(STR_directmessage, MUIA_String_Contents, &dirmsgContents);

                     if(dirmsgContents) {

                          sprintf(dirmsgLength,"%d", 140-strlen(dirmsgContents) );  

                          set(STR_dirmsgLength, MUIA_String_Contents, (int)dirmsgLength);

                          if( strlen(dirmsgContents) > 140) {
                               dirmsgContents[140]='\0'; 

                               set(STR_directmessage, MUIA_String_Contents, (int)dirmsgContents);
                          }
                     }
                     break;

                // Timeline
                case TIMELINE:
                case MEN_TIMELINE:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_loop();
                     break;

                // Retweets
                case MEN_RETWEETBYME:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_retweeted_by_me();
                     break;

                case RETWEETS:
                case MEN_RETWEETTOME:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_retweeted_to_me();
                     break;

                case MEN_RETWEETOFME:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_retweets_of_me();
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

                // Search 
                case MEN_SEARCH:
                case BTN_SEARCH:
                     set(clear_search_gad,MUIA_Disabled,FALSE);
                 	 set(search_gad,MUIA_Disabled,FALSE);
                 	 set(but_search_cancel,MUIA_Disabled,FALSE);
                     set(win_search, MUIA_Window_Open, TRUE);
                     break;

                case CLEARSEARCH:
                     set(STR_search, MUIA_String_Contents,0);
                     break;

                case SEARCH:
                     set(clear_search_gad,MUIA_Disabled,TRUE);
                 	 set(search_gad,MUIA_Disabled,TRUE);
                 	 set(but_search_cancel,MUIA_Disabled,TRUE);
                     MUI_RequestA(app,window,0, GetString(&li, MSG_SEARCH9), GetString(&li, MSG_OK), GetString(&li, MSG_SEARCH10), NULL);
                     set(win_search, MUIA_Window_Open, FALSE);
                     break;

                case CANCELSEARCH:
                     set(win_search, MUIA_Window_Open, FALSE);
                     break;

                // Users
                case MEN_USERS:
                case USERS:
                     set(win_follow, MUIA_Window_Open, TRUE);
                     break;

                case CLEARFOLLOW:
                     set(STR_follow, MUIA_String_Contents,0);
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

                // Users/Block
                case CLEARBLOCK:
                     set(STR_block, MUIA_String_Contents,0);
                     break;

                case BLOCK:
                     result=MUI_RequestA(app,0,0, GetString(&li, MSG_BLOCK2), GetString(&li, MSG_BLOCK3), GetString(&li, MSG_BLOCK4), 0);

                        if(result==1)
                            amitwitter_block(optarg);
                        else
                            break;
                     break;

                case UNBLOCK:
                     amitwitter_unblock(optarg);
                     break;

                case CANCELBLOCK:
                     set(win_follow, MUIA_Window_Open, FALSE);
                     break;

                // Users/Notify
                case CLEARNOTIFY:
                     set(STR_notify, MUIA_String_Contents,0);
                     break;

                case NOTIFY:
                     amitwitter_notify(optarg);
                     break;

                case UNNOTIFY:
                     amitwitter_unnotify(optarg);
                     break;

                case CANCELNOTIFY:
                     set(win_follow, MUIA_Window_Open, FALSE);
                     break;

                // Users/Show
                case CLEARSHOW:
                     set(STR_show, MUIA_String_Contents,0);
                     break;

                case USERSHOW:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_usershow();
                     break;

                case CANCELSHOW:
                     set(win_follow, MUIA_Window_Open, FALSE);
                     break;

                // Direct Message
                case DIRMSG:
                case MEN_DIRMSG:
                     set(win_dirmsg, MUIA_Window_Open, TRUE);
                     break;

                case CLEARDM:
                     set(STR_directmessage, MUIA_String_Contents,0);
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

                case MEN_DIRMSGRCVD:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_dirmsgrcvd();
                     break;

                // Tweet
                case TWEET:
                case MEN_TWEET:
                     set(win_tweet, MUIA_Window_Open, TRUE);              
                     break;

                case CLEAR:
                     set(STR_twitpictweet, MUIA_String_Contents,0);
                     break;

                case SENDUPDATE:
                     twitter_uploadtwitpic();
                     break;

                case SENDUPDATE2:
                     amitwitter_update(optarg);
                     break;

                case CANCELTWEET:
                     set(win_tweet, MUIA_Window_Open, FALSE);
                     break;

                // My Tweets
                case MEN_MYTWEET:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_user_loop();
                     break;

                // My Favorites
                case MEN_FAVS:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_favs();
                     break;

                // Quit
                case MEN_QUIT:
                     result=MUI_RequestA(app,0,0, GetString(&li, MSG_0017), GetString(&li, MSG_QUIT2), GetString(&li, MSG_QUIT3), 0);

                        if(result==1)
                           running=FALSE;
                     break;

                // My Friends
                case MEN_FRIENDS:
                     remove("PROGDIR:data/temp/twitter.html");
                     amitwitter_friends_loop();
                     break;

                // My Followers
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

                // Settings
                case MEN_PREFS:
                     set(win_preferences, MUIA_Window_Open, TRUE);
                     break;

                case SAVE:
                     DoMethod(app,MUIM_Application_Save,MUIV_Application_Save_ENV);
                     DoMethod(app,MUIM_Application_Save,MUIV_Application_Save_ENVARC);

                     set (STR_login, MUIA_String_Contents, (int)username);

                     MUI_RequestA(app,0,0, GetString(&li, MSG_SAVE3), GetString(&li, MSG_OK), GetString(&li, MSG_SAVED), NULL);

                     set(win_preferences, MUIA_Window_Open, FALSE);
                     break;

                case TEST:
                     amitwitter_verify_credentials(screen_name, text);
                     break;

                case CANCEL:
                     set(win_preferences, MUIA_Window_Open, FALSE);
                     break;

                // Update Profile
                case MEN_USERPROFILE:
                     MUI_RequestA(app,window,0,GetString(&li,
                     MSG_UPDATEPROFILE5), GetString(&li, MSG_OK), GetString(&li, MSG_UPDATEPROFILE6), NULL);
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

                // MUI Settings
                case MEN_MUIPREFS:
                     DoMethod(app, MUIM_Application_OpenConfigWindow, 0);
                     break;

                // FAQs
                case MEN_FAQ:
                     help();
                     break;

                // Donate
                case MEN_DONATE:
                     set(win_donate, MUIA_Window_Open, TRUE);
                     break;

                // About AmiTwitter
                case MEN_ABOUT:                   
                     about();
                     break;

                // About MUI
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
        if (running && sigs) 
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

  if (dobj) FreeDiskObject(dobj);

  return (0);
}

///

