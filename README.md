# QEdit

## Requirements
QEdit only works on windows!

In order to build and use this application ffmpeg has to be copied into the /QEdit/lib/ffmpeg/ directory.
Download binaries from https://github.com/BtbN/FFmpeg-Builds/releases (win64-lgpl-shared) or build ffmpeg yourself. Then copy the contents of the bin/ folder into /QEdit/lib/ffmpeg/.

## Build
The repository contains a visual studio 2019 solution file which can be opened in visual studio and compiled.

## TODO
- [ ] Dont use the console window to display ffmpeg (std::system). Instead use CreateProcess to run ffmpeg in a separate process and do error/progress reporting in the console
- [ ] Some sort of UI (either GUI or console menus) to enable and set different options
- [ ] (If GUI) implement a preview for the video and handles to choose start/end points