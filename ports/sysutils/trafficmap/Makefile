PORTNAME=	trafficmap
#DISTVERSIONPREFIX=	v
#DISTVERSION=	0.0.1
DISTVERSION=	g20250122
CATEGORIES=	sysutils

USE_GITHUB=	yes
GH_ACCOUNT=	tuaris
GH_TAGNAME=	3f1b64b

MAINTAINER=	daniel@morante.net
COMMENT=	Generates remap.config for Apache Traffic Server using DNS SRV records
WWW=		http://www.pacyworld.com

LICENSE=	BSD2CLAUSE
LICENSE_FILE=	${WRKSRC}/LICENSE

PLIST_FILES=	bin/trafficmap

PORTDOCS=	README.md

do-install:
	${INSTALL_PROGRAM} ${WRKSRC}/bin/${PORTNAME} ${STAGEDIR}${PREFIX}/bin/${PORTNAME}

.include <bsd.port.mk>

