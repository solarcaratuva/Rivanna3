#ifndef CAMERAPRESENTER_HPP
#define CAMERAPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class cameraView;

class cameraPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    cameraPresenter(cameraView& v);

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

    virtual ~cameraPresenter() {}

private:
    cameraPresenter();

    cameraView& view;
};

#endif // CAMERAPRESENTER_HPP
