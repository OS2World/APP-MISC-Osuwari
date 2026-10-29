
        Sitting (Osuwari) Multi for OS/2 PM, version 1.07
	
                            2001/04/01 by akira@sra.co.jp

Introduction

    Cute 'Multi' sit on active window.

    This program is based on 
    
    'Osuwari Multi' (for Win32) by Mr.Yoshy and  Mr.Katokimuti,
    
        <http://www.vc-net.ne.jp/~yoshy/to_heart/index.html#osmulti>

    and its X/UNIX port by Mr.Nakanaka
    
        <http://www01.u-page.so-net.ne.jp/ka2/nakanita/multi/xosmulti.html>
    
Copyrights

    Several rights of character 'Multi' belongs to 'Leaf'.
    
        <http://leaf.aquaplus.co.jp/>

    Bitmaps used in this program is written by Mr.Yoshy & Mr.Katokimuti,
    in their MS-Windows version,

        <http://www.vc-net.ne.jp/~yoshy/to_heart/index.html#osmulti>

    according to Leaf's permission for secondary works,
    
        <http://leaf.aquaplus.co.jp/copyrgt.html>
    
    And got Mr.Yoshy & Mr.Katokimuti's permission to use with PM version.

    Copyright of program code itself belongs to my employer

        Software Research Associates, Inc.

    and released under GNU Public Lincense.

    And, many part of this program is based on Mr.Nakanaka's X/UNIX
    version (he released it as PDS).

        <http://www01.u-page.so-net.ne.jp/ka2/nakanita/multi/xosmulti.html>

License

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Install

    Archive contains full source codes (including shape window library)
    and program binary.  If you use binary only, copy

        osmulti2.exe
        lang\*.txt   (the language files, in a "lang" folder next to
                      the .exe)

    to a directory and start osmulti2.exe, or create a program object
    for it.  The language is chosen from the "Language" submenu of the
    popup menu (right mouse button), or with a language code on the
    command line:

        osmulti2 ja         Japanese
        osmulti2 es         Spanish    (en, es, nl, de, fr, it, ja, ko,
                                        ru, el, zh_CN, zh_TW)

    Without a command line code the language saved in osmulti2.ini is
    used, else one that fits the system code page (English if none).

    The OS2World build (2026) is built with Open Watcom 2.0 and needs
    no EMX runtime.  See README.md for the build instructions.

How to use

    'Multi' will sit on active window.  But active window was maximized,
    or there is not enought space above window, 'Multi' will disppear.

    When moving mouse over 'Multi', there are two places where mouse
    pointer changes.  You can 'nade nade' (pat?) or 'funi funi' (touch?)
    on there.

    'nade nade'     hold down left button and move mouse around.    
    'funi funi'     click left button.  But too many click makes
                    her to overheat (Oh, she is really a Maid Robot).

    Click right button enables popup menu. From there, you can
    
        Option      You can change her position or message on/off.
        About       Informations about this program.
        Exit        Exit program

