#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>

// Global variables
const int width = 640;
const int height = 480;
const SDL_Color black = { 0, 0, 0 };
int score = 0;
int tail_length;

// Initialization main functuions
bool init();
void update();
void close();

// Main variables for window, renderer, snake's head, fruit
SDL_Window* mainWindow = NULL;
SDL_Renderer* mainRenderer = NULL;
SDL_Rect head = { width / 2, height / 2, 20, 20 };
SDL_Rect fruit = {
    std::rand() % width,
    std::rand() % height,
    15,
    15
};

// Tail's variables
SDL_Rect tail[100];
SDL_Rect prev = { 0, 0, 20, 20 };
SDL_Rect prev2 = { 0, 0, 20, 20 };
SDL_Color tail_color;

// Define current direction of snake
enum Directions { STOP = 0, UP, DOWN, LEFT, RIGHT } dir;

// Variables for text rendering
SDL_Texture* text_texture = NULL;
TTF_Font* font = NULL;

// Variables for sounds
Mix_Chunk* move_sound = NULL;

void renderText( SDL_Renderer* renderer, std::string text, int x, int y, int w, int h );

bool init()
{
    bool success = true;

    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        mainWindow = SDL_CreateWindow( "ColorSnake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN );

        if ( mainWindow == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            mainRenderer = SDL_CreateRenderer( mainWindow, -1, SDL_RENDERER_ACCELERATED );

            if ( mainRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL_Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                if ( TTF_Init() == -1 )
                {
                    printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                    success = false;
                }
                else
                {
                    font = TTF_OpenFont( "./pixel_font.ttf", 14 );

                    if ( font == NULL )
                    {
                        printf( "Could not open font! TTF_Error: %s\n", TTF_GetError() );
                        success = false;
                    }
                }

                if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }
                else
                {
                    move_sound = Mix_LoadWAV( "./moving.wav" );

                    if ( move_sound == NULL )
                    {
                        printf( "Unable to load sound %s! SDL_mixer Error: %s\n", "./moving.wav", Mix_GetError() );
                        success = false;
                    }
                }
            }
        }
    }

    return success;
}

// Function that renders text
void renderText( SDL_Renderer* renderer, std::string text, int x, int y, int w, int h )
{
    SDL_Surface* surf = TTF_RenderText_Blended( font, text.c_str(), { 0, 0, 0 } );

    text_texture = SDL_CreateTextureFromSurface( mainRenderer, surf );

    SDL_FreeSurface( surf );
    surf = NULL;

    SDL_Rect text_box;
    text_box.x = x;
    text_box.y = y;
    text_box.w = w;
    text_box.h = h;

    SDL_RenderCopy( renderer, text_texture, NULL, &text_box );
}

