#include <iostream>
#include <GL/glut.h>
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include<GL/freeglut.h>
using namespace std;
timeval ti;

#define WIDTH 1600
#define HEIGHT 900
#define scaling_speed 10

int power = 1, shot_flag = 0, flag_p = 1,firsttime = 1, flagg = 1, ball_count = 0, begin = 1, points = 0, view_x1 = 0, view_x2 = 0, view_y1 = 0, view_y2 = 0; 
float rot_angle = 0, ball_x = 0, ball_y = 0, projection_velocity = 15 * power, xi =0, xf = 0, yi = 0, yf = 0, left_mouse = 0, rect_y = 0, zoom_x = 0, zoom_y = 0;
double t0 = 0, t1 = 0;

class Obstacle{
	public:
		double ob_x;
		float ob_y;
		float origin_x;
		float origin_y;
		float theta;
		float vx;
		float vy;

		Obstacle (float ob_x = 0, float ob_y = 0,int origin_x = 0, int origin_y = 0,float theta = 0,float vx = 0, float vy = 0) {
			ob_x = ob_x;
			ob_y = ob_y;
			origin_x = origin_x;
			origin_y = origin_y;
			theta = theta;
			vx = vx;
			vy = vy;
		}

		void build_ob(int,int,int);

		int check_collision(int,int);

		//gives angles in radians that line of contact makes with positive x-axis
		float line_of_contact(int,int);

		float update(int);
};

Obstacle ob[10];

void Obstacle::build_ob(int i, int x, int y){
	origin_x = x;
	origin_y = y;
		float x1=0,y1=0,z1=0;
	if(i == 0){
		glBegin(GL_POLYGON);
	for(int j=0; j < 360; j+=20){
		x1 = ((j + 250) % 255) / 255.0;
		y1 = ((j +225) % 255) / 255.0;
		z1 = ((j + 200) % 255) / 255.0;
		glColor3f(x1,y1,z1);
		glVertex2f(20 * cos(j * (3.14 / 180)) + origin_x + ob_x,20 * sin(j * (3.14 / 180)) + origin_y + ob_y);
	}
		glEnd();
	}
	else{
	glBegin(GL_POLYGON);
	for(int j=0; j < 360; j+=5){
		x1 = ((j + 50) % 255) / 255.0;
		y1 = ((j + 25) % 255) / 255.0;
		z1 = ((j + 75) % 255) / 255.0;
		glColor3f(x1,y1,z1);
		glVertex2f(20 * cos(j * (3.14 / 180)) + origin_x + ob_x,20 * sin(j * (3.14 / 180)) + origin_y + ob_y);
	}
	glEnd();
	}
}

int Obstacle::check_collision(int i, int j){
	if( pow((origin_x + ob_x - ob[j].origin_x - ob[j].ob_x),2) + pow((origin_y + ob_y - ob[j].origin_y - ob[j].ob_y),2) <= pow(40,2)){
		return 1;
	}  
	else{
		return 0;
	}	

}

float Obstacle::line_of_contact(int i, int j) {

	theta = -(atan((origin_x + ob_x - ob[j].origin_x - ob[j].ob_x) / (origin_y + ob_y - ob[j].origin_y - ob[j].ob_y) ));

	float temp_vx = vx, temp_vy = vy,temp_o_vx = ob[j].vx, temp_o_vy = ob[j].vy;
	
	//equations of collision
	if( ( vx * cos( 1.57 - theta) - vy * cos(theta) ) - (ob[j].vx * cos( 1.57 - theta) - ob[j].vy * cos(theta) ) > 1 ){
	printf("IN\n");
	vx = ( temp_o_vx * cos(1.57 - theta) - temp_o_vy * cos(theta)) * cos(1.57 - theta) + (temp_vx * cos(theta) + temp_vy * cos(1.57 - theta)) * cos(theta);
	vy = - ( temp_o_vx * cos(1.57 - theta) - temp_o_vy * cos(theta)) * cos(theta) + (temp_vx * cos(theta) + temp_vy * cos(1.57 - theta)) * cos(1.57 - theta);
	ob[j].vx = (temp_vx * cos(1.57 - theta) - temp_vy * cos(theta)) * cos(1.57 - theta) + (temp_o_vx * cos(theta) + temp_o_vy * cos(1.57 - theta)) * cos(theta);
	ob[j].vy = - (temp_vx * cos(1.57 - theta) - temp_vy * cos(theta)) * cos(theta) + (temp_o_vx * cos(theta) + temp_o_vy * cos(1.57 - theta)) * cos(1.57 - theta);
	
	}
}

