#include "textutils.h"

///////////////////////////////////////////////////////////////

QString deleteSymb(QString s, char c) {

	int	i = 0;

	while ( i < s.size() ) {

		if ( s[i] == c )
			s.remove(i, 1);
		else
			i++;

	}

	return s;

}

///////////////////////////////////////////////////////////////

template <class T>
FList<T>::FList( T defValue ) {

	FList<T>::defValue = defValue;
	list = NULL;

}

template <class T>
FList<T>::~FList() {

}

template <class T>
T FList<T>::at(int n) {

/*	if ( list == NULL )
		return defValue;

	while () {
		=== выбрать элемент ===
		if ( list->number == n )
			return list->value;

	}*/

	return mas[n]; //!!! for testing!

}

///////////////////////////////////////////////////////////////
