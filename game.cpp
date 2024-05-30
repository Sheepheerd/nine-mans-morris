#include <algorithm>
#include <iostream>
#include <vector>

#include "Button.h"
#include "raylib.h"

// Constants
const int BOARD_SIZE = 7;
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

// Function to check in a direction (generalized)
int CheckDirection(Button* board[BOARD_SIZE][BOARD_SIZE], int col, int row,
                   int dCol, int dRow, char initialOwner) {
  int found = 0;
  col += dCol;
  row += dRow;

  while (col >= 0 && col < BOARD_SIZE && row >= 0 && row < BOARD_SIZE) {
    Button* currentButton = board[col][row];
    if (currentButton == nullptr || (col == 3 && row == 3)) break;

    if (currentButton->isEmpty() || currentButton->getOwner() != initialOwner) {
      break;
    }
    found++;
    col += dCol;
    row += dRow;
  }
  return found;
}

// Check all directions
bool CheckDirections(Button* button, Button* board[BOARD_SIZE][BOARD_SIZE]) {
  int row = button->getRow();
  int col = button->getCol();
  char owner = button->getOwner();

  int horizontalFound = CheckDirection(board, col, row, 1, 0, owner) +
                        CheckDirection(board, col, row, -1, 0, owner);
  int verticalFound = CheckDirection(board, col, row, 0, 1, owner) +
                      CheckDirection(board, col, row, 0, -1, owner);

  return (horizontalFound >= 2 || verticalFound >= 2);
}

// Perform an action on the button
void ButtonAction(Button* button, char currentPlayer) {
  button->changeOwner(currentPlayer);
  button->setEmpty(false);

  // Change Sprite
  if (currentPlayer == 'x') {
    button->changeSprite("resources/cross.png");
  } else if (currentPlayer == 'o') {
    button->changeSprite("resources/circle.png");
  }
}

// Draw buttons
void DrawButtons(const std::vector<Button*>& board) {
  BeginDrawing();
  for (const auto& button : board) {
    DrawTextureRec(button->buttonObj, button->sourceRec,
                   {button->btnBounds.x, button->btnBounds.y}, WHITE);
  }
  EndDrawing();
}

// Empty a button
void ButtonEmpty(Button* button) {
  button->changeSprite("resources/empty.png");
  button->setEmpty(true);
}
void InitializeButtons(Button* buttons[7][7]) {
  for (int j = 0; j < 7; ++j) {
    for (int i = 0; i < 7; ++i) {
      float x = 60.0f + 100.0f * i;
      float y = 60.0f + 100.0f * j;
      if (j == 3) y = 450.0f;
      if (i == 3) x = 450.0f;
      if (j >= 4) y = 250.0f + 100.0f * j;
      if (i >= 4) x = 250.0f + 100.0f * i;

      buttons[j][i] =
          new Button("resources/move.mp3", "resources/empty.png", x, y, j, i);
    }
  }
}

void SetupEmptyAndCollisionButtons(Button* buttons[7][7],
                                   std::vector<Button*>& emptyButtons,
                                   std::vector<Button*>& collisionButtons) {
  const std::vector<std::pair<int, int>> emptyPositions = {
      {0, 1}, {0, 2}, {0, 4}, {0, 5}, {1, 0}, {1, 2}, {1, 4}, {1, 6}, {2, 0},
      {2, 1}, {2, 5}, {2, 6}, {3, 3}, {6, 1}, {6, 2}, {6, 4}, {6, 5}, {5, 0},
      {5, 2}, {5, 4}, {5, 6}, {4, 0}, {4, 1}, {4, 5}, {4, 6}};

  for (const auto& pos : emptyPositions) {
    emptyButtons.push_back(buttons[pos.first][pos.second]);
  }

  int buttonNum = 0;
  for (int j = 0; j < 7; ++j) {
    for (int i = 0; i < 7; ++i) {
      if (std::find(emptyButtons.begin(), emptyButtons.end(), buttons[j][i]) ==
          emptyButtons.end()) {
        buttons[j][i]->setEmpty(true);
        buttons[j][i]->setButtonNum(buttonNum);
        collisionButtons.push_back(buttons[j][i]);
        buttonNum++;
      } else {
        buttons[j][i]->setEmpty(false);
      }
    }
  }
}

