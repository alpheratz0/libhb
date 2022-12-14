# Copyright (C) 2022 <alpheratz99@protonmail.com>
# This program is free software.

AR        = ar
CC        = cc
INCS      = -I./include -I./third_party/c-stringbuilder
CFLAGS    = -pedantic -Wall -Wextra -Os $(INCS)
LDLIBS    = -ljq -lhb -lsb
LDFLAGS   = -s -L. -L./third_party/c-stringbuilder
