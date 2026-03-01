# RiffExt

This is a program to extract RIFF data from arbitrary files such as video game archives or sound banks. 

Although originally made to rip audio from Baldur's Gate 3, this will work for any game data file that doesn't compress or encrypt its contents (most of them don't).

The program identifies RIFF files by the strings `RIFF`, `WAVE` and `fmt` in the header.

## Usage

```
RiffExt [options] <input file>
```

### Options
- `-h` Show help and exit.
- `-v, --verbose` Verbose output.
- `-d, --dump` Extract RIFF data to a file.
- `-s, --scan` Scan the input file and show RIFF data.
- `-w, --wav` Ignore RIFF files that don't contain WAV data.

### Extracting Video Game Sound Files

The [vgmstream](https://github.com/vgmstream/vgmstream) CLI tool is very helpful in converting extracted `.wem` files to playable `.wav` ones.

If your `.wem` files have format `0xFFFF`, you can also use [ww2ogg](https://github.com/hcs64/ww2ogg) (optionally with the ReVorb tool) to convert them into `.ogg` format.

These tools will work for a lot of games. Some games I've tried are:
- Cyberpunk 2077
- Assassin's Creed Valhalla
- Tom Clancy's The Division 2
- Divinity: Original Sin 2
- Baldur's Gate 3
- The Elder Scrolls Online
