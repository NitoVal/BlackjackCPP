#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <random>
#include <string>

struct Card {
    std::string suit;
    std::string value;
};

class Deck {
private:
    std::stack<Card> cards;
public:
    Deck() {
        std::string suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
        std::string values[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace"};

        for (const auto& suit : suits) {
            for (const auto& value : values) {
                cards.push({suit, value});
            }
        }
    }

    void shuffle() {
        std::vector<Card> temp;
        while (!cards.empty()) {
            temp.push_back(cards.top());
            cards.pop();
        }
        std::random_device rd; // Create a random device to generate a seed for the random number generator. This uses an entropy source from the hardware for higher-quality randomness.
        std::mt19937 g(rd()); // Create an instance of the Mersenne Twister random number generator (std::mt19937) using the seed from the random device.
        std::shuffle(temp.begin(), temp.end(), g);
        for (const auto& card : temp) {
            cards.push(card);
        }
    }

    Card drawCard() {
        if (cards.empty()) {
            throw std::out_of_range("Deck is empty");
        }
        Card card = cards.top();
        cards.pop();
        return card;
    }
};

class Player
{
private:
    std::queue<Card> hand;
    double balance;  
    double currentBet;  
public:
    void addCard(Card card) { hand.push(card); }

    int calculateScore() {
        int score = 0;
        int aceCount = 0;
        std::queue<Card> tempHand = hand; 
        while (!tempHand.empty()) {
            Card card = tempHand.front();
            tempHand.pop();
            if (card.value == "Jack" || card.value == "Queen" || card.value == "King") {
                score += 10;
            } else if (card.value == "Ace") {
                aceCount++;
            } else {
                score += std::stoi(card.value);
            }
        }
        for (int i = 0; i < aceCount; i++) {
            if (score + 11 <= 21) {
                score += 11;
            } else {
                score += 1;
            }
        }
        return score;
    }

    bool isBusted() { return calculateScore() > 21; }

    void printHand() {
        std::cout << "Hand: ";
        std::queue<Card> tempHand = hand;
        while (!tempHand.empty()) {
            Card card = tempHand.front();
            tempHand.pop();
            std::cout << card.value << " of " << card.suit << ", ";
        }
        std::cout << '\n';
    }

    void clearHand() { 
        while (!hand.empty()) {
            hand.pop();
        }
    }
};
struct Node {
    Player player;
    Player dealer;
    Node* next;

    Node(const Player& player, const Player& dealer) : player(player), dealer(dealer), next(nullptr) {}
};
class BlackjackGame {
private:
    Deck deck;
    Player player;
    Player dealer;
    Node* head;
public:
    BlackjackGame() : head(nullptr) {}
    void startGame() {
        bool playAgain;
        do
        {
            resetGame();
            deck.shuffle();
            dealInitialCards();
            playerTurn();
            dealerTurn();
            determineWinner();
            saveGameState();
            playAgain = askForReplay();
        }
        while (playAgain);
        showMenu();
    }
    ~BlackjackGame() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
    void displayMatchHistory() {
        std::cout << "Match History:" << '\n';
        Node* current = head;
        int matchNumber = 1;
        while (current) {
            std::cout << "Match " << matchNumber << ": ";
            if (!current->dealer.isBusted() && (current->dealer.calculateScore() > current->player.calculateScore() || current->player.isBusted())) {
                std::cout << "Dealer wins!" << '\n';
            } else if (!current->player.isBusted() && (current->player.calculateScore() > current->dealer.calculateScore() || current->dealer.isBusted())) {
                std::cout << "Player wins!" << '\n';
            } else {
                std::cout << "It's a tie!" << '\n';
            }
            current = current->next;
            matchNumber++;
        }
        showMenu();
    }
    void showMenu() {
        char choice;
        std::cout << "Menu:" << '\n';
        std::cout << "1. Start New Game" << '\n';
        std::cout << "2. View Match History" << '\n';
        std::cout << "Press any other keys if you want to exit" << '\n';
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        switch (choice) {
        case '1':
            startGame();
            break;
        case '2':
            displayMatchHistory();
            break;
        default:
            std::cout << "Thank you for playing Blackjack." << '\n';
            break;
        }
    }
private:
    void dealInitialCards() {
        player.addCard(deck.drawCard());
        dealer.addCard(deck.drawCard());
        player.addCard(deck.drawCard());
        dealer.addCard(deck.drawCard());

        std::cout << "Player's ";
        player.printHand();
        std::cout << "Dealer's ";
        dealer.printHand();
    }

    void playerTurn() {
        while (true) {
            std::string choice;
            std::cout << "Hit or Stand? (h/s): ";
            std::cin >> choice;
            if (choice == "h") {
                player.addCard(deck.drawCard());
                std::cout << "Player's ";
                player.printHand();
                if (player.isBusted()) {
                    std::cout << "Player busted! Dealer wins." << '\n';
                    break;
                }
            } else if (choice == "s") {
                break;
            } else {
                std::cout << "Invalid choice. Please enter 'h' or 's'." << '\n';
            }
        }
    }

    void dealerTurn() {
        std::cout << "Dealer's ";
        dealer.printHand();
        while (dealer.calculateScore() < 17) {
            dealer.addCard(deck.drawCard());
            std::cout << "Dealer hits. Dealer's ";
            dealer.printHand();
        }
        if (dealer.isBusted()) {
            std::cout << "Dealer busted! Player wins." << '\n';
        }
    }

    void determineWinner() {
        int playerScore = player.calculateScore();
        int dealerScore = dealer.calculateScore();
        if (!player.isBusted() && (playerScore > dealerScore || dealer.isBusted())) {
            std::cout << "Player wins!" << '\n';
        } else if (!dealer.isBusted() && (dealerScore > playerScore || player.isBusted())) {
            std::cout << "Dealer wins!" << '\n';
        } else {
            std::cout << "It's a tie!" << '\n';
        }
    }
    bool askForReplay() {
        std::string choice;
        std::cout << "Do you want to play again? (y/n): ";
        std::cin >> choice;
        return choice == "y";
    }
    void resetGame() {
        player.clearHand();
        dealer.clearHand();
    }
    void saveGameState() {
        Node* newNode = new Node(player, dealer);
        
        if (!head) {
            head = newNode;
        } else {
            newNode->next = head;
            head = newNode;
        }
    }
    
};
int main() {
    BlackjackGame game;
    game.showMenu();
    return 0;
}