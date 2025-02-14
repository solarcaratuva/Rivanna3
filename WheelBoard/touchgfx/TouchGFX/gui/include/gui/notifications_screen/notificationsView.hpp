#ifndef NOTIFICATIONSVIEW_HPP
#define NOTIFICATIONSVIEW_HPP

#include <gui_generated/notifications_screen/notificationsViewBase.hpp>
#include <gui/notifications_screen/notificationsPresenter.hpp>

class notificationsView : public notificationsViewBase
{
public:
    notificationsView();
    virtual ~notificationsView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // NOTIFICATIONSVIEW_HPP
