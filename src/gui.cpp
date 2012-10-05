#include "gui.h"

///////////////////////////////////////////////////////////////

CodeHighlighter::CodeHighlighter(QTextEdit *code, VM *machine) : QSyntaxHighlighter(code) {

	CodeHighlighter::code = (CodeEdit *) code;
	CodeHighlighter::machine = machine;

}

//=============================================================

CodeHighlighter::~CodeHighlighter() {

}

//=============================================================

void CodeHighlighter::highlightBlock(const QString &text) {

	emit changed();

	if ( ! machine->checkSyntax(text) ) {
		QTextCharFormat	formatError;
		formatError.setForeground(Qt::darkRed);
		setFormat(0, text.size(), formatError);
		return;
	}

	int		index,
			length;

	QTextCharFormat	formatNumbers;
	QTextCharFormat	formatVM;
	QTextCharFormat	formatNormal;
	QString		pNumbers = "\\b(\\d+|[0-1]+b|0x[0-9a-f]+)\\b";
	QString		pSyntaxVM = machine->syntax();
	QRegExp		numbers(pNumbers);
	QRegExp		syntaxVM(pSyntaxVM);

	formatNumbers.setForeground(Qt::darkGreen);
	formatVM.setFontWeight(QFont::Bold);
	formatVM.setForeground(Qt::darkBlue);
	formatNormal.setFontWeight(QFont::Normal);

	setFormat(0, text.size(), formatNormal);

	index = text.indexOf(numbers);
	while ( index >= 0 ) {
		length = numbers.matchedLength();
		setFormat(index, length, formatNumbers);
		index = text.indexOf(numbers, index + length);
	}

	index = text.indexOf(syntaxVM);
	while ( index >= 0 ) {
		length = syntaxVM.matchedLength();
		if ( length == 0 )
			break;
		setFormat(index, length, formatVM);
		index = text.indexOf(syntaxVM, index + length);
	}

}

///////////////////////////////////////////////////////////////

