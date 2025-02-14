#ifndef MAPVIEW_HPP
#define MAPVIEW_HPP

#include <gui_generated/map_screen/mapViewBase.hpp>
#include <gui/map_screen/mapPresenter.hpp>

class mapView : public mapViewBase
{
public:
    mapView();
    virtual ~mapView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // MAPVIEW_HPP
