#include <plasma-calendar.h>
#include <Plasma/Theme>
#include <QPainter>

PlasmaCalendar::PlasmaCalendar(QObject *parent, const QVariantList &args): Plasma::Applet(parent, args)
{
    m_icon = KIcon("kde");
     
    setBackgroundHints(DefaultBackground);

}
 
PlasmaCalendar::~PlasmaCalendar()
{

}
 
 
void PlasmaCalendar::init()
{

} 
 
void PlasmaCalendar::paintInterface(QPainter *painter,
				     const QStyleOptionGraphicsItem *option, 
				     const QRect &contentsRect)
{
    Q_UNUSED(option);
    
    painter->setPen(Qt::white);
    painter->drawText(contentsRect,
                      Qt::AlignCenter | Qt::AlignHCenter,
                      "Calendar plasmoid");
    painter->restore();
}
 
#include "plasma-calendar.moc"

