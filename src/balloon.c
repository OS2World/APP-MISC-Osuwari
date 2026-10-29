/*
 * balloon.c - Message Balloon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INCL_PM
#include <os2.h>

#include "shapewin.h"
#include "uni.h"

#include "osmulti2.h"
#include "osmulres.h"

/*
 * control message outputs
 */

BOOL    useMsg = TRUE  ;
BOOL    useSnd = FALSE ;

/*
 * Dimensions for Message Balloon
 */

#define TALK_CX         160     /* width            */
#define TALK_CY         104     /* height           */
#define TALK_OFF        16      /* Y above Bitmap   */

#define TALK_RND        15      /* Rounding Value   */
#define TALK_RIM        3       /* Width of Rim     */

#define TALK_XADJUST    6
#define TALK_YADJUST    10

#define TALK_CLR_FIL    CLR_BLUE        /* Filler       */
#define TALK_CLR_RIM    CLR_BLACK       /* RIM          */
#define TALK_CLR_BAK    CLR_WHITE       /* Message Back */
#define TALK_CLR_CHR    CLR_BLACK       /* Message Char */

static  SIZEL       talkSiz ;           /* Size of Message Area     */
static  RECTL       talkRct ;           /* Rect of Message Area     */
static  SIZEL       talkBox ;           /* Size of Char Box         */
static  POINTL      talkPos ;           /* Message Drawing Position */

/*
 * Memory DC/PS & Bitmap for Balloon Message
 */

HDC     hdcBalloon = NULLHANDLE ;
HPS     hpsBalloon = NULLHANDLE ;
HBITMAP hbmBalloon = NULLHANDLE ;

/*
 * Messages come from the language file at the time they are drawn
 * (ostext.c), so a change of language takes effect at once.
 */

#define TALK_PELS       13      /* character size   */

/*
 * balloonDispose - dispose resources for Balloon Message
 */

void    balloonDispose(void)
{
    if (hbmBalloon != NULLHANDLE) {
        GpiDeleteBitmap(hbmBalloon) ;
	hbmBalloon = NULLHANDLE ;
    }
    if (hpsBalloon != NULLHANDLE) {
        GpiSetBitmap(hpsBalloon, NULLHANDLE) ;
        GpiDestroyPS(hpsBalloon) ;
	hpsBalloon == NULLHANDLE ;
    }
    if (hdcBalloon != NULLHANDLE) {
        DevCloseDC(hdcBalloon) ;
	hdcBalloon = NULLHANDLE ;
    }
}

/*
 * balloonCreate - create Memory DC/PS & Bitmap for Balloon Message
 */

BOOL    balloonCreate(HAB hab)
{
    SIZEL   siz ;
    POINTL  pt1, pt2 ;
    BITMAPINFOHEADER2   bmi ;
    POINTL  apt[TXTBOX_COUNT] ;

    /*
     * Create Memory DC/PS for Message Ballooon
     */

    hdcBalloon =DevOpenDC(hab, OD_MEMORY, "*", 0, NULL, NULLHANDLE) ;
    siz.cx = siz.cy = 0 ;
    hpsBalloon = GpiCreatePS(hab, hdcBalloon, &siz,
            PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC) ;
    if (hdcBalloon == NULLHANDLE || hpsBalloon == NULLHANDLE) {
        return FALSE ;
    }
    
    /*
     * create and associate bitmap for Message Balloon
     */

    memset(&bmi, 0, sizeof(bmi)) ;
    bmi.cbFix = sizeof(bmi) ;
    bmi.cx = TALK_CX ;
    bmi.cy = TALK_CY ;
    bmi.cPlanes       = 1  ;
    bmi.cBitCount     = 24 ;
    bmi.ulCompression = 0  ;
    bmi.cclrUsed      = 0  ;
    bmi.cclrImportant = 0  ;

    hbmBalloon = GpiCreateBitmap(hpsBalloon, &bmi, 0, NULL, NULL) ;
    
    if (hbmBalloon == NULLHANDLE) {
        return FALSE ;
    }
    GpiSetBitmap(hpsBalloon, hbmBalloon) ;
    
    /*
     * draw message balloon
     */

    pt1.x = 0 ;
    pt1.y = 0 ;
    pt2.x = TALK_CX ;
    pt2.y = TALK_CY ;
    
    GpiMove(hpsBalloon, &pt1) ;
    GpiSetColor(hpsBalloon, TALK_CLR_FIL) ;
    GpiBox(hpsBalloon, DRO_FILL, &pt2, 0, 0) ;

    GpiMove(hpsBalloon, &pt1) ;
    GpiSetColor(hpsBalloon, TALK_CLR_RIM) ;
    GpiBox(hpsBalloon, DRO_FILL, &pt2, TALK_RND, TALK_RND) ;

    pt1.x += TALK_RIM ;
    pt1.y += TALK_RIM ;
    pt2.x -= TALK_RIM ;
    pt2.y -= TALK_RIM ;
    GpiMove(hpsBalloon, &pt1) ;
    GpiSetColor(hpsBalloon, TALK_CLR_BAK) ;
    GpiBox(hpsBalloon, DRO_FILL, &pt2, TALK_RND, TALK_RND) ;

    /*
     * Calc. Message Positions
     */

    talkRct.xLeft   = TALK_XADJUST ;
    talkRct.yBottom = TALK_YADJUST ;
    talkRct.xRight  = TALK_CX - TALK_XADJUST ;
    talkRct.yTop    = TALK_CY - TALK_YADJUST ;

    TRACE("Message Area %d, %d - %d, %d\n",
            talkRct.xLeft, talkRct.yBottom, talkRct.xRight, talkRct.yTop) ;

    talkSiz.cx = talkRct.xRight - talkRct.xLeft ;
    talkSiz.cy = talkRct.yTop - talkRct.yBottom ;

    /*
     * Text size (the line height is measured again for the language font)
     */

    talkBox.cx = TALK_PELS ;
    talkBox.cy = TALK_PELS + 3 ;

    talkPos.x = talkRct.xLeft ;
    talkPos.y = talkRct.yTop  ;
    
    return TRUE ;
}

