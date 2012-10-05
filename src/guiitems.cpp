#include "guiitems.h"

///////////////////////////////////////////////////////////////

CmdEdit::CmdEdit(bool acceptEmpty, bool keepText): QLineEdit() {

	CmdEdit::acceptEmpty = acceptEmpty;
	CmdEdit::keepText = keepText;
	init();

}

//=============================================================

CmdEdit::CmdEdit(QWidget *parent, bool acceptEmpty, bool keepText): QLineEdit(parent) {

	CmdEdit::acceptEmpty = acceptEmpty;
	CmdEdit::keepText = keepText;
	init();

}

//=============================================================

CmdEdit::~CmdEdit() {

}

//=============================================================

void CmdEdit::init() {

	listPointer = -1;

}

//=============================================================

void CmdEdit::keyPressEvent(QKeyEvent *event) {

	int	key = event->key();

	switch ( key ) {

	case Qt::Key_Return:
		if ( ( ! text().isEmpty() ) || acceptEmpty ) {
			emit input( text() );
			// history
			while ( list.size() >= MAX_CMDEDIT_HISTORY )
				list.removeFirst();
			list.append(text());
			listPointer = -1;
			// end history
			if ( !keepText )
				clear();
		}
		break;

	case Qt::Key_Up:
		if ( listPointer < 0 ) {
			listPointer = list.size();
			tempString = text();
		}
		if ( listPointer  > 0 )
			setText(list[--listPointer]);
		break;

	case Qt::Key_Down:
		if ( listPointer >= 0  && listPointer < list.size() ) {
			listPointer++;
			if ( listPointer == list.size() ) {
				setText(tempString);
				listPointer = -1;
			} else
				setText(list[listPointer]);
		}
		break;

	default:
		QString	temp = text();
		QLineEdit::keyPressEvent(event);
		if ( temp != text() )
			listPointer = -1;

	}

}

///////////////////////////////////////////////////////////////

StringEdit::StringEdit() : QLineEdit() {

}

StringEdit::StringEdit(QWidget *parent) : QLineEdit(parent) {

}

StringEdit::~StringEdit() {

}

void StringEdit::keyPressEvent(QKeyEvent *event) {

	QString	temp = text();
	QLineEdit::keyPressEvent(event);
	if ( temp != text() )
		emit changed(text());

}

///////////////////////////////////////////////////////////////

CodeEdit::CodeEdit(): QTextEdit() {

}

//=============================================================

CodeEdit::CodeEdit(QWidget *parent): QTextEdit(parent) {

}

//=============================================================

CodeEdit::~CodeEdit() {

}

//=============================================================

int CodeEdit::currentLine() {

// 	QTextBlock	b, cb;
// 	int		line = 0;

// 	cb = textCursor().block();
// 	for(b = document()->begin(); b!=document()->end(); b = b.next()){
// 		if( b == cb ) return line;
// 		line++;
// 	}

// 	return line;

	return textCursor().blockNumber();

}

///////////////////////////////////////////////////////////////
