#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <regex>
#include <stdlib.h>
using namespace std;

// UI
void showMenuAndProcessOption();
void startGame();
void initBoard();
void initLogFile();
void processOption(int);
void showGame();
void continueGame();

// Player and Tile
char getCurrentPlayer();
char getOppositePlayer(char);
void togglePlayer();
int getCorrespondCursor(int, int);
char getTileStatus(int);
char getTileStatus(int, int);
bool isEmpty(int);

// Checker
char getCorrespondChecker(char);
bool doCheckerRoad(int, int, int, int);
bool validateCheckerRoad(char, int, int, int, int);
bool isChecker(int);

// Command validation
int *parseCommand(string);
bool validateCommandTiles(char, int, int);
bool validateCommandSpell(string);
int validateEatingAndGetNeighbor(char, int, int, int, int);
bool isCrossMovement(int, int, int, int);
bool validateLocations(char, int, int, int, int);

// Command execution
void runCommand(string);
void move(int, int, int, char);
void clearTile(int);
void makeChecker(int);
void logCommand(string);

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
  initLogFile();
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
    runCommand(command);
  }
}

void logCommand(string command)
{
  string line;
  fstream logFile;
  logFile.open("log.dat", ios::app);
  if (logFile.fail())
  {
    cerr << "An error occurred while opening log file " << endl;
    logFile.close();
    exit(1);
  }

  logFile << getCurrentPlayer() << ':' << command << endl;
  logFile.close();
}

void runCommand(string command)
{
  int *info;

  // get sourceRow, sourceCol, destRow and destCol
  info = parseCommand(command);

  // get source and row cursors
  int sourceCursor = getCorrespondCursor(info[0], info[1]);
  int destCursor = getCorrespondCursor(info[2], info[3]);

  char player = getCurrentPlayer();

  if (validateCommandSpell(command))
  {
    if (validateCommandTiles(player, sourceCursor, destCursor))
    {
      if (!isCrossMovement(info[0], info[1], info[2], info[3]))
      {
        if (isChecker(sourceCursor))
        {
          if (validateCheckerRoad(player, info[0], info[1], info[2], info[3]))
          {
            if (!doCheckerRoad(info[0], info[1], info[2], info[3]))
              togglePlayer();
            logCommand(command);
          }
        }
        else
        {
          int neighbor = validateEatingAndGetNeighbor(player, info[0], info[1], info[2], info[3]);
          if (neighbor != -1)
          {
            move(sourceCursor, destCursor, info[2], player);
            clearTile(neighbor);
            logCommand(command);
            cout << "\nEating is done\n";
            system("pause");
          }
          else if (validateLocations(player, info[0], info[1], info[2], info[3]))
          {
            move(sourceCursor, destCursor, info[2], player);
            logCommand(command);
            togglePlayer();
          }
        }
      }
    }
  }
}

bool validateLocations(char player, int sourceRow, int sourceCol, int destRow, int destCol)
{
  char sourceStatus = getTileStatus(sourceRow, sourceCol);
  if (player == 'W')
  {
    if (destRow - sourceRow > 1)
    {
      if (sourceStatus == 'X')
        return true;
      cout << "\nERROR: More than one row\n";
      system("pause");
      return false;
    }
    if (destRow < sourceRow)
    {
      if (sourceStatus == 'X')
        return true;
      cout << "\nERROR: Backward move\n";
      system("pause");
      return false;
    }
  }
  else
  {
    //7->5 N        7->6 Y
    if (sourceRow - destRow > 1)
    {
      if (sourceStatus == 'Y')
        return true;
      cout << "\nERROR: More than one row\n";
      system("pause");
      return false;
    }
    //6->7 N        7->6 Y
    if (destRow > sourceRow)
    {
      if (sourceStatus == 'Y')
        return true;
      cout << "\nERROR: Backward move\n";
      system("pause");
      return false;
    }
  }
  if (abs(sourceCol - destCol) > 1)
  {
    if (sourceStatus == 'X' || sourceStatus == 'Y')
      return true;
    cout << "\nERROR: More than one col\n";
    system("pause");
    return false;
  }
  return true;
}

bool validateCommandTiles(char player, int sourceCursor, int destCursor)
{
  // source tile check
  char status = getTileStatus(sourceCursor);
  if (status == 'X' || status == 'W')
  {
    status = 'W';
  }
  else if (status == 'Y' || status == 'B')
  {
    status = 'B';
  }
  if (status != player)
  {
    cout << "\nSOURCE" << status << endl;
    cout << "\nERROR: Invalid source tile\n";
    system("pause");
    return false;
  }

  // is destination tile empty?
  if (!isEmpty(destCursor))
  {
    cout << "\nERROR: Unavailable destination tile\n";
    system("pause");
    return false;
  }
  return true;
}

bool isCrossMovement(int sourceRow, int sourceCol, int destRow, int destCol)
{
  if (sourceRow != destRow && sourceCol != destCol)
  {
    cout << "\nERROR: Cross move\n";
    system("pause");
    return true;
  }
  return false;
}

