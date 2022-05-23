#include <iostream>
#include "GLFW/glfw3.h"
#define _USE_MATH_DEFINES
#include <math.h>


#define mapW 10
#define mapH 10

struct TCell
{
	bool mine;
	bool flag;
	bool open;
	int cntAround;
};

TCell map[mapW][mapH];
int mines;
int closeCell;
int x;
int y;
bool lose;


bool IsCellInMap(int x, int y)
{
	return x >= 0 && y >= 0 && x < mapW&& y < mapH;
}

void Game_New()
{
	srand(time(NULL));
	memset(map, 0, sizeof(map));

	lose = false;
	mines = 20;
	closeCell = mapH * mapW;
	for (int i = 0; i < mines; ++i)
	{
		int x = rand() % mapW;
		int y = rand() % mapH;

		if (map[x][y].mine)
			--i;
		else
		{
			map[x][y].mine = true;

			for (int dx = -1; dx < 2; ++dx)
			{
				for (int dy = -1; dy < 2; ++dy)
				{
					if (IsCellInMap(x + dx, y + dy))
					{
						map[x + dx][y + dy].cntAround += 1;
					}
				}
			}
		}
	}
}

void Line(float x1, float y1, float x2, float y2)
{
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
}

void ShowNumber(int& a)
{
	glLineWidth(3);
	glColor3f(1, 1, 0);
	glBegin(GL_LINES);
	if (a != 1 && a != 4)
		Line(0.3f, 0.85f, 0.7f, 0.85f);

	if (a != 0 && a != 1 && a != 7)
		Line(0.3f, 0.5f, 0.7f, 0.5f);

	if (a != 1 && a != 4 && a != 7)
		Line(0.3f, 0.15f, 0.7f, 0.15f);

	if (a != 5 && a != 6)
		Line(0.7f, 0.5f, 0.7f, 0.85f);

	if (a != 2)
		Line(0.7f, 0.5f, 0.7f, 0.15f);

	if (a != 1 && a != 2 && a != 3 && a != 7)
		Line(0.3f, 0.5f, 0.3f, 0.85f);

	if (a == 0 || a == 2 || a == 6 || a == 8)
		Line(0.3f, 0.5f, 0.3f, 0.15f);
	glEnd();

}

void ShowMine()
{
	float x, y;
	float count = 10;
	float l = 0.3f;
	double a = M_PI * 2 / count;

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1, 0, 0);
	glVertex2f(0.5f, 0.5f);
	for (int i = -1; i < count; ++i)
	{
		x = (sin(a * i) * l) + 0.5;
		y = (cos(a * i) * l) + 0.5;
		glVertex2f(x, y);
	}
	glEnd();
}

void ShowField()
{
	glBegin(GL_TRIANGLE_STRIP);

	glColor3f(0.8f, 0.8f, 0.8f); glVertex2f(0, 1);
	glColor3f(0.7f, 0.7f, 0.7f); glVertex2f(1, 1); glVertex2f(0, 0);
	glColor3f(0.6f, 0.6f, 0.6f); glVertex2f(1, 0);

	glEnd();
}

void ShowFieldOpen()
{
	glBegin(GL_TRIANGLE_STRIP);

	glColor3f(0.3f, 0.8f, 0.3f); glVertex2f(0, 1);
	glColor3f(0.4f, 0.7f, 0.4f); glVertex2f(1, 1); glVertex2f(0, 0);
	glColor3f(0.5f, 0.6f, 0.5f); glVertex2f(1, 0);

	glEnd();
}

void ShowFlag()
{
	ShowField();
	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0);
	glVertex2f(0.25f, 0.75f);
	glVertex2f(0.85f, 0.5f);
	glVertex2f(0.25f, 0.25f);
	glEnd();

	glLineWidth(5);

	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	glVertex2f(0.25f, 0.75f);
	glVertex2f(0.25f, 0);
	glEnd();

}

void Game_Show()
{
	glLoadIdentity();
	glScalef(2.0 / mapW, 2.0 / mapH, 1);
	glTranslated(-mapW * 0.5, -mapH * 0.5, 0);

	for (int x = 0; x < mapW; ++x)
	{
		for (int y = 0; y < mapH; ++y)
		{
			glPushMatrix();
			glTranslated(x, y, 0);

			if (map[x][y].open)
			{
				ShowFieldOpen();
				if (map[x][y].mine)
					ShowMine();
				else if (map[x][y].cntAround > 0)
					ShowNumber(map[x][y].cntAround);
			}
			else if (map[x][y].flag)
				ShowFlag();
			else
				ShowField();

			glPopMatrix();
		}
	}
}

void OpenNullField(int x, int y)
{
	if (!IsCellInMap(x, y) || map[x][y].open)
		return;

	map[x][y].open = true;
	--closeCell;
	if (map[x][y].cntAround == 0)
	{
		for (int i = -1; i < 2; ++i)
		{
			for (int j = -1; j < 2; ++j)
			{
				OpenNullField(x + i, y + j);
			}
		}
	}
	if (map[x][y].mine)
	{
		lose = true;
		for (int x = 0; x < mapW; ++x)
		{
			for (int y = 0; y < mapH; ++y)
			{
				map[x][y].open = true;
			}
		}
	}
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	double y1;
	double x1;
	int heigth;
	int width;
	glfwGetWindowSize(window, &width, &heigth);
	glfwGetCursorPos(window, &x1, &y1);

	y = (heigth - y1) / (heigth / mapH);
	x = x1 / (width / mapW);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == 1 && !map[x][y].flag)
	{
		if (lose || mines == closeCell)
			Game_New();
		else
			OpenNullField(x, y);

	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == 1 && !map[x][y].open && !map[x][y].flag)
	{
		map[x][y].flag = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == 1 && !map[x][y].open)
	{
		map[x][y].flag = false;
	}
}

int main()
{
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(640, 640, "Saper", NULL, NULL);
	glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetMouseButtonCallback(window, mouse_callback);

	Game_New();

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		Game_Show();

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
