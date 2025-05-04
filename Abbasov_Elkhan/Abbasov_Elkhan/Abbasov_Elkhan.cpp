// Abbasov Elkhan 22101458 Section:003
// Additional functions: 
// 1.Fish color changer (keys 1–5)
// 2. Pause Screen (when you pause with F1) and Game Over screen (at the end)
// 3. Different parts of the fish give different points. (Tail: 3 pts, Nose: 4 pts, Fin: 5 pts, Body: 1 pt)
// 4. You can increase/decrease fish speed with 'Q' and 'E' keys.

#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define PI 3.1415
#define FISH_RADIUS 25
#define FISH_FIN_SIZE 25
#define MARGIN 5

int screenWidth = WINDOW_WIDTH;
int screenHeight = WINDOW_HEIGHT;

int minY;
int maxY;
int step;

int fishX[5];
int fishY[5];
int fishColorR = 255, fishColorG = 255, fishColorB = 0;
int fishSpeed = 2;
int fishDelay[5] = { 0, 20, 40, 60, 80 };


float torpedoX = 0;
float torpedoY = 0;
bool torpedoActive = false;

int fireSpeed = 0;
int submarineX = 0;
double timerVal = 30.0;
int score = 0;
bool activeTimer = false;
bool isPaused = false;
bool hasStarted = false;


