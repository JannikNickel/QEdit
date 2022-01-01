# QEdit
QEdit is a simple video transcoding tool which works in the Windows terminal.

![img](https://i.imgur.com/v7nIVj7.gif)

## Dependencies
- FFmpeg (https://github.com/FFmpeg/FFmpeg)
- FTXUI (https://github.com/ArthurSonzogni/FTXUI)

## Build
QEdit only works on windows!
In order to build and use this application, ffmpeg has to be copied into the /QEdit/lib/ffmpeg/ directory.
Binaries can be downloaded from https://github.com/BtbN/FFmpeg-Builds/releases (ffmpeg-n4.4-latest-win64-lgpl-shared-4.4.zip) or you can build ffmpeg yourself. Then copy the contents of the ffmpeg bin/ folder (ffmpeg.exe and other libraries) into /QEdit/lib/ffmpeg. The application was developed with FFmpeg 4.4 but should work with newer versions too.

The repository contains visual studio 2022 solution and project files which can be opened in visual studio and compiled into a 64bit windows executable which can be found in the /Build/ directory.

## Possible improvements
- Reading of input file video meta data and validating settings
    - There is currently no validation if the settings make sense for the current input file (e.g. start time or duration within are within the input file, or bitrate is <= than the input file's bitrate)
- Using input video file settings for unset values
    - FFmpeg will choose a bitrate by itself if, for instance, only the resolution is changed. It would be better to use the source file's bitrate in that case to prevent bad video quality
    - If no video changes are selected, the copy flag will be set and the video quality wont be altered!
- Support more codecs and other options
    - FFmpeg supports a variety of codecs and other options. This tool currently only implements the most relevant ones to quickly cut and compress clips to upload them to various online platforms.