// Function that contains main loop and render's methods
void update()
{
    bool game_over = false;

    SDL_Event e;

    while ( !game_over )
    {
        while ( SDL_PollEvent( &e ) != 0 )
        {
            if ( e.type == SDL_QUIT )
            {
                game_over = true;
            }
            if ( e.type == SDL_KEYDOWN )
            {
                if ( e.key.keysym.sym == SDLK_UP && dir != DOWN )
                    dir = UP;
                else if ( e.key.keysym.sym == SDLK_DOWN && dir != UP )
                    dir = DOWN;
                else if ( e.key.keysym.sym == SDLK_LEFT && dir != RIGHT )
                    dir = LEFT;
                else if ( e.key.keysym.sym == SDLK_RIGHT && dir != LEFT )
                    dir = RIGHT;
                else if ( e.key.keysym.sym == SDLK_q )
                    game_over = true;
            }
        }

        // Here is a logic down below
        switch ( dir )
        {
        case UP:
            prev.x = std::floor( head.x );
            prev.y = head.y;
            break;
        case DOWN:
            prev.x = std::floor( head.x );
            prev.y = head.y;
            break;
        case LEFT:
            prev.x = head.x;
            prev.y = std::floor( head.y );
            break;
        case RIGHT:
            prev.x = head.x;
            prev.y = std::floor( head.y );
            break;
        default:
            break;
        }

        switch ( dir )
        {
            case UP:
                Mix_PlayChannel( -1, move_sound, 0 );
                head.y -= 20;
                break;
            case DOWN:
                    Mix_PlayChannel( -1, move_sound, 0 );
                    head.y += 20;
                    break;
            case LEFT:
                    Mix_PlayChannel( -1, move_sound, 0 );
                    head.x -= 20;
                    break;
            case RIGHT:
                    Mix_PlayChannel( -1, move_sound, 0 );
                    head.x += 20;
                    break;
            default:
                    Mix_HaltMusic();
                    break;
        }

        for ( int i = 0; i < tail_length; i++ )
        {
            tail[i].w = 20;
            tail[i].h = 20;

            prev2.x = tail[i].x;
            prev2.y = tail[i].y;
            tail[i].x = prev.x;
            tail[i].y = prev.y;
            prev.x = prev2.x;
            prev.y = prev2.y;
        }

        // If fruit was caught
        if (
             head.y + head.h >= fruit.y
             && head.y <= fruit.y + fruit.h
             && head.x + head.w >= fruit.x
             && head.x <= fruit.x + fruit.w
        )
        {
            score++;
            fruit.x = std::rand() % width;
            fruit.y = std::rand() % height;
            tail_length++;
        }

        // If snake goes over board
        if ( head.x < 0 || head.x + head.w > width || head.y < 0 || head.y + head.h > height )
        {
            game_over = true;
        }

        // If snake eat itself
        int eaten = 1;

        for ( int t = 0; t < tail_length; t++ )
        {
            if ( head.x == tail[t].x
                 && head.y == tail[t].y
                 && t != 0
            )
            {
                eaten = t;
            }
        }

        std::string total_score = "SCORE: " + std::to_string( score );
        
        // Here is all thing linked with render
        SDL_SetRenderDrawColor( mainRenderer, 0xff, 0xff, 0xff, 0xff );
        SDL_RenderClear( mainRenderer );

            // Head drawning
        SDL_SetRenderDrawColor( mainRenderer, 0xAA, 0x12, 0xAD, 0xff );
        SDL_RenderFillRect( mainRenderer, &head );
            // Fruit drawning
        SDL_SetRenderDrawColor( mainRenderer, 0x00, 0xff, 0x00, 0xff );
        SDL_RenderFillRect( mainRenderer, &fruit );
            // Drow score text
        renderText( mainRenderer, total_score.c_str(), 20, height - 50, 100, 40 );

            // Drow tails
        for( int i = 0; i < tail_length; i++ )
        {
            if ( i != 1 && i == eaten )
            {
                tail_color.r = 0xff;
                tail_color.g = 0x00;
                tail_color.b = 0x00;
                tail_color.a = 0xff;

                SDL_SetRenderDrawColor( mainRenderer, tail_color.r, tail_color.g, tail_color.b, tail_color.a );

                game_over = true;
            }
            else
            {
                tail_color.r = 0x00;
                tail_color.g = 0x00;
                tail_color.b = 0x00;
                tail_color.a = 0xff;

                SDL_SetRenderDrawColor( mainRenderer, tail_color.r, tail_color.g, tail_color.b, tail_color.a );
            }

            SDL_RenderFillRect( mainRenderer, &tail[i] );
        }

        SDL_RenderPresent( mainRenderer );

        SDL_Delay( 150 );
    }
}

void close()
{
    Mix_FreeChunk( move_sound );
    move_sound = NULL;

    SDL_DestroyTexture( text_texture );
    TTF_CloseFont( font );
    text_texture = NULL;
    font = NULL;

    SDL_DestroyRenderer( mainRenderer );
    SDL_DestroyWindow( mainWindow );
    mainWindow = NULL;
    mainRenderer = NULL;

    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    if ( !init() )
    {
        printf( "Failed to initialize!" );
    }
    else
    {
        update();
    }

    close();

    return 0;
}