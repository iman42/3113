#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cassert>
#include<random>
#include"time.h"
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase/"
#endif

class Entity{
public:
    float x = 0;
    float y = 0;
    float width = 0.1;
    float height = 0.5;
    float vertices[12] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
    float TexCoords[12] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    GLuint texture;
    float direction = 0;
    float speed = 2;
    float xComp = rand()%2-1;
    float yComp = 0;
    
    Entity(GLuint givenTexture, float givenX = 0, float givenY = 0, float givenWidth = 0.1, float givenHeight = 0.5) {texture = givenTexture, x = givenX, y = givenY, width = givenWidth, height = givenHeight;
        
        xComp = rand()%2-1;
        if (xComp == 0) {
            xComp = 1;
        }
        
    }
    
    void move(float elapsed){
        x += xComp*speed*elapsed;
        y += yComp*speed*elapsed;
    }
    
    
};
bool paddleHit(Entity paddle, Entity ball){
    if((ball.x > paddle.x-paddle.width && ball.x < paddle.x+paddle.width) && (ball.y > paddle.y-paddle.height && ball.y < paddle.y+paddle.height)){
        return true;
    }
    return false;
}

void drawThings(ShaderProgram program, Entity ent){
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ent.vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, ent.TexCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}

SDL_Window* displayWindow;
GLuint LoadTexture(const char *filePath) {
    int w,h,comp;
    unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
    if(image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    GLuint retTexture;
    glGenTextures(1, &retTexture);
    glBindTexture(GL_TEXTURE_2D, retTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(image);
    return retTexture;
}

int main(int argc, char *argv[])
{
   
    float blueScore = 0;
    float redScore = 0;
    srand(time(NULL));
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    float lastFrameTicks = 0.0f;
        
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    
    glViewport(0, 0, 1280, 720);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, 1280, 720);
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    Matrix projectionMatrix;
    projectionMatrix.SetOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
    Matrix modelviewMatrix;
    SDL_Event event;
    bool done = false;
    
    Entity blue(LoadTexture((RESOURCE_FOLDER"blueSquare.png")), 3,  0, 0.1,0.5);
    glBindTexture(GL_TEXTURE_2D, blue.texture);
    
    Entity blueScoreBoard(LoadTexture((RESOURCE_FOLDER"blueSquare.png")), 3.55, 2, 0,.6);
    glBindTexture(GL_TEXTURE_2D, blue.texture);
    
    Entity redScoreBoard(LoadTexture((RESOURCE_FOLDER"redSquare.png")), -3.55, 2, 0,.6);
    glBindTexture(GL_TEXTURE_2D, blue.texture);
    
    Entity red(LoadTexture((RESOURCE_FOLDER"redSquare.png")), -3,  0, 0.1,0.5);;
    glBindTexture(GL_TEXTURE_2D, red.texture);
    Entity ball(LoadTexture(RESOURCE_FOLDER"redSquare.png"), 0, 0, 0.1, 0.1);
    std::cout << ball.height;
    glBindTexture(GL_TEXTURE_2D, ball.texture);
    std::cout << "\n\n\n" << red.texture << "\n\n\n";
    std::cout << "\n\n\n" << blue.texture << "\n\n\n";
    std::cout << "\n\n\n" << ball.texture << "\n\n\n";
    while (!done) {
        ball.speed += 0.01;
        //glClearColor(rand()%2, rand()%2, rand()%2, rand()%2);
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program.programID);
        program.SetModelviewMatrix(modelviewMatrix);
        program.SetProjectionMatrix(projectionMatrix);
        
        //winCheck
        if (ball.x < -3.55) {
            ++blueScore;
            ball.x = 0;
            ball.y = 0;
            ball.speed = 2;
        }
        if (ball.x > 3.55) {
            ++redScore;
            ball.x = 0;
            ball.y = 0;
            ball.speed = 2;
        }
        
        //scoreCalc
        if (blueScore > 0 || redScore > 0) {
            blueScoreBoard.width = 3.55*4*(blueScore/(blueScore+redScore));
            redScoreBoard.width = 3.55*4*(redScore/(blueScore+redScore));
        }
        
        //blueScore
        modelviewMatrix.Identity();
        modelviewMatrix.Translate(blueScoreBoard.x, blueScoreBoard.y, 0.0f);
        modelviewMatrix.Scale(blueScoreBoard.width, blueScoreBoard.height, 1.0);
        program.SetModelviewMatrix(modelviewMatrix);
        glBindTexture(GL_TEXTURE_2D, blueScoreBoard.texture);
        drawThings(program, blueScoreBoard);
        
        //redScore
        modelviewMatrix.Identity();
        modelviewMatrix.Translate(redScoreBoard.x, redScoreBoard.y, 0.0f);
        modelviewMatrix.Scale(redScoreBoard.width, redScoreBoard.height, 1.0);
        program.SetModelviewMatrix(modelviewMatrix);
        glBindTexture(GL_TEXTURE_2D, redScoreBoard.texture);
        drawThings(program, redScoreBoard);
        
        
        //blue paddle
        modelviewMatrix.Identity();
        
        if(keys[SDL_SCANCODE_UP] && blue.y < 1.6) {
            blue.y += 5*elapsed;
        }else if(keys[SDL_SCANCODE_DOWN] && blue.y > -2){
            blue.y -= 5*elapsed;
        }
        modelviewMatrix.Translate(blue.x, blue.y, 0.0f);
        modelviewMatrix.Scale(blue.width, blue.height, 1.0);
        program.SetModelviewMatrix(modelviewMatrix);
        glBindTexture(GL_TEXTURE_2D, blue.texture);
        drawThings(program, blue);
        
        
        //red paddle
        modelviewMatrix.Identity();
        if(keys[SDL_SCANCODE_W] && red.y < 1.6) {
            red.y += 5*elapsed;
        }else if(keys[SDL_SCANCODE_S] && red.y > -2){
            red.y -= 5*elapsed;
        }
        modelviewMatrix.Translate(red.x, red.y, 0.0f);
        modelviewMatrix.Scale(red.width, red.height, 1.0);
        program.SetModelviewMatrix(modelviewMatrix);
        glBindTexture(GL_TEXTURE_2D, red.texture);
        drawThings(program, red);
        
        
        //ball
        ball.move(elapsed);
        modelviewMatrix.Identity();
        modelviewMatrix.Translate(ball.x, ball.y, 0.0f);
        modelviewMatrix.Scale(ball.width, ball.height, 1.0);
        program.SetModelviewMatrix(modelviewMatrix);
        glBindTexture(GL_TEXTURE_2D, ball.texture);
        drawThings(program, ball);
        
        
        
            if(paddleHit(blue, ball) && ball.xComp >= 0){
                float difference = ball.y - blue.y;
                ball.yComp = difference/blue.height;
                ball.xComp = -ball.xComp;
                ball.texture = LoadTexture(RESOURCE_FOLDER"blueSquare.png");
                
                }
            if(paddleHit(red, ball) && ball.xComp <= 0){
                float difference = ball.y - red.y;
                ball.yComp = difference/red.height;
                ball.xComp = -ball.xComp;
                ball.texture = LoadTexture(RESOURCE_FOLDER"redSquare.png");
            
        }
            if (ball.y > 1.6 && ball.yComp > 0) {
                ball.yComp = - ball.yComp;
            }
        
    
            if (ball.y < -2 && ball.yComp < 0) {
                ball.yComp = - ball.yComp;
            }
    
        
        SDL_GL_SwapWindow(displayWindow);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
    }
    SDL_Quit();
    return 0;
}

