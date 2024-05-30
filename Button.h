#ifndef BUTTON_H
#define BUTTON_H

#include <string>
#include "raylib.h"

#define NUM_FRAMES 3

class Button {
private:
    const int screenWidth = 1000;
    const int screenHeight = 1000;
    float frameHeight;
    int row = 1;
    int col = 1;
    bool empty = false;
    bool isBoolButton = false;
    char owner = 'e';
    int btnState = 0;
    bool btnAction = false;
    const char *destroyFx = "resoucres/captured.mp3";
public:
    Rectangle btnBounds;
    Texture2D buttonObj;
    Rectangle sourceRec;
    Sound fxButton;
    Button(const char *sound, const char *texture, float x, float y, int col, int row);
    int changeBtnAction(bool action);
    int changeBtnState(int state);
    int getBtnState();
    bool getBtnAction();
    float getFrameHeight();
    int setFrameHeight(float height);
    int getRow();
    int getCol();
    int setEmpty(bool empty);
    bool isEmpty();
    bool isButton();
    void setIsButton(bool isButton);
    void changeOwner(char owner);
    char getOwner();
    void changeSprite(const char *texture);
    Texture getSprite();
    void setButtonNum(int buttonNum);
    int getButtonNum();
    int buttonNum = 0;
    Rectangle getSourceRec();
    const char destroySound();
};

#endif  // BUTTON_H
