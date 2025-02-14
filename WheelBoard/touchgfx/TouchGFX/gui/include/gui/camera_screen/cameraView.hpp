#ifndef CAMERAVIEW_HPP
#define CAMERAVIEW_HPP

#include <gui_generated/camera_screen/cameraViewBase.hpp>
#include <gui/camera_screen/cameraPresenter.hpp>

class cameraView : public cameraViewBase
{
public:
    cameraView();
    virtual ~cameraView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // CAMERAVIEW_HPP
