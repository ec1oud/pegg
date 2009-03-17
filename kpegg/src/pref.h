/***************************************************************************
 *   Copyright (C) 2004 by Daniel Amkreutz                                 *
 *   aixpresso@web.de                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _KPEGGPREF_H_
#define _KPEGGPREF_H_

#include <kdialogbase.h>
#include <qframe.h>

class kpeggPrefPageOne;
class kpeggPrefPageTwo;

class kpeggPreferences : public KDialogBase
{
    Q_OBJECT
public:
    kpeggPreferences();

private:
    kpeggPrefPageOne *m_pageOne;
    kpeggPrefPageTwo *m_pageTwo;
};

class kpeggPrefPageOne : public QFrame
{
    Q_OBJECT
public:
    kpeggPrefPageOne(QWidget *parent = 0);
};

class kpeggPrefPageTwo : public QFrame
{
    Q_OBJECT
public:
    kpeggPrefPageTwo(QWidget *parent = 0);
};

#endif // _KPEGGPREF_H_
