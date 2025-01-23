PROG=		trafficmap
CFLAGS=	-O2

${PROG}:
    ${CC} ${CFLAGS} -o ${PROG} ${PROG}.c

all: ${PROG}

install: ${PROG}
    install -s -m 755 ${PROG} /usr/local/bin

deinstall:
    rm -f /usr/local/bin/${PROG}

clean:
    rm -f ${PROG}

.PHONY: all install deinstall clean