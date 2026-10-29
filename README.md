# Osuwari Multi for OS/2 (Sitting Multi)

A cute little "Multi" (the maid robot from Leaf's *To Heart*) sits on the title bar of the
active window. Stroke her ("nade nade") by holding the left mouse button over her head and
moving the mouse, or poke her ("funi funi") by clicking; too much poking makes her overheat.
The right mouse button opens a popup menu: **Options**, **Language**, **About**, **Exit**.

This is version 1.07, based on version 1.06 of the OS/2 Presentation Manager port by akira@sra.co.jp, built with
Open Watcom 2.0 and made multilingual.

## Languages

Menus, dialogs, the About text and the messages Multi says come from UTF-8 language files
in `lang\` (the same technique as MakMan/2): English, Spanish, Dutch, German, French,
Italian, Japanese, Korean, Russian, Greek, Simplified and Traditional Chinese.

- Choose the language in the popup menu under **Language**; it is saved in `osmulti2.ini`.
- A language code on the command line overrides it: `osmulti2 ja`, `osmulti2 es`, ...
- Without either, the language that fits the system code page is used (English otherwise).
- Text is drawn through GPI code page 1208 (UTF-8). Menu items and dialog texts the system
  code page cannot show are drawn by the program itself.
- To add a language, copy `lang\en.txt` to `lang\xx.txt`, translate the values and set
  `LANG_NAME`, `LANG_ENGLISH` and `FONT`. Keys missing in a file fall back to English.

## Command line options

| Option | Effect |
|---|---|
| `en`, `es`, `ja`, ... | start in that language (any file name in `lang\`) |
| `idle` | run at idle priority |
| `full` | redraw the whole picture for each face change (default) |
| `part` | redraw only the face (old behaviour; may show a piece of the body on some systems) |

## Build

Requires Open Watcom 2.0 and the OS/2 Toolkit 4.5 on ArcaOS / OS/2.

```
compile-wat.cmd
```

Output: `bin\osmulti2.exe`, with the language files copied to `bin\lang\`. The script logs to
`compile-wat.log`. Run `osmulti2.exe` from `bin\` (the `lang` folder must stay next to it).

## Layout

```
src/            C sources, resources (osmulres.rc), images
src/ostext.*    language layer: lang files, menus, dialog texts (uses uni.*, ulang.*)
src/uni.*       UTF-8 / GPI code page 1208 text drawing and ULS conversion
src/ulang.*     loader for the lang\*.txt files
lang/           language files (UTF-8)
doc/            original README (English, Japanese) and GNU GPL text
makefile.wat    Open Watcom makefile (used by compile-wat.cmd)
```

## Changes from the original 1.06 (this is 1.07)

- Open Watcom build (`makefile.wat`, `compile-wat.cmd`); the EMX/GCC Makefile was removed.
- 12 languages from `lang\*.txt` instead of the Japanese/English resources; Language submenu.
- Face fix: the whole picture is redrawn for each face change by default, because the partial
  redraw shows a piece of the body instead of the face with this build (the original
  `full` option, now the default). Fixed swapped coordinates in the partial repaint rectangle.

## License and credits

GNU General Public License (see `doc\LICENSE.txt`). Character "Multi" belongs to Leaf.
Original Windows version by Yoshy and Katokimuti, X/UNIX version by Nakanaka, OS/2 PM
port by akira@sra.co.jp (Software Research Associates, Inc.). Open Watcom port and
multilingual support: OS2World, 2026.
