# RiffExt

This is a program to extract RIFF data from files. 

Although originally made to rip music from Baldur's Gate 3 game archives (`LSPK`), this should work for any file that doesn't compress or encrypt the RIFF data.

Output files have the `.wem` extension. 

## Compiling
The C code doesn't appear to work properly on Windows, but does work on macOS (and probably other UNIX systems too).

## Usage
`$ ./riffext <filename>`

For Baldur's Gate 3, the music files are stored in the `/Data` folder, in `SharedSounds.pak` and `SharedSoundBanks.pak`.
  Use [ww2ogg](https://github.com/hcs64/ww2ogg) with the alternate packed codebooks (or [Divinity Converter](https://steamcommunity.com/sharedfiles/filedetails/?id=297292305)) to turn the `.wem` files into a playable `.ogg` format.
