#include "vm.h"

///////////////////////////////////////////////////////////////

VM::VM() : QThread()  {

	machines	= new AbstractVM *[MACHINE_COUNT];
	machinesCount	= 0;
	vm		= NULL;

	stepMode	= false;
	error		= false;

}

//-------------------------------------------------------------

VM::~VM() {

}

//-------------------------------------------------------------

bool VM::executeNextCmd() {

	if ( vm == NULL )
		return false;

	if ( !vm->working() ) {
		dwrite(QString("warning: machine was not started"));
		return false;
	}

	QString	cmd;
	int	index;

	mutexCode.lock();
	//-----------------------------------------------------
	index = vm->nextLine(code);
	if ( index >= 0 )		// Коли менше 0, то return false
		cmd = code[index];
	//-----------------------------------------------------
	mutexCode.unlock();

	if ( index < 0 )
		return false;

	if ( ! isStepMode() ) {
		//---------------------------------------------
		mutexBreakPoints.lock();
		bool	check = breakPoints.contains(index);
		mutexBreakPoints.unlock();
		if ( check ) {
			setStepMode(true);
			return true;
		}
		//---------------------------------------------
	}

	return vm->executeCmd(cmd);;

}

//-------------------------------------------------------------

void VM::run() {

	if ( vm == NULL ) {
		dwrite(QString("warning: machine was not set"));
		return;
	}

	error = false;
	emit started();

	if ( isStepMode() ) {

		if ( ! executeNextCmd() )
			setError();

	} // --- stepMode
	else {

		while ( vm->working() ) {

			if ( ! executeNextCmd() ) {
				setError();
				break;
			}

			if ( isStepMode() )
				break;		// якщо перейшли у покроковий режим

		}

	}

	emit executed();			// в пакетному режимі спрацює лише після виконання пакету команд

}

//-------------------------------------------------------------

void VM::addMachine(AbstractVM *vm) {

	if ( machinesCount >= MACHINE_COUNT ) {
		dwrite(QString("error: too mach machines added..."));
		return;
	}

	machines[machinesCount] = vm;
	machinesCount++;

	emit addedMachine(vm);

}

//-------------------------------------------------------------

void VM::changeMachine(int n) {

	if ( n < machinesCount ) {
		//! @todo Видаляти стару машину
		if ( vm != NULL )
			vm->terminate();
		vm = machines[n];
	}

	emit changedMachine(vm);

}

//-------------------------------------------------------------

void VM::setCode(QString code) {

	mutexCode.lock();
	//! Не знищуємо пусті рядки, щоб була відповідність з кодом у формі.
	VM::code = code.split(QChar('\n'));//, QString::SkipEmptyParts);
	mutexCode.unlock();

}

//-------------------------------------------------------------

void VM::setError() {

	error = true;
	if ( vm != NULL )
		vm->stop();

}

//-------------------------------------------------------------

bool VM::isError() {

	return error;

}

//-------------------------------------------------------------

void VM::setBreakPoint(int n) {

	if ( ! breakPoints.contains(n) ) {

		mutexBreakPoints.lock();
		breakPoints.append(n);
		mutexBreakPoints.unlock();

	}

}

//-------------------------------------------------------------

void VM::removeBreakPoint(int n) {

	mutexBreakPoints.lock();

	int	i = breakPoints.indexOf(n);
	if ( i >= 0 )
		breakPoints.removeAt(i);

	mutexBreakPoints.unlock();

}

//-------------------------------------------------------------

bool VM::execute() {

	if ( isRunning() ) {
		// Теоретично цього не повинно трапитися...
		dwrite(QString("error: machine already works"));
		return false;
	}
	else
		QThread::start();

	return true;

}

//-------------------------------------------------------------

bool VM::executeCmd(QString cmd) {

	if ( vm == NULL ) {
		dwrite(QString("warning: machine was not set"));
		return false;
	}

	if ( ! vm->working() ) {
		dwrite(QString("warning: machine was not started"));
		return false;
	}

	if ( ! vm->executeCmd(cmd) ) {
		dwrite(QString("error while execution command %1").arg(cmd));
		return false;
	}

	emit executed();
	return true;

}

//-------------------------------------------------------------
void VM::setStepMode(bool mode) {

	mutexStepMode.lock();
	stepMode = mode;
	mutexStepMode.unlock();

}

//-------------------------------------------------------------
bool VM::isStepMode() {

	mutexStepMode.lock();
	bool	mode = stepMode;
	mutexStepMode.unlock();

	return mode;

}

//=============================================================

bool VM::working() {

	if ( vm != NULL )
		return vm->working();
	else
		return false;

}

//-------------------------------------------------------------

void VM::start() {

	if ( vm != NULL )
		vm->start();

}

//-------------------------------------------------------------

void VM::stop() {

	if ( vm != NULL )
		vm->stop();

}

//-------------------------------------------------------------

QString VM::syntax() {

	if ( vm != NULL )
		return vm->syntax();
	else
		return QString("");

}

//-------------------------------------------------------------

int VM::nextLine(QStringList &code) {

	if ( vm != NULL )
		return vm->nextLine(code);
	else
		return -1;

}

//-------------------------------------------------------------

bool VM::checkSyntax(QString s) {

	if ( vm != NULL )
		return vm->checkSyntax(s);
	else
		return true;

}

///////////////////////////////////////////////////////////////
