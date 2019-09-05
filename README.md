# File to URL

![demo](https://github.com/klesh/fu/raw/v2.0.0/fu2.gif)

# Introduction

`fu` stands for `File Uploader`, a utility design to help you upload images/files and produce Markdown/HTML snippets with couple of clicks.

# Features

  * Upload `Raw Image Data` or `Files` from Clipboard to server
  * Multiple files to multiple servers with one click
  * Upload to your own FTP/SFTP, or your NAS through LocalStorage protocol
  * Upload to public image hosting service like `imgur.com` and `qiniu.com`
  * Compress image before upload
  * Adding wartermark for image
  * Customizable output format, HTML/Markdown/Raw or whatever plain-text format you like
  * Tagging support
  * Reuse previous uploaded clips
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