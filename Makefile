##
## Makefile for  in /home/maret_a/Projets/AbstractVM/cpp_abstractvm/sources
## 
## Made by Adrien Maret
## Login   <maret_a@epitech.net>
## 
## 
## Started on  19/02/2014 15:26
##

CXX		=	g++

CXXFLAGS	=	-Wall -W -Wextra -pedantic -std=c++11

SRCS_DAEMON	=	DomainSocket.cpp \
			Daemon.cpp 

SRCS_SENDER	=	DomainSocket.cpp \
			Sender.cpp

OBJS_DAEMON	=	$(SRCS_DAEMON:.cpp=.o)

OBJS_SENDER	=	$(SRCS_SENDER:.cpp=.o)

DAEMON		=	server

SENDER		=	client

all:		$(DAEMON) $(SENDER)

$(DAEMON):	$(OBJS_DAEMON)
		$(CXX) -o $(DAEMON) $(OBJS_DAEMON)

$(SENDER):	$(OBJS_SENDER)
		$(CXX) -o $(SENDER) $(OBJS_SENDER)

clean:
		rm -f $(OBJS_DAEMON)
		rm -f $(OBJS_SENDER)

fclean:		clean
		rm -f $(DAEMON)
		rm -f $(SENDER)

re:	 	fclean all
