#ifndef TCOLOR_HPP
#define TCOLOR_HPP

// at least for solarized. hopefully looks sane on other screens
#ifndef NOCOLOR

#define BOLD "\033[1m"
#define RED "\033[31m"
#define MAGENTA "\033[35m"
#define DEFAULT "\033[39m"
#define RESET "\033[0m"

#else

#define BOLD ""
#define RED ""
#define MAGENTA ""
#define DEFAULT ""
#define RESET ""

#endif

#endif
