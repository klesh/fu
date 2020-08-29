# File to URL

![demo](https://github.com/klesh/fu/raw/v2.0.0/fu2.gif)

# Introduction

`fu` stands for `File to URL`, a utility design to help you upload images/files and produce Markdown/HTML snippets with couple of clicks.

# Features

  * Upload `Raw Image Data` or `Files` from Clipboard to server
  * Multiple files to multiple servers with one click
  * Upload to your own FTP/SFTP, or your NAS through LocalStorage protocol
  * Upload to public image hosting service like `imgur.com` and `qiniu.com`
  * Compress image before upload
  * Adding wartermark for image
  * Customizable output format, HTML/Markdown/Raw or whatever plain-text format you like
  * Copy output from History records
  * Manage history by tagging
  * Search history clips by similarity (base on `pHash`)
  * Easy to backup/restore your data
  * i18n support


# Installation

[Go to releases page](https://github.com/klesh/fu/releases) download archive according to your OS.

## Windows

  Download the `.7z` file from releases page. extract to whereever you want.

  If you want it to be launched at login, please make a shortcut in your `Startup` folder.

## MacOS

  currently unavailable 

## Linux

  currently unavailable 


# Build Dependencies

  * Qt5
  * libqcurl
  * qt-phash


# Build on Windows (Qt 5.13.2)

Tested on Windows 10 with Qt 5.13.2, higher version may not work.

  1. Create working directory `mkdir fu2`
  2. Create dependencies directory `mkdir fu2\deps`
  3. Download libqcurl binaries from https://github.com/klesh/libqcurl/releases/tag/v1.0.0 and extract them to `fu2\deps\i386\release` and `fu2\deps\x86_64\release` accordingly
  4. Clone repository: `git clone https://github.com/klesh/fu.git fu2\fu`
  5. Launch `Qt Creator` and open `fu2\fu\fu.pro`, select both `Desktop Qt 5.13.2 MinGW 32-bit` and `Desktop Qt 5.13.2 MinGW 64-bit` kits
  6. For each `release` configuration of each kits, add a building step `Make` with `install` as _Make arugments_
  7. Select target Kit/Build (computer-like icon on the bottom left corner), then press `Ctrl-B` to build.
  8. Open `Qt 5.13.2 (MinGW xxx 32-bit)`(or `64-bit`) command prompt accordingly and go to `fu2\builds\fu-gcc-i386-release-vx.x.x` (or `x86_64`) and run `windeployqt .`

# FAQ

   Q: How to add imgur.com?

   A: go to http://api.imgur.com/ register an application, you will receive a Client-ID, then you can create a imgur.com entry in `Preferences`. You may have to setup proxy field if you run into a "blocked by imgur.com " error.

# Like it ?

  * Star this respository right away!
  * Recommend to your friends.
  * Twitter/Facebook/Reddit it already.
  * Pull Requests are all welcome.

# License

LGPL
