#include "GLOS.h"

#include "gl.h"
#include "glu.h"
#include "glut.h"
#include "glaux.h"

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

const int x = 0, y = 1, z = 2, w = 3; // used only for indexing
const int A = 0, B = 1, C = 2, D = 3; // used only for indexing

static float lightSourcePosition[4] = { -40, 70, -40, 1 };
static float angle = 0;
static float pozitie = 0;
static float unghimotor = 0;
static float miscareapa = 0.0;
static float opacitateapa = 1.0;


GLfloat punctePlanIarba[][3] = {
	{ -250.0f, -10.0f, -150.0f },
	{ -250.0f, -10.0f, 250.0f },
	{ 250.0f, -10.0f, 250.0f } ,
	{ 250.0f, -10.0f, -150.0f }
};

float coeficientiPlanIarba[4];
float matriceUmbrire[4][4];


void CALLBACK rotireDreapta() {
	angle -= 2;
}

void CALLBACK rotireStanga() {
	angle += 2;
}
void CALLBACK mutafata()
{
	pozitie += 1;
}
void CALLBACK mutaspate()
{
	pozitie -= 1;
}



void computePlaneCoefficientsFromPoints(float points[3][3]) {
	// calculeaza 2 vectori din 3 puncte
	float v1[3]{ points[0][x] - points[1][x], points[0][y] - points[1][y], points[0][z] - points[1][z] };
	float v2[3]{ points[1][x] - points[2][x], points[1][y] - points[2][y], points[1][z] - points[2][z] };

	// produsul vectorial al celor 2 vectori => al 3lea vector cu componentele A,B,C chiar coef din ec. planului
	coeficientiPlanIarba[A] = v1[y] * v2[z] - v1[z] * v2[y];
	coeficientiPlanIarba[B] = v1[z] * v2[x] - v1[x] * v2[z];
	coeficientiPlanIarba[C] = v1[x] * v2[y] - v1[y] * v2[x];

	// determinam D - ecuatia planului in punctul random ales trebuie sa fie zero
	int randomPoint = 1; // poate fi orice punct de pe planul ierbii, asa ca alegem unul din cele 3 folosite pentru calcule
	coeficientiPlanIarba[D] =
		-(coeficientiPlanIarba[A] * points[randomPoint][x] +
			coeficientiPlanIarba[B] * points[randomPoint][y] +
			coeficientiPlanIarba[C] * points[randomPoint][z]);
}

void computeShadowMatrix(float points[3][3], float lightSourcePosition[4]) {
	// determina coef planului	
	computePlaneCoefficientsFromPoints(points);

	// temp = AxL + ByL + CzL + Dw
	float temp =
		coeficientiPlanIarba[A] * lightSourcePosition[x] +
		coeficientiPlanIarba[B] * lightSourcePosition[y] +
		coeficientiPlanIarba[C] * lightSourcePosition[z] +
		coeficientiPlanIarba[D] * lightSourcePosition[w];

	//prima coloana
	matriceUmbrire[0][0] = temp - coeficientiPlanIarba[A] * lightSourcePosition[x];
	matriceUmbrire[1][0] = 0.0f - coeficientiPlanIarba[B] * lightSourcePosition[x];
	matriceUmbrire[2][0] = 0.0f - coeficientiPlanIarba[C] * lightSourcePosition[x];
	matriceUmbrire[3][0] = 0.0f - coeficientiPlanIarba[D] * lightSourcePosition[x];
	//a 2a coloana
	matriceUmbrire[0][1] = 0.0f - coeficientiPlanIarba[A] * lightSourcePosition[y];
	matriceUmbrire[1][1] = temp - coeficientiPlanIarba[B] * lightSourcePosition[y];
	matriceUmbrire[2][1] = 0.0f - coeficientiPlanIarba[C] * lightSourcePosition[y];
	matriceUmbrire[3][1] = 0.0f - coeficientiPlanIarba[D] * lightSourcePosition[y];
	//a 3a coloana
	matriceUmbrire[0][2] = 0.0f - coeficientiPlanIarba[A] * lightSourcePosition[z];
	matriceUmbrire[1][2] = 0.0f - coeficientiPlanIarba[B] * lightSourcePosition[z];
	matriceUmbrire[2][2] = temp - coeficientiPlanIarba[C] * lightSourcePosition[z];
	matriceUmbrire[3][2] = 0.0f - coeficientiPlanIarba[D] * lightSourcePosition[z];
	//a 4a coloana
	matriceUmbrire[0][3] = 0.0f - coeficientiPlanIarba[A] * lightSourcePosition[w];
	matriceUmbrire[1][3] = 0.0f - coeficientiPlanIarba[B] * lightSourcePosition[w];
	matriceUmbrire[2][3] = 0.0f - coeficientiPlanIarba[C] * lightSourcePosition[w];
	matriceUmbrire[3][3] = temp - coeficientiPlanIarba[D] * lightSourcePosition[w];
}

