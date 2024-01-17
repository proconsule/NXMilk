NXMilk
======
![Banner](docs/nxmilk_banner.jpg)

NXMilk is a trivial audio player for Nintendo Switch.

It uses FFMpeg for audio decoding and ProjectM for the vis.
The GUI is made with ImGui.

This project started as a test suite for new audio player for NXMP

NXMilk is coded by proconsule



Features
-----
- MP3 support (with ID3 tag support)
- OGG support (with metadata support if present)
- FLAC support (with metadata support if present)
- Vis using projectM library
- Can read from SD card and from USB storage

Button Mapping
-----
- ZL/ZR Prev/Next Preset
- L/R Seek -/+ 5 seconds
- B Stop
- Y Disable projectM Vis
- Limited Touch Support

USB
-----
Press Y to toogle between SD Card and USB


Installation 
----
- Copy NXMilk.nro in a subfolder eg. /switch/NXMilk/
- (OPTIONAL) Copy milk files in /switch/NXMilk/presets/milk/
- (OPTIONAL) Copy texture files in /switch/NXMilk/presets/Textures/


Edit Config file NXMilk.ini

```
[AudioPlayer]
startpath = /switch/NXMilk
milkpresetspath = /switch/NXMilk/presets/milk/
milktexturespath = /switch/NXMilk/presets/Textures/
interfacehidesec = 10
usebuiltinpreset = true
```

NXMilk has some builtin presets. Can be disabled setting to false the usebuiltinpreset option



Presets
----
- Many milkdrop presets to use https://github.com/projectM-visualizer/presets-cream-of-the-crop

Libraries used in this project
-----
- FFMpeg https://ffmpeg.org/
- ImGui https://github.com/ocornut/imgui
- ProjectM https://github.com/projectM-visualizer/projectm
- Many code parts are taken form NXMP https://github.com/proconsule/nxmp




