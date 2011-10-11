/**
 * \file	common/server_loader_protocol.h
 * \author	Stefan Gast
 *
 * \brief	Definitionen für das Protokoll zwischen Server und Loader
 */

#ifndef SERVER_LOADER_PROTOCOL_H
#define SERVER_LOADER_PROTOCOL_H

/* Kommandos für den Loader */
#define	BROWSE_CMD		"BROWSE"	/**< Kataloge auflisten */
#define LOAD_CMD_PREFIX		"LOAD "		/**< Katalog laden */

/* Statusmeldungen des Loaders */
#define LOAD_SUCCESS_PREFIX	"LOADED, SIZE = "		/**< Katalog mit SIZE Fragen geladen */
#define LOAD_ERROR_PREFIX	"ERROR: "			/**< Prefix für Fehlermeldungen */
#define LOAD_ERROR_CANNOT_OPEN	"CANNOT OPEN FILE"		/**< Kann Katalog nicht öffnen */
#define LOAD_ERROR_CANNOT_READ	"CANNOT READ FILE"		/**< Kann Katalog nicht lesen */
#define LOAD_ERROR_INVALID	"INVALID CATALOG"		/**< Ungültiges Dateiformat */
#define LOAD_ERROR_SHMEM	"CANNOT USE SHARED MEMORY"	/**< Kann Shared Memory nicht benutzen */
#define LOAD_ERROR_OOM		"OUT OF MEMORY"			/**< Zu wenig freier Speicher */

/* Name des Shared Memory */
#define SHMEM_NAME		"/quiz_example_solution"	/**< Name des POSIX.4 Shared Memory */

#endif
