#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib> 
#include <ctime> //cstdlib and ctime are used for random number generation
using namespace std;

/*GLOBAL VARIABLES*/

//Constant strings to change the colour of the text and the highlight of the text (HL for highlight)
const string RESET = "\033[0m";
const string BLACKTEXT = "\033[30m";
const string REDTEXT = "\033[31m";
const string REDHL = "\033[41m";
const string GREENTEXT = "\033[32m"; 
const string GREENHL = "\033[42m";
const string YELLOWTEXT = "\033[33m";
const string YELLOWHL = "\033[43m";
const string BLUETEXT = "\033[34m";
const string BLUEHL = "\033[44m";
const string PURPLETEXT = "\033[35m";
const string PURPLEHL = "\033[45m";
const string CYANTEXT = "\033[36m";
const string CYANHL = "\033[46m";
const string WHITETEXT = "\033[37m";
const string WHITEHL = "\033[47m";
string colours[2][7] = {{"",REDTEXT,GREENTEXT,YELLOWTEXT,BLUETEXT,PURPLETEXT,CYANTEXT}, {"",REDHL,GREENHL,YELLOWHL,BLUEHL,PURPLEHL,CYANHL}}; //a 1-indexed 2D array that will be used to print output for each player in their selected colours

//Constant string arrays with faces and suits, sorted in increasing order of value
const string FACES[13] = {"Deuce", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King", "Ace"};
const string SUITS[4] = {"Spades", "Hearts", "Diamonds", "Clubs"};

//Maps used to convert from the constant string arrays to their values for easier card comparison
map<string, int> convertFaces; //key: face, value: numerical equivalent of face
map<string, int> convertSuits; //key: suit, value: values assigned from 1-4 to represent the suit's relative value

//Player info
string p1_name, p2_name;
int p1_colour, p2_colour;
int p1_score = 0, p2_score = 0;

//Vectors of pair<string,string> to represent card decks (original deck, each player's deck, each player's discard pile); pair<string,string> represents each card as {face, suit}
vector < pair<string,string> > originalDeck, p1_deck, p2_deck, p1_discard, p2_discard;

/*METHODS*/

/*setup method
Purpose: creating the original deck, creating conversion maps (all variables defined above)
*/
void setup() {
  //Creating the original deck
  for (int i=0; i<13; i++) {
    for (int j=0; j<4; j++) {
      pair<string, string> currentCard = {FACES[i], SUITS[j]}; //build the currentCard by selecting a face and a suit
      originalDeck.push_back(currentCard); //add the currentCard pair to the originalDeck vector
    }
  }
  //Creating maps to convert from faces and suits to numerical values 
  for (int i=0; i<13; i++) {
    convertFaces.insert({FACES[i], i+2});
  }
  for (int i=0; i<4; i++) {
    convertSuits.insert({SUITS[i], i+1});
  }
}

/*pressEnter method
Purpose: spacing large sections of output to improve readability and create a game-like environment
Parameters: string that describes what is coming after pressing ENTER
*/
void pressEnter(string message) {
  cout << endl << "[PRESS ENTER TO " << message << "]" << endl;
  string empty;
  getline(cin, empty);
}

/*printWithCircles method
Purpose: printing messages with a border of circles
Parameter: string message to be printed
*/
void printWithCircles(string message) {
  if (message.length() %2 == 0) { //only handling messages with an odd length
    message += " ";
  }
  //Building the row of ○ ○ ○...
  string circleSegment = " ○ ";
  for (int i=0; i<=message.length(); i+=2) {
    circleSegment += "○ ";
  }
  circleSegment += "○ ";
  //Printing completed arrangement
  cout << BLACKTEXT << WHITEHL << circleSegment << RESET << endl;
  cout << BLACKTEXT << WHITEHL << " ○ " << message << " ○ " << RESET << endl;
  cout << BLACKTEXT << WHITEHL << circleSegment << RESET << endl;
}

/*getColourChoice
Purpose: displaying colour options, prompting users for a valid colour choice, error-trapping to ensure validity (and repeatedly prompting until a valid choice is made)
Parameters: string that adds "DIFFERENT " to indicate that P2 cannot choose the same colour as P1
Return: int that is the player's colour choice
*/
int getColourChoice(string extraMessage) {
  //Displaying colour options
  cout << REDHL<<"1. Red"<<RESET<<"   " << GREENHL<<"2. Green"<<RESET<<"   " << YELLOWHL<<"3. Yellow"<<RESET<<"   " << BLUEHL<<"4. Blue"<<RESET<<"   " << PURPLEHL<<"5. Purple"<<RESET<<"   " << CYANHL<<"6. Cyan"<<RESET << endl;
  //Prompting
  cout << "Please select a " << extraMessage << "colour from the options listed above (enter a digit from 1-6): ";
  string tempColourChoice;
  int colourChoice;
  //Getting user input until a valid entry is made
  while (true) {
    cin >> tempColourChoice;
    if (tempColourChoice.length() != 1) {
      cout << "Invalid colour choice. Please enter a single digit from 1-6: ";
      continue;
    }
    if (!isdigit(tempColourChoice.at(0))) {
      cout << "Invalid colour choice. Please enter a numerical digit from 1-6: ";
      continue;
    }
    colourChoice = stoi(tempColourChoice); //convert after confirming the input is an int
    if (colourChoice < 1 || colourChoice > 6) {
      cout << "Invalid colour choice. Please enter a digit from 1-6: ";
      continue;
    }
    if (colourChoice == p1_colour) { //p2 chose the same colour as p1
      cout << "Invalid colour choice. Please do not choose the same colour as P1: ";
      continue;
    }
    else { //valid colour choice
      string empty;
      getline(cin, empty); //to re-align cin
      cout << endl;
      return colourChoice;
    }
  }
}
/*welcome
Purpose: welcoming the users, providing game instructions, and collecting player info (names, colour choice)
*/
void welcome() {
  printWithCircles("Welcome to the War Game !");
  //Game instructions
  pressEnter("READ THE GAME INSTRUCTIONS");
  cout << "War is a 2 player card game played with a standard 52-card deck. The goal of both players (" << BLACKTEXT << WHITEHL << "P1" << RESET << " and " << BLACKTEXT << WHITEHL << "P2" << RESET << ") is to win all 52 cards by winning battles." << endl;
  pressEnter("CONTINUE");
  cout << "To start, the deck is shuffled and divided evenly among the 2 players (26 cards in each deck). In each battle, both players will draw the top 4 cards from their deck, look at them, and choose the order to play them in. The 4 cards drawn will be labelled 1, 2, 3, and 4. To select an order, you must enter a 4-digit sequence with each of these digits appearing exactly once (e.g. 1234, 3412, 4123, etc.)." << endl;
  pressEnter("CONTINUE");
  cout << "Once each player has chosen the order their cards will be played, their cards are compared one-on-one in this order. In each of these 4 sub-battles, the player with the stronger card wins and adds both the card they played and the other player’s losing card to their discard pile." << endl << endl;
  cout << "The stronger card is the one with the higher face value: " << BLACKTEXT << WHITEHL << "Ace > King > Queen > Jack > Ten > Nine > Eight > Seven > Six > Five > Four > Three > Deuce" << RESET << ". If two cards have the same face value, then the stronger card is the one with the higher suit value: " << BLACKTEXT << WHITEHL << "Clubs > Diamonds > Hearts > Spades" << RESET << "." << endl;
  pressEnter("CONTINUE");
  cout << "P1 and P2 continue battling repeatedly. If a player is unable to draw 4 cards from their deck, their discard pile is shuffled into their deck. However, if a player is unable to draw 4 cards from their deck even with the discard pile added in (i.e. they have fewer than 4 cards in total), then they LOSE." << endl << endl << "At the end of a game of War, the winner will earn 100 points. Then, you may choose to have a rematch by playing again!" << endl;
  pressEnter("BEGIN");

  //Collecting P1 info:
  cout << "P1 - Please enter your name: ";
  getline(cin, p1_name); //getline is used instead of cin to catch the player's entire name, in case they enter more than one word
  cout << endl << "Nice to meet you, " << p1_name << "! :)" << endl;
  p1_colour = getColourChoice("");  
  //Collecting P2 info:
  cout << "P2 - Please enter your name: ";
  getline(cin, p2_name); 
  cout << endl << "Nice to meet you, " << p2_name << "! :)" << endl;
  p2_colour = getColourChoice("DIFFERENT ");

  cout << BLACKTEXT << WHITEHL << "Great! And now... it's time to play War!" << RESET << endl;
  cout << endl << colours[1][p1_colour] << p1_name << RESET <<" and " << colours[1][p2_colour] << p2_name << RESET << ", ARE YOU READY?" << RESET << endl;
  pressEnter("PLAY");
}

/*shuffle
Purpose: shuffling the card elements in a given deck
Parameters: deck that is to be shuffled
Return: shuffled deck (a vector of pair<string, string>)
*/
vector < pair<string, string> > shuffle(vector < pair<string, string> > deck) {
  //Create a copy of the inputted deck; the shuffled deck will be created by repeatedly randomly removing cards from the copy and populating the shuffled deck with the removed cards
  vector < pair<string, string> > copyDeck(deck);
  vector < pair<string, string> > shuffledDeck;
  srand(time(0)); //use time(0) to set a random seed
  while (copyDeck.size() > 0) {
    int removeCard = rand()%copyDeck.size(); //randomly generated index of card to be removed
    shuffledDeck.push_back(copyDeck[removeCard]); //add this card to shuffledDeck
    copyDeck.erase(copyDeck.begin() + removeCard); //erase this card from copyDeck to avoid drawing the same card twice
  }
  return shuffledDeck;  
}
/*resetGame
Purpose: clear all card piles and redistribute the 52 cards evenly and randomly among the 2 players
*/
void resetGame() {
  //Clear all card piles
  p1_deck.clear();
  p1_discard.clear();
  p2_deck.clear();
  p2_discard.clear();
  //Creating p1_deck and p2_deck by shuffling the original deck and splitting it in half
    vector < pair<string, string> > shuffledDeck = shuffle(originalDeck);
    for (int i=0; i<26; i++) {
      p1_deck.push_back(shuffledDeck[i]); //add the first half of the deck to p1_deck
    }
    for (int i=26; i<52; i++) {
      p2_deck.push_back(shuffledDeck[i]); //add the other half of the deck to p2_deck
    }
}
/*isValidOrder
Purpose: verifying if the user's order entry is valid
Parameter: user's input
Return: true if valid, false if invalid
*/
bool isValidOrder(string order) {
  if (order.length() != 4) { //input is not a 4-digit entry
    cout << "Invalid order choice. Please enter a 4-digit number: ";
    return false;
  }
  bool usedDigits[5] = {false, false, false, false, false}; //1-index array that determines if a given digit has already appeared in the order choice; helps catch entries like 1134
  for (int i=0; i<4; i++) {
    if (!isdigit(order.at(i)) || stoi(order.substr(i,1)) > 4 || stoi(order.substr(i,1)) < 1) { //input contains characters that are not 1, 2, 3, or 4
      cout << "Invalid order choice. Please enter a number consisting of only the digits 1-4: ";
      return false;
    }
    if (usedDigits[stoi(order.substr(i,1))]) { //if a digit appears in the input, the corresponding array position will be set to true; if the position is already set to true, then it means a duplicate digit was used
      cout << "Invalid order choice. Please use each of the digits 1-4 exactly once: ";
      return false;
    }
    usedDigits[stoi(order.substr(i,1))] = true; //if the current digit was valid, set its value in the usedDigits array to be true
  }
  return true; //false has not yet been returned, so return true
}
/*getOrderChoice
Purpose: displaying the user's 4 cards, prompting for their order choice, error-trapping to ensure the entry is a permutation of 1234
Parameters: int that is the player number (to access the correct information), 2D array of pair<string,string> containing both players' current hands
Return: string that is the player's valid order choice
*/
string getOrderChoice(int playerNum, pair<string, string> currentHands[2][4]) {
  //Displaying with the correct highlight colour
  if (playerNum == 1) {
    cout << colours[1][p1_colour]; 
  }
  else {
    cout << colours[1][p2_colour];
  }
  cout << "P" << playerNum << " - Here are your 4 cards:" << RESET << endl;
  //Loop through the corresponding currentHands array to output the 4 top cards
  for (int i=0; i<4; i++) {
    cout << i+1 << ". " << currentHands[playerNum-1][i].first << " of " << currentHands[playerNum-1][i].second << endl;    
  }
  //Displaying with the correct text colour
  if (playerNum == 1) {
    cout << colours[0][p1_colour]; 
  }
  else {
    cout << colours[0][p2_colour];
  }
  cout << "Enter the order you would like to play your cards in (enter the corresponding 4-digit number): ";
  string choice;
  bool validChoice = true;
  do {
    cin >> choice;
    if (!isValidOrder(choice)) { //invalid choice, keep looping
      validChoice = false;
    }
    else { //valid order choice
      break;
    }
  }
  while (!validChoice);
  cout << RESET << endl;
  return choice;
}
/*compareCards
Purpose: comparing each pairing of cards for the 4 sub-battles in each battle
Parameter: both players' order choices, stored in a 2D pair<string,string> (representing face&suit) array
*/
void compareCards(pair<string, string> orderChoices[2][4]) {
  for (int i=0; i<4; i++) { //loop 4 times to cover all sub-battles
    if (convertFaces[orderChoices[0][i].first] > convertFaces[orderChoices[1][i].first]) { //P1's card has a larger face
      //p1_discard gains both cards
      p1_discard.push_back(orderChoices[0][i]);
      p1_discard.push_back(orderChoices[1][i]);
      cout << colours[0][p1_colour] << "Sub-battle " << i+1 << ": P1 wins with " << orderChoices[0][i].first << " of " << orderChoices[0][i].second;
    }
    else if (convertFaces[orderChoices[0][i].first] < convertFaces[orderChoices[1][i].first]) { //P2's card has a larger face
      //p2_discard gains both cards
      p2_discard.push_back(orderChoices[0][i]);
      p2_discard.push_back(orderChoices[1][i]);
      cout << colours[0][p2_colour] << "Sub-battle " << i+1 << ": P2 wins with " << orderChoices[1][i].first << " of " << orderChoices[1][i].second;
    }
    else { //matching faces → suit tiebreaker
      if (convertSuits[orderChoices[0][i].second] > convertSuits[orderChoices[1][i].second]) { //P1's card has a larger suit
        //p1_discard gains both cards
        p1_discard.push_back(orderChoices[0][i]);
        p1_discard.push_back(orderChoices[1][i]);
        cout << colours[0][p1_colour] << "Sub-battle " << i+1 << ": P1 wins with " << orderChoices[0][i].first << " of " << orderChoices[0][i].second;
      }
      else { //P2's card has a larger suit
        //p2_discard gains both cards
        p2_discard.push_back(orderChoices[0][i]);
        p2_discard.push_back(orderChoices[1][i]);
        cout << colours[0][p2_colour] << "Sub-battle " << i+1 << ": P2 wins with " << orderChoices[1][i].first << " of " << orderChoices[1][i].second;
      }
    }
    cout << RESET << endl;
  }  
}
/*battle
Purpose: executing the battle
Parameter: battle number
*/
void battle(int battleNum) {  
  cout << BLACKTEXT << WHITEHL << " * * * BATTLE " << battleNum <<" * * * " <<RESET << endl;
  //Shuffle in cards if the deck size is below 4
  if (p1_deck.size() < 4) {
    cout << colours[1][p1_colour] << "P1 - Shuffling discard pile..." << RESET << endl;
    //add any leftover cards from p1_deck to the discard pile
    p1_discard.insert(p1_discard.end(), p1_deck.begin(), p1_deck.end()); 
    //place the shuffled discard pile in p1_deck and clear the discard pile
    p1_deck = shuffle(p1_discard);
    p1_discard.clear();
  }
  if (p2_deck.size() < 4) {
    cout << colours[1][p2_colour] << "P2 - Shuffling discard pile..." << RESET << endl;
    p2_discard.insert(p2_discard.end(), p2_deck.begin(), p2_deck.end()); 
    p2_deck = shuffle(p2_discard);
    p2_discard.clear();
  }
  //Display the size of each player's deck and discard piles
  cout << endl << "~ CURRENT CARD COUNT ~" << endl;
  cout << colours[0][p1_colour] << "P1 deck: " << p1_deck.size() << " cards" << endl;
  cout << "P1 discard: " << p1_discard.size() << " cards" << RESET << endl;
  cout << colours[0][p2_colour] << "P2 deck: " << p2_deck.size() << " cards" << endl;
  cout << "P2 discard: " << p2_discard.size() << " cards" << RESET << endl << endl;
  
  //Select the top 4 cards from each player's deck - these are their current hands
  pair<string, string> currentHands[2][4] = {
    {p1_deck[0], p1_deck[1], p1_deck[2], p1_deck[3]}, 
    {p2_deck[0], p2_deck[1], p2_deck[2], p2_deck[3]}
    };
  //Remove these cards from their respective decks
  p1_deck.erase(p1_deck.begin(), p1_deck.begin()+4);
  p2_deck.erase(p2_deck.begin(), p2_deck.begin()+4);
  //Get the players' order choices (for how they wish to play their four cards)
  string p1_orderChoice = getOrderChoice(1, currentHands);
  string p2_orderChoice = getOrderChoice(2, currentHands);
  //Place chosen order of cards into a new 2D array
  pair<string, string> orderChoices[2][4]; 
  for (int i=0; i<4; i++) { 
    orderChoices[0][i] = currentHands[0][stoi(p1_orderChoice.substr(i,1))-1];
    orderChoices[1][i] = currentHands[1][stoi(p2_orderChoice.substr(i,1))-1];
  }  
  //Determine and display results
  cout << "~ BATTLE " << battleNum << " RESULTS ~" << endl;
  compareCards(orderChoices);
  string empty;
  getline(cin, empty);
}
/*displayWarWinner
Purpose: printing congratulatory message to the war winner and printing updated scores
Parameters: winner's colour choice, name, and the war number
*/
void displayWarWinner(int winnerColour, string winnerName, int warNum) {
  cout << endl << BLACKTEXT << WHITEHL << " * * * WE HAVE A WINNER! * * * " << RESET << endl << endl;
  cout << colours[1][winnerColour] << winnerName << " wins WAR " << warNum << "! Congratulations!" << RESET << endl;
  pressEnter("SEE UPDATED SCORES");
  cout << colours[0][p1_colour] << "P1: " << p1_score << RESET << endl;
  cout << colours[0][p2_colour] << "P2: " << p2_score << RESET << endl;
}
/*getAgainChoice
Purpose: determining if the players wish to start a new game of war (and only accept valid entries)
Return: true if "YES", false if "NO"
*/
bool getAgainChoice() {
  cout << "Would you like to start another game of War? Enter YES or NO (case sensitive): ";
  string againChoice;
  bool isValid = true;
  do {
    cin >> againChoice;
    if (againChoice == "NO") {
      return false;
    }
    else if (againChoice == "YES") {
      return true;
    }
    else {
      cout << "Invalid input. Please enter either YES or NO (match text exactly): ";
      isValid = false;
    }
  }
  while (!isValid); //continue looping until valid input
  return true;
}
/*gameConclusion
Purpose: printing final results, determining overall winner, thanking user and saying goodbye
*/
void gameConclusion() {
  cout << BLACKTEXT << WHITEHL << " * * * FINAL RESULTS * * * " << RESET << endl << endl;
  //Determining which player won by comparing their scores
  if (p1_score > p2_score) {
    cout << colours[1][p1_colour] << p1_name << " IS THE OVERALL WINNER!" << RESET << endl;
  }
  else if (p2_score > p1_score) {
    cout << colours[1][p2_colour] << p2_name << " IS THE OVERALL WINNER!" << RESET << endl;
  }
  else {
    cout << BLACKTEXT << WHITEHL << "IT'S A TIE! " << WHITETEXT << colours[1][p1_colour] << p1_name << BLACKTEXT << WHITEHL << " and " << WHITETEXT << colours[1][p2_colour] << p2_name << BLACKTEXT << WHITEHL << ", well done to both of you!" << RESET << endl;
  }
  cout << endl << "Hope you enjoyed playing the War Game!" << endl << endl;
  printWithCircles("Goodbye! :)");
}

int main() {
  setup();
  welcome(); 
  bool playAgain = true;
  int warNum = 0; //to count the number of rounds of War played
  do {
    resetGame();
    warNum++;
    printWithCircles(("WAR " + to_string(warNum)));
    cout << endl;

    int p1_totalCards, p2_totalCards;
    int battleNum = 0;
    while (true) {
      battleNum++;
      battle(battleNum);
      p1_totalCards = p1_deck.size() + p1_discard.size();
      p2_totalCards = p2_deck.size() + p2_discard.size();
      if (p1_totalCards < 4) { //p1 loses
        p2_score += 100;
        displayWarWinner(p2_colour, p2_name, warNum);
        break;
      }
      else if (p2_totalCards < 4) { //p2 loses
        p1_score += 100;
        displayWarWinner(p1_colour, p1_name, warNum);
        break;
      }
      pressEnter("BEGIN THE NEXT BATTLE"); //only executes if the war has not ended yet (i.e. loop has not been broken out of)
    }    
    cout << endl;
    playAgain = getAgainChoice();
    cout << endl;
  }
  while (playAgain);
  //If the above loop has been exited, the game is over
  gameConclusion();
  return 0;
}