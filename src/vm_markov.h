#ifndef VM_MARKOV_H
#define VM_MARKOV_H

///////////////////////////////////////////////////////////////

#include "config.h"
#include "debug.h"
#include "vm.h"
#include "textutils.h"
#include "guiitems.h"

///////////////////////////////////////////////////////////////

class vmMarkov: public AbstractVM {

	Q_OBJECT

  public:

	vmMarkov(VM *);
	~vmMarkov();

	void	start();
	void	stop();
	void	terminate();
	QWidget	*initGui();

	QString	syntax();
	int	nextLine(QStringList &);
	bool	checkSyntax(QString);
	bool	executeCmd(QString);

  private:

	VM		*machine;
	bool		execConnected;
	bool		startConnected;

	QString		memory;

	//-----------------------------------------------------
	// GUI

	QWidget		*win;
	QVBoxLayout	*vbox;
	StringEdit	*log;


  public slots:

	void	setMemory(QString);
	void	startExecuting();
	void	stopExecuting();
	void	refresh();


};

///////////////////////////////////////////////////////////////

#endif
