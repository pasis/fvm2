#ifdef WITH_VM_DUMMY

///////////////////////////////////////////////////////////////

#include "vm_dummy.h"

//-------------------------------------------------------------

vmDummy::vmDummy(VM *machine) : AbstractVM() {

	vmDummy::machine = machine;

	machineName = tr("Dummy Machine");

	win = NULL;
	log = NULL;

	counter = 0;
	execConnected = false;

}

//-------------------------------------------------------------

vmDummy::~vmDummy() {

}

//-------------------------------------------------------------

void vmDummy::start() {

	if ( machineWorking )
		return;

	if ( ! execConnected ) {
		connect(machine, SIGNAL(executed()), this, SLOT(refresh()));
		execConnected = true;
	}

	counter = 0;
	machineWorking = true;

}

//-------------------------------------------------------------

void vmDummy::stop() {

	if ( ! machineWorking )
		return;

	machineWorking = false;

}

//-------------------------------------------------------------

void vmDummy::terminate() {

	if ( execConnected ) {
		disconnect(machine, SIGNAL(executed()), this, SLOT(refresh()));
		execConnected = false;
	}

	machineWorking = false;

	if ( win != NULL )
		win->hide();

}

//-------------------------------------------------------------

QWidget *vmDummy::initGui() {

	if ( win != NULL )
		return win;

	win = new QWidget();
	vbox = new QVBoxLayout(win);
	log = new QLineEdit(win);
	vbox->addWidget(log);

	return win;

}

//-------------------------------------------------------------

QString vmDummy::syntax() {

	return QString("*");

}

//-------------------------------------------------------------

int vmDummy::nextLine(QStringList &code) {

	if ( counter >= code.size() )
		return -1;
	else
		return counter;

}

//-------------------------------------------------------------

bool vmDummy::checkSyntax(QString) {

	return true;

}

//-------------------------------------------------------------

bool vmDummy::executeCmd(QString cmd) {

	counter++;
	dwrite(QString("executing command: %1").arg(cmd));
	return true;

}

//-------------------------------------------------------------

void vmDummy::refresh() {

	if ( log == NULL )
		return;

	log->setText(QString("Executed #%1").arg(QString().number(counter - 1)));

}

///////////////////////////////////////////////////////////////

#endif
