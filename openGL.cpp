#include <GL/glut.h>
#include "all.h"

int numberOfProductionLinesForLiquid;
int numberOfProductionLinesForPills;
int InspectionCounter[11] = {};
int PackagingCounter[11] = {};
int Success[11] = {};
int Fail[11] = {};
int packagesEmployees[11] = {};
int inspectionEmployees[11] = {};
int EmployeesCount[11] = {};
int ProducedCount[11] = {};
int Speed[11] = {};
// Function to initialize the OpenGL environment
void init() {
    // Set the background color to black
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

void drawRectangle(float centerX, float centerY, float width, float height, float red, float green, float blue) {
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    // Set the color
    glColor3f(red, green, blue);

    glBegin(GL_QUADS);
    glVertex2f(centerX - halfWidth, centerY - halfHeight); // Bottom left corner
    glVertex2f(centerX + halfWidth, centerY - halfHeight); // Bottom right corner
    glVertex2f(centerX + halfWidth, centerY + halfHeight); // Top right corner
    glVertex2f(centerX - halfWidth, centerY + halfHeight); // Top left corner
    glEnd();
}

void renderText(float x, float y, void *font, const char* string) {
    glRasterPos2f(x, y);  // Set the position for the text

    // Loop through the characters in the string and render them
    for (const char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void signalHandler(int sig, siginfo_t *si, void *unused) {
    int speed = rand() % 4 + 1;
    int line = si->si_value.sival_int;  // Get the line number
    if (sig == SIGRTMIN) {
        InspectionCounter[line]++;
    } else if (sig == SIGRTMIN + 1) {
        PackagingCounter[line]++;
    } else if (sig == SIGRTMIN + 2) {
        Fail[line]++;
    } else if (sig == SIGRTMIN + 3) {
        Success[line]++;
    } else if (sig == SIGRTMIN + 4) {
        EmployeesCount[line]++;
    } else if (sig == SIGRTMIN + 5) {
        EmployeesCount[line]--;
    } else if (sig == SIGRTMIN + 6) {
        ProducedCount[line]+=speed;
    }
}

// Function to set up signal handlers
void setupSignalHandler(int sig) {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signalHandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(sig, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

void checkSignals() {
    for (int i = SIGRTMIN; i <= SIGRTMIN + 6; i++) {
        setupSignalHandler(i);
    }
}
int findMinSpeed(){
    int j=0;
    int min=Speed[1];
    for(int i=1; i<11; i++){
        if(min > Speed[i] && EmployeesCount[i] >2){
            min = Speed[i];
            j=i;
        }
    }
    return j;
}
void checkThreshold(){
    for(int i=1; i<11; i++){
        if(ProducedCount[i] - InspectionCounter[i]>= threshold){
            int id = findMinSpeed();
            EmployeesCount[i]++;
            EmployeesCount[id]--;
            ProducedCount[i]-=10;
        }
    }
}
// Function to display the rectangle
void display() {

    checkSignals();
    checkThreshold();
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    float x = 1.0;
    float y = 0.70;
    float j = 0.1;
    drawRectangle(-0.1, 0.8, 0.1, 0.1, 0.0,0.0,1.0);

    char buffer[50];
    sprintf(buffer, "Packaging");
    renderText(0.020, 0.78, GLUT_BITMAP_HELVETICA_18, buffer);
    drawRectangle(-0.1, 0.5, 0.1, 0.1, 0.5,0.5,0.5);
    buffer[50];
    sprintf(buffer, "Inspecting");
    renderText(0.020, 0.48, GLUT_BITMAP_HELVETICA_18, buffer);

    for (int i = 1; i <= numberOfProductionLinesForLiquid; i++) {
        sprintf(buffer, "%d", InspectionCounter[i]);
        renderText(x - 0.1, y + 0.20, GLUT_BITMAP_HELVETICA_18, buffer);
        sprintf(buffer, "%d", PackagingCounter[i]);
        renderText(x - 0.3, y + 0.20, GLUT_BITMAP_HELVETICA_18, buffer);
        sprintf(buffer, "Emp");
        renderText(x-0.463, y+0.2, GLUT_BITMAP_HELVETICA_18, buffer);
        sprintf(buffer, "%d", EmployeesCount[i]);
        renderText(x - 0.44, y + 0.13, GLUT_BITMAP_HELVETICA_18, buffer);
        drawRectangle(x - 0.1, y + 0.13, 0.1, 0.1, 0.0, 0.0, 1.0);
        drawRectangle(x - 0.3, y + 0.13, 0.1, 0.1, 0.5, 0.5, 0.5);
        drawRectangle(x, y, 1.0, 0.1, 1.0, 0.0 + j, 0.0 + j);
        sprintf(buffer, "Success");
        renderText(x-0.7, y+0.2, GLUT_BITMAP_HELVETICA_18, buffer);
        sprintf(buffer, "%d", Success[i]);
        renderText(x - 0.6, y + 0.1, GLUT_BITMAP_HELVETICA_18, buffer);
        sprintf(buffer, "Fail");
        renderText(x-0.63, y, GLUT_BITMAP_HELVETICA_18, buffer);
        sprintf(buffer, "%d", Fail[i]);
        renderText(x - 0.6, y - 0.1, GLUT_BITMAP_HELVETICA_18, buffer);
        j += 0.3;
        y -= 0.40;
    }
    x = -1.0;
    y = 0.70;
    j = 0.1;
    int c = 6;
    for (int i = 0; i < numberOfProductionLinesForPills; i++) {
        char buffer[50];
        sprintf(buffer, "%d", InspectionCounter[c]);
        renderText(x + 0.1, y + 0.20, GLUT_BITMAP_HELVETICA_18, buffer);
        sprintf(buffer, "%d", PackagingCounter[c]);
        renderText(x + 0.3, y + 0.20, GLUT_BITMAP_HELVETICA_18, buffer);
        sprintf(buffer, "Emp");
        renderText(x+0.42, y+0.2, GLUT_BITMAP_HELVETICA_18, buffer);
        sprintf(buffer, "%d", EmployeesCount[c]);
        renderText(x + 0.44, y + 0.13, GLUT_BITMAP_HELVETICA_18, buffer);
        drawRectangle(x + 0.1, y + 0.13, 0.1, 0.1, 0.0, 0.0, 1.0);
        drawRectangle(x + 0.3, y + 0.13, 0.1, 0.1, 0.5, 0.5, 0.5);
        drawRectangle(x, y, 1.0, 0.1, 0.0 + j, 1.0, 0.0 + j);
        sprintf(buffer, "Success");
        renderText(x+0.55, y+0.2, GLUT_BITMAP_HELVETICA_18, buffer);
        sprintf(buffer, "%d", Success[c]);
        renderText(x + 0.6, y + 0.1, GLUT_BITMAP_HELVETICA_18, buffer);
        sprintf(buffer, "Fail");
        renderText(x+0.57, y, GLUT_BITMAP_HELVETICA_18, buffer);
        sprintf(buffer, "%d", Fail[c]);
        renderText(x + 0.6, y - 0.1, GLUT_BITMAP_HELVETICA_18, buffer);
        j += 0.3;
        y -= 0.40;
        c++;
    }
    // glColor3f(1.0, 1.0, 1.0);
    // // Draw a rectangle in the middle of the screen
    // glBegin(GL_QUADS);
    // glVertex2f(0.3f, -0.075f); // Bottom left corner
    // glVertex2f(1.0f, -0.075f); // Bottom right corner
    // glVertex2f(1.0f, 0.075f); // Top right corner
    // glVertex2f(0.3f, 0.075f); // Top left corner
    // glEnd();
    // Flush the OpenGL buffers to the window
    glFlush();
}

// Timer function to redisplay OpenGL every 15 milliseconds
void timer(int value) {
    glutPostRedisplay(); // Request a redisplay
    glutTimerFunc(15, timer, 0);
}

// Main function
int main(int argc, char** argv) {
    for(int i=0; i<11; i++){
        EmployeesCount[i] = numEmp;
        Speed[i] = getRandomValue(minSpeed, maxSpeed, i);
    }
    glutInit(&argc, argv);
    numberOfProductionLinesForPills = atoi(argv[2]);
    numberOfProductionLinesForLiquid = atoi(argv[1]);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    // Create the window with the title "Rectangle"
    glutCreateWindow("Rectangle");

    // Initialize the OpenGL environment
    init();

    // Register the display callback function
    glutDisplayFunc(display);

    // Set the timer function
    glutTimerFunc(15, timer, 0);

    // Enter the GLUT main loop
    glutMainLoop();

    return 0;
}