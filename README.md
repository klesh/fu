# File to URL

![demo](https://github.com/klesh/fu/raw/master/example.gif)
# Introduction

I once needed a tool to convert images/files in Clipboard to URLs, so I looked for such a tool, I found iPic which quite awesome. But unfortunately, it supports image only and wouldn't let me upload to my own server. It would be greate to have a another option, wasn't it?   

What can fu capable of?
  * Upload file to a 3rd-party server and put a URL/Markdown Snippet into your clipboard
  * Supports both Image and File
  * Supports both BitmapData (like hitting PrintScreen) and FileData (like Ctrl-C on a file)
  * Supports uploading to you own server by FTP/SFTP/SSH
  * CrossPlatform (Windows/MacOS/Linux)

# Installation

[Go to releases page](https://github.com/klesh/fu/releases) download archive according to your OS.

## Windows

  download the `.7z` file from releases page. extract to whereever you want.

  You may need to install vc++ runtime library, it can be downloaded from here: http://www.microsoft.com/en-us/download/details.aspx?id=48145

  If you want it to be launched at login, please make a shortcut in your `Startup` folder.

## MacOS

  Download the `.dmg` file from releases page, double click it and drag the fu icon into `Applications` folder

  If you want it to be launched at login, please go to `System Preferences > Users & Groups > Login Items` and add it to your list.

  If you receive a error says `app is damaged, should be moved to trash`, that is because, OSX allows only signed app to be launch **by default**, and it's not free even for a open source free app. However, you can change this behavior at your own risk:

  ### macOS Sierra
  run following command in your Terminal
  ```
  sudo spctl --master-disable
  ```

  ### Other OSX
  go to [System Preferences] -> [Secruity & Privacy] -> [General] -> [Allow apps downloaded from] -> [Anywhere]

## Linux

  download the `.tar.gz` file, extract, `make` and `make install`.
  should be able to work on most distros.

### Ubuntu
```bash
sudo apt install libwxgtk3.0-dev libcurl4-openssl-dev libssh2-1-dev libboost1.61-dev libgtk2.0-dev zlibc zlib1g zlib1g-dev libappindicator3-dev
tar zxvf fu-vxxx.tar.gz
./configure
make
sudo make install
```

# Build Dependencies

  * wxWidgets 3.0.2
  * libcurl
  * libssh2
  * boost

# Development Dependencies

  * automake
  * autoconf

# Compile

```
$ aclocal
$ autoconf
$ automake --add-missing
```

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
