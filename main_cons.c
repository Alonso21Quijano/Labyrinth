#include "stdio.h"
#include "SDL2/SDL.h"
#include "unistd.h"
#define SIZE 1000

void draw(int map[20][20][2], int sw, int n, int len, SDL_Renderer* rend)
{
    int i, j, dx;
    dx = len / 4;
    SDL_Rect rect1, rect2;
    rect1.w = rect1.h = rect2.h = rect2.w =  2 * dx;
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderClear(rend);
    for (i = 1; i <= n; i++)
        for (j = 1; j <= n; j++)
        {
            SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
            if (map[i][j][sw] & 32)
                SDL_RenderDrawLine(rend, (j - 1) * len, (i - 1) * len, (j - 1) * len, i * len);
            if (map[i][j][sw] & 4)
                SDL_RenderDrawLine(rend, (j - 1) * len, (i - 1) * len, j * len, (i - 1) * len);
            if ((j == n) && (map[i][j][sw] & 8))
                SDL_RenderDrawLine(rend, j * len, (i - 1) * len, j * len, i * len);
            if ((i == n) && (map[i][j][sw] & 16))
                SDL_RenderDrawLine(rend, (j - 1) * len, i * len, j * len, i * len);
            if (map[i][j][sw] & 1)
            {
                SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
                rect1.x = (j - 1) * len + dx;
                rect1.y = (i - 1) * len + dx;
                SDL_RenderFillRect(rend, &rect1);
            }
            if (map[i][j][sw] & 2)
            {
                SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
                rect2.x = (j - 1) * len + dx;
                rect2.y = (i - 1) * len + dx;
                SDL_RenderFillRect(rend, &rect2);
            }
        }
    SDL_RenderPresent(rend);
    getchar();
}

int main(int argc, char *argv[])
{
    FILE *f;
    int map[20][20][2], sw = 0, n, i, j, x, y, a, b, len;
    char cond = 0;
    SDL_Window *window;
    SDL_Renderer* renderer;

    if (SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    if (argc == 1)
    {
        printf("File name not specified\n");
        return 1;
    }

    if ((f = fopen(argv[1], "r")) == NULL)
    {
        printf("Error during opening file");
        return 1;
    }
    fscanf(f, "%d", &n);
    if (n > 998)
    {
        printf("Too much");
        return 0;
    }

    len = SIZE / n;
    for (i = 0; i <= n + 1; i++)
        for (j = 0; j <= n + 1; j++)
            fscanf(f, "%d", &map[i][j][sw]);
    fscanf(f, "%d %d", &x, &y);
    map[x][y][sw] |= 1;
    fscanf(f, "%d %d", &x, &y);
    map[x][y][sw] |= 2;
    fclose(f);


    window = SDL_CreateWindow(
                "Labyrinth",                  // window title
                SDL_WINDOWPOS_UNDEFINED,           // initial x position
                SDL_WINDOWPOS_UNDEFINED,           // initial y position
                SIZE + 1,                               // width, in pixels
                SIZE + 1,                               // height, in pixels
                SDL_WINDOW_OPENGL                  // flags - see below
                );

    if ((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)) == NULL)
    {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        return 1;
    }
    draw(map, sw, n, len, renderer);
    do
    {
        for (i = 0; i <= n + 1; i++)
            for (j = 0; j <= n + 1; j++)
                map[i][j][1 - sw] = map[i][j][sw];
        for (i = 1; i <= n; i++)
            for (j = 1; j <= n; j++)
            {
                // если горизонтальная стенка -- одиночная стенка вправо или влево
                if ((map[i][j][sw] & 16) && 
				((!(map[i][j][sw] & 8) && !(map[i + 1][j][sw] & 8) && !(map[i][j + 1][sw] & 16)) 
				|| 
				(!(map[i][j][sw] & 32) && !(map[i + 1][j][sw] & 32) && !(map[i][j - 1][sw] & 16)))
                        && !((map[i][j][sw] & 36) == 36) && !((map[i + 1][j][sw] & 36 )== 36)) //и клетки с этой стенкой не содержат left-up уголок
                {
                    map[i][j][1 - sw] ^= 16;
                    map[i + 1][j][1 - sw] ^= 4; //удаляем эту стенку
                }
                // если горизонтальная стенка -- одиночная стенка вправо или влево
                if ((map[i][j][sw] & 8) && 
				((!(map[i][j][sw] & 4) && !(map[i][j + 1][sw] & 4) && !(map[i - 1][j][sw] & 8)) 
				|| 
				(!(map[i][j][sw] & 16) && !(map[i][j + 1][sw] & 16) && !(map[i + 1][j][sw] & 8)))
                        && !((map[i][j][sw] & 36) == 36) && !((map[i][j + 1][sw] & 36) == 36)) //и клетки с этой стенкой не содержат left-up уголок
                {
                    map[i][j][1 - sw] ^= 8;
                    map[i][j + 1][1 - sw] ^= 32; //удаляем эту стенку
                }
                if ((map[i][j][sw] & 36) == 36) //если клетка содержит left-up angle
                {
                    map[i][j][1 - sw] |= 24;
                    map[i + 1][j][1 - sw] |= 4;
                    map[i][j + 1][1 - sw] |= 32;
                    if (!(map[i][j - 1][sw] & 4)) //если из левой клетки можно перейти в левую-верхнюю
                    {
                        map[i][j][1-sw] ^= 32;
                        map[i][j - 1][1-sw] ^= 8; //создаем переход из левой в себя
                    }
                    if (!(map[i - 1][j][sw] & 32)) //если из верхней клетки можно перейти в левую-верхнюю
                    {
                        map[i][j][1-sw] ^= 4;
                        map[i - 1][j][1-sw] ^= 16; //создаем переход из верхней в себя
                    }
                }
                if (map[i][j][sw] & 3) //если клетка -- старт/финиш
                {
                    if (!(map[i][j][sw] & 16)) //клетка связана с нижней
                    {
                        map[i + 1][j][1 -sw] |= map[i][j][sw] & 3;
                        map[i][j][1 - sw] &= ~3; //переходим в неё
                    }
                    else
                        if (!(map[i][j][sw] & 8)) //клетка связана с правой
                        {
                            map[i][j + 1][1 -sw] |= map[i][j][sw] & 3;
                            map[i][j][1 - sw] &= ~3; //переходим в неё
                        }
                }
            }
        sw = 1 - sw;
	for (a = 0; a <= n + 1; a++)
	{
		for (b = 0; b <= n + 1; b++)
		{	
			if ((map[a][b][sw] & 3) == 3)
				cond = 1;	
			if ((map[a][b][sw] == 61) || (map[a][b][sw] == 62))
				cond = -1;	
			printf(" %2d", map[a][b][sw]);
		}
		printf("\n");
	}
        draw(map, sw, n, len, renderer);
    }
    while (!cond);
    if (cond == 1)
        printf("Path exists!\n");
    else
        printf("Path does not exist!\n");
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