int validateEatingAndGetNeighbor(char player, int sourceRow, int sourceCol, int destRow, int destCol)
{

  // dikey mı?
  if (player == 'W')
  {
    if (getTileStatus(destRow - 1, sourceCol) == getOppositePlayer(player) && sourceCol == destCol && destRow == sourceRow + 2)
    {
      return getCorrespondCursor(sourceRow + 1, sourceCol);
    }
  }
  else
  {
    if (getTileStatus(destRow + 1, sourceCol) == getOppositePlayer(player) && sourceCol == destCol && destRow == sourceRow - 2)
    {
      return getCorrespondCursor(sourceRow - 1, sourceCol);
    }
  }
  int neighborCol = sourceCol + (destCol - sourceCol) / 2;
  if (getTileStatus(sourceRow, neighborCol) == getOppositePlayer(player) && sourceCol != destCol)
  {
    return getCorrespondCursor(sourceRow, neighborCol);
  }
  return -1;
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

void move(int sourceCursor, int destCursor, int destRow, char player)
{
  fstream gameFile;
  char c;
  gameFile.open("game.dat");
  gameFile.seekg(sourceCursor, ios::beg);
  // get player
  gameFile.get(c);

  clearTile(sourceCursor);

  if (destRow == 0 || destRow == 7)
  {
    makeChecker(destCursor);
    gameFile.close();
  }

  else
  {
    // write on new tile
    gameFile.seekg(destCursor, ios::beg);
    gameFile << c;
    gameFile.close();
  }
}

bool validateCheckerRoad(char player, int sourceRow, int sourceCol, int destRow, int destCol)
{
  int a, b;
  // yatay
  if (sourceRow == destRow)
  {
    if (sourceCol > destCol)
    {
      a = destCol + 1;
      b = sourceCol;
    }
    else
    {
      a = sourceCol + 1;
      b = destCol;
    }
    while (a != b)
    {
      // filled two neighbor tiles case
      if (!isEmpty(getCorrespondCursor(sourceRow, a)) && !isEmpty(getCorrespondCursor(sourceRow, a + 1)))
      {
        cout << "\nfilled two neighbor tiles case\n";
        return false;
      }

      // the tile is filled with the current player case
      if (getTileStatus(sourceRow, a) == player || getTileStatus(sourceRow, a) == getCorrespondChecker(player))
      {
        cout << "\nthe tile is filled with the current player\n";
        return false;
      }

      a++;
    }
  }
  // dikey
  else
  {
    if (sourceRow > destRow)
    {
      a = destRow + 1;
      b = sourceRow;
    }
    else
    {
      a = sourceRow + 1;
      b = destRow;
    }
    while (a != b)
    {
      if (!isEmpty(getCorrespondCursor(a, sourceCol)) && !isEmpty(getCorrespondCursor(a + 1, sourceCol)))
      {
        cout << "\ndikey filled two neighbor tiles case\n";
        return false;
      }
      if (getTileStatus(a, sourceCol) == player || getTileStatus(a, sourceCol) == getCorrespondChecker(player))
      {
        cout << "\ndikey the tile is filled with the current player\n";
        return false;
      }
      a++;
    }
  }
  return true;
}

bool doCheckerRoad(int sourceRow, int sourceCol, int destRow, int destCol)
{
  bool eat = false;
  move(getCorrespondCursor(sourceRow, sourceCol), getCorrespondCursor(destRow, destCol), destRow, getCorrespondChecker(getCurrentPlayer()));
  int a, b;
  // yatay
  if (sourceRow == destRow)
  {
    if (sourceCol > destCol)
    {
      a = destCol + 1;
      b = sourceCol;
    }
    else
    {
      a = sourceCol + 1;
      b = destCol;
    }
    while (a != b)
    {
      if (getTileStatus(sourceRow, a) != ' ')
        eat = true;
      clearTile(getCorrespondCursor(sourceRow, a));
      a++;
    }
  }
  // dikey
  else
  {
    if (sourceRow > destRow)
    {
      a = destRow + 1;
      b = sourceRow;
    }
    else
    {
      a = sourceRow + 1;
      b = destRow;
    }

    while (a != b)
    {
      if (getTileStatus(a, sourceCol) != ' ')
        eat = true;
      clearTile(getCorrespondCursor(a, sourceCol));
      a++;
    }
  }
  return eat;
}

char getCorrespondChecker(char player)
{
  if (player == 'W')
    return 'X';
  else
    return 'Y';
}

void makeChecker(int cursor)
{
  fstream gameFile;
  char c;
  char player = getCurrentPlayer();
  gameFile.open("game.dat");
  gameFile.seekg(cursor, ios::beg);
  if (player == 'W')
    gameFile << 'X';
  else
    gameFile << 'Y';
  gameFile.close();
}

void clearTile(int cursor)
{
  fstream gameFile;
  char c;
  gameFile.open("game.dat");
  gameFile.seekg(cursor, ios::beg);
  gameFile << ' ';
  gameFile.close();
}

int getCorrespondCursor(int row, int column)
{
  return 10 * row + column;
}

char getTileStatus(int cursor)
{
  char status;
  ifstream gameFile;
  gameFile.open("game.dat");
  gameFile.seekg(cursor, ios::beg);
  gameFile.get(status);
  gameFile.close();
  return status;
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

bool isChecker(int cursor)
{
  char status = getTileStatus(cursor);
  return status == 'X' || status == 'Y';
}

bool isEmpty(int cursor)
{
  char status = getTileStatus(cursor);
  return status == ' ';
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

void initLogFile()
{
  ofstream logFile{"log.dat"};
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

char getOppositePlayer(char currentPlayer)
{
  if (currentPlayer == 'B')
    return 'W';
  return 'B';
}

void togglePlayer()
{
  char player;
  fstream gameFile;
  gameFile.open("game.dat");
  gameFile.seekg(-1, ios::end);
  gameFile.get(player);
  gameFile.seekg(-1, ios::end);
  if (player == 'W')
  {
    gameFile << 'B';
  }
  else
  {
    gameFile << 'W';
  }
  gameFile.close();
}