GUI::GUI(QApplication *app, VM *machine, QTranslator *translator) : QMainWindow() {

	GUI::app = app;
	GUI::machine = machine;

	currentVMWidget = NULL;
	aboutMessage = NULL;

	readOnly = false;
	running = false;
	isMachineSet = false;

	//-----------------------------------------------------

	GUI::translator = translator;
	QDir	langDir(":/lang");
	languages = langDir.entryList(QStringList("fvm_*.qm"));
	
	//-----------------------------------------------------

	connect(machine, SIGNAL(addedMachine(AbstractVM *)), this, SLOT(addMachine(AbstractVM *)));
	connect(machine, SIGNAL(changedMachine(AbstractVM *)), this, SLOT(changeMachine(AbstractVM *)));
	connect(machine, SIGNAL(executed()), this, SLOT(afterExecute()));

	//-----------------------------------------------------

	openDialog	= new QFileDialog(this);
	openDialog->setViewMode(QFileDialog::Detail);
	openDialog->setAcceptMode(QFileDialog::AcceptOpen);
	openDialog->setFileMode(QFileDialog::ExistingFile);

	saveDialog	= new QFileDialog(this);
	saveDialog->setViewMode(QFileDialog::List);
	saveDialog->setAcceptMode(QFileDialog::AcceptSave);
	saveDialog->setFileMode(QFileDialog::AnyFile);

	connect(openDialog, SIGNAL(accepted()), this, SLOT(openFromFile()));
	connect(saveDialog, SIGNAL(accepted()), this, SLOT(saveToFile()));

	help = new HelpBrowser();

	//-----------------------------------------------------

	splitVertical = new QSplitter(Qt::Vertical, this);
	setCentralWidget(splitVertical);

	splitHorizontal = new QSplitter(Qt::Horizontal, splitVertical);

	//-----------------------------------------------------

	win1 = new QWidget(splitHorizontal);
	win1Layout = new QVBoxLayout(win1);
	win1Layout->setSpacing(2);

	code = new CodeEdit(win1);
	//-------------------------
	//code->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	//code->setLineWrapMode (QTextEdit::NoWrap);
	code->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	code->setLineWrapColumnOrWidth(65535);
	code->setLineWrapMode(QTextEdit::FixedPixelWidth);
	//-------------------------
	win1Layout->addWidget(code);

	highlight = new CodeHighlighter(code, machine);
	highlightedLine = -1;
	connect(highlight, SIGNAL(changed()), this, SLOT(light()));
	connect(machine, SIGNAL(executed()), this, SLOT(light()));

	formatLight.setBackground( QBrush( QColor(Qt::lightGray) ) );
	formatNormal.clearBackground();
	formatBreakPoint.setBackground( QBrush( QColor(0xFF, 0x80, 0x80) ) );

	cmd = new CmdEdit( win1, false /*acceptEmpty*/ );
	win1Layout->addWidget(cmd);
	connect(cmd, SIGNAL(input(QString)), machine, SLOT(executeCmd(QString)));

	//-----------------------------------------------------

	win2 = new QWidget(splitHorizontal);
	win2Layout = new QVBoxLayout(win2);

	vmList = new QListWidget(win2);
	win2Layout->addWidget(vmList);
	connect(vmList, SIGNAL(activated(QModelIndex)), this, SLOT(selectMachine(QModelIndex)));

	groupMode = new QGroupBox(win2);
	win2Layout->addWidget(groupMode);
	groupModeLayout = new QVBoxLayout(groupMode);
	groupMode->setTitle(tr("Modes"));

	modeStep = new QCheckBox(groupMode);
	modeStep->setText(tr("Step mode"));
	modeReadOnly = new QCheckBox(groupMode);
	modeReadOnly->setText(tr("Read only"));
	groupModeLayout->addWidget(modeStep);
	groupModeLayout->addWidget(modeReadOnly);
	connect(modeStep, SIGNAL(clicked(bool)), this, SLOT(changeStepMode()));
	connect(modeReadOnly, SIGNAL(clicked(bool)), this, SLOT(changeReadOnly()));

	buttonRun = new QPushButton(win2);
	win2Layout->addWidget(buttonRun);
	buttonRun->setText(tr("Execute / Step"));
	connect(buttonRun, SIGNAL(clicked(bool)), this, SLOT(execute()));

	buttonBreakPount = new QPushButton(win2);
	win2Layout->addWidget(buttonBreakPount);
	buttonBreakPount->setText(tr("Breakpoint"));
	connect(buttonBreakPount, SIGNAL(clicked(bool)), this, SLOT(setBreakPoint()));

	//-----------------------------------------------------

	win3 = new QWidget(splitVertical);
	win3Layout = new QVBoxLayout(win3);

	groupVM = new QGroupBox(win3);
	win3Layout->addWidget(groupVM);
	groupVMLayout = new QVBoxLayout(groupVM);
	groupVMLayout->setSpacing(0);
	groupVM->setVisible(false);

	//-----------------------------------------------------

	menuBar = new QMenuBar();
	menuFile = new QMenu();
	menuEdit = new QMenu();
	menuExec = new QMenu();
	menuLang = new QMenu();
	menuAbout = new QMenu();

	menuFile->setTitle(tr("&File"));
	menuFile->addAction(tr("&Open"), openDialog, SLOT(show()));
	menuFile->addAction(tr("&Save"), saveDialog, SLOT(show()));
	menuFile->addSeparator();
	menuFile->addAction(tr("E&xit"), this, SLOT(Quit()));

	menuEdit->setTitle(tr("&Edit"));
	menuEditReadOnly = menuEdit->addAction(tr("Read only"), this, SLOT(changeReadOnly()));
	menuEditReadOnly->setCheckable(true);

	menuExec->setTitle(tr("Executing"));
	menuExecStepMode = menuExec->addAction(tr("Step mode"), this, SLOT(changeStepMode()));
	menuExecStepMode->setCheckable(true);
	menuExec->addAction(tr("Command history"));//! @TODO: this, SLOT(setMenuHistoryUp()));
	menuExecRun = menuExec->addAction(tr("Execute / Step"), this, SLOT(execute()));

	menuLang->setTitle(tr("&Language"));
	for (int i = 0; i < languages.size(); i++) {
		QAction *id = menuLang->addAction(languages[i], this, SLOT(selectLanguage()));
		id->setData(i);
	}

	menuAbout->setTitle(tr("&About"));
	menuAbout->addAction(tr("&Help"), help, SLOT(show()));
	menuAbout->addSeparator();
	menuAbout->addAction(tr("About FVM"), this, SLOT(about()));
	menuAbout->addAction(tr("About Qt"), app, SLOT(aboutQt()));

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuEdit);
	menuBar->addMenu(menuExec);
	menuBar->addMenu(menuLang);
	menuBar->addMenu(menuAbout);

	setMenuBar(menuBar);

	//-----------------------------------------------------

	show();

}

//=============================================================

GUI::~GUI() {

}

//=============================================================

void GUI::closeEvent(QCloseEvent *event) {

	if ( event->type() == QEvent::Close )
		app->quit();
	else
		QMainWindow::closeEvent(event);

}

//=============================================================

