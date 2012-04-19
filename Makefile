
TARGET:=	pwexec

GLIB:=		glib-2.0
GNOME_KEYRING:=	gnome-keyring-1

CFLAGS+=	-O2 -Wall
#CFLAGS+=	-g -Wall

CFLAGS+=	$(shell pkg-config --cflags $(GLIB))
CFLAGS+=	$(shell pkg-config --cflags $(GNOME_KEYRING))
LIBS+=		$(shell pkg-config --libs   $(GLIB))
LIBS+=		$(shell pkg-config --libs   $(GNOME_KEYRING))

all:	build

build:	$(TARGET)

clean:
	rm -f $(TARGET) *.o

$(TARGET):	pwexec.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)

