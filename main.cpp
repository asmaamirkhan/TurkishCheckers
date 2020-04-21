#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <regex>

using namespace std;

void showMenuAndProcessOption();
void startGame();
void initBoard();
void updateGame();
void processOption(int);
void showGame();
char getCurrentPlayer();
void togglePlayer();
bool validateCommandSpell(string);
bool validateCommandLegality(string);
void continueGame();

int main()
{
  showMenuAndProcessOption();
  return 0;
}

/**
  * @brief prints the menu on the console
  */
void showMenuAndProcessOption()
{
  int option;
  cout << "Select an option please:\n"
       << "1. New game\n"
       << "2. Continue game\n"
       << "3. End game\n"
       << "Your option: ";
  cin >> option;
  processOption(option);
}

/**
  * @brief Processes the user input
  * @param option user's input, in [1, 2, 3]
  */
void processOption(int option)
{
  switch (option)
  {
  case 1:
    startGame();
    break;
  case 2:
    updateGame();
    break;
  case 3:
    exit(0);
    break;
  default:
    cout << "Invalid character, please try again";
  }
}

void startGame()
{
  initBoard();
  continueGame();
}

void continueGame()
{
  string command;
  cout << "\nYour command:  ";
  cin >> command;
  if (validateCommandSpell(command))
  {
    validateCommandLegality(command);
  }
}

bool validateCommandLegality(string command)
{
  char player = getCurrentPlayer();
  int sourceRow, sourceColumn, destRow, destColumn;

  // convert to ascii and subtract 'a' to get order of letter
  sourceColumn = tolower(command[0]) - 'a';
  destColumn = tolower(command[2]) - 'a';

  // convert to ascii and subtract '0' to get order of letter
  // then subtract from 8 to get the order in developer perspictive
  sourceRow = 8 - (command[1] - '0');
  destRow = 8 - (command[3] - '0');

  if (player == 'W')
  {
    if (destRow <= sourceRow)
      return false;
  }
  else
  {
    if (destRow >= sourceRow)
      return false;
  }
  return true;
}

bool validateCommandSpell(string command)
{
  regex pattern("[a-h][1-8][a-h][1-8]", regex_constants::icase);
  if (regex_match(command, pattern))
    return true;
  return false;
}

/**
  * @brief Inits the board on an exisiting file and creates file if it's first run
  */
void initBoard()
{
  int i, j;
  ofstream gameFile;
  gameFile.open("game.dat");

  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
    {
      if (i == 1 || i == 2)
      {
        gameFile << "W";
      }
      else if (i == 5 || i == 6)
      {
        gameFile << "B";
      }
      else
      {
        gameFile << " ";
      }
    }
    gameFile << "\n";
  }
  gameFile << "W";
  cout << "The game is initialized successfully:\n\n";
  gameFile.close();
  showGame();
}

/**
  * @brief Prints last status of game on the console
  */
void showGame()
{
  string line;
  int i = 8;
  char c;
  ifstream gameFile;

  gameFile.open("game.dat");
  cout << i << "  |";

  while (gameFile.get(c))
  {
    if (c == '\n')
    {
      i--;
      if (i == 0)
        break;
      cout << c << i << "  |";
    }
    else if (c == 'X')
    {
      cout << "\t"
           << "WW";
    }
    else if (c == 'Y')
    {
      cout << "\t"
           << "BB";
    }
    else
      cout << "\t" << c;
  }
  cout << "\n\n";
  for (i = 'A'; i < 'I'; i++)
  {
    cout << "\t" << char(i);
  }
  getCurrentPlayer();
  cout << "\n\nCurrent Player: " << getCurrentPlayer() << endl;
  gameFile.close();
}

/**
  * @brief finds current player for the exisisting round
  * @return current player 0 for W, 1 for B
  */
char getCurrentPlayer()
{
  char player;
  ifstream gameFile;
  gameFile.open("game.dat");
  gameFile.seekg(-1, ios::end);
  gameFile.get(player);
  gameFile.close();
  return player;
}

void togglePlayer()
{
  string player;
  fstream gameFile;
  gameFile.open("game.dat");
  gameFile.seekg(-1, ios::end);
  getline(gameFile, player);
  gameFile.seekg(-1, ios::cur);
  if (player == "W")
  {
    gameFile << "B";
  }
  else
  {
    gameFile << "W";
  }
  gameFile.close();
}

void updateGame()
{
  // TODO: impelemnt this
  system("CLS");
  showGame();
  cout << getCurrentPlayer();
}
