#ifndef GRAPHVIEW_HPP
#define GRAPHVIEW_HPP

#include <gui_generated/graph_screen/graphViewBase.hpp>
#include <gui/graph_screen/graphPresenter.hpp>

class graphView : public graphViewBase
{
public:
    graphView();
    virtual ~graphView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // GRAPHVIEW_HPP
