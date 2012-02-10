#ifndef CONTACTITEM_H
#define CONTACTITEM_H

#include <QObject>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

#include <Plasma/IconWidget>
#include <Plasma/Label>


class ContactItem : public QGraphicsWidget
{
    Q_OBJECT

public:
    ContactItem(QGraphicsWidget *parent = 0);
    
    virtual ~ContactItem() { };

private:

    QGraphicsLinearLayout *m_layout;
    
    Plasma::IconWidget *contact_picture;
    Plasma::Label *contact_name;

};


#endif // CONTACTITEM_H
