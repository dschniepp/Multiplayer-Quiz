/**
 * \file	loader/browse.c
 * \author	Stefan Gast
 *
 * \brief	Implementierung des Verzeichnislisters
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include "common/util.h"
#include "browse.h"

/**
 * \brief	Eine Zeichenkette in einer Zeile mit einem einzelnen write-Aufruf ausgeben.
 *
 * Gibt die angegebene Zeichenkette als einzelne Zeile mit anschließendem Zeilenvorschub aus, ähnlich
 * wie puts. Diese Funktion garantiert aber, dass keine zusätzliche Pufferung erfolgt und nur genau ein
 * write-Aufruf benutzt wird.
 *
 * \return	Die Anzahl der ausgegebenen Zeichen
 */
ssize_t myPuts(const char *str		/**< Der auszugebende Text */
	      )
{
	size_t len = strlen(str);
	char *buffer = alloca(len+1);	/* siehe man 3 alloca */

	strcpy(buffer, str);
	buffer[len] = '\n';		/* \0 durch \n ersetzen */

	return write(STDOUT_FILENO, buffer, len+1);
}


/**
 * \brief	Den Inhalt eines Verzeichnisses auf der Standardausgabe ausgeben
 *
 * Gibt den Inhalt eines Verzeichnisses auf der Standardausgabe zeilenweise aus.
 * Es werden keine versteckten Dateien (deren Name mit einem Punkt beginnt) aufgelistet.
 * Eine Leerzeile signalisiert das Ende der Ausgabe.
 * Fehlermeldungen erscheinen auf der Standardfehlerausgabe.
 */
void browse(const char *directory_name		/**< Der Name des aufzulistenden Verzeichnisses */
	   )
{
	DIR *dirp = opendir(directory_name);
	struct dirent *entry;

	if(dirp == NULL)
	{
		debugPrint("Kann Verzeichnis %s nicht auflisten: %s",
			   directory_name, strerror(errno));
		write(STDOUT_FILENO, "\n", 1);
		return;
	}

	errno = 0;
	entry = readdir(dirp);
	while(entry != NULL)
	{
		if(entry->d_name[0] != '.')		/* versteckte Dateien ignorieren... */
			myPuts(entry->d_name);		/* ...und nur die anderen ausgeben */

		errno = 0;
		entry = readdir(dirp);
	}

	if(errno != 0)			/* Falls ein Fehler aufgetreten ist... */
	{
		debugPrint("Fehler beim Auflisten des Verzeichnisses %s: %s",		/* ...Meldung ausgeben */
			   directory_name, strerror(errno));
	}

	write(STDOUT_FILENO, "\n", 1);
}
