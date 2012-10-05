#ifndef DEBUG_H
#define DEBUG_H

//=============================================================

#ifdef DEBUG

  #include <QString>
  #include <iostream>

#endif

//=============================================================

#ifdef DEBUG
	#define dwrite(s) std::cout << __FILE__ << ":" << __LINE__ << ": " << s.toLocal8Bit().data() << std::endl
#else
	#define dwrite(s)
#endif

//=============================================================

#endif
