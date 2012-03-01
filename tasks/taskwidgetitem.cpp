/*
    Akonadi google tasks plasmoid - taskwidgetitem.cpp
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


#include "taskwidgetitem.h"

#include <KDateTime>

TaskWidgetItem::TaskWidgetItem(const Akonadi::Item & item, QGraphicsWidget * parent)
    : QGraphicsWidget(parent),
      m_copleted(0),
      m_date(0),
      m_name(0)
{
    m_item = item;
    
    m_todo = m_item.payload<KCalCore::Todo::Ptr>();
        
    m_mainLayout = new QGraphicsLinearLayout(Qt::Horizontal,this);
    m_infoLayout = new QGraphicsLinearLayout(Qt::Vertical,m_mainLayout);
    
    setLayout(m_mainLayout);
    
    setItemInfo();
}

void TaskWidgetItem::setItemInfo()
{

    m_copleted = new Plasma::CheckBox(this);
    m_copleted->setChecked(m_todo->isCompleted());
    m_copleted->setMaximumSize(20,20);
    
    m_mainLayout->addItem(m_copleted);
    
    m_date = new Plasma::Label(this);
    m_date->setText(m_todo->completed().toString(KDateTime::LocalDate));
    m_date->setMaximumSize(20,200);
    
    m_name = new Plasma::Label(this);
    m_name->setText(m_todo->summary());
    m_name->setMaximumSize(20,200);
    
    m_infoLayout->addItem(m_name);
    m_infoLayout->addItem(m_date);
    
    m_mainLayout->addItem(m_infoLayout);
}
