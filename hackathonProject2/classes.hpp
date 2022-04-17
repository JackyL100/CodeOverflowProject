#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <stack>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const char operations[4] = {'+','-','*','/'};

class player;
 
const int WIDTH = 960;
const int HEIGHT = 640;

int randNum(int const &min, int const &max) {
    std::random_device randDev;
    std::mt19937 rng(randDev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(min,max);
    return dist6(rng);
}

class Node {
    private:
    const std::string i = "i";
    bool isInt(int x) {
        return typeid(x).name() == i;
    }
    public:
    std::random_device randDev;
    int points;
    double answer;
    bool answered;
    std::string question;
    const static int max = 100;
    const static int min = -100;    
    Node() {
        answered = false;
    }
    int calculate(std::string s) {
        int len = s.length();
        if (len == 0) return 0;
        std::stack<int> stack;
        int currentNumber = 0;
        char operation = '+';
        for (int i = 0; i < len; i++) {
            char currentChar = s[i];
            if (isdigit(currentChar)) {
                currentNumber = (currentNumber * 10) + (currentChar - '0');
            }
            if (!isdigit(currentChar) && !iswspace(currentChar) || i == len - 1) {
                if (operation == '-') {
                    stack.push(-currentNumber);
                } else if (operation == '+') {
                    stack.push(currentNumber);
                } else if (operation == '*') {
                    int stackTop = stack.top();
                    stack.pop();
                    stack.push(stackTop * currentNumber);
                } else if (operation == '/') {
                    int stackTop = stack.top();
                    stack.pop();
                    stack.push(stackTop / currentNumber);
                }
                operation = currentChar;
                currentNumber = 0;
            }
        }
        int result = 0;
        while (stack.size() != 0) {
            result += stack.top();
            stack.pop();
        }
        return result;
    }
};

class player {
    private:
    int velo;
    public:
    SDL_Scancode keys[4];
    int grade = 100;
    int correct = 0;
    int longestTime = 0;
    std::vector<Node*> tough;
    SDL_Rect hitbox;
    player(int x, int y, int w, int h) {
        hitbox.x = x;
        hitbox.y = y;
        hitbox.w = w;
        hitbox.h = h;
        keys[0] = SDL_SCANCODE_W;
        keys[1] = SDL_SCANCODE_S;
        keys[2] = SDL_SCANCODE_A;
        keys[3] = SDL_SCANCODE_D;
        velo = 2;
    }
    void move(const Uint8 *keystate, SDL_Renderer *target) {
        if (keystate[keys[0]]) {
            if (hitbox.y > 2) {
                hitbox.y -= 2;
            } 
        } else if (keystate[keys[1]]) {
            if (hitbox.y < HEIGHT - hitbox.h) {
                hitbox.y += 2;
            }
        } else if (keystate[keys[2]]) {
            if (hitbox.x > 5) {
                hitbox.x -= 2;
            }
        } else if (keystate[keys[3]]) {
            if (hitbox.x < WIDTH - hitbox.w) {
                hitbox.x += 2;
            }
        }
    }
};

class Question : public Node{
    public:
    int chosenOne;
    Question() : Node() {
        question += std::to_string(generateRandomNum(min, max));
        question += operations[generateRandomNum(0,3)];
        question += std::to_string(generateRandomNum(0,max));
        answer = calculate(question);
        points = 3;
        chosenOne = generateRandomNum(0, 7);
    }
    int generateRandomNum(int min, int max) {
        std::mt19937 rng(randDev());
        std::uniform_int_distribution<std::mt19937::result_type> dist6(min,max);
        return dist6(rng);
    }
};

class answer {
    public:
    SDL_Rect hitbox;
    SDL_Texture* texture;
    static std::vector<answer*> answers;
    static bool checkhit(player &p, Question* q) {
        for (int i = 0; i < answers.size(); i++) {
            if (SDL_HasIntersection(&p.hitbox, &answers[i]->hitbox)) {
                if (i == q->chosenOne) {
                    p.grade += q->points;
                } else {
                    p.grade -= 10;
                }
                return true;
            }
        }
        return false;
    }
    static bool checkUnique(answer* an, int pos) {
        for (int i = 0; i < pos; i++) {
            if (SDL_HasIntersection(&an->hitbox, &answers[i]->hitbox)) {
                return false;
            }
        }
        return true;
    }
};
