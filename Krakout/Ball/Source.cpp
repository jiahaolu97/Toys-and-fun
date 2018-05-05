#include<GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>
#include <vector>
#include <iostream>
#include <string>
#include <windows.h>
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#define MAX_CHAR 128
using namespace std;
/*
状态机:
state 0: ready
state 1: playing
state 2: lose
state 3: win
函数声明:
random    自己编写的随机数生成器

brick类
brick() 生成函数，参数一二:x,y 参数三:生命值,默认为1
show() 绘画函数，可以在display的子函数drawBricks中调用
collide()碰撞判断，可以考虑在计时器函数中调用

init   初始化所有静态元素。如坐标系，窗口，玩家起始位置，根据关卡新建对应的多少个砖块

display  画出所有动态元素, 其子函数有:
drawPlayer  画出当前board的位置，包含越界判定
drawBall   画出当前球的位置，包含碰撞和越界判定
drawSolidCircle  是drawBall的子函数，画出一个圆
bounce  球与板子的碰撞判断  是drawBall的子函数
drawBricks  根据砖块的生命值决定是否调用每个砖块的show()函数将它们画出来

keyBorad  键盘响应函数，AD控制移动，P暂停，每次触发时调用重画
specialKey 也是键盘响应函数 可以用左右键控制平移

timer  计时器函数  每隔33秒重画当前的画面
*/
const GLfloat TwoPI = 2.0f * 3.1415926f;
const GLfloat PI = 3.1415926f;
float playerx, playery, playerLength, playerSpeed;			//player的x,y坐标确定的是上表面中心点的位置,厚度默认为8,初始速度为16
float ballx, bally, radius, ballSpeed, angle, radian;			//ball的x,y是圆心,初始半径是8,速度是6,角度随机产生  angle角度（360)  radian 弧度 2PI
float radianright, radianleft;
bool clockwise = true;
int ColorFashion = 2;
int state = 4;		//state 0:ready  1:playing  2:lose  3:win
float colors[7][3] = { { 255,0,0 },{ 255,128,0 },{ 255,255,0 },{ 0,255,0 },{ 0,255,255 },{ 0,0,255 },{ 128,0,255 } };
int stage = 1;  //先设置1,2,3三个关卡

int random(int start, int end) {
	return start + rand() % (end - start);
}

class brick {  //100 * 30 的长方形砖块
public:
	brick();
	brick(float x1, float y1, int life);
	void show();
	bool collide();
	void move(float x, float y);
	int life;
	void changeColor();

private:
	float x, y;		// x,y 是砖块上表面的中点坐标
	bool exist;
	int colorID , colorNum;
	float height = 30;
	float length = 100;
	float left, right, bottom;
	GLubyte brickColor[3];
};
brick::brick() {
	colorID = random(0, 7);
	colorNum = random(210, 236);
	for (int i = 0; i < 3; i++) {
		brickColor[i] = colorNum;
	}
	brickColor[ColorFashion] = random(226, 256);
	life = 1;
}
brick::brick(float x1, float y1, int life = 1) {
	colorID = random(0, 7);
	x = x1;
	y = y1;
	left = x1 - length / 2;
	right = x1 + length / 2;
	bottom = y1 - height;
	exist = true;
}
void brick::changeColor() {
	colorNum = random(210, 236);
	for (int i = 0; i < 3; i++) {
		brickColor[i] = colorNum;
	}
	brickColor[ColorFashion] = random(226, 256);
}
void brick::show() {
	if (life >= 1)
	{
		glColor3f(0.0, 0.0, 0.0);
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2f(left, y);
		glVertex2f(right, y);

		glVertex2f(right, y);
		glVertex2f(right, bottom);

		glVertex2f(right, bottom);
		glVertex2f(left, bottom);

		glVertex2f(left, bottom);
		glVertex2f(left, y);
		glEnd();
		//glColor3fv(colors[colorID]);
		//glColor3f(1.0,1.0,1.0);
		//glColor3ub(124, 166, 210);
		glColor3ubv(brickColor);
		glBegin(GL_QUADS);
		glVertex2f(left, y);
		glVertex2f(right, y);
		glVertex2f(right, bottom);
		glVertex2f(left, bottom);
		glEnd();
	}
}
bool brick::collide() {
	if (life >= 1) {
		if (bottom - bally - radius >-10.5 && bottom - bally - radius <2.5 && left < ballx && ballx<right) //碰下边
		{
			this->life--;
			angle = 360 - angle;
			return true;
		}
		else if (left - ballx - radius > -4.5 && left - ballx - radius<2.5 && bottom < bally && bally < y) //碰左边
		{
			this->life--;
			if (0 <= angle < 90)angle = 180 - angle;
			else if (270 < angle <= 360) angle = 540 - angle;
			return true;
		}
		else if (ballx - right - radius>-4.5 && ballx - right - radius<2.5 &&bottom < bally && bally<y) //碰右边
		{
			this->life--;
			if (90 < angle <= 180) angle = 180 - angle;
			else if (180 < angle < 270) angle = 540 - angle;
			return true;
		}
		else if (bally - y - radius >-4.5 &&bally - y - radius<2.5 && left < ballx && ballx<right) //碰上边
		{
			this->life--;
			angle = 360 - angle;
			return true;
		}
		return false;
	}
	return false;
}
void brick::move(float x1, float y1) {
	x = x1;
	y = y1;
	left = x1 - length / 2;
	right = x1 + length / 2;
	bottom = y1 - height;
}