void DrawPlayerIndicators(char currentPlayer, Texture2D crossIndicator,
                          Texture2D circleIndicator) {
  if (currentPlayer == 'x') {
    DrawTexturePro(crossIndicator, {0, 90, 90, 90}, {375, 450, 90, 90}, {0, 0},
                   0, WHITE);
    DrawTexturePro(circleIndicator, {0, 0, 90, 90}, {535, 450, 90, 90}, {0, 0},
                   0, WHITE);
  } else {
    DrawTexturePro(crossIndicator, {0, 0, 90, 90}, {375, 450, 90, 90}, {0, 0},
                   0, WHITE);
    DrawTexturePro(circleIndicator, {0, 90, 90, 90}, {535, 450, 90, 90}, {0, 0},
                   0, WHITE);
  }
}

void CleanupResources(Button* buttons[7][7]) {
  for (int j = 0; j < 7; ++j) {
    for (int i = 0; i < 7; ++i) {
      UnloadTexture(buttons[j][i]->buttonObj);
      UnloadSound(buttons[j][i]->fxButton);
      delete buttons[j][i];
    }
  }
  CloseAudioDevice();
  CloseWindow();
}

int main(void) {
  const int screenWidth = 1000;
  const int screenHeight = 1000;

  InitWindow(screenWidth, screenHeight,
             "raylib [textures] example - sprite button");
  InitAudioDevice();

  Button* buttons[7][7];
  InitializeButtons(buttons);

  std::vector<Button*> emptyButtons;
  std::vector<Button*> collisionButtons;
  SetupEmptyAndCollisionButtons(buttons, emptyButtons, collisionButtons);

  Vector2 mousePoint = {0.0f, 0.0f};
  SetTargetFPS(60);

  char currentPlayer = 'x';
  int currentState = 0;
  int player1Pieces = 9, player2Pieces = 9;
  int player1PiecesPlayed = 0, player2PiecesPlayed = 0;
  bool isMill = false;
  bool hasSelectedButton = false;
  Button* selectedButton = nullptr;

  Texture2D background = LoadTexture("resources/board.png");
  Texture2D crossIndicator = LoadTexture("resources/crossIndicator.png");
  Texture2D circleIndicator = LoadTexture("resources/circleIndicator.png");
  while (!WindowShouldClose())  // Detect window close button or ESC key
  {
    DrawTexture(background, 0, 0, WHITE);

    if (currentPlayer == 'x') {
      DrawTexturePro(crossIndicator,
                     (Rectangle){.x = 0, .y = 90, .width = 90, .height = 90},
                     (Rectangle){.x = 375, .y = 450, .width = 90, .height = 90},
                     (Vector2){
                         .x = 0,
                         .y = 0,
                     },
                     0, WHITE);

      DrawTexturePro(circleIndicator,
                     (Rectangle){.x = 0, .y = 0, .width = 90, .height = 90},
                     (Rectangle){.x = 535, .y = 450, .width = 90, .height = 90},
                     (Vector2){
                         .x = 0,
                         .y = 0,
                     },
                     0, WHITE);
    } else {
      DrawTexturePro(crossIndicator,
                     (Rectangle){.x = 0, .y = 0, .width = 90, .height = 90},
                     (Rectangle){.x = 375, .y = 450, .width = 90, .height = 90},
                     (Vector2){
                         .x = 0,
                         .y = 0,
                     },
                     0, WHITE);

      DrawTexturePro(circleIndicator,
                     (Rectangle){.x = 0, .y = 90, .width = 90, .height = 90},
                     (Rectangle){.x = 535, .y = 450, .width = 90, .height = 90},
                     (Vector2){
                         .x = 0,
                         .y = 0,
                     },
                     0, WHITE);
    }

    isMill = false;
    mousePoint = GetMousePosition();

    // Player Turn
    if (currentState == 0) {
      for (const auto& button : collisionButtons) {
        button->changeBtnAction(false);  // Reset btnAction for each button

        // Check button state
        if (CheckCollisionPointRec(mousePoint, button->btnBounds) &&
            button->isEmpty()) {
          if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            button->changeBtnState(2);
          else
            button->changeBtnState(1);

          if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            button->changeBtnAction(true);
        } else
          button->changeBtnState(0);

        // Handle button action
        if (button->getBtnAction()) {
          if (currentPlayer == 'x') {
            player1Pieces = player1Pieces - 1;
            player1PiecesPlayed = player1PiecesPlayed + 1;

          } else if (currentPlayer == 'o') {
            player2Pieces = player2Pieces - 1;
            player2PiecesPlayed = player2PiecesPlayed + 1;
          }

          PlaySound(button->fxButton);

          // Set Button To Occupied By currentPlayer
          ButtonAction(button, currentPlayer);
          isMill = CheckDirections(button, buttons);

          if (isMill) {
            currentState = 1;
            DrawButtons(collisionButtons);
            button->sourceRec.y =
                button->getBtnState() * button->getFrameHeight();

            break;
          }

          if (player1Pieces == 0 && player2Pieces == 0) {
            currentState = 2;
            if (currentPlayer == 'x') {
              currentPlayer = 'o';
            } else {
              currentPlayer = 'x';
            }
            DrawButtons(collisionButtons);
            button->sourceRec.y =
                button->getBtnState() * button->getFrameHeight();

            break;
          } else {
            if (currentPlayer == 'x') {
              currentPlayer = 'o';

            } else if (currentPlayer == 'o') {
              currentPlayer = 'x';
            }
          }
        }

        // Calculate button frame rectangle to draw depending on button state
        button->sourceRec.y = button->getBtnState() * button->getFrameHeight();
      }
      DrawButtons(collisionButtons);
    }

    // Mill
    else if (currentState == 1) {
      for (const auto& button : collisionButtons) {
        button->changeBtnAction(false);  // Reset btnAction for each button

        // Check button state
        if (CheckCollisionPointRec(mousePoint, button->btnBounds) &&
            !button->isEmpty() && button->getOwner() != currentPlayer) {
          if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            button->changeBtnState(2);
          else
            button->changeBtnState(1);

          if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            button->changeBtnAction(true);
        } else
          button->changeBtnState(0);

        if (button->getBtnAction()) {
          if (currentPlayer == 'x') {
            player2PiecesPlayed = player2PiecesPlayed - 1;
          } else {
            player1PiecesPlayed = player1PiecesPlayed - 1;
          }

          Sound destroySound = LoadSound("resources/captured.mp3");

          PlaySound(destroySound);

          ButtonEmpty(button);
          if (player1Pieces <= 0 && player1PiecesPlayed <= 2) {
            currentState = 4;
            break;
          } else if (player2Pieces <= 0 && player2PiecesPlayed <= 2) {
            currentState = 4;
            break;
          } else if (player1Pieces <= 0 && player2Pieces <= 0) {
            currentState = 2;
            if (currentPlayer == 'x') {
              currentPlayer = 'o';
            } else if (currentPlayer == 'o') {
              currentPlayer = 'x';
            }

          }

          else {
            currentState = 0;
            if (currentPlayer == 'x') {
              currentPlayer = 'o';
            } else if (currentPlayer == 'o') {
              currentPlayer = 'x';
            }

            break;
          }
        }

        button->sourceRec.y = button->getBtnState() * button->getFrameHeight();
      }

      DrawButtons(collisionButtons);
    }

    // Move Pieces
    else if (currentState == 2 && !hasSelectedButton) {
      for (const auto& button : collisionButtons) {
        button->changeBtnAction(false);  // Reset btnAction for each button

        int buttonNum = button->getButtonNum();
        if (CheckCollisionPointRec(mousePoint, button->btnBounds) &&
            !button->isEmpty() && button->getOwner() == currentPlayer) {
          if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            button->changeBtnState(2);

          } else {
            button->changeBtnState(1);
          }

          if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            // Testing Up
            int buttonCol = button->getCol();
            int buttonRow = button->getRow();
            if (currentPlayer == 'x' && player1PiecesPlayed < 4 &&
                player1Pieces == 0) {
              button->changeBtnAction(true);
            } else if (currentPlayer == 'o' && player2PiecesPlayed < 4 &&
                       player2Pieces == 0) {
              button->changeBtnAction(true);
            } else if ((buttonCol - 1 >= 0) &&
                       buttons[buttonCol - 1][buttonRow]->isEmpty()) {
              button->changeBtnAction(true);
            }

            else if ((buttonCol - 2 >= 0) &&
                     buttons[buttonCol - 2][buttonRow]->isEmpty() &&
                     buttonRow != 2 && buttonRow != 4 && buttonRow != 3) {
              button->changeBtnAction(true);
            } else if ((buttonCol - 3 >= 0) &&
                       buttons[buttonCol - 3][buttonRow]->isEmpty() &&
                       buttonRow != 3) {
              button->changeBtnAction(true);
            }

            // Testing Down
            else if (((buttonCol + 1 <= 6) && (buttonRow <= 6)) &&
                     buttons[buttonCol + 1][buttonRow]->isEmpty()) {
              button->changeBtnAction(true);
            } else if ((buttonCol + 2 <= 6 && buttonRow <= 6) &&
                       (buttons[buttonCol + 2][buttonRow]->isEmpty()) &&
                       buttonRow != 2 && buttonRow != 4 && buttonRow != 3) {
              button->changeBtnAction(true);
            } else if ((buttonCol + 3 <= 6 && buttonRow <= 6) &&
                       (buttons[buttonCol + 3][buttonRow]->isEmpty() &&
                        buttonRow != 3)) {
              button->changeBtnAction(true);
            }
            // Testing Right
            else if ((((buttonRow + 3 <= 6) && (buttonCol <= 6)) &&
                      buttons[buttonCol][buttonRow + 3]->isEmpty()) &&
                     button->getCol() != 3) {
              button->changeBtnAction(true);
            } else if (((buttonRow + 2 <= 6 && buttonCol <= 6)) &&
                       (buttons[buttonCol][buttonRow + 2]->isEmpty()) &&
                       button->getCol() != 3 && button->getCol() != 4 &&
                       button->getCol() != 2) {
              button->changeBtnAction(true);
            } else if (((buttonRow + 1 <= 6 && buttonCol <= 6)) &&
                       buttons[buttonCol][buttonRow + 1]->isEmpty()) {
              button->changeBtnAction(true);
            }

            // Moving Left
            else if ((((buttonRow - 3 >= 0) && (buttonCol >= 0)) &&
                      buttons[buttonCol][buttonRow - 3]->isEmpty()) &&
                     button->getCol() != 3) {
              button->changeBtnAction(true);
            } else if (((buttonRow - 2 >= 0 && buttonCol >= 0)) &&
                       (buttons[buttonCol][buttonRow - 2]->isEmpty()) &&
                       buttonCol != 3 && buttonCol != 4 && buttonCol != 2) {
              button->changeBtnAction(true);
            } else if (((buttonRow - 1 >= 0 && buttonCol >= 0)) &&
                       buttons[buttonCol][buttonRow - 1]->isEmpty()) {
              button->changeBtnAction(true);
            } else {
              Sound errorFx = LoadSound("resources/error.mp3");
              PlaySound(errorFx);
              button->changeBtnAction(false);
            }
          }
        } else {
          button->changeBtnState(0);
        }
        if (button->getBtnAction()) {
          selectedButton = button;
          hasSelectedButton = true;
        }

        button->sourceRec.y = button->getBtnState() * button->getFrameHeight();
      }

      DrawButtons(collisionButtons);
    } else if (currentState == 2 && hasSelectedButton == true) {
      for (const auto& button : collisionButtons) {
        button->changeBtnAction(false);  // Reset btnAction for each button
        if (CheckCollisionPointRec(mousePoint, button->btnBounds) &&
            button->isEmpty() &&
            (currentPlayer == 'x' && player1PiecesPlayed < 4 &&
             player1Pieces == 0)) {
          if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            button->changeBtnState(2);
          else
            button->changeBtnState(1);

          if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            button->changeBtnAction(true);
        } else if (CheckCollisionPointRec(mousePoint, button->btnBounds) &&
                   button->isEmpty() &&
                   (currentPlayer == 'o' && player2PiecesPlayed < 4 &&
                    player2Pieces == 0)) {
          if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            button->changeBtnState(2);
          else
            button->changeBtnState(1);

          if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            button->changeBtnAction(true);
        } else if (CheckCollisionPointRec(mousePoint, button->btnBounds) &&
                   button->isEmpty() &&
                   (
                       // Moving Right
                       (selectedButton->getRow() == button->getRow() + 3 &&
                        selectedButton->getCol() == button->getCol() &&
                        button->getCol() != 3) ||
                       (selectedButton->getRow() == button->getRow() + 2 &&
                        selectedButton->getCol() == button->getCol() &&
                        button->getCol() != 3) ||
                       (selectedButton->getRow() == button->getRow() + 1 &&
                        selectedButton->getCol() == button->getCol()) ||
                       // Moving Down
                       (selectedButton->getCol() == button->getCol() + 3 &&
                        selectedButton->getRow() == button->getRow() &&
                        button->getRow() != 3) ||
                       (selectedButton->getCol() == button->getCol() + 2 &&
                        selectedButton->getRow() == button->getRow() &&
                        button->getRow() != 3 && button->getRow() != 2 &&
                        button->getRow() != 4) ||
                       (selectedButton->getCol() == button->getCol() + 1 &&
                        selectedButton->getRow() == button->getRow()) ||

                       // Moving Left
                       (selectedButton->getRow() == button->getRow() - 3 &&
                        selectedButton->getCol() == button->getCol() &&
                        button->getCol() != 3) ||
                       (selectedButton->getRow() == button->getRow() - 2 &&
                        selectedButton->getCol() == button->getCol() &&
                        button->getCol() != 3) ||
                       (selectedButton->getRow() == button->getRow() - 1 &&
                        selectedButton->getCol() == button->getCol()) ||

                       // Moving Up
                       (selectedButton->getCol() == button->getCol() - 3 &&
                        selectedButton->getRow() == button->getRow() &&
                        button->getRow() != 3) ||
                       (selectedButton->getCol() == button->getCol() - 2 &&
                        selectedButton->getRow() == button->getRow() &&
                        button->getRow() != 3 && button->getRow() != 2 &&
                        button->getRow() != 4) ||
                       (selectedButton->getCol() == button->getCol() - 1 &&
                        selectedButton->getRow() == button->getRow()))) {
          if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            button->changeBtnState(2);
          else
            button->changeBtnState(1);

          if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            button->changeBtnAction(true);
        } else {
          button->changeBtnState(0);
        }

        if (button->getBtnAction()) {
          PlaySound(button->fxButton);
          hasSelectedButton = false;
          button->changeOwner(selectedButton->getOwner());
          button->setEmpty(false);
          if (selectedButton->getOwner() == 'x') {
            button->changeSprite("resources/cross.png");
          } else if (selectedButton->getOwner() == 'o') {
            button->changeSprite("resources/circle.png");
          }

          selectedButton->changeOwner('e');
          selectedButton->setEmpty(true);
          selectedButton->changeSprite("resources/empty.png");

          ButtonAction(button, currentPlayer);
          isMill = CheckDirections(button, buttons);

          if (isMill) {
            currentState = 1;
            DrawButtons(collisionButtons);

            button->sourceRec.y =
                button->getBtnState() * button->getFrameHeight();

            break;
          }
          if (!isMill) {
            if (currentPlayer == 'x') {
              currentPlayer = 'o';
            } else if (currentPlayer == 'o') {
              currentPlayer = 'x';
            }
          }
        }

        button->sourceRec.y = button->getBtnState() * button->getFrameHeight();
      }

      DrawButtons(collisionButtons);
    }
    if (currentState == 4) {
      std::cout << "Winner: " << std::endl;
      CloseWindow();
    }
  }

  UnloadTexture(background);
  UnloadTexture(crossIndicator);
  UnloadTexture(circleIndicator);

  CleanupResources(buttons);
  return 0;
}
