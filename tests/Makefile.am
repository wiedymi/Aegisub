# $Id$

bin_PROGRAMS = run

run_LDFLAGS= -L../libaegisub -laegisub-2.2 -lgtest -L/mnt/free/build/debug-wx-trunk-r63622/lib -lwx_baseu-2.9 -pthread -rpath /mnt/free/aegisub/commit/libaegisub/.libs
run_CPPFLAGS= @WX_CPPFLAGS@ -I../src/include/aegisub -I../libaegisub/include

run_SOURCES = \
		main.cpp \
		libaegisub_access.cpp \
		libaegisub_cajun.cpp
		
run_SOURCES += \
	*.h
