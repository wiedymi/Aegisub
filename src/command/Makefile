include ../../Makefile.inc

CXXFLAGS += -I../../ -I.. -I. -I../../libaegisub/include -I../include -DAEGISUB

CXXFLAGS += $(CFLAGS_WX)
PRECOMPILED_HEADER_NAME=../agi_pre.h


LIB = aegisub_command.a

SRC_OLD = \
	icon.cpp \
	keyframe.cpp \
	menu.cpp \
	medusa.cpp \
	recent.cpp \
	subtitle.cpp \
	time.cpp \
	timecode.cpp \
	tool.cpp \
	video.cpp

SRC = \
	automation.cpp \
	app.cpp \
	audio.cpp \
	edit.cpp \
	grid.cpp \
	help.cpp

SRC += \
	command.cpp

HEADER = *.h

include ../../Makefile.target




