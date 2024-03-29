/*
    Akonadi google tasks plasmoid - taskwidget.cpp
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


#include "taskwidget.h"

#include <Akonadi/Entity>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemDeleteJob>

TaskWidget::TaskWidget(QGraphicsWidget * parent)
    : QGraphicsWidget(parent),
      m_expiredColor("#c80000"),
      m_todayColor("#e64600"),
      m_weekColor("#e6f000"),
      m_otherColor(""),
      m_taskWidth(300),
      m_autoHide(false),
      m_autoDel(false),
      m_order(DNC)
{

    m_layout = new TaskLayout(Qt::Vertical, this);
    
    setLayout(m_layout);

    m_monitor = new Akonadi::Monitor();
    m_monitor->itemFetchScope().fetchFullPayload(true);

    connect(m_monitor, SIGNAL(itemAdded(Akonadi::Item, Akonadi::Collection)),
            SLOT(itemAdded(Akonadi::Item, Akonadi::Collection)));
    connect(m_monitor, SIGNAL(itemChanged(Akonadi::Item, QSet<QByteArray>)),
            SLOT(itemChanged(Akonadi::Item, QSet<QByteArray>)));
    connect(m_monitor, SIGNAL(itemRemoved(Akonadi::Item)),
            SLOT(itemRemoved(Akonadi::Item)));
    
}

void TaskWidget::setExpiredColor(QString color)
{

    m_expiredColor = color;
}

void TaskWidget::setTodayColor(QString color)
{

    m_todayColor = color;
}

void TaskWidget::setWeekColor(QString color)
{

    m_weekColor = color;
}

void TaskWidget::setOtherColor(QString color)
{

    m_otherColor = color;
}

void TaskWidget::setOrderBy(TaskWidget::OrderBy order)
{

    m_order = order;
}

void TaskWidget::setAutoHideCompleted(bool hide)
{

    m_autoHide = hide;
    
}

void TaskWidget::setAutoDeleteCompleted(bool del)
{
    
    m_autoDel = del;
}

void TaskWidget::setCollections(QList<Akonadi::Entity::Id> ids)
{
    clear();

    m_idList = ids;
    
    if (!m_idList.isEmpty())

        fetchCollections();

}

void TaskWidget::updateTasksWidth(int width)
{

    m_taskWidth = width;
    
    TaskWidgetItem * item;
    
    for (int i = 0; i < m_layout->count(); i++) {
	
	item = static_cast<TaskWidgetItem*>(m_layout->itemAt(i));
	
	item->setMaxWidth(width);
    }
    
}

void TaskWidget::fetchCollections()
{

    Akonadi::CollectionFetchJob * job = new Akonadi::CollectionFetchJob(Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this);

    job->fetchScope();

    connect(job, SIGNAL(result(KJob *)), SLOT(fetchCollectionsFinished(KJob *)));

}

void TaskWidget::fetchCollectionsFinished(KJob * job)
{
    if (job->error()) {

        qDebug() << "fetchCollections failed";

        return;
    }

    Akonadi::CollectionFetchJob * fetchJob = qobject_cast<Akonadi::CollectionFetchJob *>(job);

    const Akonadi::Collection::List collections = fetchJob->collections();

    foreach (const Akonadi::Collection & collection, collections) {
	
        if (m_idList.contains(collection.id())) {

            m_monitor->setCollectionMonitored(collection, true);

            fetchItems(collection);

        }

    }

}

void TaskWidget::fetchItems(const Akonadi::Collection & collection)
{

    Akonadi::ItemFetchJob * job = new Akonadi::ItemFetchJob(collection);

    connect(job, SIGNAL(result(KJob *)), SLOT(fetchItemsFinished(KJob *)));

    job->fetchScope().fetchFullPayload(true);


}

void TaskWidget::fetchItemsFinished(KJob * job)
{
    if (job->error()) {

        qDebug() << "fetchItems failed";

        return;
    }

    Akonadi::ItemFetchJob * fetchJob = qobject_cast<Akonadi::ItemFetchJob *>(job);

    const Akonadi::Item::List items = fetchJob->items();

    foreach (const Akonadi::Item & item, items) {

        TaskWidgetItem * contact;

        contact = new TaskWidgetItem(item, this);

        addItem(contact);

    }


}

void TaskWidget::itemDeleted(KJob * job)
{
    if (job->error()) {

        qDebug() << "Error occurred";
	
    } else {

        qDebug() << "Item removed successfully";
	
    }
    
}


void TaskWidget::addItem(TaskWidgetItem * item)
{

    m_layout->addItem(item);

    updateCompletedTasks();
}


void TaskWidget::clear()
{

    m_layout->clear();

}

void TaskWidget::updateCompletedTasks()
{

    if (!m_autoDel && !m_autoHide) {
	
	return;
	
    }
    
    QList<TaskWidgetItem*> list = m_layout->updateCompletedTasks();
    
    for (int i = 0; i < list.count(); i++) {
	
	m_layout->removeItem(list.at(i));
	list.at(i)->hide();
	
	if (m_autoDel) {
	 
	    Akonadi::ItemDeleteJob * job = new Akonadi::ItemDeleteJob(list.at(i)->item());
	    connect(job, SIGNAL(result(KJob*)), this, SLOT(itemDeleted(KJob*)));
	    
	    list.at(i)->deleteLater();
	    
	}
	
    }
}


void TaskWidget::itemAdded(const Akonadi::Item & item, const Akonadi::Collection & collection)
{

    for (int i = 0; i < m_idList.count(); i++) {

        if (m_idList.at(i) == collection.id()) {

            TaskWidgetItem * task;

            task = new TaskWidgetItem(item, this);

            addItem(task);
	    
        }

    }

}

void TaskWidget::itemChanged(const Akonadi::Item & item, QSet< QByteArray > array)
{

    Q_UNUSED(array);

    TaskWidgetItem * task;

    for (int i = 0; i < m_layout->count(); i++) {

        task = static_cast<TaskWidgetItem *>(m_layout->itemAt(i));

        if (task->operator==(item)) {

            task->updateTask(item);

            m_layout->updateItem(task);
	    
	    updateCompletedTasks();
	    
            return;
        }

    }

}

void TaskWidget::itemRemoved(const Akonadi::Item & item)
{

    TaskWidgetItem * task;

    for (int i = 0; i < m_layout->count(); i++) {

        task = static_cast<TaskWidgetItem *>(m_layout->itemAt(i));

        if (task->operator==(item)) {

            task->hide();

            m_layout->removeItem(task);

            task->deleteLater();

            return;
        }

    }

}
