/**
 * \file	common/util.c
 * \author	Stefan Gast
 *
 * \brief	Implementierung diverser Hilfsfunktionen für Ein-/Ausgabe, Fehlersuche usw.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "util.h"

static const char *prog_name = "<unknown>";	/**< Aufrufname des Programms (argv[0]) */
static int debug_enabled = 0;			/**< Debug-Ausgaben eingeschaltet? */


/**
 * \brief	Den Programmnamen setzen
 *
 * Setzt den Namen des Programms (normalerweise argv[0] von main).
 * Es wird kein Speicher reserviert, sondern die gleiche Adresse wie für
 * das Argument verwendet!
 *
 * \see		getProgName
 */
void setProgName(const char *argv0	/**< argv[0] von main */
		)
{
	prog_name = argv0;
}


/**
 * \brief	Den Programmnamen abfragen
 *
 * Fragt den Aufrufnamen des Programms ab, der zuvor mit setProgName
 * gesetzt wurde.
 *
 * \return	Der zuvor mit setProgName gesetzte Name, oder "<unknown>"
 * 		falls setProgName noch nicht aufgerufen wurde.
 *
 * \see		setProgName
 */
const char *getProgName(void)
{
	return prog_name;
}


/**
 * \brief	Debug-Ausgaben einschalten
 *
 * Schaltet die Debug-Ausgaben ein.
 */
void debugEnable(void)
{
	debug_enabled = 1;
}


/**
 * \brief	Prüfen, ob Debug-Meldungen aktiv sind
 *
 * \retval	0: Debug-Meldungen derzeit ausgeschaltet
 * \retval	1: Debug-Meldungen aktiv
 */
int debugEnabled(void)
{
	return debug_enabled;
}


/**
 * \brief	Debug-Ausgaben ausschalten
 *
 * Schaltet die Debug-Ausgaben aus.
 */
void debugDisable(void)
{
	debug_enabled = 0;
}


/**
 * \brief	Debug-Meldung ausgeben
 *
 * Gibt eine Meldung auf der Standardfehlerausgabe aus, falls Debug-Ausgaben aktiviert sind.
 * Der Meldung wird der Name des Programms, gefolgt von ": " vorangestellt. Jede Ausgabe wird
 * durch einen Zeilenumbruch abgeschlossen.
 *
 * \see		debugEnable, debugDisable, setProgName
 */
void debugPrint(const char *fmt,	/**< printf-Formatstring */
		...			/**< zusätzliche Argumente */
	       )
{
	va_list args;

	if(debug_enabled)
	{
		va_start(args, fmt);
		flockfile(stderr);			/* Stream sperren, sodass die Zeile immer komplett und zusammenhängend ausgegeben wird */
		fprintf(stderr, "%s: ", getProgName());
		vfprintf(stderr, fmt, args);
		putc_unlocked('\n', stderr);
		funlockfile(stderr);			/* Sperre freigeben */
		va_end(args);
	}
}


/**
 * \brief	Informations-Meldung ausgeben
 *
 * Gibt eine Meldung auf der Standardausgabe aus.
 * Der Meldung wird der Name des Programms, gefolgt von ": " vorangestellt. Jede Ausgabe wird
 * durch einen Zeilenumbruch abgeschlossen.
 *
 * \see		setProgName
 */
void infoPrint(const char *fmt,		/**< printf-Formatstring */
		...			/**< zusätzliche Argumente */
	       )
{
	va_list args;

	va_start(args, fmt);
	flockfile(stdout);			/* Stream sperren, sodass die Zeile immer komplett und zusammenhängend ausgegeben wird */
	printf("%s: ", getProgName());
	vprintf(fmt, args);
	putc_unlocked('\n', stdout);
	funlockfile(stdout);			/* Sperre freigeben */
	va_end(args);
}


/**
 * \brief	Fehlermeldung ausgeben
 *
 * Gibt eine Meldung auf der Standardfehlerausgabe aus.
 * Der Meldung wird der Name des Programms, gefolgt von ": " vorangestellt. Jede Ausgabe wird
 * durch einen Zeilenumbruch abgeschlossen.
 *
 * \see		setProgName
 */
void errorPrint(const char *fmt,	/**< printf-Formatstring */
		...			/**< zusätzliche Argumente */
	       )
{
	va_list args;

	va_start(args, fmt);
	flockfile(stderr);			/* Stream sperren, sodass die Zeile immer komplett und zusammenhängend ausgegeben wird */
	fprintf(stderr, "%s: ", getProgName());
	vfprintf(stderr, fmt, args);
	putc_unlocked('\n', stderr);
	funlockfile(stderr);			/* Sperre freigeben */
	va_end(args);
}


/**
 * \brief	Fehlermeldung in Abhängigkeit zu errno ausgeben
 *
 * Gibt eine Fehlermeldung nach dem Muster
 * "Programmname: prefix: strerror(errno)" aus. Falls kein zu errno passender
 * Text gefunden wird, wird nur "Programmname: prefix" ausgegeben.
 * Im Gegensatz zu strerror ist diese Funktion threadsicher.
 *
 * \see		setProgName
 */
void errnoPrint(const char *prefix	/**< Text, der der Fehlerbeschreibung vorangestellt wird */
	       )
{
	char errnoText[1024];

	if(strerror_r(errno, errnoText, sizeof(errnoText)) == 0)
		errorPrint("%s: %s", prefix, errnoText);
	else
		errorPrint("%s", prefix);
}


/**
 * \brief	Eine Zeile aus einem Eingabekanal lesen
 *
 * Liest eine Zeile aus einem Eingabekanal. Der Speicher dafür wird dynamisch mit malloc
 * reserviert. Der abschließende Zeilenumbruch wird durch ein Nullbyte ersetzt.
 *
 * \attention	Die Funktion ist nicht cancel-safe, da beim Abbruch der mit malloc reservierte
 * 		Speicher verloren geht!
 *
 * \return	Die gelesene Zeile, oder NULL bei Fehler (errno wird entsprechend gesetzt)
 */
char *readLine(int fd		/**< Der Dateideskriptor, von dem gelesen werden soll */
	      )
{
	static const size_t bufferGrow = 512;
	size_t bufferSize = 0;
	size_t bufferPos = 0;
	char *buffer = NULL;
	char *newBuffer = NULL;

	for(;;)
	{
		/* mehr Speicher reservieren, falls Puffer voll */
		if(bufferPos >= bufferSize)
		{
			bufferSize += bufferGrow;
			newBuffer = realloc(buffer, bufferSize);
			if(newBuffer == NULL)
			{
				free(buffer);
				return NULL;
			}
			buffer = newBuffer;
		}

		/* Byte lesen */
		errno = 0;
		if(read(fd, &buffer[bufferPos], 1) < 1)
		{
			free(buffer);
			return NULL;
		}

		/* Zeilenumbruch gelesen? */
		if(buffer[bufferPos] == '\n')
		{
			buffer[bufferPos] = '\0';	/* Zeilenumbruch durch Nullbyte ersetzen */
			return buffer;	/* Puffer zurückgeben und beenden */
		}

		++bufferPos;
	}

	return NULL;	/* wird nie erreicht */
}
