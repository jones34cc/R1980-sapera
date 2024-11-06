#include <iostream>
#include <raylib.h>
#include <deque>
#include<raymath.h>


using namespace std;

Color green = {173, 204, 96, 255};
Color lightgreen={144,238,144,255};
Color darkgreen = {43, 51, 24, 255};
Color black = {0, 0, 0, 255};
Color white={255,255,255,255};
Color retro={150,176,5,255};
int cellsize = 30;
int cellcount = 25;
int offset=75;
double lastupdatetime=0;

bool eventtriggered(double interval)
{
    double currenttime=GetTime();
    if(currenttime-lastupdatetime>=interval)
    {
        lastupdatetime=currenttime;
        return true;
    }
    return false;
}

bool elementindeque(Vector2 element, deque<Vector2> deque)
{
    for(unsigned int i=0;i<deque.size();i++)
    {
        if(Vector2Equals(deque[i],element))
        {
            return true;
        }
    }
    return false;

}

class snake
{
public:
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1,0};
    bool addsegment=false;
    void draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset+x * cellsize,offset+ y * cellsize, (float)cellsize, (float)cellsize};
            DrawRectangleRounded(segment, 0.5, 6, black);
        }
    }
    void update()
    {
        body.push_front(Vector2Add(body[0],direction));
        if(addsegment==true)
        {
            
            addsegment=false;

        }
        else{
            body.pop_back();
        }
    }
    void reset()
    {
        body={Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction={1,0};
    }
};

class food
{
public:
    Vector2 position;
    Texture2D texture;
    food(deque<Vector2> snakebody)
    {
        Image img = LoadImage("apple.png");
        texture = LoadTextureFromImage(img);
        UnloadImage(img);
        position = generaterandompos(snakebody);
    }
    ~food()
    {
        UnloadTexture(texture);
    }

    void draw()
    {
        DrawTexture(texture, offset+position.x * cellsize,offset+ position.y * cellsize, WHITE);
    }

    Vector2 generaterandomcell()
    {
        float x = GetRandomValue(0, cellcount - 1);
        float y = GetRandomValue(0, cellcount - 1);
        return Vector2{x,y};

    }

    Vector2 generaterandompos(deque<Vector2> snakebody)
    {
        Vector2 position= generaterandomcell();
        while(elementindeque(position,snakebody))
        {
            position=generaterandomcell();

        }
        return position;
        
    }
};

class game{
    public:
    snake snakeobj;
    food foodobj= food(snakeobj.body);
    bool running=true;
    int score=1;
    Sound eatsound;
    Sound wallsound;

    game()
    {
        InitAudioDevice();
        eatsound=LoadSound("snake_eat_sound.mp4");
        wallsound=LoadSound("collision_sound.mp4");
    }
    ~game()
    {
        UnloadSound(eatsound);
        UnloadSound(wallsound);
        CloseAudioDevice();
    }

    void draw()
    {
        foodobj.draw();
        snakeobj.draw();
    }

    void update()
    {
        if(running)
        {
        snakeobj.update();
        checkcollisionwithfood();
        checkcollisionwithedges();
        checkcollisionwithtail();
        }
    }

    void checkcollisionwithfood()
    {
        if(Vector2Equals(snakeobj.body[0],foodobj.position))
        {
            foodobj.position=foodobj.generaterandompos(snakeobj.body);
            snakeobj.addsegment=true;
            score++;
            PlaySound(eatsound);
        }
    }

    void checkcollisionwithedges()
    {
        if(snakeobj.body[0].x==cellcount|| snakeobj.body[0].x==-1)
        {
            gameover();
        }
        if(snakeobj.body[0].y==cellcount|| snakeobj.body[0].y==-1)
        {
            gameover();
        }
    }

    void gameover()
    {
        snakeobj.reset();
        foodobj.position=foodobj.generaterandompos(snakeobj.body);
        running=false;
        score=0;
        PlaySound(wallsound);
    }

    void checkcollisionwithtail()
    {
        deque<Vector2>headlessbody=snakeobj.body;
        headlessbody.pop_front();
        if(elementindeque(snakeobj.body[0],headlessbody))
        {
            gameover();

        }
    }

};

int main()
{
    cout << "Starting the game..." << endl;
    InitWindow(2*offset+cellsize*cellcount,2*offset+ cellsize * cellcount, "R1980-sapera");
    SetTargetFPS(60);
    game gameobj;

    while (WindowShouldClose() == false)
    {
        BeginDrawing();
        // drawing
        if(eventtriggered(0.2))
        {
            gameobj.update();
        }
        if((IsKeyPressed(KEY_UP)||IsKeyPressed(KEY_W)) && gameobj.snakeobj.direction.y != 1)
        {
            gameobj.snakeobj.direction={0,-1};
            gameobj.running=true;
        }
        if((IsKeyPressed(KEY_DOWN)||IsKeyPressed(KEY_S))&&gameobj.snakeobj.direction.y != -1)
        {
            gameobj.snakeobj.direction={0,1};
            gameobj.running=true;
        }
        if((IsKeyPressed(KEY_LEFT)||IsKeyPressed(KEY_A))&& gameobj.snakeobj.direction.x != 1)
        {
            gameobj.snakeobj.direction={-1,0};
            gameobj.running=true;
        }
        if((IsKeyPressed(KEY_RIGHT)||IsKeyPressed(KEY_D)) && gameobj.snakeobj.direction.x!= -1)
        {
            gameobj.snakeobj.direction={1,0};
            gameobj.running=true;
        }
        ClearBackground(retro);
        DrawRectangleLinesEx(Rectangle{(float)offset-5,(float)offset-5,(float)cellsize*cellcount+10,(float)cellsize*cellcount+10},5,darkgreen);
        DrawText("R1980-sapera",offset-5,20,40,black);
        DrawText(TextFormat("%i",gameobj.score),offset-5,offset+cellsize*cellcount+10,40,black);
        gameobj.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}