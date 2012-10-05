#ifndef GUIITEMS
#define GUIITEMS

#include "debug.h"

#include <QtGui>

///////////////////////////////////////////////////////////////////////////////
#define MAX_CMDEDIT_HISTORY 15

class CmdEdit : public QLineEdit {

	Q_OBJECT

  public:

	CmdEdit(bool acceptEmpty = false, bool keepText = false);
	CmdEdit(QWidget *, bool acceptEmpty = false, bool keepText = false);
	~CmdEdit();

	void	keyPressEvent(QKeyEvent *);

  private:

	QStringList	list;
	QString		tempString;
	int		listPointer;

	bool		acceptEmpty;
	bool		keepText;

	//-----------------------------------------------------

	void	init();

  signals:

	void	input(QString);

};

///////////////////////////////////////////////////////////////////////////////

class StringEdit : public QLineEdit {

	Q_OBJECT

  public:

	StringEdit();
	StringEdit(QWidget *);
	~StringEdit();

	void	keyPressEvent(QKeyEvent *);

  signals:

	void	changed(QString);

};

///////////////////////////////////////////////////////////////////////////////

class CodeEdit : public QTextEdit {

	Q_OBJECT

  public:

	CodeEdit();
	CodeEdit(QWidget *);
	~CodeEdit();

	int	currentLine();

};

///////////////////////////////////////////////////////////////////////////////

#endif
