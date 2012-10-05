#include "vm_markov.h"

///////////////////////////////////////////////////////////////

vmMarkov::vmMarkov(VM *machine) {

	machineName = tr("Markov algorithm");

	vmMarkov::machine = machine;

	win = NULL;
	vbox = NULL;
	log = NULL;

	execConnected = false;
	startConnected = false;

}

//-------------------------------------------------------------

vmMarkov::~vmMarkov() {

}

//-------------------------------------------------------------

void vmMarkov::start() {

	if ( machineWorking )
		return;

	if ( ! execConnected ) {
		connect(machine, SIGNAL(executed()), this, SLOT(stopExecuting()));
		execConnected = true;
	}

	if ( ! startConnected ) {
		connect(machine, SIGNAL(started()), this, SLOT(startExecuting()));
		startConnected = true;
	}

	machineWorking = true;

}

//-------------------------------------------------------------

void vmMarkov::stop() {

	if ( ! machineWorking )
		return;

	machineWorking = false;

}

//-------------------------------------------------------------

void vmMarkov::terminate() {

	if ( execConnected ) {
		disconnect(machine, SIGNAL(executed()), this, SLOT(stopExecuting()));
		execConnected = false;
	}

	if ( startConnected ) {
		disconnect(machine, SIGNAL(started()), this, SLOT(startExecuting()));
		startConnected = false;
	}

	machineWorking = false;
	memory = QString();
	log->clear();

	if ( win != NULL )
		win->hide();

}

//-------------------------------------------------------------

QWidget *vmMarkov::initGui() {

	if ( win != NULL )
		return win;

	win = new QWidget();
	vbox = new QVBoxLayout();
	log = new StringEdit(win);

	connect(log, SIGNAL(changed(QString)), this, SLOT(setMemory(QString)));

	win->setLayout(vbox);
	vbox->addWidget(log);
	log->setReadOnly(false);

	return win;

}

//-------------------------------------------------------------

QString vmMarkov::syntax() {

	return QString("->|\\.");

}

//-------------------------------------------------------------

bool vmMarkov::executeCmd(QString cmd) {

	if ( ! checkSyntax(cmd) )
		return false;

	if ( cmd == "" )
		return true;

	int	index = cmd.indexOf("->");
	if ( index < 0 )
		return false;

	QString	s1 = deleteSymb( cmd.mid( 0, index ), 'E' );
	QString	s2 = deleteSymb( cmd.mid( index + 2, cmd.size() - index - 2 ), 'E' );
	QString	lastMemory = memory;
	bool	last = false;

	if ( s2[0] == '.' ) {
		last = true;
		s2.remove(0, 1);
	}

	index = memory.indexOf(s1);
	if ( index < 0 )
		return false;
	memory.remove(index, s1.size());
	memory.insert(index, s2);

	if ( last )
		stop();
	#ifndef WITHOUT_INFINITE_LOOP_CHECK
	else if ( memory == lastMemory ) {
		stop(); //! @todo в покроковий режим
		dwrite(QString().fromUtf8("Помічено зациклення, виконання припинено"));
	}
	#endif

	return true;

}

//-------------------------------------------------------------

bool vmMarkov::checkSyntax(QString cmd) {

	if ( cmd == "" )
		return true;

	int	index = cmd.indexOf(QString("->"));
	int	dotindex = cmd.indexOf(QString("."));

	if ( ( index < 0 ) || ( cmd.indexOf(QString("->"), index + 1) >= 0 ) )
		return false;

	if ( ( dotindex >= 0 ) && ( ( dotindex != index + 2 ) || ( cmd.indexOf(QString("."), dotindex + 1) >= 0 ) ) )
		return false;

	return true;

}

//-------------------------------------------------------------

int vmMarkov::nextLine(QStringList &cmds) {

	int	count = cmds.count();

	for ( int i = 0; i < count; i++ ) {
		QString	s = cmds[i];
		if ( s == "" )
			continue;
		if ( ! checkSyntax(s) )
			return -1;
		if ( memory.indexOf( deleteSymb( s.mid(0, s.indexOf(QString("->"))), 'E' ) ) >= 0 )
			return i;
	}

	return -1;

}


//-------------------------------------------------------------

void vmMarkov::refresh() {

	if ( log == NULL )
		return;

	log->setText(memory);

}

//-------------------------------------------------------------

void vmMarkov::setMemory(QString s) {

	memory = s;

}

//-------------------------------------------------------------

void vmMarkov::startExecuting() {

	log->setReadOnly(true);

}

//-------------------------------------------------------------

void vmMarkov::stopExecuting() {

	log->setReadOnly(false);
	refresh();

}

///////////////////////////////////////////////////////////////
