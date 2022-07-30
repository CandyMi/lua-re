.PHONY : build pcre pcre2

default :
	@echo "======================================="
	@echo "Please use 'make build' command to build it.."
	@echo "Please use 'make pcre' command to build it.."
	@echo "Please use 'make pcre2' command to build it.."
	@echo "======================================="

INCLUDES += -I../ -I../../ -I../../../ -I../../src -I/usr/local/include
LIBS = -L../ -L../../ -L../../../ -L/usr/local/lib

pcre:
	@$(CC) -o lre.so lre.c $(CFLAGS) $(INCLUDES) $(LIBS) -lcore -lpcreposix
	@mv lre.so re.lua ../

pcre2:
	@$(CC) -o lre.so lre.c $(CFLAGS) $(INCLUDES) $(LIBS) -lcore -lpcre2-posix
	@mv lre.so re.lua ../

build:
	@$(CC) -o lre.so lre.c $(CFLAGS) $(INCLUDES) $(LIBS) -lcore
	@mv lre.so re.lua ../
