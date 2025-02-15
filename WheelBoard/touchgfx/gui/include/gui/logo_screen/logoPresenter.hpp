#ifndef LOGOPRESENTER_HPP
#define LOGOPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class logoView;

class logoPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    logoPresenter(logoView& v);

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

    virtual ~logoPresenter() {}

private:
    logoPresenter();

    logoView& view;
};

#endif // LOGOPRESENTER_HPP
