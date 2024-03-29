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

#SERVER_OBJECTS = server/main.o common/util.o common/message.o	# Liste der Server-Objektdateien, WEITERE MODULE HIER HINZUFÜGEN!

CLIENT_OBJECTS = client/main.o common/util.o client/gui/gui_interface.o client/common/client_socket.o client/common/client_util.o client/common/li_qu_thread.o	# Liste der Client-Objektdateien, WEITERE MODULE HIER HINZUFÜGEN!

TARGETS = bin/loader bin/client #bin/server 


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

#loader/%.o: loader/%.c
	#gcc $(COPTS) -Iloader -o $@ $<

#server/%.o: server/%.c
	#gcc $(COPTS) -Iserver -o $@ $<

client/%.o: client/%.c
	gcc $(COPTS) -Iclient -o $@ $<

common/%.o: common/%.c
	gcc $(COPTS) -o $@ $<
	
client/gui/%.o: client/gui/%.c
	gcc $(COPTS) -o $@ $<

####################################################################

#
# Die einzelnen Programme linken
#
# $+: Liste aller Abhängigkeiten
#

#bin/loader:	$(LOADER_OBJECTS)
#	gcc -o $@ $+ -lrt

#bin/server:	$(SERVER_OBJECTS)
#	gcc -o $@ $+ -lpthread -lrt

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
#common/socket.o: common/socket.c common/socket.h 
#common/message.o: common/message.c common/message.h

# Hier weitere gemeinsam verwendete Module eintragen!

###################################################################

#
# Objektdateien des Loaders
#
#loader/main.o:		loader/main.c loader/browse.h common/util.h common/server_loader_protocol.h common/socket.h common/message.h

#loader/browse.o:	loader/browse.c loader/browse.h common/util.h common/socket.h common/message.h

#loader/load.o:		loader/load.c loader/load.h loader/parser.h common/server_loader_protocol.h common/question.h common/util.h

#loader/parser.o:	loader/parser.c loader/parser.h common/question.h

##################################################################

#
# Objektdateien des Servers
#

# Hier Abhängigkeiten der Server-Module eintragen!
#server/main.o:		server/main.c common/util.h common/message.h

#################################################################

#
# Objektdateien des Clients
#

client/gui/gui_interface.o: client/gui/gui_interface.c client/gui/gui_interface.h
client/common/client_util.o: client/common/client_util.c client/common/client_util.h
client/common/client_socket.o: client/common/client_socket.c client/common/client_socket.h
client/common/li_qu_thread.o: client/common/li_qu_thread.c client/common/li_qu_thread.h

# Hier Abhängigkeiten der Client-Module eintragen!
client/main.o:		client/main.c client/main.h common/util.h client/gui/gui_interface.h client/common/client_socket.h client/common/client_util.h client/common/li_qu_thread.h client/common/client_global.h
    