#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/utils.hpp>

int random_number = std::rand() % 101;
// bool leftBlinker = false;

Screen1View::Screen1View()
{
    
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::function1()
{
    touchgfx_printf("buttonClicked\n");
    gauge1.updateValue(5, 25);
    gauge1.setValue(5);
    // int rand = std::rand()%2;
    // if(rand == 1)
    // {
    //     leftBlinker = true;
    // }
    // if(leftBlinker)
    // {
        
    // }
}

void Screen1View::function2()
{
    gauge1.updateValue(random_number, 100);
}

void Screen1View::function3()
{
    random_number = std::rand() % 101;
    
}
