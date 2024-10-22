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
    - [ ] Making JWT
      - [x] Base 64 encoding.
      - [ ] SHA256 encoding.
  - [ ] Parce reply.
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

> Cjson