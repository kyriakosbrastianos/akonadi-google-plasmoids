#include <plasma-contacts.h>
#include <Plasma/Theme>
#include <QPainter>

PlasmaContacts::PlasmaContacts(QObject *parent, const QVariantList &args): Plasma::Applet(parent, args)
{
    m_icon = KIcon("kde");
    m_layout = NULL;

    contact_list = NULL;
    
    setHasConfigurationInterface(false);  
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setBackgroundHints(DefaultBackground);

}

PlasmaContacts::~PlasmaContacts()
{

}

 
void PlasmaContacts::init()
{
    
    contact_list = new ContactsWidget(this);
    m_layout = new QGraphicsLinearLayout(Qt::Vertical,this);
    
    m_layout->addItem(contact_list);
  
    ContactItem * item;
    
    for (int i = 0; i < 10; i++) {
	
	item = new ContactItem(this);
	
	contact_list->addContact(item);
	
    }


} 
 
#include "plasma-contacts.moc"

 
