#ifndef GUI_H
#define GUI_H

#include "config.h"
#include "debug.h"
#include "vm.h"
#include "guiitems.h"
#include "help.h"

#include <iostream>
#include <fstream>

///////////////////////////////////////////////////////////////

class	CodeHighlighter: public QSyntaxHighlighter {

	Q_OBJECT

  public:

	CodeHighlighter(QTextEdit *, VM *);
	~CodeHighlighter();

	void	highlightBlock(const QString &);

  private:

	CodeEdit	*code;
	VM		*machine;

  signals:

	void	changed();

};

//=============================================================

class GUI: public QMainWindow {

	Q_OBJECT

  public:

	GUI(QApplication *, VM *, QTranslator *);
	~GUI();

  private:

	VM		*machine;
	QApplication	*app;

	bool		readOnly;
	bool		running;
	bool		isMachineSet;
	QWidget		*currentVMWidget;

	QList<int>	breakPoints;
	QMutex		mutexBreakPoints;

	QTranslator	*translator;
	QStringList	languages;	// возможно поменять на др структуру, чтобы файлу соответствовала строка с названием языка

	// Допоміжні діалоги
	QMessageBox	*aboutMessage;
	QFileDialog	*openDialog;
	QFileDialog	*saveDialog;
	HelpBrowser	*help;

	// Головний інтерфейс
	QSplitter	*splitVertical;
	QSplitter	*splitHorizontal;

	// Фрейм з кодом
	QWidget		*win1;
	QVBoxLayout	*win1Layout;
	CodeEdit	*code;
	CmdEdit		*cmd;

	int			highlightedLine;
	CodeHighlighter		*highlight;
	QTextBlockFormat	formatLight;
	QTextBlockFormat	formatBreakPoint;
	QTextBlockFormat	formatNormal;

	// Фрейм зі списком машин
	QWidget		*win2;
	QVBoxLayout	*win2Layout;
	QListWidget	*vmList;
	QGroupBox	*groupMode;
	QVBoxLayout	*groupModeLayout;
	QCheckBox	*modeStep;
	QCheckBox	*modeReadOnly;
	QPushButton	*buttonRun;
	QPushButton	*buttonBreakPount;

	// Фрейм із оболонкою віртуальної машини
	QWidget		*win3;
	QVBoxLayout	*win3Layout;
	QGroupBox	*groupVM;
	QVBoxLayout	*groupVMLayout;

	// Меню
	QMenuBar	*menuBar;
	QMenu		*menuFile;
	QMenu		*menuEdit;
	QAction		*menuEditReadOnly;
	QMenu		*menuExec;
	QAction		*menuExecStepMode;
	QAction		*menuExecRun;
	QMenu		*menuLang;
	QMenu		*menuAbout;

	//-----------------------------------------------------

	void	closeEvent(QCloseEvent *);
	void	keyPressEvent(QKeyEvent *);

	void	cleanLight(int);
	void	cleanBreakPoints();

  public slots:

	void	Quit();
	void	about();
	void	openFromFile();
	void	saveToFile();
	void	addMachine(AbstractVM *);
	void	changeMachine(AbstractVM *);
	void	selectMachine(QModelIndex);
	void	selectLanguage();
	void	setBreakPoint();
	void	execute();
	void	afterExecute();
	void	light();
	void	changeStepMode();
	void	changeReadOnly();

};

///////////////////////////////////////////////////////////////

#endif
