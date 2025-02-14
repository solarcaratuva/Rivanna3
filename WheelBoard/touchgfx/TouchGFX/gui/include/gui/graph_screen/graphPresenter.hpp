#ifndef GRAPHPRESENTER_HPP
#define GRAPHPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class graphView;

class graphPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    graphPresenter(graphView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~graphPresenter() {}

private:
    graphPresenter();

    graphView& view;
};

#endif // GRAPHPRESENTER_HPP
