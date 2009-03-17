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

#include "pref.h"

#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>

kpeggPreferences::kpeggPreferences()
    : KDialogBase(TreeList, i18n("kpegg Preferences"),
                  Ok|Apply|Cancel, Ok)
{
    // this is the base class for your preferences dialog.  it is now
    // a Treelist dialog.. but there are a number of other
    // possibilities (including Tab, Swallow, and just Plain)
    QFrame *frame;
    frame = addPage(i18n("Printer"), i18n("Printer setup"));
    m_pageOne = new kpeggPrefPageOne(frame);

    frame = addPage(i18n("Label"), i18n("label options	"));
    m_pageTwo = new kpeggPrefPageTwo(frame);
}

kpeggPrefPageOne::kpeggPrefPageOne(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAutoAdd(true);

    new QLabel(i18n("Choose your model and add preferences"), this);
}

kpeggPrefPageTwo::kpeggPrefPageTwo(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAutoAdd(true);

    new QLabel(i18n("Select your labels"), this);
}
#include "pref.moc"
