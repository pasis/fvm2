#ifndef HELP_H
#define HELP_H

#include "config.h"
#include "debug.h"
#include "guiitems.h"

class HelpBrowser: public QWidget {

	Q_OBJECT

  public:

	HelpBrowser();
	~HelpBrowser();

  private:

	QVBoxLayout	*vbox;
	QHBoxLayout	*hbox;
	QPushButton	*buttonForward;
	QPushButton	*buttonBack;
	CmdEdit		*address;
	QTextBrowser	*browser;

	void	keyPressEvent(QKeyEvent *);

  public slots:

	void	loadUrl(QString);
	void	changeAddr(QUrl);

};

#endif
