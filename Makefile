PROG=		trafficmap
CFLAGS=	-O2
SRCDIR=	src
OBJDIR=	obj
BINDIR=	bin

${OBJDIR}/${PROG}.o:
	@mkdir -p ${OBJDIR}
	${CC} ${CFLAGS} -c -o ${OBJDIR}/${PROG}.o ${SRCDIR}/${PROG}.c

${BINDIR}/${PROG}: ${OBJDIR}/${PROG}.o
	${CC} -o ${BINDIR}/${PROG} ${OBJDIR}/${PROG}.o

all: ${BINDIR}/${PROG}

install: ${BINDIR}/${PROG}
	install -s -m 755 ${BINDIR}/${PROG} /usr/local/bin

deinstall:
	rm -f /usr/local/bin/${PROG}

clean:
	rm -rf ${BINDIR}
	rm -rf ${OBJDIR}

.PHONY: all install deinstall clean
.MAIN: all
