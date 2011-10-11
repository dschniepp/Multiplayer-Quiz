/**
 * \file	loader/parser.c
 * \author	Stefan Gast
 *
 * \brief	Implementierung des Fragekatalog-Parsers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"


/**
 * \brief	Einen Fragestack initialisieren
 *
 * \attention	Das Verhalten eines Stacks, der nicht initialisiert wurde,
 * 		ist für die Stack-Operationen nicht definiert.
 */
void stackInit(Stack *stack		/**< Zeiger auf den zu initialisierenden Stack */
	      )
{
	stack->head = NULL;
	stack->size = 0;
}


/**
 * \brief	Frage auf dem Stack ablegen
 *
 * Legt eine Frage auf dem Stack ab.
 *
 * \retval	1 bei Erfolg
 * \retval	0 bei zu wenig Speicher
 */
int stackPush(Stack *stack,		/**< Der Stack, zu dem das Element hinzugefügt werden soll */
	      const Question *newElem	/**< Das neue Element */
	     )
{
	StackItem *newItem = malloc(sizeof(StackItem));
	if(newItem == NULL)
		return 0;

	newItem->next = stack->head;
	newItem->question = *newElem;

	stack->head = newItem;
	++stack->size;

	return 1;
}


/**
 * \brief	Frage vom Stack holen
 *
 * Holt eine Frage vom Stack.
 *
 * \retval	1 bei Erfolg
 * \retval	0 wenn der Stack leer war
 */
int stackPop(Stack *stack,		/**< Der Stack, von dem das Element geholt werden soll */
	     Question *elem		/**< Zeiger auf eine Speicherstelle, an der das Element abgelegt wird */
	    )
{
	StackItem *oldHead = stack->head;

	if(oldHead == NULL)
		return 0;

	if(elem != NULL)
		*elem = oldHead->question;	/* Frage kopieren */
	
	stack->head = oldHead->next;		/* Kopf-Zeiger auf das nächste Element verschieben */
	--stack->size;				/* Zähler aktualisieren */
	free(oldHead);				/* Element freigeben */

	return 1;
}


/**
 * \brief	Anzahl der Elemente auf dem Stack ermitteln
 *
 * \return	Die Anzahl der Elemente auf dem Stack
 */
size_t stackCount(const Stack *stack		/**< Der Stack, dessen Elemente gezählt werden sollen */
		 )
{
	return stack->size;
}


/**
 * \brief	Einen Stack leeren
 */
void stackClear(Stack *stack		/**< Der zu leerende Stack */
	       )
{
	while(stackPop(stack, NULL))
		;
}


/**
 * \brief	Überprüfen, ob eine Zeile leer ist
 *
 * Eine leere Zeile enthält nur Leerzeichen und Tabs, gefolgt von einem Zeilenumbruch.
 *
 * \return	1 für Leerzeilen, sonst 0
 */
static int isEmpty(const char *line		/**< Die zu überprüfende Zeile */
		  )
{
	while(*line)
	{
		if(*line == '\n' && *(line+1) == '\0')
			return 1;

		if(*line != ' ' && *line != '\t')
			return 0;

		++line;
	}

	/* Ende des Strings erreicht, aber kein Zeilenumbruch davor */
	return 0;
}


/**
 * \brief	Überprüfen, ob der übergebene String eine komplette Zeile enthält
 *
 * Eine komplette Zeile enthält einen Zeilenumbruch, gefolgt von einem Nullbyte.
 *
 * \return	Einen Zeiger auf den Zeilenumbruch, falls komplette Zeile, sonst NULL
 */
static char *isComplete(char *str		/**< Die zu überprüfende Zeichenkette */
	 	       )
{
	while(*str)
	{
		if(*str == '\n' && *(str+1) == '\0')
			return str;
		++str;
	}
	return NULL;
}


/**
 * \brief	Eine einzelne Frage aus einem geöffneten Fragekatalog parsen
 *
 * Parst eine einzelne Frage aus einem offenen Fragekatalog.
 *
 * \retval	1 bei Erfolg
 * \retval	0 bei Fehler oder Dateiende, in res steht die genaue Ursache
 */
