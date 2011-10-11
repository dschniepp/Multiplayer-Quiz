/**
 * \file	client/gui/gui_interface.h
 * \author	Stefan Gast
 *
 * \brief	Definiert die API für die GUI des Quiz-Spiels
 */

#ifndef GUI_INTERFACE_H
#define GUI_INTERFACE_H

enum
{
	GUI_MAX_USERS	= 6,		/**< maximal 6 User */
	GUI_NUM_ANSWERS = 4		/**< pro Frage 4 Antwortmöglichkeiten */
};

/**
 * \brief	Übergabewert für game_setStatusIcon
 *
 * Spezifiziert das in der Statuszeile des Spielfenster anzuzeigende Icon.
 */
typedef enum
{
	STATUS_ICON_NONE	= 0,	/**< Kein Statusicon anzeigen */
	STATUS_ICON_CORRECT	= 1,	/**< Icon für richtige Antwort anzeigen */
	STATUS_ICON_WRONG	= 2,	/**< Icon für falsche Antwort anzeigen */
	STATUS_ICON_TIMEOUT	= 3	/**< Icon für "Zeit abgelaufen" anzeigen */
} StatusIcon;

/**
 * \brief	Übergabewert für preparation_setMode
 *
 * Spezifiziert den Modus, in dem sich das Vorbereitungsfenster befindet.
 */
typedef enum
{
	PREPARATION_MODE_BUSY		= 0,	/**< Momentan beschäftigt, es werden keine Eingaben zugelassen */
	PREPARATION_MODE_NORMAL		= 1,	/**< Normaler Modus, Katalogauswahl und Start durch User nicht möglich */
	PREPARATION_MODE_PRIVILEGED	= 2	/**< Modus für den Spielleiter, Katalogauswahl und Start möglich */
} PreparationMode;


/******************************************
 * allgemeine, bereitgestellte Funktionen *
 ******************************************/
void guiInit(int *argc, char ***argv);
void guiMain(void);
void guiQuit(void);
void guiDestroy(void);
void guiShowErrorDialog(const char *message, int quitOnClose);
void guiShowMessageDialog(const char *message, int quitOnClose);


/*******************************************
 * Funktionen für den Vorbereitungs-Dialog *
 *******************************************/

/* vorgegebene Funktionen */
void preparation_setMode(PreparationMode mode);
void preparation_showWindow(void);
void preparation_addCatalog(const char *name);
int preparation_selectCatalog(const char *name);
void preparation_addPlayer(const char *name);
int preparation_removePlayer(const char *name);
void preparation_clearPlayers(void);
void preparation_hideWindow(void);
void preparation_reset(void);

/* zu implementierende Funktionen */
void preparation_onCatalogChanged(const char *newSelection);
void preparation_onStartClicked(const char *currentSelection);
void preparation_onWindowClosed(void);


/***********************************
 * Funktionen für das Spielfenster *
 ***********************************/

/* vorgegebene Funktionen */
void game_showWindow(void);
void game_setStatusText(const char *text);
void game_setStatusIcon(StatusIcon icon);
void game_setQuestion(const char *text);
void game_setAnswer(int index, const char *text);
void game_markAnswerCorrect(int index);
void game_markAnswerWrong(int index);
void game_unmarkAnswers(void);
void game_setAnswerButtonsEnabled(int enable);
void game_setPlayerName(int position, const char *name);
void game_setPlayerScore(int position, unsigned long score);
void game_highlightPlayer(int position);
void game_hideWindow(void);
void game_reset(void);

/* zu implementierende Funktionen */
void game_onAnswerClicked(int index);
void game_onWindowClosed(void);

#endif
