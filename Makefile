CC = gcc 

EXEC = pthread_coap
OBJS = CoAPpthread.o   test.o main.o YCpthread.o  list.o \
#function.o  Blockpthread.o

ZLOGLIB=zlog
COAPLIB=coap
#LIBS=zlog coap
LIBPATH=./lib
COAPINCLUDE=./coapinclude

#COMMPATH = ../api6g2c
#COMMOBJ = $(COMMPATH)/safecalls.o $(COMMPATH)/api.o $(COMMPATH)/que.o $(COMMPATH)/rs232.o

all: $(EXEC)

$(EXEC): $(OBJS) $(COMMOBJ)
	$(CC)  $^ -pthread -I $(COAPINCLUDE) -L $(LIBPATH) -l $(ZLOGLIB) -l $(COAPLIB) -O3   -lmagicboxapi -o $@ -lrt
%.o:%.c
	$(CC) -o $@ -c $< -I $(COAPINCLUDE) -L $(LIBPATH) -l $(ZLOGLIB) -l $(COAPLIB)  -pthread -lrt  -O2

clean:
	rm -f $(EXEC) *.elf *.gdb *.o

