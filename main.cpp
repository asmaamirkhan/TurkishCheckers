#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <regex>
#include <stdlib.h>
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
int getCorrespondCursor(int, int);
char getTileStatus(int, int);
void test();
void move(int, int);
int *parseCommand(string);
bool checkEating(int, int, int, int);

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
    continueGame();
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
  while (command != "EXIT")
  {
    system("CLS");
    showGame();
    cout << "\nIf you want to exit write EXIT\nYour command:  ";
    cin >> command;
    int *info = parseCommand(command);
    if (validateCommandSpell(command))
    {
      if (validateCommandLegality(command))
      {
        move(getCorrespondCursor(info[0], info[1]), getCorrespondCursor(info[2], info[3]));
        togglePlayer();
      }
    }
  }
}

int *parseCommand(string command)
{
  // sourceRow, sourceColumn, destRow, destColumn;
  static int info[4];

  // convert to ascii and subtract '0' to get order of letter
  // then subtract from 8 to get the order in developer perspictive

  // sourceRow
  info[0] = 8 - (command[1] - '0');

  // sourceColumn
  info[1] = tolower(command[0]) - 'a';

  // destRow
  info[2] = 8 - (command[3] - '0');

  // destColumn
  info[3] = tolower(command[2]) - 'a';

  return info;
}

bool validateCommandLegality(string command)
{
  char player = getCurrentPlayer();
  int *info;
  info = parseCommand(command);

  // source tile check
  // sourceRow, sourceColumn
  if (getTileStatus(info[0], info[1]) != player)
  {
    cout << "\nERROR: Invalid source tile\n";
    system("pause");
    return false;
  }

  // is destination tile empty?
  // destRow, destColumn
  if (getTileStatus(info[2], info[3]) != ' ')
  {
    cout << "\nERROR: Unavailable destination tile\n";
    system("pause");
    return false;
  }

  // cross movement
  if (info[0] != info[2] && info[1] != info[3])
  {
    cout << "\nERROR: Cross move\n";
    system("pause");
    return false;
  }

  // checkEating(info[0], info[1], info[2], info[3]);

  if (player == 'W')
  {
    // destRow - sourceRow
    if (info[2] - info[0] > 1)
    {
      cout << "\nERROR: More than one row\n";
      system("pause");
      return false;
    }
    else if (info[2] - info[0] < 1)
    {
      cout << "\nERROR: Backward move\n";
      system("pause");
      return false;
    }
  }
  else
  {
    // sourceRow - destRow
    if (info[0] - info[2] > 1)
    {
      cout << "\nERROR: More than one row\n";
      system("pause");
      return false;
    }
    else if (info[0] - info[2] < 1)
    {
      cout << "\nERROR: Backward move\n";
      system("pause");
      return false;
    }
  }
  // sourceColumn - destColumn
  if (abs(info[1] - info[3]) > 1)
  {
    cout << "\nERROR: More than one col\n";
    system("pause");
    return false;
  }
  return true;
}


bool checkEating(int sourceRow, int sourceCol, int destRow, int destCol){
  char temp = getTileStatus(destRow, destCol);
  return false;
}

void move(int sourceCursor, int destCursor)
{
  fstream gameFile;
  char c;
  gameFile.open("game.dat");
  gameFile.seekg(sourceCursor, ios::beg);
  // get player
  gameFile.get(c);

  gameFile.seekg(sourceCursor, ios::beg);
  gameFile << ' ';

  // write on new tile
  gameFile.seekg(destCursor, ios::beg);

  gameFile << c;
  gameFile.close();
}

int getCorrespondCursor(int row, int column)
{
  return 10 * row + column;
}

char getTileStatus(int row, int column)
{
  // cursor position
  int cursor = getCorrespondCursor(row, column);
  char status;
  ifstream gameFile;
  gameFile.open("game.dat");
  gameFile.seekg(cursor, ios::beg);
  gameFile.get(status);
  gameFile.close();
  return status;
}

bool validateCommandSpell(string command)
{
  regex pattern("[a-h][1-8][a-h][1-8]", regex_constants::icase);
  if (regex_match(command, pattern))
    return true;
  cout << "\nERROR: Invalid command format\n";
  system("pause");
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
      cout << c << c << i << "  |";
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
  //system("CLS");
  //continueGame();
  //cout << getCurrentPlayer();
  //test();
}

void test()
{
  int i, j;
  char c;
  fstream gameFile;
  gameFile.open("game.dat");
  for (i = 0; i < 80; i++)
  {
    gameFile.seekg(i, ios::beg);
    gameFile.get(c);
    cout << i << "     " << c << endl;
  }
}