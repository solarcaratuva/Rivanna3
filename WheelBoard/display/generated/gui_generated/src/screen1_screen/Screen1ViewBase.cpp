/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
#include <touchgfx/Color.hpp>
#include <images/BitmapDatabase.hpp>
#include <texts/TextKeysAndLanguages.hpp>

Screen1ViewBase::Screen1ViewBase() :
    buttonCallback(this, &Screen1ViewBase::buttonCallbackHandler),
    frameCountInteraction5Interval(0),
    frameCountInteraction11Interval(0),
    interaction8Counter(0)
{
    touchgfx::CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);
    
    __background.setPosition(0, 0, 800, 480);
    __background.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
    add(__background);

    button1.setXY(269, 0);
    button1.setBitmaps(touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_MEDIUM_ROUNDED_NORMAL_ID), touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_MEDIUM_ROUNDED_PRESSED_ID));
    button1.setAction(buttonCallback);
    add(button1);

    gauge1.setBackground(touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_GAUGE_MEDIUM_BACKGROUNDS_ACTIVE_ID));
    gauge1.setPosition(17, 14, 240, 240);
    gauge1.setCenter(120, 120);
    gauge1.setStartEndAngle(-113, 112);
    gauge1.setRange(0, 100);
    gauge1.setValue(50);
    gauge1.setEasingEquation(touchgfx::EasingEquations::linearEaseIn);
    gauge1.setNeedle(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_GAUGE_MEDIUM_NEEDLES_SMOOTH_ID, 7, 67);
    gauge1.setMovingNeedleRenderingAlgorithm(touchgfx::TextureMapper::BILINEAR_INTERPOLATION);
    gauge1.setSteadyNeedleRenderingAlgorithm(touchgfx::TextureMapper::BILINEAR_INTERPOLATION);
    add(gauge1);

    gauge2.setBackground(touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_GAUGE_MEDIUM_BACKGROUNDS_LIGHT_FILLED_ID));
    gauge2.setPosition(529, 14, 240, 240);
    gauge2.setCenter(120, 120);
    gauge2.setStartEndAngle(-117, 121);
    gauge2.setRange(0, 600);
    gauge2.setValue(50);
    gauge2.setEasingEquation(touchgfx::EasingEquations::linearEaseIn);
    gauge2.setNeedle(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_GAUGE_MEDIUM_NEEDLES_ROUGH_ID, 10, 65);
    gauge2.setMovingNeedleRenderingAlgorithm(touchgfx::TextureMapper::BILINEAR_INTERPOLATION);
    gauge2.setSteadyNeedleRenderingAlgorithm(touchgfx::TextureMapper::BILINEAR_INTERPOLATION);
    gauge2.setArcVisible();
    gauge2Painter.setBitmap(touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_GAUGE_MEDIUM_FILLERS_SWOOP_ID));
    gauge2.getArc().setPainter(gauge2Painter);
    gauge2.getArc().setRadius(100);
    gauge2.getArc().setLineWidth(99);
    gauge2.getArc().setCapPrecision(180);
    gauge2.setArcPosition(23, 23, 192, 142);
    add(gauge2);

    textArea1.setXY(589, 62);
    textArea1.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
    textArea1.setLinespacing(0);
    textArea1.setTypedText(touchgfx::TypedText(T___SINGLEUSE_IMVQ));
    add(textArea1);

    scalableImage1.setBitmap(touchgfx::Bitmap(BITMAP_ARROW2_ID));
    scalableImage1.setPosition(429, 102, 80, 80);
    scalableImage1.setScalingAlgorithm(touchgfx::ScalableImage::NEAREST_NEIGHBOR);
    add(scalableImage1);

    scalableImage2.setBitmap(touchgfx::Bitmap(BITMAP_ARROW_ID));
    scalableImage2.setPosition(284, 102, 80, 80);
    scalableImage2.setScalingAlgorithm(touchgfx::ScalableImage::NEAREST_NEIGHBOR);
    add(scalableImage2);

    scalableImage3.setBitmap(touchgfx::Bitmap(BITMAP_ARROW3_ID));
    scalableImage3.setPosition(284, 102, 80, 80);
    scalableImage3.setScalingAlgorithm(touchgfx::ScalableImage::NEAREST_NEIGHBOR);
    scalableImage3.setVisible(false);
    add(scalableImage3);
}

Screen1ViewBase::~Screen1ViewBase()
{
    touchgfx::CanvasWidgetRenderer::resetBuffer();
}

void Screen1ViewBase::setupScreen()
{

}

void Screen1ViewBase::buttonCallbackHandler(const touchgfx::AbstractButton& src)
{
    if (&src == &button1)
    {
        //Interaction1
        //When button1 clicked call virtual function
        //Call function1
        function1();
        //Interaction4
        //When button1 clicked update value gauge1
        //Update value gauge1 over 0ms time
        gauge1.updateValue(1, 0);
        //Interaction6
        //When button1 clicked hide scalableImage2
        //Hide scalableImage2
        scalableImage2.setVisible(false);
        scalableImage2.invalidate();
    
        //Interaction7
        //When Interaction6 completed show scalableImage3
        //Show scalableImage3
        scalableImage3.setVisible(true);
        scalableImage3.invalidate();
    
        //Interaction8
        //When Interaction7 completed delay
        //Delay for 500 ms (30 Ticks)
        interaction8Counter = INTERACTION8_DURATION;
    }
}

void Screen1ViewBase::handleTickEvent()
{
    if (interaction8Counter > 0)
    {
        interaction8Counter--;
        if (interaction8Counter == 0)
        {

            //Interaction9
            //When Interaction8 completed hide scalableImage3
            //Hide scalableImage3
            scalableImage3.setVisible(false);
            scalableImage3.invalidate();
        
            //Interaction10
            //When Interaction9 completed show scalableImage2
            //Show scalableImage2
            scalableImage2.setVisible(true);
            scalableImage2.invalidate();
        }
    }

    frameCountInteraction5Interval++;
    if(frameCountInteraction5Interval == TICK_INTERACTION5_INTERVAL)
    {
        //Interaction5
        //When every N tick call virtual function
        //Call function2
        function2();
        frameCountInteraction5Interval = 0;
    }

    frameCountInteraction11Interval++;
    if(frameCountInteraction11Interval == TICK_INTERACTION11_INTERVAL)
    {
        //Interaction11
        //When every N tick call virtual function
        //Call function3
        function3();
        frameCountInteraction11Interval = 0;
    }
}
