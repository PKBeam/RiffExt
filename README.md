# RiffExt

This is a program to extract RIFF data from files. 

Although originally made to rip audio from Baldur's Gate 3, this should work for any file that doesn't do anything too weird to the RIFF headers.

Output files have the `.wem` extension. 

The program identifies RIFF files by the strings `RIFF`, `WAVE` and `fmt` in the header.

## RiffScan

This utility scans files and displays info on any RIFF files inside. It's useful to scope out a data file before you start extracting anything.

**Example**
```
$ riffscan.exe soundbank.dat
file 1 | format: 0xFFFF, size: 2385.2kB, length: 2:08, bitrate: 148.1kbps, sample rate: 48.0kHz, channels: 2
file 2 | format: 0xFFFE, size: 1013.6kB, length: 0:14, bitrate: 576.0kbps, sample rate: 36.0kHz, channels: 1
file 3 | format: 0xFFFE, size: 163.9kB, length: 0:02, bitrate: 576.0kbps, sample rate: 36.0kHz, channels: 1
```

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

### Extracting Video Game Sound Files

#### Baldur's Gate 3

For Baldur's Gate 3, the sound files are stored in the `/Data` folder, in `SharedSounds.pak` and `SharedSoundBanks.pak` (as of Patch 3, the music seems to be in files 13870 to 14055 in the latter).
  Use [ww2ogg](https://github.com/hcs64/ww2ogg) with the alternate packed codebooks (or [Divinity Converter](https://steamcommunity.com/sharedfiles/filedetails/?id=297292305)) to turn the `.wem` files into a playable `.ogg` format.

#### Assassin's Creed Valhalla

The files are in `sounddata/PC/`.
The output `.wem` files will have a mix of OGG Vorbis and Opus codecs. You can use [vgmstream](https://github.com/vgmstream/vgmstream)'s CLI tool to decode the Opus files.

----

These tools will work for a lot of other games too, for example, Divinity: Original Sin 2 and The Elder Scrolls Online.
