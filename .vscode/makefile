#source file search paths
VPATH = ./src ../PathFinder/src

	# compiler include search paths
INCS = -I./include \
	-I../windex/include \
	-I../raven-set \

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs \

	#-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static -lComctl32

# folders
ODIR = ./.vscode/obj

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_a2t = \
	$(ODIR)/main.o \
	$(ODIR)/cGUI.o \
	$(ODIR)/allocator.o \
	$(ODIR)/cHungarian.o \
	$(ODIR)/Janusz.o \
	$(ODIR)/unitTest.o \
	$(ODIR)/input.o \
	$(ODIR)/cGraph.o \
	$(ODIR)/GraphTheory.o 


OBJ_console = \
	$(ODIR)/console.o \
	$(ODIR)/allocator.o \
	$(ODIR)/cHungarian.o \
	$(ODIR)/Janusz.o \
	$(ODIR)/unitTest.o \
	$(ODIR)/input.o \
	$(ODIR)/cGraph.o \
	$(ODIR)/GraphTheory.o \

OBJ_starter = \
	$(ODIR)/A2TStarter.o \

### COMPILE

$(ODIR)/%.o: %.cpp allocator.h cGUI.h cAgent.h cTask.h
	g++  $(CCflags) \
	-c -o $@ $< $(INCS)

a2t: $(OBJ_a2t)
	g++ $(OPT) \
	-o bin/Agents2Tasks.exe $^  \
	$(static_link_flags) $(LIBS)

console: $(OBJ_console)
	g++ $(OPT) \
	-o bin/Agents2TasksConsole.exe $^  \
	$(static_link_flags) $(LIBS)

starter: $(OBJ_starter)
	g++ $(OPT) \
	-o bin/Agents2TasksStarter.exe $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

rebuild: clean a2t console

