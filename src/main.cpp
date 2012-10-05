/* FVM2 (F Virtual Machine v2)
 * Copyright Dmitry Podgorny <pasis.ua@gmail.com>
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of GNU Lesser General Public License.
 */

#include "main.h"

int main(int argc, char **argv) {

	QApplication	app(argc, argv);

	QCoreApplication::setOrganizationName("FTeam");
	QCoreApplication::setApplicationName("fvm2");
	QSettings	settings;

	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTranslator	translator;
	QString		locale = QLocale::system().name();
	locale = locale.mid(0, locale.indexOf('_'));
	QString		language = settings.value("language", "fvm_" + locale).toString();
	
	translator.load(":/lang/" + language);
	app.installTranslator(&translator);

	VM		machine;
	GUI		gui(&app, &machine, &translator);

	#ifdef WITH_VM_DUMMY
	vmDummy		dummy(&machine);
	machine.addMachine(&dummy);
	#endif

	vmMarkov	markov(&machine);
	machine.addMachine(&markov);

	app.exec();

}