void GUI::keyPressEvent(QKeyEvent *event) {

	int	key = event->key();

	switch ( key ) {

	case Qt::Key_F5:
		setBreakPoint();
		break;

	case Qt::Key_F6:
		setBreakPoint();
		if ( machine->isStepMode() )
			changeStepMode();
		execute();
		break;

	case Qt::Key_F7:
		if ( ! machine->isStepMode() )
			changeStepMode();
		execute();
		break;

	case Qt::Key_F8:
		execute();
		break;

	case  Qt::Key_F9:
		if ( machine->isStepMode() )
			changeStepMode();
		execute();
		break;

	default:
		QMainWindow::keyPressEvent(event);

	};

}

//=============================================================

void GUI::Quit() {

	//! Чи може просто close()? ^___^
	app->quit();

}

//=============================================================

void GUI::about() {

	if ( aboutMessage == NULL ) {

		aboutMessage	= new QMessageBox(this);
		aboutMessage->setText(tr("F Virtual Machine") + QString("\n\n") + tr("Version") + QString(": ") + QString::fromUtf8(VERSION) + QString("\n") + tr("Author: Dmitry Podgorny <pasis.ua@gmail.com>") + QString("\n") + tr("License: GPL2 or later"));
		aboutMessage->setWindowTitle(tr("About FVM"));
		aboutMessage->setIconPixmap(QPixmap("img/about.png"));

	}

	aboutMessage->show();

}

//=============================================================

void GUI::openFromFile() {

	disconnect(highlight, SIGNAL(changed()), this, SLOT(light()));

	QString		name = openDialog->selectedFiles()[0];
	char		data[1024];

	dwrite(QString("opening file %1").arg(name));

	//-----------------------------------------------------
	// можливо знищити звязок з підсвіткою а потім поновити...

	std::ifstream	f(name.toLocal8Bit().data());

	cleanBreakPoints();
	code->clear();
	while ( ! f.eof() ) {
		f.getline(data, 1024);
		code->append(QString().fromUtf8( (char *) data ));
	}

	f.close();

	code->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);

	//-----------------------------------------------------

	connect(highlight, SIGNAL(changed()), this, SLOT(light()));

	light();			// Підсвічування синтаксису завантаженого коду

}

//=============================================================

void GUI::saveToFile() {

	QString		name = saveDialog->selectedFiles()[0];

	dwrite(QString("saving to file %1").arg(name));

	std::ofstream	f(name.toLocal8Bit().data());
	f << code->toPlainText().toUtf8().data();
	f.close();

}

//=============================================================

void GUI::addMachine(AbstractVM *vm) {

	vmList->addItem(vm->name());

}

//=============================================================

void GUI::changeMachine(AbstractVM *vm) {

	groupVM->setTitle(vm->name());
	setWindowTitle(vm->name());

	if ( currentVMWidget != NULL )
		groupVMLayout->removeWidget(currentVMWidget);

	currentVMWidget = vm->initGui();
	groupVMLayout->addWidget(currentVMWidget);
	currentVMWidget->show();

	groupVM->setVisible(true);

	isMachineSet = true;

	//-----------------------------------------------------
	// синтаксис з новою машиною змінюється...
	disconnect(highlight, SIGNAL(changed()), this, SLOT(light()));

	int		count = code->document()->blockCount();
	QTextCursor	cursor = code->textCursor();
	cursor.movePosition(QTextCursor::Start);

	for(int i = 0; i < count; i++) {
		cursor.setBlockFormat(formatNormal);
		cursor.movePosition(QTextCursor::Down);
	}

	connect(highlight, SIGNAL(changed()), this, SLOT(light()));
	//-----------------------------------------------------

	light();

}

//=============================================================

void GUI::selectMachine(QModelIndex index) {

	machine->changeMachine(index.row());

}

//=============================================================

void GUI::selectLanguage() {

	QAction		*lang = static_cast<QAction *>(sender());
	int		langId = lang->data().toInt();
	QSettings	settings;

	settings.setValue("language", languages[langId]);
	translator->load(":/lang/" + languages[langId]);
	//! @TODO: пересоздать GUI

}

//=============================================================

void GUI::cleanBreakPoints()  {

	while ( ! breakPoints.isEmpty() ) {
		int	n = breakPoints[0];
		machine->removeBreakPoint(n);
		breakPoints.removeFirst();
// 		cleanLight(n);
	}

}

//=============================================================

void GUI::setBreakPoint() {

	mutexBreakPoints.lock();

	int	n = code->currentLine();
	int	i = breakPoints.indexOf(n);

	if ( i < 0 ) {
		breakPoints.append(n);
		machine->setBreakPoint(n);
	}
	else {
		breakPoints.removeAt(i);
		machine->removeBreakPoint(n);
	}

	cleanLight(n);

	mutexBreakPoints.unlock();

}

