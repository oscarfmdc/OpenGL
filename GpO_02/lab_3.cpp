﻿/************************  GPO_02 ************************************
ATG, 2015
******************************************************************************/


#include "GpO.h"


char* WINDOW_TITLE="MI PRIMER OBJETO 3D (GpO)";
int CurrentWidth = 600,   CurrentHeight = 450,  WindowHandle = 0;  // Tamaño ventana, handle a ventana
unsigned FrameCount = 0;



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////     CODIGO SHADERS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define GLSL(src) "#version 330 core\n" #src

const char* vertex_prog = GLSL(
layout(location = 0) in vec3 pos; 
layout(location = 1) in vec3 color;
out vec3 col;
uniform mat4 MVP=mat4(1.0f);
void main()
 {
  gl_Position = MVP*vec4(pos,1); // Construyo coord homogéneas y aplico matriz transformacion M
  col=color;                 // Paso color a fragment shader
 }
);


const char* fragment_prog = GLSL(
in vec3 col;
out vec4 outputColor;
void main() 
 {
  outputColor = vec4(col,1);
 }
);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   RENDER CODE AND DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLuint prog;
objeto cubo;

objeto crear_objeto(void)
{
	objeto obj;
	GLuint VAO;
	GLuint buffer, i_buffer;


	GLfloat vertex_data[] = {
		1.0f, 0.0f, 0.0f, 0.00f, 0.00f, 1.00f, // v0
		0.0f,-1.0f, 0.0f, 0.00f, 1.00f, 0.00f, // v1
		-1.0f, 0.0f, 0.0f, 0.00f, 1.00f, 1.00f, // v2
		0.0f, 1.0f, 0.0f, 1.00f, 0.00f, 0.00f, // v3
		0.0f, 0.0f, 1.0f, 1.00f, 1.00f, 0.00f, // v4
	};

	GLbyte  indices[] = {	
		0,4,1,
		0,3,4,
		4,3,2,
		4,2,1,
		2,3,0,
		2,0,1
	};

	obj.tipo_indice = GL_UNSIGNED_BYTE;
	obj.Nv = 5;
	obj.Ni = 6 * 3;


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

	// Especifico como encontrar 1er argumento (atributo 0) del vertex shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	// Defino 2º argumento (atributo 1) del vertex shader
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);  // Asignados atributos, podemos desconectar BUFFER

	glGenBuffers(1, &i_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glBindVertexArray(0);  //Cerramos Vertex Array con todo lidto para ser pintado

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	obj.VAO = VAO;

	return obj;

}


void dibujar_indexado(objeto obj)
{
  glBindVertexArray(obj.VAO);              // Activamos VAO asociado al objeto
  glDrawElements(GL_TRIANGLES,obj.Ni,obj.tipo_indice,(void*)0);  // Dibujar (indexado)
  glBindVertexArray(0);
}



// Compilación programas a ejecutar en la tarjeta gráfica:  vertex shader, fragment shaders
// Preparación de los datos de los objetos a dibujar, envialarlos a la GPU
// Opciones generales de render de OpenGL
void init_scene()  
{

  glEnable(GL_CULL_FACE);	
  cubo=crear_objeto();  // Datos del objeto, mandar a GPU
  prog=LinkShaders(vertex_prog,fragment_prog); // Compile shaders, crear programa a usar, Mandar a GPU
  glUseProgram(prog);    // Indicamos que programa vamos a usar 

}




vec3 pos_obs=vec3(4.0f,0.0f,0.0f);
vec3 target = vec3(0.0f,0.0f,0.0f);
vec3 up= vec3(0,0,1);

