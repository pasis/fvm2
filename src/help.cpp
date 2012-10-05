#include "help.h"

HelpBrowser::HelpBrowser() {

	browser		= new QTextBrowser(this);
	buttonBack	= new QPushButton(this);
	buttonForward	= new QPushButton(this);
	address		= new CmdEdit(this, true, true);

	vbox = new QVBoxLayout();
	hbox = new QHBoxLayout();
	hbox->addWidget(buttonBack);
	hbox->addWidget(buttonForward);
	hbox->addWidget(address);
	vbox->addLayout(hbox);
	vbox->addWidget(browser);
	setLayout(vbox);

	browser->setReadOnly(true);

	buttonBack->setText(QString("<"));
	buttonBack->setFixedWidth(25);

	buttonForward->setText(QString(">"));
	buttonForward->setFixedWidth(25);

	setWindowTitle(QString().fromUtf8("Допомога"));

	connect(buttonBack, SIGNAL(clicked(bool)), browser, SLOT(backward()));
	connect(buttonForward, SIGNAL(clicked(bool)), browser, SLOT(forward()));
	connect(address, SIGNAL(input(QString)), this, SLOT(loadUrl(QString)));
	connect(browser, SIGNAL(sourceChanged(QUrl)), this, SLOT(changeAddr(QUrl)));

	browser->setSource(QUrl(QDir::currentPath() + QString("/docs/index.html")));

	resize(640, 480);

}

HelpBrowser::~HelpBrowser() {

}

void HelpBrowser::keyPressEvent(QKeyEvent *event) {

	int	key = event->key();

	switch ( key ) {

	case Qt::Key_F5:
		browser->reload();
		break;

	default:
		QWidget::keyPressEvent(event);

	};

}

void HelpBrowser::loadUrl(QString url) {

	browser->setSource(QUrl(url));

}

void HelpBrowser::changeAddr(QUrl url) {

	address->setText(url.toString());

}