float Obstacle::update(int i){
	if( i == 0 ){
		//printf("%f\n",ob_y + origin_y);
		if( vx < 0.01 && vx >= -0.01 && ( ob_y + origin_y <= 20 || (ob_x  == 0 && ob_y  == 0)) && vy < 0.01 && vy > -0.01 ) {
			vx = 0;
			vy = 0;
			begin = 1, shot_flag = 0;
		//	printf("IN END\n");
		}
		else {
			ob_x = ob_x + (vx * (t1 - t0) ) * scaling_speed;	
			ob_y = ob_y + ( vy * (t1 - t0) * scaling_speed -  5 * (t1 - t0) * (t1 - t0) * scaling_speed );
			vx = 0.99999 * vx;
			vy = 0.99999 * (vy - 10 * (t1 - t0) );
			if( (vy > 0 && origin_y + ob_y >= 830) || ( vy < 0 && origin_y + ob_y <= 20)){
				vy = - 2.0 / 3.0 * vy;
				vx = 0.9 * vx;
			}
			else if ( origin_x + ob_x <= 20) {
				vx = 0.9 * vx;
			}
			if( (vx > 0 && origin_x + ob_x >= 1580) || ( vx < 0 && origin_x + ob_x <= 20)){
				vx = - 2.0 / 3.0 * vx;
				vy = 0.9 * vy;
			}
		}

	}
	else {
		if( (vy <= 0.01 && vy >= -0.01) && (vx <= 0.01 && vy >= -0.01) && (origin_y + ob_y <= 20 || (ob_y == 0 && ob_x == 0))) {
			vx = 0;
			vy = 0;
		}
		else {
			ob_x = ob_x + (vx * (t1 - t0) ) * scaling_speed;	
			ob_y = ob_y + ( vy * (t1 - t0) * scaling_speed -  5 * (t1 - t0) * (t1 - t0) * scaling_speed );
			vx = 0.99999 * vx;
			vy = 0.99999 * (vy - 10 * (t1 - t0) );
			if( (vy > 0 && origin_y + ob_y >= 780) || ( vy < 0 && origin_y + ob_y <= 20)){
				vy = - 2.0 / 3.0 * vy;
				vx = 0.9 * vx;
			}
			else if ( origin_x + ob_x <= 20) {
				vx = 0.9 * vx;
			}
			if( (vx > 0 && origin_x + ob_x >= 1580) || ( vx < 0 && origin_x + ob_x <= 20)){
				vx = - 2.0 / 3.0 * vx;
				vy = 0.9 * vy;
			}
		}
	}
}

void mouse (int button, int state, int x, int y) {
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON){
		left_mouse = 1;
	}
	if(state == GLUT_UP && button == GLUT_LEFT_BUTTON ){
		left_mouse = 0;
		if(ball_count <= 5 && begin == 1){
			shot_flag = 1;
			projection_velocity = 15 * power;
			ob[0].vx = projection_velocity * sin(-rot_angle * (3.14 / 180));
			ob[0].vy = projection_velocity * cos(-rot_angle * (3.14 / 180));
			ob[0].ob_x = 0, ob[0].ob_y = 0;
			ball_count++;
		}
	}
}

void motion(int x, int y){
	xf = x;
	yf = y;
	if(left_mouse){
		if(yf > yi && rot_angle > -60)
			rot_angle -= 1;
		else if(yf < yi && rot_angle < 60)
			rot_angle += 1;
		if(xf > xi && power < 100)
			power += 1;
		else if(xf < xi && power > 0)
			power -= 1;
	}
	xi = xf;
	yi = yf;
}


