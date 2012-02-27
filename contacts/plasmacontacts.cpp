/*
    Akonadi google contact plasmoid - plasmacontacts.cpp
    Copyright (C) 2012  Jan Grulich <grulja@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <plasmacontacts.h>

#include <KABC/Addressee>

#include <QWidget>

#include <Akonadi/Entity>
#include <Akonadi/EntityDisplayAttribute>
#include <Akonadi/CollectionFetchScope>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/ItemFetchJob>

#include <Plasma/Theme>
#include <Plasma/IconWidget>


PlasmaContacts::PlasmaContacts(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
      m_id(-1),
      m_findData(true),
      m_showEmails(true),
      m_showNumbers(true),
      m_showEmptyContacts(true)
{
    setConfigurationRequired(true);
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setBackgroundHints(DefaultBackground);
    setMinimumSize(300,500);
}

void PlasmaContacts::init()
{
    m_find = new Plasma::LineEdit(this);
    m_find->setClearButtonShown(true);
    m_find->setText(i18n(" Find contact "));

    m_contactList = new ContactWidget(this);

    m_scroll = new Plasma::ScrollWidget(this);
    m_scroll->setWidget(m_contactList);
    m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_mainLayout = new QGraphicsLinearLayout(Qt::Vertical,this);

    m_mainLayout->addItem(m_find);
    m_mainLayout->addItem(m_scroll);

    setLayout(m_mainLayout);

    connect(m_find,SIGNAL(textChanged(QString)),SLOT(lineChanged(QString)));
    connect(m_find,SIGNAL(focusChanged(bool)),SLOT(lineFocusChanged(bool)));

    configChanged();

}

void PlasmaContacts::configChanged()
{

    KConfigGroup conf = config();

    m_find->setText("");

    if (conf.readEntry("findData",true) != m_findData) {

        m_findData = conf.readEntry("findData",true);

        m_contactList->setFilterData(m_findData);

    }

    if (conf.readEntry("showEmptyContacts",true) != m_showEmptyContacts) {

        m_showEmptyContacts = conf.readEntry("showEmptyContacts",true);

        m_contactList->setShowEmptyContacts(m_showEmptyContacts);

    }

    if (conf.readEntry("collection",-1) != m_id) {

        setConfigurationRequired(false);

        m_id = conf.readEntry("collection",-1);

        m_contactList->setCollection(m_id);
    }


}

void PlasmaContacts::createConfigurationInterface(KConfigDialog* parent)
{
    QWidget *widget = new QWidget(0);

    configDialog.setupUi(widget);

    KConfigGroup conf = config();

    configDialog.loadCollections->setIcon(KIcon("view-refresh"));
    configDialog.findData->setChecked(conf.readEntry("findData",true));

    fetchCollections();

    configDialog.showEmptyContacts->setChecked(conf.readEntry("showEmptyContacts",true));

    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
    connect(configDialog.collectionBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(settingsModified()));
    connect(configDialog.findData, SIGNAL(clicked(bool)), parent, SLOT(settingsModified()));
    connect(configDialog.showEmptyContacts, SIGNAL(clicked(bool)), parent, SLOT(settingsModified()));
    connect(configDialog.loadCollections,SIGNAL(clicked(bool)),SLOT(fetchCollections()));

    parent->addPage(widget,"General",icon());
}

void PlasmaContacts::configAccepted()
{
    KConfigGroup conf = config();

    conf.writeEntry("collection",configDialog.collectionBox->itemData(configDialog.collectionBox->currentIndex()).toInt());
    conf.writeEntry("findData", configDialog.findData->isChecked());
    conf.writeEntry("showEmptyContacts",configDialog.showEmptyContacts->isChecked());

    emit configNeedsSaving();

}

void PlasmaContacts::lineChanged(const QString & text)
{

    m_contactList->showContactsContains(text);


}

void PlasmaContacts::lineFocusChanged(bool change)
{

    if (change && m_find->text().contains(i18n(" Find "))) {

        m_find->setText("");

    }

}

void PlasmaContacts::fetchCollections()
{

    configDialog.collectionBox->clear();

    Akonadi::CollectionFetchJob * job = new Akonadi::CollectionFetchJob(Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this );

    job->fetchScope();

    connect(job,SIGNAL(result(KJob*)), SLOT(fetchCollectionsFinished(KJob*)));

}

void PlasmaContacts::fetchCollectionsFinished(KJob* job)
{

    if (job->error()) {

        qDebug() << "fetchCollections failed";

        return;
    }

    Akonadi::CollectionFetchJob *fetchJob = qobject_cast<Akonadi::CollectionFetchJob*>(job);
    const Akonadi::Collection::List collections = fetchJob->collections();

    foreach ( const Akonadi::Collection &collection, collections ) {

        if (collection.resource().contains("akonadi_googlecontacts_resource") &&
            collection.contentMimeTypes().contains(KABC::Addressee::mimeType())) {

            Akonadi::EntityDisplayAttribute *attribute = collection.attribute< Akonadi::EntityDisplayAttribute > ();
	
            if (!attribute)
		
                configDialog.collectionBox->addItem(collection.name(), collection.id());
	    
            else
		
                configDialog.collectionBox->addItem(attribute->displayName(), collection.id());

        }

    }

    if (m_id != -1) {
	
        int i = configDialog.collectionBox->findData(m_id);
        configDialog.collectionBox->setCurrentIndex(i);
	
    }

}




#include "plasmacontacts.moc"


