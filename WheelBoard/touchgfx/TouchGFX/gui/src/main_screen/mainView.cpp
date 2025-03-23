#include <gui/main_screen/mainView.hpp>


mainView::mainView()
{

}

void mainView::setupScreen()
{
    mainViewBase::setupScreen();
    textArea1_2.setWildcard(textArea1_2Buffer);  // <-- Link buffer to TextArea
}

void mainView::tearDownScreen()
{
    mainViewBase::tearDownScreen();
}

void mainView::function1()
{
    int random = rand() % 10000; // Generates a number between 0-9999
    Unicode::snprintf(textArea1_2Buffer, TEXTAREA1_2_SIZE, "%i", random);
    textArea1_2.invalidate();
}
