/**
 * \file	loader/main.c
 * \author	Stefan Gast
 *
 * \brief	Einstiegspunkt und Hauptschleife des Loaders
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include "common/util.h"
#include "common/server_loader_protocol.h"
#include "browse.h"
#include "load.h"


static const char *cataloges_dir = NULL;	/**< Verzeichnis mit den Fragekatalogen */


/**
 * \brief	Hauptschleife des Loaders
 *
 * Verarbeitet Kommandos von der Standardeingabe, bis das Dateiende erreicht wurde.
 */
static void processCommands(void)
{
	char *read_buffer;
	
	while((read_buffer = readLine(STDIN_FILENO)) != NULL)
	{
		if(!strncmp(read_buffer, BROWSE_CMD, sizeof(BROWSE_CMD)-1))
		{
			debugPrint("BROWSE Kommando erhalten.");
			browse(cataloges_dir);
		}
		else if(!strncmp(read_buffer, LOAD_CMD_PREFIX, sizeof(LOAD_CMD_PREFIX)-1))
		{
			debugPrint("LOAD Kommando erhalten für: %s", read_buffer+sizeof(LOAD_CMD_PREFIX)-1);
			load(cataloges_dir, read_buffer+sizeof(LOAD_CMD_PREFIX)-1);
		}
		else
		{
			debugPrint("Ungültiges Kommando: %s", read_buffer);
		}

		free(read_buffer);
	}

	if(errno != 0)
		debugPrint("Fehler beim Lesen der Standardeingabe: %s", strerror(errno));
	else
		debugPrint("Eingabekanal geschlossen, Programm wird beendet!");
}


/**
 * \brief	Kommandozeilen-Argumente verarbeiten
 *
 * Verarbeitet die Argumente der Kommandozeile
 *
 * \return	0 bei Erfolg, 1 bei Fehler
 */
static int parseCommandline(int argc,		/**< argc von main */
		            char **argv		/**< argv von main */
			   )
{
	if(argc == 3)	/* Aufruf mit Debug-Flag und Katalogpfad */
	{
		if(!strcmp(argv[1], "-d") || !strcmp(argv[1], "--debug"))	/* Debug-Flag erster Parameter */
		{
			cataloges_dir = argv[2];
			debugEnable();
			return 0;
		}
		else if(!strcmp(argv[2], "-d") || !strcmp(argv[2], "--debug"))	/* Debug-Flag zweiter Parameter */
		{
			cataloges_dir = argv[1];
			debugEnable();
			return 0;
		}
		else		/* Zwei Parameter, aber kein Debug-Flag */
			return 1;
	}
	else if(argc == 2)	/* Aufruf nur mit Katalogpfad */
	{
		cataloges_dir = argv[1];
		return 0;
	}
	else	/* Falsche Anzahl an Parametern */
	{
		return 1;
	}
}


/**
 * \brief	Hinweis zum Aufruf ausgeben und mit Fehler beenden
 *
 * Gibt einen Hinweis zum Aufruf des Programms auf der Standardfehlerausgabe
 * aus und beendet das Programm anschließend mit Fehlercode.
 */
static void usage(void)
{
	errorPrint("Aufruf:  %s [-d] KATALOG_VERZEICHNIS",
			getProgName());
	exit(1);
}


/**
 * \brief	Einstiegspunkt in das Programm
 *
 * Verarbeitet Kommandozeilen-Argumente, initialisiert den Zufallsgenerator
 * und beginnt danach mit der Abarbeitung von Kommandos.
 *
 * \return	0 bei regulärem Ende, 1 im Fehlerfall
 */
int main(int argc,	/**< Länge der Kommandozeile */
	 char **argv	/**< Kommandozeilen-Argumente */
	)
{
	setProgName(argv[0]);
	if(parseCommandline(argc, argv))
		usage();

	srand((unsigned int)time(NULL));

	debugPrint("Loader gestartet, bereit für Kommandos.");
	processCommands();

	return 0;
}
