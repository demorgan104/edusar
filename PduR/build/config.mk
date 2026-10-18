CC = cc
PYTHON ?= python3
CPPFLAGS += -I../Common/include -I../Com/include -I../CanIf/include -I../PduR/include
CFLAGS ?= -std=c99 -O2 -Wall -Wextra -Werror -pedantic
CONFIG ?= ../config/demo.json
PORT ?= 8000