static int parseQuestion(FILE *fp,		/**< Datei, aus der gelesen werden soll */
			 Question *buffer,	/**< Puffer zum Aufnehmen der Frage */
			 ParserResult *res	/**< Zeiger auf ein ParserResult zum Abspeichern des Status (status = PARSE_OK bei Dateiende) */
			)
{
	const char timeoutPrefix[] = "TIMEOUT:";	/* Prefix für Timeout-Angabe */
	const uint16_t timeoutDefault = (uint16_t)10;	/* Default-Timeout, wenn keine Angabe im Katalog */
	char questionBuffer[QUESTION_SIZE+1];	/* um 1 länger wegen Zeilenumbruch am Ende */
	char answerBuffer[ANSWER_SIZE+3];	/* um 3 länger wegen Markierung (2 Zeichen) und Zeilenumbruch */
	char *newlinePos;
	char *endNum;
	int currentAnswer;
	unsigned long timeoutTmp;		/* temporäre Variable für sichere Umwandlung des Timeout-Wertes */

	/* Leerzeilen überspringen */
	while(fgets(questionBuffer, sizeof(questionBuffer), fp) && isEmpty(questionBuffer))
		++res->lineNumber;

	if(ferror(fp))
	{
		res->status = PARSE_CANNOT_READ;
		return 0;
	}
	else if(feof(fp))
	{
		res->status = PARSE_OK;
		return 0;
	}

	/* Keine Leerzeile mehr, also haben wir die Frage. Wir überprüfen auf Zeilenende, um sicherzugehen,
	 * dass die Frage nicht zu lang war und wir sie komplett haben. */
	if((newlinePos = isComplete(questionBuffer)) == NULL)
	{
		res->status = PARSE_LINE_TOO_LONG;
		return 0;
	}

	/* Frage ohne Zeilenvorschub in Puffer kopieren */
	*newlinePos = '\0';
	strcpy(buffer->question, questionBuffer);

	/* Antworten und Timeout einlesen */
	buffer->timeout = timeoutDefault;	/* Timeout auf Default, falls keine Angabe gefunden wird */
	buffer->correct = -1;			/* zunächst keine Antwort als korrekt markieren */
	currentAnswer = 0;
	while(currentAnswer < NUM_ANSWERS)
	{
		++res->lineNumber;	/* Zeilenzähler erhöhen */

		if(fgets(answerBuffer, sizeof(answerBuffer), fp) == NULL)
		{
			/* Hier dürfen wir nicht auf das Dateiende laufen, weil wir noch nicht alle
			 * Antwortmöglichkeiten eingelesen haben. Dateifehler ist natürlich auch
			 * nicht erwünscht. */
			if(ferror(fp))
				res->status = PARSE_CANNOT_READ;
			else if(feof(fp))
				res->status = PARSE_UNEXPECTED_EOF;

			return 0;
		}

		if((newlinePos = isComplete(answerBuffer)) == NULL)
		{
			res->status = PARSE_LINE_TOO_LONG;
			return 0;
		}

		*newlinePos = '\0';	/* Zeilenvorschub löschen */

		if(!strncmp(answerBuffer, "- ", 2))	/* falsche Antwortmöglichkeit gefunden */
		{
			strcpy(buffer->answers[currentAnswer], answerBuffer+2);
			++currentAnswer;
		}
		else if(!strncmp(answerBuffer, "+ ", 2))	/* richtige Antwortmöglichkeit gefunden */
		{
			if(buffer->correct != -1)		/* Schon eine richtige Antwort gelesen? Mehr als eine richtige nicht erlaubt */
			{
				res->status = PARSE_TOO_MANY_CORRECT;
				return 0;
			}
			
			strcpy(buffer->answers[currentAnswer], answerBuffer+2);
			buffer->correct = currentAnswer;
			++currentAnswer;
		}
		else
		{
			/* Timeout kann in der ersten Zeile nach der Frage stehen */
			if(currentAnswer == 0 && !strncmp(answerBuffer, timeoutPrefix, sizeof(timeoutPrefix)-1))
			{
				timeoutTmp = strtoul(answerBuffer+sizeof(timeoutPrefix)-1, &endNum, 10);
				if(*endNum != '\0' || endNum == answerBuffer)	/* Unsinnige Zeichen nach der Zahl, oder gar keine Zahl */
				{
					res->status = PARSE_INVALID_TIMEOUT;
					return 0;
				}
				if(timeoutTmp == 0 || timeoutTmp > 60)	/* Timeout zu klein oder zu groß */
				{
					res->status = PARSE_INVALID_TIMEOUT;
					return 0;
				}
				buffer->timeout = (uint16_t)timeoutTmp;
			}
			else	/* kein Timeout oder nicht erste Zeile nach der Frage */
			{
				res->status = PARSE_INVALID_ANSWER;
				return 0;
			}
		}
	}

	/* Haben wir eine richtige Antwortmöglichkeit gelesen? */
	if(buffer->correct == -1)
	{
		res->status = PARSE_CORRECT_MISSING;
		return 0;
	}

	/* Alles Okay! */
	res->status = PARSE_OK;
	++res->lineNumber;
	return 1;
}


/**
 * \brief	Einen Fragekatalog parsen und die Fragen auf einem Stack speichern
 *
 * Parst einen Fragekatalog und speichert die so erhaltenen Fragestrukturen auf einem Stack.
 * Falls beim Parsen ein Fehler auftritt, wird der Stack mit den bis dahin gelesenen
 * Fragen automatisch wieder freigegeben.
 *
 * \attention	Der Stack muss vor Verwendung mit stackInit initialisiert werden, ansonsten
 * 		ist das Verhalten nicht definiert!
 *
 * \return	Erfolgsstatus \see ParserResult
 */
ParserResult parseCatalog(const char *path,		/**< Der Dateiname des Katalogs, inklusive Verzeichnis */
			  Stack *questions		/**< Initialisierter Stack, auf dem die Fragen abgelegt werden */
			 )
{
	FILE *fp;
	Question buffer;
	ParserResult res;

	if((fp = fopen(path, "r")) == NULL)		/* übergebenen Dateipfad öffnen */
	{
		res.status = PARSE_CANNOT_OPEN;
		res.lineNumber = 0UL;
		return res;
	}

	res.lineNumber = 1UL;

	while(parseQuestion(fp, &buffer, &res))		/* alle Fragen parsen... */
	{
		if(!stackPush(questions, &buffer))	/* ...und auf den Stack legen, Fehler bei zu wenig Speicher */
		{
			stackClear(questions);		/* Stack im Fehlerfall freigeben */
			fclose(fp);
			res.status = PARSE_OOM;
			return res;
		}
	}

	fclose(fp);

	if(res.status == PARSE_OK)		/* kein Fehler beim Parsen der einzelnen Fragen */
	{
		/* Prüfen, ob wir überhaupt eine Frage eingelesen haben, denn
		 * ein Fragekatalog darf nicht leer sein! */
		if(stackCount(questions) > 0)
			res.lineNumber = 0UL;		/* Zeilennummer zurücksetzen, status ist schon auf ok */
		else
			res.status = PARSE_EMPTY;	/* Fehler für leeren Katalog setzen */
	}
	else
		stackClear(questions);		/* Stack im Fehlerfall löschen */

	return res;		/* Status zurückmelden */
}
