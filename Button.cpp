#include "Button.h"

Button::Button(const char *sound, const char *texture, float x, float y, int col, int row) {
    this->row = row;
    this->col = col;
    fxButton = LoadSound(sound);
    buttonObj = LoadTexture(texture);
    setFrameHeight((float)buttonObj.height / NUM_FRAMES);
    sourceRec = { 0, 0, (float)buttonObj.width, getFrameHeight() };
    btnBounds = { x, y, (float)buttonObj.width, getFrameHeight() };
}

int Button::changeBtnAction(bool action) {
    btnAction = action;
    return 0;
}

int Button::changeBtnState(int state) {
    btnState = state;
    return 0;
}

int Button::getBtnState() {
    return btnState;
}

bool Button::getBtnAction() {
    return btnAction;
}

float Button::getFrameHeight() {
    return frameHeight;
}

int Button::setFrameHeight(float height) {
    frameHeight = height;
    return 0;
}

void Button::changeOwner(char owner) {
    this->owner = owner;
}

int Button::getRow() {
    return row;
}

int Button::getCol() {
    return col;
}

bool Button::isEmpty() {
    return empty;
}

int Button::setEmpty(bool empty) {
    this->empty = empty;
    return 0;
}

void Button::changeSprite(const char *texture) {
    buttonObj = LoadTexture(texture);
}

Texture Button::getSprite() {
    return buttonObj;
}

void Button::setIsButton(bool isButton) {
    isBoolButton = true;
}

bool Button::isButton() {
    return isBoolButton;
}

void Button::setButtonNum(int buttonNum) {
    this->buttonNum = buttonNum;
}

int Button::getButtonNum() {
    return buttonNum;
}

char Button::getOwner(){
    return owner;
}

Rectangle Button::getSourceRec(){
    return sourceRec;
} 

const char Button::destroySound(){
    return *destroyFx;
}