void circle(float x, float y, float r) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++) {
        float angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void vprint2(int x, int y, float size, const char* str, ...) {
    va_list ap;
    char text[1024];
    va_start(ap, str);
    vsprintf(text, str, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);
    for (int i = 0; i < strlen(text); i++)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
    glPopMatrix();
}

void drawSubmarine() {
    float subY = -screenHeight / 2 + 30;

    glColor3ub(80, 80, 150);
    glBegin(GL_QUADS);
    glVertex2f(submarineX - 60, subY - 15);
    glVertex2f(submarineX + 60, subY - 15);
    glVertex2f(submarineX + 60, subY + 15);
    glVertex2f(submarineX - 60, subY + 15);
    glEnd();

    circle(submarineX - 60, subY, 15);
    circle(submarineX + 60, subY, 15);

    glColor3ub(100, 200, 255);
    circle(submarineX - 25, subY, 7);
    circle(submarineX, subY, 7);
    circle(submarineX + 25, subY, 7);

    glColor3ub(60, 60, 110);
    glBegin(GL_QUADS);
    glVertex2f(submarineX - 5, subY + 15);
    glVertex2f(submarineX + 5, subY + 15);
    glVertex2f(submarineX + 5, subY + 35);
    glVertex2f(submarineX - 5, subY + 35);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(submarineX + 5, subY + 35);
    glVertex2f(submarineX + 20, subY + 35);
    glVertex2f(submarineX + 20, subY + 42);
    glVertex2f(submarineX + 5, subY + 42);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(submarineX - 60, subY);
    glVertex2f(submarineX - 80, subY + 20);
    glVertex2f(submarineX - 80, subY - 20);
    glEnd();
}

void drawFish() {
    for (int i = 0; i < 5; i++) {
        float x = fishX[i];
        float y = fishY[i];

        glColor3ub(fishColorR, fishColorG, fishColorB);
        glBegin(GL_POLYGON);
        glVertex2f(x, y);
        glVertex2f(x - 30, y + 15);
        glVertex2f(x - 45, y + 8);
        glVertex2f(x - 55, y);
        glVertex2f(x - 45, y - 8);
        glVertex2f(x - 30, y - 15);
        glEnd();

        glBegin(GL_TRIANGLES);
        glVertex2f(x - 25, y + 5);
        glVertex2f(x - 40, y + 5);
        glVertex2f(x - 32, y + 25);
        glEnd();

        glBegin(GL_TRIANGLES);
        glVertex2f(x - 25, y - 5);
        glVertex2f(x - 40, y - 5);
        glVertex2f(x - 32, y - 25);
        glEnd();

        glBegin(GL_TRIANGLES);
        glVertex2f(x - 55, y);
        glVertex2f(x - 65, y + 12);
        glVertex2f(x - 65, y - 12);
        glEnd();

        glColor3f(0, 0, 0);
        circle(x - 13, y + 2, 2.5);
    }
}

void checkCollision() {
    float torpedoYAbs = -screenHeight / 2 + 30 + torpedoY;

    for (int i = 0; i < 5; i++) {
        float fishNoseX = fishX[i];
        float fishTailX = fishX[i] - 55;
        float fishCenterX = fishX[i] - 30;
        float fishYCenter = fishY[i];

        float dx = torpedoX - fishCenterX;
        float dy = torpedoYAbs - fishYCenter;
        float dist = sqrt(dx * dx + dy * dy);
        float maxHitDistance = FISH_RADIUS + 10;

        if (dist <= maxHitDistance) {
            int point = 1;
            if (torpedoX >= fishTailX - 5 && torpedoX <= fishTailX + 5) point = 3;
            else if (torpedoX >= fishNoseX - 5 && torpedoX <= fishNoseX + 5) point = 4;
            else if (torpedoYAbs <= fishYCenter - 5 && torpedoYAbs >= fishYCenter - 25 &&
                torpedoX >= fishCenterX - 10 && torpedoX <= fishCenterX + 10) point = 5;

            score += point;

            fishX[i] = -screenWidth / 2 - rand() % 100;
            fishY[i] = minY + rand() % (maxY - minY);
            torpedoActive = false;
            fireSpeed = 0;
            torpedoY = 0;
        }
    }
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
    glColor3f(0.3, 0.8, 0.9);
    glVertex2f(screenWidth / 2, screenHeight / 2);
    glVertex2f(-screenWidth / 2, screenHeight / 2);
    glColor3f(0, 0.4, 0.6);
    glVertex2f(-screenWidth / 2, -screenHeight / 2);
    glVertex2f(screenWidth / 2, -screenHeight / 2);
    glEnd();

    // Top panel
    glColor3f(0.1, 0.1, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(-screenWidth / 2, screenHeight / 2 - 40);
    glVertex2f(screenWidth / 2, screenHeight / 2 - 40);
    glVertex2f(screenWidth / 2, screenHeight / 2);
    glVertex2f(-screenWidth / 2, screenHeight / 2);
    glEnd();

    glColor3ub(255, 255, 255);
    vprint2(-screenWidth / 2 + 20, screenHeight / 2 - 30, 0.15, "Time: %.2f", timerVal);
    vprint2(-screenWidth / 2 + 190, screenHeight / 2 - 30, 0.15, "Score: %d", score);

    if (!hasStarted || (!activeTimer && timerVal >= 29.9))
        vprint2(-screenWidth / 2 + 370, screenHeight / 2 - 30, 0.12, "Keys: Left/Right, Space=Fire, F1=Start, 1-5 Colors");
    else if (isPaused)
        vprint2(-screenWidth / 2 + 370, screenHeight / 2 - 30, 0.12, "Keys: Left/Right, Space=Fire, F1=Continue, 1-5 Colors");
    else
        vprint2(-screenWidth / 2 + 370, screenHeight / 2 - 30, 0.12, "Keys: Left/Right, Space=Fire, F1=Pause, 1-5 Colors");

    drawFish();
    drawSubmarine();

    if (isPaused) {
        glColor3f(1, 1, 1);
        vprint2(-80, 0, 0.3, "PAUSED");
        vprint2(-220, -60, 0.3, "Press 'F1' to Resume.");
    }

    if (timerVal <= 0.02) {
        glColor3f(1, 0.2, 0.2);
        vprint2(-120, 25, 0.3, "TIME'S UP!");
        glColor3f(1, 1, 0.5);
        vprint2(-140, -20, 0.3, "Final Score: %d", score);
        glColor3f(1, 1, 1);
        vprint2(-220, -70, 0.3, "Press 'F1' to Play Again.");
    }

    float ty = torpedoActive ? -screenHeight / 2 + 30 + torpedoY : -screenHeight / 2 + 30;
    if (!torpedoActive) torpedoX = submarineX;

    glColor3f(0.6, 0.6, 0.6);
    glBegin(GL_QUADS);
    glVertex2f(torpedoX - 3, ty);
    glVertex2f(torpedoX + 3, ty);
    glVertex2f(torpedoX + 3, ty + 18);
    glVertex2f(torpedoX - 3, ty + 18);
    glEnd();

    glColor3f(0.8, 0.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex2f(torpedoX - 4, ty + 18);
    glVertex2f(torpedoX + 4, ty + 18);
    glVertex2f(torpedoX, ty + 24);
    glEnd();

    glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_TRIANGLES);
    glVertex2f(torpedoX - 3, ty);
    glVertex2f(torpedoX + 3, ty);
    glVertex2f(torpedoX, ty - 5);
    glEnd();

    glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y) {
    if (key == ' ' && !torpedoActive) {
        torpedoX = submarineX;
        torpedoY = 0;
        fireSpeed = 5;
        torpedoActive = true;
    }

    if (key >= '1' && key <= '5') {
        switch (key) {
        case '1': fishColorR = 255; fishColorG = 0; fishColorB = 0; break;
        case '2': fishColorR = 0; fishColorG = 255; fishColorB = 0; break;
        case '3': fishColorR = 0; fishColorG = 0; fishColorB = 255; break;
        case '4': fishColorR = 255; fishColorG = 255; fishColorB = 0; break;
        case '5': fishColorR = 255; fishColorG = 0; fishColorB = 255; break;
        }
    }

    if (key == 'q' || key == 'Q') {
        if (fishSpeed > 1) fishSpeed--; 
    }

    if (key == 'e' || key == 'E') {
        if (fishSpeed < 10) fishSpeed++; 
    }
}

void onSpecialKey(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT && submarineX > -screenWidth / 2 + 60) submarineX -= 30;
    if (key == GLUT_KEY_RIGHT && submarineX < screenWidth / 2 - 60) submarineX += 30;

    if (key == GLUT_KEY_F1) {
        if (!hasStarted || timerVal <= 0.01) {
            timerVal = 30.0;
            score = 0;
            torpedoY = 0;
            fireSpeed = 0;

            for (int i = 0; i < 5; i++) {
                fishX[i] = -screenWidth / 2 - i * 140;
                fishY[i] = minY + step * i;
                fishDelay[i] = i * 20;
            }

            isPaused = false;
            activeTimer = true;
            hasStarted = true;
        }
        else if (activeTimer) {
            activeTimer = false;
            isPaused = true;
        }
        else if (!activeTimer && isPaused) {
            activeTimer = true;
            isPaused = false;
        }

        glutPostRedisplay();
    }
}

void onTimer(int v) {
    if (activeTimer) {
        timerVal -= 0.01;
        if (timerVal <= 0.01) {
            activeTimer = false;
            hasStarted = false;
        }

        if (torpedoActive) {
            torpedoY += 5;
            if (-screenHeight / 2 + 80 + torpedoY + 24 > screenHeight / 2 - 40) {
                torpedoActive = false;
                fireSpeed = 0;
                torpedoY = 0;
            }
        }

        for (int i = 0; i < 5; i++) {
            if (fishDelay[i] > 0) fishDelay[i]--;
            else {
                fishX[i] += fishSpeed;
                if (fishX[i] > screenWidth / 2) {
                    fishX[i] = -screenWidth / 2 - rand() % 100;
                    fishY[i] = minY + rand() % (maxY - minY);
                    fishDelay[i] = 0;
                }
            }
        }

        checkCollision();
        glutPostRedisplay();
    }
    glutTimerFunc(10, onTimer, v);
}

void onResize(int w, int h) {
    screenWidth = w;
    screenHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char* argv[]) {
    minY = -screenHeight / 2 + 100;
    maxY = screenHeight / 2 - 50 - FISH_FIN_SIZE - MARGIN;
    step = (maxY - minY) / 6;

    for (int i = 0; i < 5; i++) {
        fishX[i] = -screenWidth / 2 - i * 140;
        fishY[i] = minY + step * i;
        fishDelay[i] = i * 20;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(270, 50);
    glutCreateWindow("Submarine Torpedo Attack - Abbasov Elkhan");

    glutDisplayFunc(drawScene);
    glutReshapeFunc(onResize);
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKey);
    glutTimerFunc(10, onTimer, 1);

    glutMainLoop();
    return 0;
}
