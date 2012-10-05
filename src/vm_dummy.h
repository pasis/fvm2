#ifndef VM_DUMMY_H
#define VM_DUMMY_H

// компылюємо підтримку, коли цього хоче користувач (configure --with-vm-dummy)
#ifdef WITH_VM_DUMMY

///////////////////////////////////////////////////////////////

#include "config.h"
#include "debug.h"
#include "vm.h"

//-------------------------------------------------------------

class vmDummy : public AbstractVM {

	Q_OBJECT

  public:

	vmDummy(VM *);
	~vmDummy();

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

	QWidget		*win;
	QVBoxLayout	*vbox;
	QLineEdit	*log;

	int		counter;

	//-----------------------------------------------------

  public slots:

	void	refresh();

};

///////////////////////////////////////////////////////////////

#endif

#endif