void keyboard( unsigned char key, int x, int y ){
	if ( key == 'q' ){
		if(begin == 1){
			shot_flag = 0;
			ob[0].ob_x = 0, ob[0].ob_y = 0;
		}
		if(power > 1){
			power--;
		}
	}
	else if ( key == 'w'){
		if(begin == 1){
			shot_flag = 0;
			ob[0].ob_x = 0, ob[0].ob_y = 0;
		}
		if( power < 10){
			power++;
		}
	}
	else if ( key == 'a' ){
		if(begin == 1){
			shot_flag = 0;
			ob[0].ob_x = 0, ob[0].ob_y = 0;
			rot_angle++;
		}

	}
	else if ( key == 's'){
		if(begin == 1){
			shot_flag = 0;
			ob[0].ob_x = 0, ob[0].ob_y = 0;
			rot_angle--;
		}
	}
	else if ( key == ' ' ){
		if(ball_count <= 5 && begin == 1){
			shot_flag = 1;
			projection_velocity = 15 * power;
			ob[0].vx = projection_velocity * sin(-rot_angle * (3.14 / 180));
			ob[0].vy = projection_velocity * cos(-rot_angle * (3.14 / 180));
			ob[0].ob_x = 0, ob[0].ob_y = 0;
			ball_count++;
		}
	}
	else if ( key == 'n') {
		rect_y = 0;
		shot_flag = 0, begin = 1;
		flagg = 1, points = 0;
		//Obstacle Obstacle();
		for(int i = 0; i < 5; i++) {
			ob[i].ob_x = 0, ob[i].ob_y = 0, ob[i].vx = 0, ob[i].vy = 0;
		}
		ball_count = 0, power = 1, rot_angle = 0;
		for ( int i = 1; i < 5; i++){
			ob[i].build_ob(i,300 + rand() % 1200, 200 + rand() % 550);
		}
	}
	else if ( key == '+' ) {
		if(zoom_x <= 2){
		zoom_x += 0.05;
		zoom_y += 0.05;
		}

	
	}
	else if ( key == '-' ) {
		if(zoom_x >= - 0.9){
		zoom_x -= 0.05;
		zoom_y -= 0.05;
		}
	}
	else if ( key == '8' ) {
		view_y1 += 10;
//		view_y2 += 10;
//		glViewport(view_x1, view_y1, view_x2, view_y2);
	}
	else if ( key == '4' ) {
		view_x1 -= 10;
	//	view_x2 -= 100;
	//	glViewport(view_x1, view_y1, view_x2, view_y2);

	}
	else if ( key == '6' ) {
		view_x1 += 10;
	//	view_x2 += 100;
	//	glViewport(view_x1, view_y1, view_x2, view_y2);

	}
	else if ( key == '2' ) {
		view_y1 -= 10;
	//	view_y2 -= 100;
	//	glViewport(view_x1, view_y1, view_x2, view_y2);

	}
}

void checkpoint (void) {
	for(int i = 1; i < 5; i++){
		if( ob[i].ob_x + ob[i].origin_x >= 1530 && ( ob[i].ob_y + ob[i].origin_y >= 200 || ob[i].ob_y + ob[i].origin_y <= 650) ) {
			points = points + 10;
		}
	}

}

void fire (void) {

	glPushMatrix();
	glColor3f(1,1,0);
	glTranslatef(60,20,0);
	glRotatef(rot_angle,0,0,1);
	glTranslatef(0,120,0);
	glRotatef(-rot_angle,0,0,1);

	//Checking collision
	for(int i = 0; i < 4; i++) {
		for(int j = i+1; j < 5; j++){
			if( ob[i].check_collision(i,j) ){
				ob[i].line_of_contact(i,j);
			//	printf("ob[1].theta=%f\n",ob[i].theta);
				//		exit(0);
			}
		}

	}
	glPopMatrix();

	checkpoint();

			glColor3f(1,0,1);
			ob[0].build_ob(0,60 + 120 * sin(-rot_angle * (3.14 / 180)), 20 + 120 * cos(-rot_angle * (3.14 / 180)));
	
		if(flagg == 1){
		glColor3f(0,1,0);
		ob[1].build_ob(1,ob[1].origin_x, ob[1].origin_y);
		ob[2].build_ob(2,ob[2].origin_x, ob[2].origin_y);
		ob[3].build_ob(3,ob[3].origin_x, ob[3].origin_y);
		ob[4].build_ob(4,ob[4].origin_x, ob[4].origin_y);
	}

	gettimeofday(&ti,NULL);	
	t1 = ti.tv_sec + ti.tv_usec / 1000000.0;
	for ( int i = 0; i < 5; i++){
		if(i == 0 && shot_flag == 1 && ball_count <= 5){
			ob[i].update(i);
			begin = 0;
	//	printf("IN\n");
		}
		else{
			ob[i].update(i);
		}
		}
	
	t0 = t1;

}

