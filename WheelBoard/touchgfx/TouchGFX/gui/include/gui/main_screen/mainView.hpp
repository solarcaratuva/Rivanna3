#ifndef MAINVIEW_HPP
#define MAINVIEW_HPP

#include <gui_generated/main_screen/mainViewBase.hpp>
#include <gui/main_screen/mainPresenter.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>

class mainView : public mainViewBase
{
public:
    mainView();
    virtual ~mainView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void function1();
protected:
    
};

#endif // MAINVIEW_HPP
