# RiffExt

This is a program to extract RIFF data from files. 

Although originally made to rip audio from Baldur's Gate 3, this should work for any file that doesn't do anything too weird to the RIFF headers.

Output files have the `.wem` extension. 

The program identifies RIFF files by the strings `RIFF`, `WAVE` and `fmt` in the header.

## RiffScan

This utility scans files and displays info on any RIFF files inside. It's useful to scope out a data file before you start extracting anything.
RiffScan outputs in CSV text (separated by commas).

## Usage
**Windows**  

You can compile the source yourself, or download executables for RiffExt and RiffScan from the [Releases](https://github.com/PKBeam/RiffExt/releases/) section.

In a Command Prompt, type:

`riffext.exe <filename>`  
`riffscan.exe <filename>`  

**macOS/Linux**  

Compile the source code using your favourite C compiler. 

Then type in bash:

`$ ./riffext <filename>`  
`$ ./riffscan <filename>`  

----

### RiffExt args

You can use `-m` to set a minimum filesize (in bytes) for extraction, **or** `-i` to extract a file at a certain index (starting from 1).

```
riffext.exe soundbank.dat -m 500000
riffext.exe soundbank.dat -i 900
```
### RiffScan args

RiffScan normally does a couple more checks that RiffExt doesn't, so if something's not working try the argument `-s`.

```
riffscan.exe soundbank.dat > out.csv
riffscan.exe other_soundbank.dat -s > out.csv
```

### Extracting Video Game Sound Files

#### Baldur's Gate 3

For Baldur's Gate 3, the sound files are stored in the `/Data` folder, in `SharedSounds.pak` and `SharedSoundBanks.pak` (as of Patch 3, the music seems to be in files 13870 to 14055 in the latter).
  Use [ww2ogg](https://github.com/hcs64/ww2ogg) with the alternate packed codebooks (or [Divinity Converter](https://steamcommunity.com/sharedfiles/filedetails/?id=297292305)) to turn the `.wem` files into a playable `.ogg` format.

#### Assassin's Creed Valhalla

The files are in `sounddata/PC/`.
The output `.wem` files will have a mix of OGG Vorbis and Opus codecs. You can use [vgmstream](https://github.com/vgmstream/vgmstream)'s CLI tool to decode the Opus files.

----

These tools will work for a lot of other games too, for example, Divinity: Original Sin 2 and The Elder Scrolls Online.
