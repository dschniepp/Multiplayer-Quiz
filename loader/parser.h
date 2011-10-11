/**
 * \file	loader/parser.h
 * \author	Stefan Gast
 *
 * \brief	Definitionen für den Fragekatalog-Parser
 */

#ifndef CATALOG_PARSER_H
#define CATALOG_PARSER_H

#include "common/question.h"		/* für Frage-Struktur, evtl. anpassen */

/**
 * \brief	Statuscodes des Parsers
 */
typedef enum
{
	PARSE_OK		= 0,	/**< Erfolg */
	PARSE_CANNOT_OPEN	= 1,	/**< Kann Datei nicht öffnen */
	PARSE_CANNOT_READ	= 2,	/**< Kann Datei nicht lesen */
	PARSE_OOM		= 3,	/**< Zu wenig Speicher */
	PARSE_LINE_TOO_LONG	= 4,	/**< Eine Zeile des Fragekatalogs ist zu lang */
	PARSE_UNEXPECTED_EOF	= 5,	/**< Datei hört zu früh auf (z.B. zu wenige Antwortmöglichkeiten für letzte Frage */
	PARSE_TOO_MANY_CORRECT	= 6,	/**< Mehr als eine korrekte Antwortmöglichkeit für eine Frage angegeben */
	PARSE_CORRECT_MISSING	= 7,	/**< Richtige Antwortmöglichkeit fehlt */
	PARSE_INVALID_TIMEOUT	= 8,	/**< Ungültiger Timeout angegeben */
	PARSE_INVALID_ANSWER	= 9,	/**< Falsche Antwortzeile */
	PARSE_EMPTY		= 10	/**< Die Datei enthält keine Fragen */
} ParserStatus;

/**
 * \brief	Ergebnis des Parsers
 */
typedef struct
{
	ParserStatus status;		/**< Statuscode */
	unsigned long lineNumber;	/**< Zeile, in der ein Problem aufgetreten ist (0UL bei Erfolg oder Fehler beim Öffnen) */
} ParserResult;

/**
 * \brief	Element auf dem Fragestack
 *
 * \see		Stack
 */
typedef struct StackItem
{
	struct StackItem *next;		/**< Zeiger auf den nächsten Eintrag */
	Question question;		/**< Die Frage mit Antworten */
} StackItem;

/**
 * \brief	Stack mit geladenen Fragen
 *
 * Stack mit den geladenen Fragen, welcher nach dem Laden in ein Array im Shared Memory 
 * verschoben werden muss.
 */
typedef struct
{
	StackItem *head;
	size_t size;
} Stack;

void stackInit(Stack *stack);
int stackPush(Stack *stack, const Question *newElem);
int stackPop(Stack *stack, Question *elem);
size_t stackCount(const Stack *stack);
void stackClear(Stack *stack);
ParserResult parseCatalog(const char *path, Stack *questions);

#endif
