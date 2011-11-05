# Makefile-Vorlage für das Multiplayer-Quiz
# Stefan Gast, 2010, 2011

####################################################################

#
# Optionen für den Compiler
#

COPTS = -c -Wall -g -I.
GTK_LIBS = `pkg-config gtk+-2.0 --libs` `pkg-config gthread-2.0 --libs`

####################################################################

#
# Liste der Module der Binaries
#

LOADER_OBJECTS = loader/main.o loader/browse.o loader/load.o loader/parser.o common/util.o

SERVER_OBJECTS = server/main.o common/util.o common/socket.o	# Liste der Server-Objektdateien, WEITERE MODULE HIER HINZUFÜGEN!

CLIENT_OBJECTS = client/main.o common/util.o common/socket.o	# Liste der Client-Objektdateien, WEITERE MODULE HIER HINZUFÜGEN!

TARGETS = bin/loader bin/server bin/client


####################################################################

#
# Top-Level Targets
#

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	for i in $(LOADER_OBJECTS); do [ -f "$$i" ] && rm -v "$$i"; done; exit 0
	for i in $(SERVER_OBJECTS); do [ -f "$$i" ] && rm -v "$$i"; done; exit 0
	for i in $(CLIENT_OBJECTS); do [ -f "$$i" ] && rm -v "$$i"; done; exit 0

.PHONY: mrproper
mrproper:	clean
	for i in $(TARGETS); do [ -f "$$i" ] && rm -v "$$i"; done; exit 0

####################################################################

#
# Pattern Rules für Objektdateien
#
# $@: Dateiname des Zieles
# $<: Dateiname der ersten Abhängigkeit
#

loader/%.o: loader/%.c
	gcc $(COPTS) -Iloader -o $@ $<

server/%.o: server/%.c
	gcc $(COPTS) -Iserver -o $@ $<

client/%.o: client/%.c
	gcc $(COPTS) -Iclient -o $@ $<

common/%.o: common/%.c
	gcc $(COPTS) -o $@ $<

####################################################################

#
# Die einzelnen Programme linken
#
# $+: Liste aller Abhängigkeiten
#

bin/loader:	$(LOADER_OBJECTS)
	gcc -o $@ $+ -lrt

bin/server:	$(SERVER_OBJECTS)
	gcc -o $@ $+ -lpthread -lrt

# For 32-bit Version

bin/client:	$(CLIENT_OBJECTS) client/gui/libquizgui32.a
	gcc -o $@ $+ $(GTK_LIBS) -lpthread -lrt

# For 64-bit Version

#bin/client:	$(CLIENT_OBJECTS) client/gui/libquizgui64.a
#	gcc -o $@ $+ $(GTK_LIBS)

####################################################################

#
# Gemeinsam verwendete Objektdateien
#
common/util.o:	common/util.c common/util.h

# Hier weitere gemeinsam verwendete Module eintragen!

###################################################################

#
# Objektdateien des Loaders
#
loader/main.o:		loader/main.c loader/browse.h common/util.h common/server_loader_protocol.h common/socket.h

loader/browse.o:	loader/browse.c loader/browse.h common/util.h common/socket.h

loader/load.o:		loader/load.c loader/load.h loader/parser.h common/server_loader_protocol.h common/question.h common/util.h

loader/parser.o:	loader/parser.c loader/parser.h common/question.h

##################################################################

#
# Objektdateien des Servers
#

# Hier Abhängigkeiten der Server-Module eintragen!
server/main.o:		server/main.c common/util.h common/socket.h

#################################################################

#
# Objektdateien des Clients
#

# Hier Abhängigkeiten der Client-Module eintragen!
client/main.o:		client/main.c common/util.h common/socket.h