// Dibujar objetos 
// Actualizar escena: cambiar posición objetos, nuevos objetros, posición cámara, luces, etc.
void render_scene()
{
	FrameCount++;
	float tt = glutGet(GLUT_ELAPSED_TIME)/1000.0f;  // Tiempo en segundos

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Especifica color (RGB+alfa)
	glClear(GL_COLOR_BUFFER_BIT);                   // Aplica color asignado 

	///////// Aqui vendría nuestr código para actualizar escena  /////////

	mat4 P = perspective(40.0f, 4.0f / 3.0f, 0.1f, 20.0f);  //40º Y-FOV,  4:3 ,  Znear=0.1, Zfar=20
	mat4 V = lookAt(pos_obs, target,up  );  // Pos camara, Lookat, head up
	mat4 M,T,R,S;

	mat4 S2 = scale(vec3(1.0f,1.0f,1.5f));
	R = rotate(50*tt,vec3(0.0f,1.0f,0.0f));
	float fase = 2*3.14*tt/6;
	T = translate(1.5f*sin(fase),1.0f*cos(fase),0.0f);
	M = T*R*S2;	

	transfer_mat4("MVP",P*V*M);
    dibujar_indexado(cubo);

	////////////////////////////////////////////////////////

	glutSwapBuffers();  // Intercambiamos buffers de refresco y display
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////  INTERACCION  TECLADO RATON
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void keyboard(unsigned char, int, int);
void key_special(int, int, int);
void mouse(int,int, int, int);
void mouse_mov(int,int);


void eventos_teclado_mouse()
{
	glutKeyboardFunc(keyboard);      // Caso de pulsar alguna tecla
//	glutSpecialFunc(key_special);  // Teclas de función, cursores, etc

//	glutMouseFunc(mouse);           // Eventos del ratón
//	glutPassiveMotionFunc(mouse_mov); // Mov del raón
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	  case 27:  // Escape key
		  glutLeaveMainLoop(); // Salimos del bucle  
		  return;
		  break;
     
	}
}

void key_special(int key, int x, int y)
{
  switch (key)
	{
	  case GLUT_KEY_F1:  // Teclas de Funcion
	    break;
	  case GLUT_KEY_UP:  //Teclas cursor;  
       
		break;         
	  case GLUT_KEY_DOWN:

         break;
	  case GLUT_KEY_LEFT:
	     break;
      case GLUT_KEY_RIGHT:
		break;
	}
}

void mouse(int but,int state, int x, int y)
{
}

void mouse_mov(int x, int y)
{
	fprintf(stdout,"Pos (%3d,%3d)\n",x-CurrentWidth/2,y-CurrentHeight/2);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GLUT:  CREACION VENTANAS E INTERACCION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void Init_Opengl(void);
void Init_Window(int, char*[]);
void cambia_window(int, int);
void TimerFunction(int);
void IdleFunction(void);
void clean_up(void);


int main(int argc, char* argv[])
{
 Init_Window(argc, argv);  // Prepara y abre ventana
 Init_Opengl();            // Inicializa OpenGL, comprueba versión.
 init_scene();             // Preppara escena
 glutMainLoop();           // Entra en bucle (render_scene se ejecuta constantemente)
 exit(EXIT_SUCCESS);
}


void Init_Opengl(void)
{   
   LoadFunctions();
   fprintf(stdout,"OpenGL Version:  ",glGetString(GL_VERSION));
   if(IsVersionGEQ(3,3)) printf(">=3.3 => OK.\n");
   else
	{
	 printf("Se necesita OpenGL >= 3.3 para estos ejemplos.\n");
	 glutDestroyWindow(WindowHandle);
	 exit(EXIT_FAILURE);
	}
	
}


void Init_Window(int argc, char* argv[])
{
    glutInit(&argc, argv);
	
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA);
    glutInitContextVersion(3,3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	fprintf(stdout,"Pantalla  %d x %d pixels\n",glutGet(GLUT_SCREEN_WIDTH),glutGet(GLUT_SCREEN_HEIGHT));


    // Creamos ventana centrada en pantalla
    glutInitWindowSize(CurrentWidth, CurrentHeight);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-CurrentWidth)/2,(glutGet(GLUT_SCREEN_HEIGHT)-CurrentHeight)/2);
    WindowHandle = glutCreateWindow("TITULO");
    if(WindowHandle < 1) 
	  { 
	    fprintf(stderr,"ERROR: Could not create a new rendering window.\n"); exit(EXIT_FAILURE);
      }
	else   fprintf(stderr,"Ventana creada (%d)\n",WindowHandle);


	// Asociar funciones de eventos
	glutDisplayFunc(render_scene);   //render, llamada de forma continua en el bucle
	glutReshapeFunc(cambia_window); // Caso de cambiar tamaño de ventana
    glutIdleFunc(IdleFunction);
    glutTimerFunc(200, TimerFunction, 0);
	glutCloseFunc(clean_up);


	eventos_teclado_mouse();

}

//Called whenever the window is resized. 
void cambia_window(int Width, int Height)
{
    CurrentWidth = Width; CurrentHeight = Height;
	
    glViewport(0, 0, CurrentWidth, CurrentHeight);
}

void IdleFunction(void)
{
    glutPostRedisplay();
}
 

void clean_up(void) // Borrar los objetos creados (programas, objetos graficos)
{
 glDeleteProgram(prog);
 glDeleteVertexArrays(1, &(cubo.VAO));
}