void myInit(void) {
	glClearColor(0.0, 0.5, 1.0, 1.0);

	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// sursa lumina
	float lightAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightSourcePosition);

	// material
	float materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float materialShininess[] = { 128.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);

	// use current color set with glColor3f() to directly set the material ambient and diffuse
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);
}

void deseneazaCilindru() {
	GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH); // default
	gluQuadricDrawStyle(quadric, GLU_FILL);
	double baseRadius = 10;
	double topRadius = 10;
	double height = 35;
	int slices = 360;
	int stacks = 30;
	gluCylinder(quadric, baseRadius, topRadius, height, slices, stacks);
	gluDeleteQuadric(quadric);
}

void desenareModel(bool umbra) 
{
	glEnable(GL_BLEND);
	if (umbra) {
		glColor3f(0.0, 0.22, 0.5);
	}
	else {
		glColor3f(0.3, 0.3, 0.3);
	}
	//roteste submarinul
	glRotatef(angle,0.0,1.0,0.0);
	glTranslatef(pozitie, 0.0, 0.0);
	
	//corp submarin
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	auxSolidCylinder(8.0, 30.0);
	glPopMatrix();
	//varf submarin
	glPushMatrix();
	glTranslatef(30.0, 0.0, 0.0);
	auxSolidSphere(7.8);
	glPopMatrix();
	//capat submarin
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	auxSolidSphere(7.8);
	glPopMatrix();
	//elice motor
	glPushMatrix();
	glRotatef(unghimotor, 1.0, 0.0, 0.0);
	glTranslatef(-8.0, 6.0, 0.0);
	auxSolidCylinder(0.5, 14.0);
	glPopMatrix();
	//elice motor
	glPushMatrix();
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(unghimotor, 1.0, 0.0, 0.0);
	glTranslatef(-8.0, 6.0, 0.0);
	auxSolidCylinder(0.5, 14.0);
	glPopMatrix();
	//vizor
	glPushMatrix();
	glTranslatef(25.0, 12.0, 0.0);
	auxSolidCylinder(2.0, 7.0);
	glPopMatrix();
	//varf vizor
	glPushMatrix();
	glTranslatef(25.0, 13.0, 0.0);
	auxSolidSphere(2.0);
	glPopMatrix();
	//aripi laterale
	glPushMatrix();
	glRotatef(90, 1.0, 0.0, 0.0);
	glTranslatef(6.0, 0.0, 0.0);
	auxSolidTorus(1.0, 8.5);
	glPopMatrix();
	//aripi sus jos
	glPushMatrix();
	glTranslatef(6.0, 0.0, 0.0);
	auxSolidTorus(1.0, 8.5);
	glPopMatrix();

	//
	glPushMatrix();
	glEnable(GL_BLEND);
	if (umbra) {
		glColor4f(0.5, 0.5, 0.5, opacitateapa/5);
	}
	else {
		glColor4f(1.0, 1.0, 1.0, opacitateapa);
	}
	glTranslatef(-8.0, 5.0, -3.0);
	glTranslatef(miscareapa, 0.0, 0.0);
	auxSolidSphere(2.0);
	glPopMatrix();
	//
	glPushMatrix();
	glEnable(GL_BLEND);
	if (umbra) {
		glColor4f(0.5, 0.5, 0.5, opacitateapa / 5);
	}
	else {
		glColor4f(1.0, 1.0, 1.0, opacitateapa);
	}
	glTranslatef(-7.0, 1.0, 0.0);
	glTranslatef(miscareapa *3.5, 0.0, 0.0);
	auxSolidSphere(2.0);
	glPopMatrix();
	//
	glPushMatrix();
	glEnable(GL_BLEND);
	if (umbra) {
		glColor4f(0.5, 0.5, 0.5, opacitateapa / 5);
	}
	else {
		glColor4f(1.0, 1.0, 1.0, opacitateapa);
	}
	glTranslatef(-8.0, -3.0, 2.0);
	glTranslatef(miscareapa *2, 0.0, 0.0);
	auxSolidSphere(2.0);
	glPopMatrix();
	//
	glPushMatrix();
	glEnable(GL_BLEND);
	if (umbra) {
		glColor4f(0.5, 0.5, 0.5, opacitateapa / 5);
	}
	else {
		glColor4f(1.0, 1.0, 1.0, opacitateapa);
	}
	glTranslatef(-7.0, -1.0, 1.0);
	glTranslatef(miscareapa * 2.9, 0.0, 0.0);
	auxSolidSphere(2.0);
	glPopMatrix();
	//
	glPushMatrix();
	glEnable(GL_BLEND);
	if (umbra) {
		glColor4f(0.5, 0.5, 0.5, opacitateapa / 5);
	}
	else {
		glColor4f(1.0, 1.0, 1.0, opacitateapa);
	}
	glTranslatef(-9.0, 2.0, -3.0);
	glTranslatef(miscareapa *1.8, 0.0, 0.0);
	auxSolidSphere(2.0);
	glPopMatrix();



}

