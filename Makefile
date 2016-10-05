# Makefile for echo client and server

CXX=			g++ $(CCFLAGS)

ECHO-SERVER=		echo-server.o  server.o message.o
ECHO-CLIENT=		echo-client.o  client.o
OBJS =			$(ECHO-CLIENT) $(ECHO-SERVER)

LIBS=

CCFLAGS= -g

all:	echo-client echo-server


echo-server:$(ECHO-SERVER)
	$(CXX) -o server $(ECHO-SERVER) $(LIBS)

echo-client:$(ECHO-CLIENT)
	$(CXX) -o client $(ECHO-CLIENT) $(LIBS)

clean:
	rm -f $(OBJS) $(OBJS:.o=.d)

realclean:
	rm -f $(OBJS) $(OBJS:.o=.d) server client


# These lines ensure that dependencies are handled automatically.
%.d:	%.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

include	$(OBJS:.o=.d)
