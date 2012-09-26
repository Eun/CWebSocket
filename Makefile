BIN = a.out
CC = gcc
CPLUS = g++
LINK = g++
OBJC = $(shell find ./src/ -name "*.c" | sed 's/\.c/\.o/g' | sed 's/\.\/src\//\.\/obj\//g')
OBJCPP = $(shell find ./src/ -name "*.cpp" | sed 's/\.cpp/\.opp/g' | sed 's/\.\/src\//\.\/obj\//g')
OBJ = $(OBJC) $(OBJCPP)

CFLAGS = -Wall -I./inc/
CPPFLAGS = -Wall -I./inc/
LDFLAGS = -Wall
LIBS = -lssl -lcrypto


# ========================================================================= #

SILENT = 2>/dev/null || true
pathpat=(.*/[^/]*)+:[0-9]+
pathpatlink=(.*/[^/]*)+:
ccred=$(shell echo -e "\033[0;31m")
ccyellow=$(shell echo -e "\033[0;33m")
ccbold=$(shell echo -e "\033[1m")
ccend=$(shell echo -e "\033[0m")
LANG=en_US.UTF-8

all: dirs $(BIN)



dirs:
	-@mkdir bin $(SILENT)
	-@mkdir obj $(SILENT)
	-@mkdir -p $(shell find ./src/ -type d | sed 's|./src/|./obj/|g')
	-@rm -rf output
	@mkfifo output
	@echo -e "\033[1m\033[4m1. COMPILING\033[0m"

$(BIN): $(OBJ)
	@echo -e "\033[1m\033[4m2. LINKING\033[0m"
	@sed -E -e "/undefined[: ]/ s%$(pathpatlink)%$(ccred)&$(ccend)%g" -e "s/obj\///" -e "s/.\///" < output &
	@$(LINK) $(LDFLAGS) $(OBJ) $(LIBS) -o ./bin/$@ &> output
	-@rm -rf output
	@echo -e "\033[1m\033[4m3. DONE\033[0m"	

	
./obj/%.o: ./src/%.c
	@echo -e "\033[1m>> $<\033[0m" | sed 's/src\///g'
	@sed -E -e "/[Ee]rror[: ]/ s%$(pathpat)%$(ccred)&$(ccend)%g" -e "/[Ww]arning[: ]/ s%$(pathpat)%$(ccyellow)&$(ccend)%g" -e "s/src\///" < output &
	@$(CC) $(CFLAGS) -o $@ -c $< &> output
	
./obj/%.opp: ./src/%.cpp
	@echo -e "\033[1m>> $<\033[0m" | sed 's/src\///g'
	@sed -E -e "/[Ee]rror[: ]/ s%$(pathpat)%$(ccred)&$(ccend)%g" -e "/[Ww]arning[: ]/ s%$(pathpat)%$(ccyellow)&$(ccend)%g" -e "s/src\///" < output &
	@$(CPLUS) $(CPPFLAGS) -o $@ -c $< &> output
	

	
clean:
	@echo -e "\033[1m\033[4mCLEANING\033[0m"
	rm -rf bin
	rm -rf obj
	-@rm -rf output
