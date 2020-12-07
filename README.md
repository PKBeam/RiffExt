# RiffExt

This is a program to extract RIFF data from files. 

Although originally made to rip audio from Baldur's Gate 3 game archives (`.pak` with header `LSPK`), this should work for any file that doesn't compress or encrypt the RIFF headers.

Output files have the `.wem` extension. 

The program identifies RIFF files by the strings `RIFF`, `WAVE` and `fmt` in the header.

## RiffScan

This utility scans RIFF files to show some information before you extract anything.

## Building
Compile `riffext.c` and `riffscan.c`. 

## Usage
**Windows**  

You can download the `.exe` from the [Releases](https://github.com/PKBeam/RiffExt/releases/) section.

In a Command Prompt, type:

`riffext.exe <filename>`  
`riffscan.exe <filename>`  

**macOS/Linux**  

`$ ./riffext <filename>`  
`$ ./riffscan <filename>`  

### Extracting Video Game Sound Files

#### Baldur's Gate 3

For Baldur's Gate 3, the sound files are stored in the `/Data` folder, in `SharedSounds.pak` and `SharedSoundBanks.pak` (as of Patch 3, the music seems to be in files 13870 to 14055 in the latter).
  Use [ww2ogg](https://github.com/hcs64/ww2ogg) with the alternate packed codebooks (or [Divinity Converter](https://steamcommunity.com/sharedfiles/filedetails/?id=297292305)) to turn the `.wem` files into a playable `.ogg` format.

#### Assassin's Creed Valhalla

The files are in `sounddata/PC/`.
The output `.wem` files will have a mix of OGG Vorbis and Opus codecs. You can use [vgmstream](https://github.com/vgmstream/vgmstream)'s CLI tool to decode the Opus files.