Updates History

    1.07 (OS2World build, 2026)    Open Watcom port, multilingual

        - Builds with Open Watcom 2.0 (makefile.wat / compile-wat.cmd);
          the EMX/GCC Makefile was removed.
        - Menus, dialogs, About text and the messages of 'Multi' come
          from UTF-8 language files (lang\*.txt) instead of resources:
          12 languages (English, Spanish, Dutch, German, French,
          Italian, Japanese, Korean, Russian, Greek, Simplified and
          Traditional Chinese).  Text is drawn through GPI code page
          1208 (UTF-8); anything the system code page cannot show is
          drawn by the program (owner drawn menu items, own controls).
          More languages can be added by dropping a new file in lang\.
        - New "Language" submenu in the popup menu; the choice is saved
          in osmulti2.ini.  A language code on the command line
          (ja, en, es, ...) overrides it.
        - Face fix: the whole picture is now redrawn for each face
          change (the old 'full' behaviour is the default, because the
          partial redraw shows a piece of the body instead of the face
          with Open Watcom builds).  The old behaviour is available as
          the 'part' option.
        - Fixed swapped coordinates in the repaint rectangle of the
          partial redraw.

    1.06        More fix for hang after screen lock
    
        Fix on 1.05 is not perfect, still (sometimes) hangs on screen 
        lock. After serval trials, it seems WS_TOPMOST window style
        causes this problem.  Hidden windows having WS_TOPMOST style 
	will lockup system(PM) when lock the screen.  It happens not
        only this Osuwari Multi, but other program using WS_TOPMOST
        having same problem.
	
	So, as version 1.06, I deleted WS_TOPMOST style and built-in
	Z-order control code internally.  This version works well with
	screen lock.
	
    1.05        Fix for hang after screen lock
    
        On Wrap 4.51, returning after screen lock, 'osmulti2' causes
	lot of load on system, nearly hanging.  This was cuased by
	redundant redrawing code.  Deleting those code seems solved 
	this problem.

    1.04        Fix for troubles on some display drivers
    
        On some display drivers, animation does not work correctly (
        Multi's face was corrupted).  It happend on my new machine.
	So fixed it with drawing entire images for animation. It is only
        a Q.D.Hack, but works well with Wrap4.51.  If you have such
        trouble, try following option.
	
	    osmulti2 full

    1.03        Speedup,'idle'option
        
	Re-compiled to include speedup version of 'shapewin' library.
	
	Appended 'idle' option, run 'osmulti' in idle priority.
            (Suggestion from  Mike.Reichel@E-Technik.TU-Chemnitz.DE)
        Generally, it speed up window operation, but sometimes,
        combination with another process's priority, it hangs PM.
	So, try it on your environment and if no problem happen,
	use it.

    1.02        Appended 'english' mode.

        According to request from forigners, appended 'english' mode.
	Command line option 'en' enables english mode.
	
	Messages are based on Mr.Nakanaka's X/UNIX version.

    1.01        Changes window control and bitmap format.

        In first release, there are several problems in some environments
	(especially Warp4).

            a. nothing appears
	    b. 'Multi' appears only few
	    c. Optsion setting was lost

	Problem (a) seems result of differences between Warp3 and Warp4.
	This was fixed by use of window style of WS_TOPMOST.  It is
        undocumented in Warp3, but worked if fixepack was applied.
        
	Problem (b) seems a problem of GpiLoadBitmap with compressed
	(RLE32) bitmap.  It semms some referenes to un-set local
        varibales under GpiLoadBitmap (it depend of graphic driver).
	So I replaced to un-compressed bitmaps.

        But if still have problems, try to drop optiomization for
        compiling 'bitmap.c' module.

        Problem (c) was my mistake. Sorry.

    1.00        First Release (in Japan).

Known Problems

    It may not work on some environments.  Refer to update history 1.01.

    'Object Desktop' causes problems, a part of bitmap is not redrawn.
    
    If face of the Multi was broken, use the 'full' option (default in
    the OS2World build; 'part' selects the old partial redraw).

Acknowledges

    Mr.Yoshy & Mr.Katokimuti        E-mail: yoshy@vc-net.ne.jp

	They are developper of original WIndows Version.  Thanks for
	permission to use great bitmaps.        

    Mr.Nakana                       E-mail: nakanita@ka2.so-net.ne.jp

        I found his X/UNIX version, and it encouraged me to port to PM.

    And original game developper 'leaf'.

        They made 'Multi'.  As side notes, PS version and ANIME will
        comming soon.

    And for all who send report on Osuwari Multi

        If this program works other than Warp3 (jp), then it is result
        of peoples who send me reports. Thanks for contributions.

Contact

    Send bug report, questions, requests for OS/2-PM version to
    (let me know version number on 'about' dialog).

        akira@sra.co.jp
    
    Newset version will placed on
    
        http://www.sra.co.jp/people/akira/os2/osmulti/index-e.html
