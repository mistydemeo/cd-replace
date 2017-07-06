--------------------------------------------------------------------------------
cd-replace                                                Copyright (C) 2012 CUE
--------------------------------------------------------------------------------

CD-REPLACE is a tool to replace data files in a MODE0/MODE1 CD image.

Usage: CD-REPLACE imagename filename newfile

- 'imagename' is the name of the CD image in BIN/ISO/MDF/IMG format
- 'filename' is the file in the CD image with the data to be replaced
- 'newfile' is the file with the new data

* 'imagename' must be a valid MODE0/MODE1 image
* 'filename' can use either the slash or backslash
* 'newfile' can be different size as 'filename'
* the tool update all EDC/ECC changed in MODE1 images
* the tool do not change cuesheet files
* do not use the tool to change digital audio

Examples:
> CD-REPLACE CD1.iso Data\Bin/demo.bin newdemo.bin
> CD-REPLACE game.mdf /MOVIE/END/1.MOV e:\work\subtitled.mov


This package also include the tool "rs-test.c" to test CD-ROM MODE1 or CD-ROM-XA
MODE2 images (used in PSX/PS2).

Usage: RS-TEST imagename

- 'imagename' is the name of the CD image in BIN/ISO/MDF/IMG format


Source code and executable files are included, with GNU General Public License.

To replace files in a CD-ROM-XA image use "PSX-MODE2" (google is your friend).

--------------------------------------------------------------------------------
End of file                                               Copyright (C) 2012 CUE
--------------------------------------------------------------------------------
