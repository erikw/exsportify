## exsportify - Spotify Playlist Exporter
<img alt="logo" src="img/logo_250x100.png" align="right">

A set of well crafted music playlists is something invaluable you don't want to loose. A playlist can easily be deleted or you may want to move to another music service. To alleviate the uncertainties of the future this program can be used to automate backups (ideal with e.g. cron) of all your Spotify playlist to files on disk.

This product uses Music by Spotify but is not endorsed, certified or otherwise approved in any way by Spotify. Spotify is the registered trade mark of the Spotify Group.

## How to build
The installation procedure is simple. The only special thing is that you need to replace the dummy Spotify appkey with your own.

```console
$ git clone https://github.com/erikw/exsportify && cd $(basename "$_" .git)
$ cp path/to/your/downloaded/spotify/appkey.c src/appkey.c
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

The program can be uninstalled if the build directory is kept (or rebuilt)

```console
$ cd exsportify/build
$ sudo make uninstall
```


### Requirements

* [cmake](http://www.cmake.org/) >= `v2.6`
* [boost](http://www.boost.org/) >= `v1.50`
* [libspotify](https://developer.spotify.com/technologies/libspotify/) >= `v12.1.51`
* Spotify Premium & downloaded personal [application key](https://developer.spotify.com/technologies/libspotify/).

### Status

Apparently libspotify is deprecated now :-(
