EXEC = audioBetter

CLIB = -I./lib/include ./lib/lib/.libs/libportaudio.a -lrt -lasound -ljack -pthread

execute: main.cpp ButterworthFilter.cpp
	g++ -o $@ $^ $(CLIB)

install-deps:
	mkdir -p lib

	curl http://files.portaudio.com/archives/pa_stable_v190700_20210406.tgz | tar -zx -C lib
	cd lib/portaudio && ./configure && $(MAKE) -j
.PHONY: install-deps

uninstall-deps:
	cd lib/portaudio && $(MAKE) uninstall
	rm -rf lib/portaudio
.PHONY: uninstall-deps

clean:
	rm -f $(EXEC)
.PHONY: clean