/*
 * balloonShow/Hide - show / hide Balloon Message
 */

static  BOOL    visible = FALSE ;

void    balloonShow(void)
{
    SWP     scr ;
    SWP     swp ;
    
    TRACE("balloonShow\n") ;
    
    if (useMsg == FALSE) {
        WinSetWindowPos(hwndTalk, NULLHANDLE, 0, 0, 0, 0, SWP_HIDE) ;
        visible = FALSE ;
        return ;
    }
    
    WinQueryWindowPos(HWND_DESKTOP, &scr) ;
    WinQueryWindowPos(hwndShape, &swp)    ;
    
    if (swp.x < (scr.cx / 2)) {
        swp.x = swp.x + swp.cx ;        /* balloon at left  */
    } else {
        swp.x = swp.x - TALK_CX ;       /* balloon at right */
    }
    swp.y = swp.y + TALK_OFF ;

    WinSetWindowPos(hwndTalk, HWND_TOP, 
            swp.x, swp.y, 0, 0, (SWP_MOVE | SWP_ZORDER | SWP_SHOW)) ;
    visible = TRUE ;
}

void    balloonHide(void)
{
    TRACE("balloonHide\n") ;

    WinSetWindowPos(hwndTalk, NULLHANDLE, 0, 0, 0, 0, SWP_HIDE) ;
    visible = FALSE ;
}

/*
 * balloonMove - Move Balloon with Bitmap
 */

void    balloonMove(void)
{
    TRACE("balloonMove\n") ;

    if (visible) {
        balloonShow() ;
    } else {
        balloonHide() ;
    }
}

/*
 * message drawing functions
 */

static  void    cleanup(void)
{
    POINTL  pt1, pt2 ;
    
    /*
     * clear message area
     */
     
    pt1.x = talkRct.xLeft   ;
    pt1.y = talkRct.yBottom ;
    pt2.x = talkRct.xRight  ;
    pt2.y = talkRct.yTop    ;
    GpiMove(hpsBalloon, &pt1) ;
    GpiSetColor(hpsBalloon, TALK_CLR_BAK) ;
    GpiBox(hpsBalloon, DRO_FILL, &pt2, 0, 0) ;

    /*
     * reset drawing position
     */
     
    talkPos.x = talkRct.xLeft ;
    talkPos.y = talkRct.yTop  ;
}

static  void    scrollup(void)
{
    POINTL  apt[3] ;
    POINTL  pt1, pt2 ;
    
    /*
     * scrollup messages
     */
     
    apt[0].x = talkRct.xLeft          ;
    apt[0].y = talkRct.yBottom + talkBox.cy - 2 ;
    apt[1].x = talkRct.xRight         ;
    apt[1].y = talkRct.yTop           ;
    apt[2].x = talkRct.xLeft          ;
    apt[2].y = talkRct.yBottom - 2    ;
    
    GpiBitBlt(hpsBalloon, hpsBalloon, 3, apt, ROP_SRCCOPY, 0) ;

    /*
     * clear bottom area
     */

    pt1.x = talkRct.xLeft              ;
    pt1.y = talkRct.yBottom - 2        ;
    pt2.x = talkRct.xRight             ;
    pt2.y = talkPos.y + talkBox.cy - 2 ;
    GpiMove(hpsBalloon, &pt1) ;
    GpiSetColor(hpsBalloon, TALK_CLR_BAK) ;
    GpiBox(hpsBalloon, DRO_FILL, &pt2, 0, 0) ;
}

