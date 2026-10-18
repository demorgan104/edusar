CC = cc
PYTHON ?= python3
CPPFLAGS += -I../modules/Common/include -I../modules/Com/include -I../modules/CanIf/include -I../modules/PduR/include
CFLAGS ?= -std=c99 -O2 -Wall -Wextra -Werror -pedantic
CONFIG ?= ../config/demo.json
PORT ?= 8000
