![exsportify-logo](img/logo_250x100.png)
## Spotify Playlist Exporter

A set of well crafted music playlists is something invaluable you don't want to loose. A playlist can easily be deleted or you may want to move to another music service. To alleviate the uncertainties of the future this program can be used to automate backups (ideal with e.g. cron) of all your Spotify playlist to files on disk.

This product uses Music by Spotify but is not endorsed, certified or otherwise approved in any way by Spotify. Spotify is the registered trade mark of the Spotify Group.

## How to build
The installation procedure is simple. The only special thing is that you need to replace the dummy Spotify appkey with your own.

```console
$ git clone https://github.com/erikw/exsportify
$ cd exsportify
$ cp path/to/your/downloaded/spotify/appkey.c src/appkey.c
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make install
```

### Requirements

* [cmake](http://www.cmake.org/) version Z
* [boost](http://www.boost.org/) version Y
* [libspotify](https://developer.spotify.com/technologies/libspotify/) version X
* Spotify Premium & downloaded application key. To compile the source you need to obtain a [developer key](https://developer.spotify.com/technologies/libspotify/) from Spotify.
