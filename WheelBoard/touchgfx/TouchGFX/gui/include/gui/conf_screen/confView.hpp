#ifndef CONFVIEW_HPP
#define CONFVIEW_HPP

#include <gui_generated/conf_screen/confViewBase.hpp>
#include <gui/conf_screen/confPresenter.hpp>

class confView : public confViewBase
{
public:
    confView();
    virtual ~confView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // CONFVIEW_HPP
