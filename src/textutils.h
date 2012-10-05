#ifndef TEXTUTILS_H
#define TEXTUTILS_H

#include "config.h"
#include "debug.h"

///////////////////////////////////////////////////////////////

	QString		deleteSymb(QString, char);


///////////////////////////////////////////////////////////////

template <class T>
struct FListItem {

	int		number;
	T		value;
	FListItem<T>	*next;

};

template <class T>
class FList {

  public:

	FList( T defValue );
	~FList();

	T	at(int);

	T	operator[] (int n) { return at(n); }

  private:

	T		defValue;
	FListItem<T>	*list;

	T		mas[1024]; //!!! for testing!

};

///////////////////////////////////////////////////////////////

#endif
