# RiffExt

This is a program to extract RIFF data from files. 

Although originally made to rip audio from Baldur's Gate 3, this should work for any file that doesn't do anything too weird to the RIFF headers.

Output files have the `.wem` extension. 

The program identifies RIFF files by the strings `RIFF`, `WAVE` and `fmt` in the header.

## RiffScan

This utility scans files and displays info on any RIFF files inside. It's useful to scope out a data file before you start extracting anything.
RiffScan outputs in CSV text (separated by commas). You can pipe the output directly to a file.

```
riffscan.exe soundbank.dat > out.csv
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

`./riffext <filename>`  
`./riffscan <filename>`  

----

### RiffExt args

You can use `-m` to set a minimum filesize (in bytes) for extraction, **or** `-i` to extract a file at a certain index (starting from 1).

```
riffext.exe soundbank.dat -m 500000
riffext.exe soundbank.dat -i 900
```
### RiffScan args

RiffScan normally does a few more checks that RiffExt doesn't, in order to get the information needed to calculate things like file size and play length. This may result in some files not scanning or scanning incorrectly, if they don't conform to the RIFF standard properly.

When this is the case, you can use the arguments `-s0` or `-s1` to ignore these checks.

The argument `-s0` performs no checks in addition to what RiffExt does, but will not output any information other than filesize.  

The argument `-s1` performs less checks in addition to what RiffExt does, but may output incorrect information on some more complex RIFF files. For this reason, avoid using `-s1` unless the scan fails without it.

```
riffscan.exe soundbank.dat -s0
riffscan.exe soundbank.dat -s1
```

### Extracting Video Game Sound Files

The [vgmstream](https://github.com/vgmstream/vgmstream) CLI tool is very helpful in converting extracted `.wem` files to playable formats.

These tools will work for a lot of games. Some games I've tested are:
- Cyberpunk 2077
- Assassin's Creed Valhalla
- Tom Clancy's The Division 2 (you may need to use `-s1` with RiffScan for some files)
- Divinity: Original Sin 2
- Baldur's Gate 3
- The Elder Scrolls Online (do not use `-s1` with RiffScan)
