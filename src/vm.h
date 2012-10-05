#ifndef VM_H
#define VM_H

//=============================================================

#include "config.h"
#include "debug.h"

//=============================================================

class AbstractVM : public QObject {

  public:

	AbstractVM() { machineWorking = false; }
	~AbstractVM() {}

	virtual QString	name() { return machineName; }
	virtual bool	working() { return machineWorking; }

	virtual void	start() = 0;
	virtual void	stop() = 0;
	virtual void	terminate() = 0;
	virtual QWidget	*initGui() = 0;

	virtual QString	syntax() = 0;
	virtual int	nextLine(QStringList &) = 0;
	virtual bool	checkSyntax(QString) = 0;
	virtual bool	executeCmd(QString) = 0;

  protected:

	QString		machineName;
	bool		machineWorking;

};

//-------------------------------------------------------------

class VM : public QThread {

	Q_OBJECT

  public:

	VM();
	~VM();

	void	run();
	bool	isStepMode();
	bool	isError();

	// Перенаправлення на віртуальну машину
	bool	working();
	void	start();
	void	stop();
	QString	syntax();
	int	nextLine(QStringList &);
	bool	checkSyntax(QString);

  private:

	AbstractVM	*vm;
	AbstractVM	**machines;
	int		machinesCount;

	QList<int>	breakPoints;
	QMutex		mutexBreakPoints;

	bool		stepMode;
	QMutex		mutexStepMode;

	QStringList	code;
	QMutex		mutexCode;

	bool		error;

	//-----------------------------------------------------

	bool		executeNextCmd();
	void		setError();

  signals:

	void	addedMachine(AbstractVM *);
	void	changedMachine(AbstractVM *);
	void	started();
	void	executed();

  public slots:

	void	addMachine(AbstractVM *);
	void	changeMachine(int);
	void	setCode(QString);
	void    setStepMode(bool);
	void	setBreakPoint(int);
	void	removeBreakPoint(int);
	bool	execute();
	bool	executeCmd(QString);

};

//=============================================================

#endif