vector<brick> bricks;
vector<brick>::iterator it;

void init() {		//初始化所有非动态元素
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition(30, 30);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Hit Bricks");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClearColor(0.4, 0.4, 0.4, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glOrtho(-400, 400, -300, 300, -1, 1);		//规定窗口的坐标系:(-400,-300)到(400,300)
	srand(unsigned(time(NULL)));		//设置随机数种子
	state = 0;								//设置初始状态为ready状态
	playerLength = 400;
	playerx = 0;
	playery = -270;
	playerSpeed = 16;
	radius = 8;
	ballSpeed = 10;
	ballx = playerx;
	bally = playery + radius;
	angle = 20;
	brick a(50, 0, 2);
	//下面根据关卡来新建砖块数组
	switch (stage) {
	case 1:
		for (int xx = 0; xx < 8; xx++) {
			for (int yy = 0; yy < 8; yy++) {
				brick b;
				b.move(xx * 100 - 350, 300 - 30 * yy);
				bricks.push_back(b);
			}
		}
		break;
	}
}

void drawString(const char* str,int size) {
	// 选择字体
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Comic Sans MS");
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);

	static int isFirstCall = 1;
	static GLuint lists;

	if (isFirstCall) { // 如果是第一次调用，执行初始化
					   // 为每一个ASCII字符产生一个显示列表
		isFirstCall = 0;
		// 申请MAX_CHAR个连续的显示列表编号
		lists = glGenLists(MAX_CHAR);
		// 把每个字符的绘制命令都装到对应的显示列表中
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	// 调用每个字符对应的显示列表，绘制每个字符
	for (; *str != '\0'; ++str)
		glCallList(lists + *str);
}

void drawPlayer() {		//画出当前board的位置,是display的子函数
	if (playerx - playerLength / 2 < -400)
		playerx = playerLength / 2 - 400;
	if (playerx + playerLength / 2 > 400)
		playerx = 400 - playerLength / 2;
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glVertex2f(playerx - playerLength / 2.0, playery);
	glVertex2f(playerx + playerLength / 2.0, playery);
	glVertex2f(playerx + playerLength / 2.0, playery - 8);
	glVertex2f(playerx - playerLength / 2.0, playery - 8);
	glEnd();
}

void drawSolidCircle(float x, float y, float radius) {
	glColor3f(1.0, 1.0, 1.0);
	int sections = 200;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);
	for (int count = 0; count <= sections; count++) {
		glVertex2f(x + radius*cos(count * TwoPI / sections), y + radius*sin(count * TwoPI / sections));
	}
	glEnd();
}

void drawArrow(float x,float y,float angle) {
	radian = angle / 360 * TwoPI;
	radianright = (angle - 2) / 360 * TwoPI;
	radianleft = (angle + 2) / 360 * TwoPI;
	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex2f(x, y);
	glVertex2f(x + 50*cos(radian), y + 50*sin(radian));
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(x + 50 * cos(radian), y + 50 * sin(radian));
	glVertex2f(x + 40 * cos(radianright), y + 40 * sin(radianright));
	glVertex2f(x + 40 * cos(radianleft), y + 40 * sin(radianleft));
	glEnd();
}

bool bounce() {
	if (bally <= playery + radius + 0.5 && bally >= playery + radius - 0.5
		&& ballx >= playerx - playerLength / 2 && ballx <= playerx + playerLength / 2)
		return true;
	return false;
}

