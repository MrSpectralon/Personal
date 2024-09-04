CFLAGS = -g

all: final

final: out/main.o out/helper_methods.o out/spotify_methods.o
	gcc $(CFLAGS) out/main.o out/helper_methods.o out/spotify_methods.o -o playlist_diff

out/main.o: src/main.c
	gcc $(CFLAGS) -o out/main.o -c src/main.c -lcurl

out/helper_methods.o: src/helper_methods.c
	gcc $(CFLAGS) -o out/helper_methods.o  -c src/helper_methods.c -lcurl

out/spotify_methods.o: src/spotify_methods.c
	gcc $(CFLAGS) -o out/spotify_methods.o -c src/spotify_methods.c -lcurl


