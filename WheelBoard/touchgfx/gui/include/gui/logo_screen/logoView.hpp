#ifndef LOGOVIEW_HPP
#define LOGOVIEW_HPP

#include <gui_generated/logo_screen/logoViewBase.hpp>
#include <gui/logo_screen/logoPresenter.hpp>

class logoView : public logoViewBase
{
public:
    logoView();
    virtual ~logoView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // LOGOVIEW_HPP
