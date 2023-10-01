.SILENT:

OBJECT = object
LIB = lib
QT = ClientQt
CO = g++

all:	Server Client

Client:	$(OBJECT)/mainclient.o $(OBJECT)/windowclient.o $(OBJECT)/moc_windowclient.o $(OBJECT)/TCP.o
	echo Creation de Client
	g++ -Wno-unused-parameter -o Client $(OBJECT)/TCP.o $(OBJECT)/mainclient.o $(OBJECT)/windowclient.o $(OBJECT)/moc_windowclient.o  /usr/lib64/libQt5Widgets.so /usr/lib64/libQt5Gui.so /usr/lib64/libQt5Core.so /usr/lib64/libGL.so -lpthread
Server:	server.cpp $(OBJECT)/TCP.o $(OBJECT)/ovesp.o
	echo Creation de Server
	g++ server.cpp $(OBJECT)/TCP.o -I $(LIB) -o Server -I/usr/include/mysql -m64 -L/usr/lib64/mysql -lmysqlclient -lpthread 

$(OBJECT)/moc_windowclient.o:	$(QT)/moc_windowclient.cpp
		echo Creation de moc_windowclient.o
		$(CO) -c -pipe -O2 -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -I $(QT) -o $(OBJECT)/moc_windowclient.o $(QT)/moc_windowclient.cpp
$(OBJECT)/windowclient.o:	$(QT)/windowclient.cpp
		echo Creation de windowclient.o
		$(CO) -c -pipe -O2 -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -I $(QT) -o $(OBJECT)/windowclient.o $(QT)/windowclient.cpp
$(OBJECT)/mainclient.o:	$(QT)/mainclient.cpp
		echo Creation de mainclient.o
		$(CO) -c -pipe -O2 -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -I $(QT) -o $(OBJECT)/mainclient.o $(QT)/mainclient.cpp

$(OBJECT)/TCP.o:	$(LIB)/TCP.cpp $(LIB)/TCP.h
	echo Creation de TCP.o
	g++ -c $(LIB)/TCP.cpp -o $(OBJECT)/TCP.o
$(OBJECT)/ovesp.o:	$(LIB)/ovesp.h $(LIB)/ovesp.cpp
	echo Creation de ovesp.o
	g++ -c $(LIB)/ovesp.cpp -o $(OBJECT)/ovesp.o -I/usr/include/mysql -m64 -L/urs/lib64/mysql -lmysqlclient


clean:	
	rm -f Client
	rm -f Server
	rm -f $(OBJECT)/*.o
