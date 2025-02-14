#ifndef MAPPRESENTER_HPP
#define MAPPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class mapView;

class mapPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    mapPresenter(mapView& v);

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

    virtual ~mapPresenter() {}

private:
    mapPresenter();

    mapView& view;
};

#endif // MAPPRESENTER_HPP
