include ../../Makefile.inc

CXXFLAGS += -I../../ -I.. -I. -I../../libaegisub/include -I../include -DAEGISUB

CXXFLAGS += $(CFLAGS_WX)
PRECOMPILED_HEADER_NAME=../agi_pre.h


LIB = aegisub_command.a

SRC_OLD = \
	medusa.cpp \
	video.cpp

SRC = \
	automation.cpp \
	app.cpp \
	audio.cpp \
	edit.cpp \
	grid.cpp \
	recent.cpp \
	keyframe.cpp \
	subtitle.cpp \
	time.cpp \
	timecode.cpp \
	menu.cpp \
	tool.cpp \
	help.cpp

SRC += \
	icon.cpp \
	command.cpp

HEADER = *.h

include ../../Makefile.target




