/**
 * \file	common/question.h
 * \author	Stefan Gast
 *
 * \brief	Repräsentation einer Frage mit den dazugehörigen Antworten im Quiz
 */

#ifndef QUIZ_QUESTION_H
#define QUIZ_QUESTION_H

#include <stdint.h>

enum
{
	QUESTION_SIZE		= 256,	/**< Maximale Länge einer Frage (in Bytes) */
	ANSWER_SIZE		= 128,	/**< Maximale Länge einer Antwort (in Bytes) */
	NUM_ANSWERS		= 4	/**< Anzahl der Antworten pro Frage */
};

#pragma pack(push,1)

/**
 * \brief	Interne Darstellung einer Frage
 *
 * \note	Gepackt für gleiche Darstellung auf allen Compilern
 */
typedef struct
{
	char question[QUESTION_SIZE];			/**< Text der Fragestellung */
	char answers[NUM_ANSWERS][ANSWER_SIZE];		/**< Antworten */
	uint16_t timeout;				/**< Zeit für diese Frage in Sekunden */
	int correct;					/**< Index der richtigen Antwort */
} Question;

/**
 * \brief	Darstellung einer Frage während des Transports zum Client
 *
 * \attention	Muss bis auf das Fehlen der Variable correct exakt gleich sein wie
 * 		die Question-Struktur
 */
typedef struct
{
	char question[QUESTION_SIZE];			/**< Text der Fragestellung */
	char answers[NUM_ANSWERS][ANSWER_SIZE];		/**< Antworten */
	uint16_t timeout;				/**< Zeit für diese Frage in Sekunden (zu Informationszwecken mitgesendet) */
} QuestionMessage;

#pragma pack(pop)

#endif
