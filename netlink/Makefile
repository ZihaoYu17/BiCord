all: print_packets get_first_bfee parse_log wifi_recv nl_bf_to_eff

KERNEL = $(strip $(shell uname -r))
KERNEL_INCLUDE = -I /usr/src/linux-headers-$(KERNEL)/include
CFLAGS = -Wall -Werror $(KERNEL_INCLUDE) -lm
CC = gcc

wifi_recv: wifi_recv.c handle_csi.c

nl_bf_to_eff: nl_bf_to_eff.c bf_to_eff.o iwl_nl.o util.o q_approx.o

clean:
	rm -f get_first_bfee wifi_recv print_packets parse_log