static  BOOL    uniOn = FALSE ;     /* the language font is selected    */

static  int     textWidth(const char *str, int len)
{
    POINTL  apt[TXTBOX_COUNT] ;

    if (uniOn) {
        return uni_width(hpsBalloon, str, len) ;
    }
    GpiQueryTextBox(hpsBalloon, len, (PCH) str, TXTBOX_COUNT, apt) ;
    return (int) (apt[TXTBOX_TOPRIGHT].x - apt[TXTBOX_TOPLEFT].x) ;
}

static  void    drawLine(const char *str, int len)
{
    talkPos.x = talkRct.xLeft ;
    if ((talkPos.y - talkBox.cy) >= talkRct.yBottom) {
        talkPos.y -= talkBox.cy ;
    } else {
        scrollup() ;
    }
    GpiSetColor(hpsBalloon, TALK_CLR_CHR) ;
    if (uniOn) {
        uni_draw(hpsBalloon, talkPos.x, talkPos.y, str, len) ;
    } else {
        GpiCharStringAt(hpsBalloon, &talkPos, len, (PCH) str) ;
    }
}

/*
 * drawWrap - draw a text on as many lines as needed; breaks at the last
 * space when there is one, else between two characters (UTF-8 aware)
 */

static  void    drawWrap(const char *str, int len)
{
    int     start = 0, n, lastSp, used ;

    while (start < len) {
        n = start ;
        lastSp = -1 ;
        while (n < len) {
            used = uniOn ? utf8_seqlen((unsigned char) str[n]) : 1 ;
            if (n + used > len) {
                used = len - n ;
            }
            if (n > start && textWidth(str + start, n + used - start) > talkSiz.cx) {
                break ;
            }
            if (str[n] == ' ') {
                lastSp = n ;
            }
            n += used ;
        }
        if (n >= len) {
            drawLine(str + start, len - start) ;
            break ;
        }
        if (lastSp > start) {
            drawLine(str + start, lastSp - start) ;
            start = lastSp + 1 ;
        } else {
            drawLine(str + start, n - start) ;
            start = n ;
        }
    }
}

static  void    drawMessage(int strId)
{
    ULONG       ocp ;
    int         len ;
    const char  *str ;

    uniOn = uni_begin(hpsBalloon, osFace(), TALK_PELS, &ocp) ? TRUE : FALSE ;
    if (uniOn) {
        str = osTrw(strId, &len) ;
        talkBox.cy = uni_height(hpsBalloon) ;
    } else {
        str = osTr(strId) ;             /* no font: code page text  */
        len = strlen(str) ;
        talkBox.cy = TALK_PELS + 3 ;
    }
    drawWrap(str, len) ;
    uni_end(hpsBalloon, ocp) ;
    uniOn = FALSE ;

    WinSendMsg(hwndTalk, SHAPEWIN_MSG_UPDATE, MPFROMP(&talkRct), NULL) ;
}

/*
 * put Messages on Balloon
 */

static  int     countNade = 0 ;
static  int     countFuni = 0 ;
static  BOOL    isMail = FALSE ;

void    balloonNade(void)
{
    drawMessage(STR_B_NADE) ;
    
    countNade += 1 ;
    
    if (countNade == 4) {
        drawMessage(STR_B_NADE1) ;
    } else if (countNade == 8) {
        drawMessage(STR_B_NADE2) ;
    } else if (countNade == 12) {
        drawMessage(STR_B_NADE3) ;
    } else if ((countNade % 4) == 0) {
        drawMessage(STR_B_NADE4) ;
    }

    if (! visible) {
        balloonShow() ;
    }
}

void    balloonFuni(void)
{
    drawMessage(STR_B_FUNI) ;
    
    countFuni += 1 ;
    
    if (countFuni == 4) {
        drawMessage(STR_B_FUNI1) ;
    } else if (countFuni == 8) {
        drawMessage(STR_B_FUNI2) ;
    } else if (countFuni == 12) {
        drawMessage(STR_B_FUNI3) ;
    } else if (countFuni == 16) {   /* 20 > causes 'psyu'   */
        drawMessage(STR_B_FUNI4) ;
    }

    if (! visible) {
        balloonShow() ;
    }
}

void    balloonPsyu(void)
{
    drawMessage(STR_B_PSYU) ;

    if (! visible) {
        balloonShow() ;
    }
    countNade = 0 ;
    countFuni = 0 ;
}

void    balloonClear(void)
{
    TRACE("balloonClear\n") ;

    if (visible) {
        balloonHide() ;
    }

    countNade = 0  ;
    countFuni = 0  ;

    cleanup() ;
}
