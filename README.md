# RiffExt

This is a program to extract RIFF data from files. 

Although originally made to rip audio from Baldur's Gate 3 game archives (`.pak` with header `LSPK`), this should work for any file that doesn't compress or encrypt the RIFF headers.

Output files have the `.wem` extension. 

Note that the way this program identifies RIFF data is quite crude - it only checks for the raw data `RIFF`, which means that sometimes it will find false positives, i.e. it will output files that are not actually RIFF files.

## Building
On Windows, compile `riffext_win.c`. 

On macOS/Linux, compile `riffext.c`. 

## Usage
**Windows**  

You can download the `.exe` from the [Releases](https://github.com/PKBeam/RiffExt/releases/) section.

In a Command Prompt, type:

`riffext.exe <filename>`  

**macOS/Linux**  

`$ ./riffext <filename>`  

### Extracting Video Game Sound Files

#### Baldur's Gate 3

For Baldur's Gate 3, the sound files are stored in the `/Data` folder, in `SharedSounds.pak` and `SharedSoundBanks.pak` (the music seems to be in files 12839 to 13025 in the latter).
  Use [ww2ogg](https://github.com/hcs64/ww2ogg) with the alternate packed codebooks (or [Divinity Converter](https://steamcommunity.com/sharedfiles/filedetails/?id=297292305)) to turn the `.wem` files into a playable `.ogg` format.

#### Assassin's Creed Valhalla

The files are in `sounddata/PC/`.
The output `.wem` files will have a mix of OGG Vorbis and Opus codecs. You can use [vgmstream](https://github.com/vgmstream/vgmstream)'s CLI tool to decode the Opus files.