void drawBall() {
	if (state == 0)		//ready状态下，球随着板子的移动而移动
	{
		ballx = playerx;
		bally = playery + radius;	
		glColor3f(0.0, 0.0, 0.0);
		drawArrow(ballx, bally, angle);
		drawSolidCircle(ballx, bally, radius);
	}
	else if (state == 1)	//开始游戏
	{
		radian = angle / 360 * TwoPI;
		ballx += ballSpeed*cos(radian);
		bally += ballSpeed*sin(radian);
		if (ballx + radius >= 400 || ballx - radius <= -400) //碰右壁或者碰右壁
		{
			if (angle <= 90) angle = 180 - angle;
			else if (270 <= angle <= 360) angle = 540 - angle;
		}
		if (bally + radius >= 300 || bounce()) //碰天花板或碰板子
			angle = 360 - angle;
		drawSolidCircle(ballx, bally, radius);
		if (bally + 300 < radius) state = 2;
	}
	else if (state == 3) {
		drawSolidCircle(ballx, bally, radius);
	}
}

void drawBricks() {
	for (it = bricks.begin(); it != bricks.end(); it++) {
		(*it).show();
	}
}

void timer(int id) {
	glutPostRedisplay();
	glutTimerFunc(23, timer, 1);
}

void checkoutCollide(int id) {
	int brickCount = 64;
	for (it = bricks.begin(); it < bricks.end(); it++) {
		(*it).collide();
		if ((*it).life == 0) brickCount--;
	}
	if(brickCount!=0)
		glutTimerFunc(10, checkoutCollide, 3);
	else
	{
		state = 3;
	}
}

void directionControlTimer(int id) {
	if (state == 0) {
		if (clockwise == true) angle += 2;
		else angle -= 2;
		if (angle >= 179) clockwise = false;
		else if (angle <= 1) clockwise = true;
		glutTimerFunc(50, directionControlTimer, 2);
	}
}

void keyboard(unsigned char key, int x, int y) {	//AD左右  P暂停 空格键开始
	switch (key) {
	case 27:
		exit(0);
	case 'a':
		if (state == 0 || state == 1) 
		playerx -= playerSpeed;
		break;
	case 'd':
		if (state == 0 || state == 1)
		playerx += playerSpeed;
		break;
	case 32:				//按下空格键，开始游戏
		if (state == 0) state = 1;
		break;
	}

}

void specialKey(int key, int x, int y) {  //F1重启
	switch (key) {
	case GLUT_KEY_LEFT:
		if (state == 0 || state == 1) 
			playerx -= playerSpeed;
		break;
	case GLUT_KEY_RIGHT:
		if (state == 0 || state == 1) 
			playerx += playerSpeed;
		break;
	case GLUT_KEY_F1:
		ballx = playerx;
		bally = playery + radius;
		clockwise = true;
		angle = random(30, 160);
		glutTimerFunc(46, directionControlTimer, 2);
		state = 0;
		break;
	}
}

void restart() {
	glClear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < 64; i++) {
		bricks.pop_back();
	}
	playerx = 0;
	playery = -270;
	playerSpeed = 16;
	radius = 8;
	ballSpeed = 10;
	ballx = playerx;
	bally = playery + radius;
	angle = 20;
	state = 0;
	glutTimerFunc(46, directionControlTimer, 2);
	switch (stage) {
	case 1:
		for (int xx = 0; xx < 8; xx++) {
			for (int yy = 0; yy < 8; yy++) {
				brick b;
				b.move(xx * 100 - 350, 300 - 30 * yy);
				bricks.push_back(b);
			}
		}
		break;
	}
}

void myMenu(int value) {
	if (value == 1) {
		restart();
	}
	else if (value == 2) {
		if (ColorFashion != 2) ColorFashion++;
		else ColorFashion = 0;
		for (it = bricks.begin(); it != bricks.end(); it++) {
			(*it).changeColor();
		}
	}
	else if (value == 3) {
		exit(0);
	}
}

void display() {	//所有动态元素在此绘制
	glClear(GL_COLOR_BUFFER_BIT);
	if (state == 0 || state == 1) {
		drawPlayer();
		drawBricks();
		drawBall();
	}
	else if (state == 2) {
		drawPlayer();
		drawBricks();
		glColor3f(1.0, 0.0, 0.0);
		glRasterPos2f(-160.0, -100.0);
		drawString("You  lose",96);
	}
	else if (state == 3) {
		drawPlayer();
		glColor3f(1.0, 0.0, 0.0);
		glRasterPos2f(-140.0, 0.0);
		drawString("You win!", 96);
	}
	glutSwapBuffers();
}

int main(int argv, char** argc) {
	glutInit(&argv, argc);
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);
	glutTimerFunc(23, timer, 1);		//每隔23毫秒自动刷新
	glutTimerFunc(46, directionControlTimer, 2);
	glutTimerFunc(40, checkoutCollide, 3);
	glutCreateMenu(myMenu);
	glutAddMenuEntry("Restart", 1);
	glutAddMenuEntry("Change Brick Color", 2);
	glutAddMenuEntry("Exit", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
}