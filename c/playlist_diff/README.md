# Playlist diff

A project made for the sole purpose of learning C - with the extra benefit of becomming a tool i can use to easily migrate my 1500 song playlist from Spotify to YouTube Music.

The finished application should be able to find information about any playlist your account has access to on either YT(YouTube) or Spotify, and edit the playlists that you have management rights for.

Implementation checklist:

### Spotify support
- [x] Authentication > O-Auth-v2.0.
  - [x] Request auth token.
  - [x] Parce reply.
- [x] Request playlist data.
- [x] Parce reply data.
- [x] Save reply data.
- [ ] Track lookup
- [ ] Add track to playlist

### YouTube Music support;
- [ ] Authentication > O-Auth-v2.0.
  - [ ] Request auth token.
    - [ ] Make a webserver to handle redirect with authentication key
      - [x] using hidden python web server until above checkbox is ready. 
    - [x] Making JWT  -- Probably not needed, this is for different API usage...
      - [x] Base 64 encoding.
      - [x] SHA256 encoding.
      - [x] HMAC-SHA256 encoding
  - [ ] Parce reply.
- [ ] Support for state parameter.
- [ ] Request playlist data.

- [ ] Parce reply data.
- [ ] Save reply data.
- [ ] Track lookup.
- [ ] Add track to playlist.

### General functionality.
- [ ] Diff search.
  - [ ] Find equal songs - Identical data, same name etc.
  - [ ] Partial equal - Album discrepancy, artists tend to publish same songs on multiple albums.
  - [ ] Fuzzy lookup - On YT the user publishing the song is not necessarily the artist, typical semantic used in such cases are "\<Artist> - \<Song name>" as the track title.
- [ ] Terminal interface.
  - [ ] Implementing an interface through NCurses.

# External packages

> Libcurlc
```sh
sudo apt install libcurl4-openssl-dev
```
> Cjson

[cJSON github page](https://github.com/DaveGamble/cJSON?tab=readme-ov-file#cmake)

# Compiling the application.
Running `make` will do it. It currently have several development flags enabled though.

# hidden_from_git folder:
## secret file setup
```
service -> spotify of youtube
clientID
client_secret
playlistID
```
## .env file for Flask server (Temporary solution until i've implemented a web server in c for token exchanges)
```
GOOGLE_CLIENT_ID=your_google_client_id_here
GOOGLE_CLIENT_SECRET=your_google_client_secret_here
GOOGLE_DISCOVERY_URL=https://accounts.google.com/.well-known/openid-configuration
SECRET_KEY=your_flask_secret_key
```
