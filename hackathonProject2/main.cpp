#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <random>
#include <unistd.h>
#include "classes.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <ctime>

//g++ main.cpp -lSDL2 -lSDL2_image -lSDL2_ttf -std=c++11
bool timing = false;
int questions = 0;
player p(WIDTH / 2, HEIGHT - 100, 50, 50);

SDL_Texture* texturefromtext(std::string equation, TTF_Font* font, SDL_Color color, SDL_Renderer* target) {
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, equation.c_str(), color); 
    // now you can convert it into a texture
    SDL_Texture* Message = SDL_CreateTextureFromSurface(target, surfaceMessage);
    SDL_FreeSurface( surfaceMessage);
    return Message;
}

void timer(int hr, int min, int sec) {
  timing = true;
  int total_time = 3600 * hr + 60 * min + sec;
  time_t current_time = time(0);
  while(time(0) - current_time < total_time)
  {
    sleep(1);
  }
  timing = false;
}

std::vector<answer*> answer::answers;
int main() {
    SDL_Init( SDL_INIT_EVERYTHING );
    SDL_Window *window = SDL_CreateWindow( "Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI );
    SDL_Renderer *renderTarget = nullptr;
    if ( NULL == window ) {
        std::cout << "Could not create window: " << SDL_GetError( ) << std::endl;
        return 1;
    }
    SDL_Event windowEvent;
    renderTarget = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = nullptr;
    SDL_Surface *surface = nullptr;
    Question* currentQuestion = nullptr;
    std::string s = "bigBg.jpg";
    surface = IMG_Load(s.c_str());
    if (surface == nullptr) {
        std::cout << "couldn't load surface " << SDL_GetError() << std::endl;
    } else {
        texture = SDL_CreateTextureFromSurface(renderTarget, surface);
        SDL_FreeSurface(surface);
        surface = nullptr;
    }
    if (texture == nullptr) {
        std::cout << "couldn't load texture " << SDL_GetError() << std::endl;
    }
    TTF_Init();
    TTF_Font* roboto = TTF_OpenFont("Roboto-Medium.ttf", 24);
    SDL_Color Black = {0,0,0};
    SDL_Rect MessageRect;
    MessageRect.x = 0;
    MessageRect.y = 0;
    MessageRect.w = 300;
    MessageRect.h = 75;    
    SDL_SetRenderDrawColor(renderTarget, 255,0,0,255);
    bool isRunning = true;    
    for (int i = 0; i < 9; i++) {
        answer::answers.push_back(new answer());
        answer::answers[i]->hitbox.w = 100;
        answer::answers[i]->hitbox.h = 100;
    }
    const Uint8* keyState;
    bool questionChange = false;
    SDL_Rect scorebox = {WIDTH - 150, 0, 150, 100};
    std::thread thr(timer, 0, 2, 30);
    while (isRunning) {
        if (SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_QUIT) {
                isRunning = false;
            } else if (windowEvent.type == SDL_KEYDOWN) {
                switch(windowEvent.key.keysym.sym) {
                    case SDLK_SPACE:
                        currentQuestion->answered = true;
                        break;
                }
            }
        }
        keyState = SDL_GetKeyboardState(NULL);
        p.move(keyState, renderTarget);
        if (currentQuestion == nullptr) {
            currentQuestion = new Question();
        }
        std::string question = currentQuestion->question;

        for (int i = 0; i < answer::answers.size(); i++) {
            if (questionChange || answer::answers[i]->texture == nullptr) {
                do {
                    answer::answers[i]->hitbox.x = randNum(0, WIDTH - 100);
                    answer::answers[i]->hitbox.y = randNum(0, HEIGHT - 100);
                } while (SDL_HasIntersection(&answer::answers[i]->hitbox, &p.hitbox) || SDL_HasIntersection(&answer::answers[i]->hitbox, &MessageRect)
                || (!answer::checkUnique(answer::answers[i], i)) || SDL_HasIntersection(&answer::answers[i]->hitbox, &scorebox));
            }
        }
        SDL_RenderClear(renderTarget);
        SDL_SetRenderTarget(renderTarget, texture);
        SDL_RenderCopy(renderTarget, texture, NULL, NULL);
        SDL_RenderDrawRect(renderTarget, &p.hitbox);
        SDL_RenderFillRect(renderTarget, &p.hitbox);
        for (int i = 0; i < answer::answers.size(); i++) {
            if (questionChange || answer::answers[i]->texture == nullptr) {
                int n;
                do {
                    n = randNum(-1000, 1000);
                } while (n == currentQuestion->answer);
                if (i == currentQuestion->chosenOne) {
                    answer::answers[i]->texture = texturefromtext(std::to_string(static_cast<int>(currentQuestion->answer)), roboto, Black, renderTarget);
                } else {
                    bool negative = (randNum(0,3) == 1);
                    if (negative) {
                        n *= -1;
                    }
                    answer::answers[i]->texture = texturefromtext(std::to_string(n), roboto, Black, renderTarget);
                }
            }
            SDL_RenderCopy(renderTarget, answer::answers[i]->texture, NULL, &answer::answers[i]->hitbox);
        }
        ;
        if (answer::checkhit(p, currentQuestion)) {
            currentQuestion->answered = true;
        }
        if (questionChange) {
            questionChange = false;
        }
        SDL_RenderCopy(renderTarget, texturefromtext(question, roboto, Black ,renderTarget), NULL, &MessageRect);
        SDL_RenderCopy(renderTarget, texturefromtext("Grade: " + std::to_string(p.grade), roboto, Black, renderTarget), NULL, &scorebox);
        SDL_RenderPresent(renderTarget);
        if (currentQuestion->answered == true) {
            delete currentQuestion;
            currentQuestion = nullptr;
            questionChange = true;
            questions++;
        }
        if (p.grade < 65 || timing == false) {
            isRunning = false;
        }
    }
    thr.join();
    std::cout << "Time's up!" << std::endl;
    std::cout << "Questions Completed: " << questions << "\n";
    SDL_DestroyRenderer(renderTarget);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture);
    for (int i = 0; i < answer::answers.size(); i++) {
        SDL_DestroyTexture(answer::answers[i]->texture);
        answer::answers[i]->texture = nullptr;
    }
    renderTarget = nullptr;
    window = nullptr;
    texture = nullptr;
    SDL_Quit();
    return EXIT_SUCCESS;
}
