# File to URL

![demo](https://github.com/klesh/fu/raw/master/example.gif)
# Introduction

I once needed a tool to upload images/files in Clipboard and generate a URL back to it for me. So I looked for such a tool, I found iPic which quite awesome. But unfortunately, it supported image only and wouldn't let me upload to own server. It would be greate to have a another option, wasn't it?   

The plan was to make a CrossPlatform app for (Windows/MacOS/Linux), and it can also processes Captured Bitmap Data in Clipboard, but itself doesn't have any Screen Capturing functionality. You may need other tool to do the job.

# Installation

[Go to releases page](https://github.com/klesh/fu/releases) download file according to your OS.

## Windows

  download the .7z file from releases page. extract to whereever you want.   
  You may also need to install vc++ runtime library, it can be download fomr here: http://www.microsoft.com/en-us/download/details.aspx?id=48145   
  If you want it to be launched at login, please make a shortcut in your `Startup` folder which link to fu.exe.

## MacOS

  download the .dmg file from releases page, double click it and drag the fu icon into `Applications` folder
  If you want it to be launched at login, please go to `System Preferences > Users & Groups > Login Items` and add it to your list.

## Linux

  download the .tar.gz file, extract, make and make install:

### Ubuntu
  ```bash
sudo apt install libssh2-1-dev libboost1.61-dev libappindicator3-dev
make
  ```

# Build Dependencies

  * wxWidgets 3.0
  * libcurl
  * libssh2
  * boost

# Like it ?

  * Star this respository right away!
  * Recommand to your friends.
  * Twitter/Facebook/Reddit it already.
  * Pull Requests are all welcome.

# License

LGPL
