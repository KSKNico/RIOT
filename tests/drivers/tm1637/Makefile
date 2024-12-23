BOARD ?= nrf52840dk

include ../Makefile.drivers_common

USEMODULE += tm1637

USEMODULE += ztimer
USEMODULE += ztimer_msec

CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-function


include $(RIOTBASE)/Makefile.include