void canon (void){

	glTranslatef(0,50,0);
	glColor3f(0,191.0/255.0,1);
	glBegin(GL_QUADS);
	glVertex2f(0,0);
	glVertex2f(1600,0);
	glVertex2f(1600,850);
	glVertex2f(0,850);
	glEnd();

	glColor3f(0,0,0);
	char sc[100];
	sprintf(sc,"Score : %d",points);
	glRasterPos3d(10, 740, 0);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,(unsigned char*)sc);

	glColor3f(1,0,0);
	glBegin(GL_QUADS);
	glVertex2f(1600,200);
	glVertex2f(1600,650);
	glVertex2f(1550,650);
	glVertex2f(1550,200);
	glEnd();

//	glBegin(GL_LINES);
//	glVertex2f(0,0);
//	glVertex2f(1600,0);
//	glEnd();

	glColor3f(1,127.0/255.0,0);
	glBegin(GL_POLYGON);
	for ( int i = 0; i <= 180; i++ ) {
		glVertex2f(60 + 40 * cos( i * (3.14 / 180)), 40 * sin( i * (3.14 / 180)));
	}
	glEnd();

	//Deciding canon movement
//	glColor3f(0,1,0);
	glPushMatrix();
	glTranslatef(60,20,0);
	glRotatef(rot_angle,0,0,1);

	glBegin(GL_QUADS);
	glVertex2f(-20,0);
	glVertex2f(-20,100);
	glVertex2f(20,100);
	glVertex2f(20,0);
	glEnd();
	glPopMatrix();

	//Fires the canon
	fire();
	
	//Grass
	glColor3f(0.196,0.804,0.196);
	glBegin(GL_QUADS);
	glVertex2f(0,0);
	glVertex2f(1600,0);
	glVertex2f(1600,-10);
	glVertex2f(0,-10);
	glEnd();

	//Soil
	glColor3f(0.545,0.537,0);
	glBegin(GL_QUADS);
	glVertex2f(0,-10);
	glVertex2f(1600,-10);
	glVertex2f(1600,-50);
	glVertex2f(0,-50);
	glEnd();


	glTranslatef(10,HEIGHT - 100,0);
	glColor3f(0,0,1);

	int j = 2,dummy_power;
	dummy_power = power;
	for(int i=0;i<10;i++){
		glBegin(GL_QUADS);
		glVertex2f(0+i*10,0);
		glVertex2f(0+i*10,5*j);
		glVertex2f(5+i*10,5*j);
		glVertex2f(5+i*10,0);
		glEnd();
		if(dummy_power>1){
			dummy_power--;
		}
		else{
			j=1;
		}
	}

	glTranslatef(0,-25,0);
	glColor3f(0,0,1);
	for(int i = 0; i < 5 - ball_count; i++) {
		glBegin(GL_POLYGON);
		for(int j = 0; j < 360; j++) { 
			glVertex2f(5 * cos(j * (3.14 / 180)) + i * 15, 5 * sin(j * (3.14 / 180)));
		}
		glEnd();
	}
}

void display( void ) {

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glScalef(1 + zoom_x, 1 + zoom_y, 1);
	glColor3f(1.0f,0,0);

	glTranslatef(view_x1,view_y1,0);
	//Calling canon function
	glPushMatrix();
	canon();
	glPopMatrix();

	glutSwapBuffers();

	//Here it uses the above mentioned color to fill the window.
}

int main( int argc, char **argv )
{

	int srand(100);

	gettimeofday(&ti,NULL);	
	t0 = ti.tv_sec + ti.tv_usec / 1000000.0;
	//Initializations related to GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowSize( WIDTH, HEIGHT );
	glutCreateWindow( "Transformations in 2D" );
	//Telling Glut about which function does what
	glutDisplayFunc( display );
	glutIdleFunc( display );
	//	glutTimerFunc(1,update_coordinate,0);
	glutKeyboardFunc( keyboard );
	glutMouseFunc( mouse );
	glutMotionFunc( motion );

	//Setting the OpenGL Init State.

	//Following command decides how much part of window to be used for rendering.
	glViewport( 0, 0, WIDTH, HEIGHT );
	//For Time being assume this is Magic
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glScalef(1 + zoom_x, 1 + zoom_y, 1);
	gluOrtho2D ( 0, WIDTH, 0, HEIGHT );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	//Your transformation are controlled by the following Matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glScalef(1 + zoom_x, 1 + zoom_y, 1);

	glutMainLoop();

	return 0;
}
