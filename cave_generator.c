// author: chohyunjae
// date: 2022-10-17
// program: generates a random dungeon map.
// references
// https://gamedevelopment.tutsplus.com/tutorials/generate-random-cave-levels-using-cellular-automata--gamedev-9664
// https://blog.jrheard.com/procedural-dungeon-generation-cellular-automata
// generate dungeon cave

#include <stdlib.h>
#include <stdio.h>
#include<time.h>


#define ROW 25
#define COLUMN 25
#define ALIVE '#'
#define DEAD '.'

char** generate_dungeon(unsigned int seed)
{
	time_t t;
	srand(seed);
	char** grids = (char**)calloc(ROW,sizeof(char*));
	for(int i = 0; i < ROW;++i)
	{
		grids[i] = (char*)calloc(COLUMN,sizeof(char));
	}
	//initialize a map
	//randomly set them, wall or ground
	printf("\n=====Raw Data========\n");
	for(int i = 0; i <ROW; ++i)
	{
		for (int j = 0; j < COLUMN;++j)
		{
			int isWall = 0;
			float x = ((float)rand()/(float)(RAND_MAX));
			if( x < 0.45)
			{
				isWall = 1;
			}
			
			grids[i][j] = isWall ? ALIVE : DEAD;
			printf("%c",isWall ? ALIVE : DEAD);
		}
		printf("\n");
		
	}
	printf("\n===================\n");
	int iterations = 5;
	while(iterations > 0)
	{
		char newGrids[ROW][COLUMN];
		//apply rules!
		for(int i = 0; i <ROW; ++i)
		{
			for (int j = 0; j < COLUMN;++j)
			{
				//check neighbours
				int count = 0;
				for(int k = -1; k < 2;++k)
				{
					for(int l = -1; l < 2;++l)
					{
						int neighbour_x = i + k;
						int neighbour_y = j + l;
						if (k == 0 && l == 0)
						{
							//do nothing
						}
						else if(neighbour_x < 0 || 
								neighbour_y < 0 ||
							    neighbour_x >= ROW ||
							    neighbour_y >= COLUMN)
						{
							count += 2;
						}
						else if(grids[neighbour_x][neighbour_y] == ALIVE)
						{
							count += 1;
						}
					}
				}
				
				if(grids[i][j] == DEAD && count > 5)
				{
					newGrids[i][j] = ALIVE;
				}
				else if(grids[i][j] == ALIVE && count > 2)
				{
					newGrids[i][j] = ALIVE;
				}
				else
				{
					newGrids[i][j] = DEAD;
				}
			}
		}
		printf("\n====%d generation====\n", 5 - iterations);
		for(int i = 0; i < ROW; ++i)
		{
			for (int j = 0; j < COLUMN;++j)
			{
				printf("%c",newGrids[i][j]);
			}
			printf("\n");
		}
		printf("\n===================\n");
		for(int i = 0; i <ROW; ++i)
		{
			for (int j = 0; j < COLUMN;++j)
			{
				grids[i][j] = newGrids[i][j];
			}
		}
		
		iterations--;
	}

	
	return grids;
}

/*
int main()
{
	char** dungeon = generate_dungeon(time(NULL));
	
	for(int i = 0; i <ROW; ++i)
	{
		for (int j = 0; j < COLUMN;++j)
		{
			free(dungeon[j]);
		}
	}	
	
	return 0;

}

*/
