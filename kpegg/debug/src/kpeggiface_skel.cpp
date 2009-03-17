/****************************************************************************
**
** DCOP Skeleton created by dcopidl2cpp from kpeggiface.kidl
**
** WARNING! All changes made in this file will be lost!
**
*****************************************************************************/

#include "/home/daniel/Projekte/kpegg/src/kpeggiface.h"

#include <kdatastream.h>


static const char* const kpeggIface_ftable[2][3] = {
    { "void", "openURL(QString)", "openURL(QString url)" },
    { 0, 0, 0 }
};
static const int kpeggIface_ftable_hiddens[1] = {
    0,
};

bool kpeggIface::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
    if ( fun == kpeggIface_ftable[0][1] ) { // void openURL(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	replyType = kpeggIface_ftable[0][0]; 
	openURL(arg0 );
    } else {
	return DCOPObject::process( fun, data, replyType, replyData );
    }
    return true;
}

QCStringList kpeggIface::interfaces()
{
    QCStringList ifaces = DCOPObject::interfaces();
    ifaces += "kpeggIface";
    return ifaces;
}

QCStringList kpeggIface::functions()
{
    QCStringList funcs = DCOPObject::functions();
    for ( int i = 0; kpeggIface_ftable[i][2]; i++ ) {
	if (kpeggIface_ftable_hiddens[i])
	    continue;
	QCString func = kpeggIface_ftable[i][0];
	func += ' ';
	func += kpeggIface_ftable[i][2];
	funcs << func;
    }
    return funcs;
}