//=============================================================

void GUI::execute() {

//! Допрацювати

	if ( ! isMachineSet ) {
		QMessageBox::information( 0, tr("Notification"), tr("Select a machine"), QMessageBox::Ok, QMessageBox::NoButton );
		return;
	}

	if ( running ) {
		machine->stop();
		return;
	}

	running = true;
	menuExecRun->setText(tr("Stop"));
	buttonRun->setText(tr("Stop"));

	if ( ! machine->working() ) {
		dwrite(QString("starting machine..."));
		machine->start();
	}

	machine->setCode(code->toPlainText());
	machine->execute();

}

//=============================================================

void GUI::afterExecute() {

	running = false;
	menuExecRun->setText(tr("Execute / Step"));
	buttonRun->setText(tr("Execute / Step"));

	if ( machine->isError() ) {
		QMessageBox::warning( 0, tr("Notification"), tr("Runtime error!"), QMessageBox::Ok, QMessageBox::NoButton );
	}
	else if ( ! machine->working() ) {
		QMessageBox::information( 0, tr("Notification"), tr("Execution completed"), QMessageBox::Ok, QMessageBox::NoButton );
	}

	if ( machine->isStepMode() && ( ! modeStep->isChecked() ) ) {
		modeStep->setChecked(true);
		menuExecStepMode->setChecked(true);
	}

}

//=============================================================

void GUI::cleanLight(int line) {

	int	currentline = code->currentLine();
	int	count = code->document()->blockCount();

	QTextCursor			cursor = code->textCursor();
	QTextCursor::MoveOperation	operation;

	if ( line >= 0 ) {

		cursor.movePosition(QTextCursor::StartOfLine);

		if ( currentline > line )
			operation = QTextCursor::Up;
		else
			operation = QTextCursor::Down;

		//---------------------------------------------
		cursor.movePosition(operation, QTextCursor::MoveAnchor, abs(currentline - line));
		if ( count > line ) {
			if ( breakPoints.contains(line) )
				cursor.setBlockFormat(formatBreakPoint);
			else
				cursor.setBlockFormat(formatNormal);
		}
		//---------------------------------------------

		if ( count > line + 1 ) {
			cursor.movePosition(QTextCursor::Down);
			if ( breakPoints.contains(line + 1) )
				cursor.setBlockFormat(formatBreakPoint);
			else
				cursor.setBlockFormat(formatNormal);
		}
		//---------------------------------------------
		if ( line != 0 && count > line - 1 ) {
			if ( count == line + 1 )
				cursor.movePosition(QTextCursor::Up,  QTextCursor::MoveAnchor, 1);
			else if ( count != line )
				cursor.movePosition(QTextCursor::Up,  QTextCursor::MoveAnchor, 2);

			if ( breakPoints.contains(line - 1) )
				cursor.setBlockFormat(formatBreakPoint);
			else
				cursor.setBlockFormat(formatNormal);
		}
		//---------------------------------------------

	}

}

void GUI::light() {

	disconnect(highlight, SIGNAL(changed()), this, SLOT(light()));

	QStringList	codeList = code->toPlainText().split(QChar('\n'));;
	int		line = machine->nextLine(codeList);
	int		currentline = code->currentLine();

	QTextCursor			cursor = code->textCursor();;
	QTextCursor::MoveOperation	operation;

	//-----------------------------------------------------

	cleanLight(highlightedLine);
	for (int i = 0; i < breakPoints.size(); i++)
		cleanLight(breakPoints[i]);

	//-----------------------------------------------------

	highlightedLine = line;

	if ( line >= 0 ) {

		if ( currentline > line )
			operation = QTextCursor::Up;
		else
			operation = QTextCursor::Down;

		cursor.movePosition(operation, QTextCursor::MoveAnchor, abs(currentline - line));
		cursor.setBlockFormat(formatLight);
	}

	//-----------------------------------------------------

	connect(highlight, SIGNAL(changed()), this, SLOT(light()));

}

//=============================================================

void GUI::changeStepMode() {

	bool	mode = ! machine->isStepMode();

	machine->setStepMode(mode);
	modeStep->setChecked(mode);
	menuExecStepMode->setChecked(mode);

}

//=============================================================

void GUI::changeReadOnly() {

	bool	mode = ! readOnly;

	readOnly = mode;

	modeReadOnly->setChecked(mode);
	menuEditReadOnly->setChecked(mode);

	code->setReadOnly(mode);

}

///////////////////////////////////////////////////////////////