void desenareIarba() {
	glPushMatrix();
	glColor3f(0.0, 0.35, 1.0);
	glTranslatef(0, -0.1, 0);
	glBegin(GL_QUADS);
	{
		glNormal3f(0, 1, 0);
		for (int i = 0; i < 4; i++) {
			glVertex3fv(punctePlanIarba[i]);
		}
	}
	glEnd();
	glPopMatrix();
}

void deseneazaLumina()
{
	glPushMatrix();
	glTranslatef(lightSourcePosition[x], lightSourcePosition[y], lightSourcePosition[z]);
	glColor3f(1.0, 0.9, 0);
	auxSolidSphere(5.0);
	glPopMatrix();
}

void CALLBACK display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0, 0, -150);
	glRotatef(15, 1, 0, 0);

	computeShadowMatrix(punctePlanIarba, lightSourcePosition);

	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, lightSourcePosition);
	desenareIarba();
	desenareModel(false);
	glPopMatrix();

	//deseneaza umbra
	glDisable(GL_LIGHTING);
	deseneazaLumina();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	glPushMatrix();
	glMultMatrixf((GLfloat*)matriceUmbrire); // se inmulteste matricea curenta cu matricea de umbrire
	desenareModel(true);
	glPopMatrix();

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);

	auxSwapBuffers();
}
void CALLBACK IdleFunction(void)

{

	unghimotor += 3;
	miscareapa -= 0.1;
	opacitateapa -= 0.01;
	if (opacitateapa < -0.2)
	{
		miscareapa = 0.0;
		opacitateapa = 1.0;

	}
		
	
		

	display();
	Sleep(1);

}

void CALLBACK myReshape(GLsizei w, GLsizei h) {
	if (!h) return;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0 * (GLfloat)w / (GLfloat)h, 0.5, 300.0);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
	auxInitDisplayMode(AUX_DOUBLE | AUX_DEPTH24   | AUX_RGB);
	auxInitPosition(10, 10, 1200, 800);

	auxKeyFunc(AUX_LEFT, rotireDreapta);
	auxKeyFunc(AUX_RIGHT, rotireStanga);
	auxKeyFunc(AUX_UP, mutaspate);
	auxKeyFunc(AUX_DOWN, mutafata);
	auxInitWindow("Submarin");
	myInit();
	auxReshapeFunc(myReshape);
	auxIdleFunc(IdleFunction);

	auxMainLoop(display);
	return 0;
	
}