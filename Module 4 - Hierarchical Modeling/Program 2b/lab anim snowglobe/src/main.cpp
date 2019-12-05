/*
ZJ Wood CPE 471 Lab 3 base code
*/

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;
double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime = glfwGetTime();
	double difference = actualtime - lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		pos = glm::vec3(0, 0, -15);
		rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process(double frametime)
	{
		double ftime = frametime;
		float speed = 0;
		if (w == 1)
		{
			speed = 10 * ftime;
		}
		else if (s == 1)
		{
			speed = -10 * ftime;
		}
		float yangle = 0;
		if (a == 1)
			yangle = -1 * ftime;
		else if (d == 1)
			yangle = 1 * ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed, 1);
		dir = dir * R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R * T;
	}
};

static float PI = 3.1415926;
float snowFall = 2;
camera mycam;

float incSnow(float* snowFall) {
	if (*snowFall < -1.75) {
		*snowFall = 1.75;
	}
	return -0.01;
}

class Application : public EventCallbacks
{


public:
	float rotation = 0;

	int kn = 0;
	WindowManager* windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> shapeprog;
	std::shared_ptr<Program> cylinderProg;
	std::shared_ptr<Program> globeProg;
	std::shared_ptr<Program> snowProg;



	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexColorIDBox, IndexBufferIDBox;

	Shape shape;

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{	
		/*
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
		if (key == GLFW_KEY_N && action == GLFW_PRESS) kn = 1;
		if (key == GLFW_KEY_N && action == GLFW_RELEASE) kn = 0;

		*/
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			rotation -= 0.1;
		}
		else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			rotation += 0.1;
		}
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow* window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX << " Pos Y " << posY << std::endl;

		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow* window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}


	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{

		string resourceDirectory = "../resources";
		//try t800.obj or F18.obj ...
		shape.loadMesh(resourceDirectory + "/sphere.obj");
		shape.resize();
		shape.init();


		//generate the VAO for the Cylinder
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		glm::vec3 cylinder[3 * 480];

		float x = 0;
		float y = 0.5;
		float z = 0;
		float nextX = 0;
		float nextZ = 0;
		float deg = 0;
		float nextDeg = 0;
		float rad = 0;
		float nextRad = 0;
		float toRad = PI / 180;


		for (int i = 0; i < 480; i += 12)
		{
			int a = i * 3;
			int b = i * 3 + 1;
			int c = i * 3 + 2;
			int d = i * 3 + 3;
			int e = i * 3 + 4;
			int f = i * 3 + 5;
			int g = i * 3 + 6;
			int h = i * 3 + 7;
			int j = i * 3 + 8;
			int k = i * 3 + 9;
			int l = i * 3 + 10;
			int m = i * 3 + 11;

			rad = toRad * deg;
			x = cos(rad);
			z = sin(rad);

			nextDeg = deg + 9;
			nextRad = toRad * nextDeg;
			nextX = cos(nextRad);
			nextZ = sin(nextRad);



			cylinder[a] = glm::vec3(x, y, z);
			cylinder[b] = glm::vec3(x, -y, z);
			cylinder[c] = glm::vec3(nextX, -y, nextZ);

			cylinder[g] = glm::vec3(x, y, z);
			cylinder[h] = glm::vec3(nextX, y, nextZ);
			cylinder[j] = glm::vec3(nextX, -y, nextZ);

			cylinder[d] = glm::vec3(0, y, 0);
			cylinder[e] = glm::vec3(x, y, z);
			cylinder[f] = glm::vec3(nextX, y, nextZ);

			cylinder[k] = glm::vec3(0, y, 0);
			cylinder[l] = glm::vec3(x, -y, z);
			cylinder[m] = glm::vec3(nextX, y, nextZ);

			deg += 9;
		}

		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(cylinder), cylinder, GL_DYNAMIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}


	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		// Enable blending/transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Initialize the Cylinder program.
		cylinderProg = std::make_shared<Program>();
		cylinderProg->setVerbose(true);
		cylinderProg->setShaderNames(resourceDirectory + "/cylinder_vertex.glsl", resourceDirectory + "/cylinder_fragment.glsl");
		if (!cylinderProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		cylinderProg->addUniform("P");
		cylinderProg->addUniform("V");
		cylinderProg->addUniform("M");
		cylinderProg->addAttribute("vertPos");
		cylinderProg->addAttribute("vertColor");

		
		// Initialize the Body program.
		shapeprog = std::make_shared<Program>();
		shapeprog->setVerbose(true);
		shapeprog->setShaderNames(resourceDirectory + "/shape_vertex.glsl", resourceDirectory + "/shape_fragment.glsl");
		if (!shapeprog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		shapeprog->addUniform("P");
		shapeprog->addUniform("V");
		shapeprog->addUniform("M");
		shapeprog->addAttribute("vertPos");
		shapeprog->addAttribute("vertNor");
		shapeprog->addAttribute("vertTex");
		

		// Initialize the Globe program.
		globeProg = std::make_shared<Program>();
		globeProg->setVerbose(true);
		globeProg->setShaderNames(resourceDirectory + "/globe_vertex.glsl", resourceDirectory + "/globe_fragment.glsl");
		if (!globeProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		globeProg->addUniform("P");
		globeProg->addUniform("V");
		globeProg->addUniform("M");
		globeProg->addAttribute("vertPos");
		globeProg->addAttribute("vertNor");
		globeProg->addAttribute("vertTex");


		// Initialize the Snow program.
		snowProg = std::make_shared<Program>();
		snowProg->setVerbose(true);
		snowProg->setShaderNames(resourceDirectory + "/snow_vertex.glsl", resourceDirectory + "/snow_fragment.glsl");
		if (!snowProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		snowProg->addUniform("P");
		snowProg->addUniform("V");
		snowProg->addUniform("M");
		snowProg->addAttribute("vertPos");
		snowProg->addAttribute("vertNor");
		snowProg->addAttribute("vertTex");


	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{

		double frametime = get_last_elapsed_time();
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width / (float)height;
		glViewport(0, 0, width, height);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Create the matrix stacks - please leave these alone for now

		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = glm::mat4(1);
		M = glm::mat4(1);
		// Apply orthographic projection....
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width / (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

		//animation with the model matrix:
		static int armAnimation = 0;
		static float rfa = -0.8;
		static float ra = -1.92;
		static float hatz = 0;
		static float hatx = 0;
		if (armAnimation == 0)
		{
			rfa -= 0.01;
			if (rfa < -2.8)
			{
				armAnimation = 1;
			}
		}
		
		if (armAnimation == 1) {
			ra += 0.01;
			hatz -= 0.0115;
			hatx += 0.003;
			if (ra > -1.0) {
				armAnimation = 2;
			}
		}

		if (armAnimation == 2) {
			ra -= 0.01;
			hatz += 0.0115;
			hatx -= 0.003;
			if (ra < -1.92) {
				armAnimation = 3;
			}
		}
		
		if (armAnimation == 3){
			rfa += 0.01;
			if (rfa > -0.8){
				armAnimation = 0;
			}
		}
		
		
		glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));

		// Draw Spheres
		shapeprog->bind();
		V = mycam.process(frametime);
		mat4 Shead = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.8f, 0.8f));
		mat4 Thead = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1, 0));
		M = RotateY * Thead*Shead;
		glUniformMatrix4fv(shapeprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(shapeprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(shapeprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(shapeprog);

		mat4 S2 = glm::scale(glm::mat4(1.0f), glm::vec3(0.7f, 0.5f, .5f));
		mat4 T2 = glm::translate(glm::mat4(1.0f), glm::vec3(0, 1, 0));
		mat4 To2 = glm::translate(glm::mat4(1.0f), glm::vec3(0, 1, 0));

		mat4 M1 = M * T2;
		M = M1 * S2;
		glUniformMatrix4fv(shapeprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(shapeprog);

		mat4 S3 = glm::scale(glm::mat4(1.0f), glm::vec3(0.4f, 0.4f, 0.4f));
		mat4 T3 = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.75f, 0));
		mat4 To3 = glm::translate(glm::mat4(1.0f), glm::vec3(0, 1, 0));

		mat4 M2 = M1 * T3;
		M = M2 * S3;
		glUniformMatrix4fv(shapeprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(shapeprog);

		shapeprog->unbind();

		// Draw Cylinders
		cylinderProg->bind();
		glUniformMatrix4fv(cylinderProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(cylinderProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(cylinderProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);

		// Right upper-arm
		mat4 CS = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.80f, 0.05f));
		mat4 CTo1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.1f, 0.2f));
		mat4 CTpz1 = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.4f, 0));
		mat4 CRx1 = glm::rotate(glm::mat4(1.0f), rfa, glm::vec3(1.0f, 0.0f, 0.0f));
		mat4 CRz1 = glm::rotate(glm::mat4(1.0f), 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));

		mat4 Mcylinder = CTo1 * CRx1 * CRz1 * CTpz1;
		M = RotateY * Mcylinder * CS;
		glUniformMatrix4fv(cylinderProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, 1440);
		
		// Right arm
		mat4 CTo2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.4f, 0.0f));
		mat4 CTpx2 = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.4f, 0));
		mat4 CRx2 = glm::rotate(glm::mat4(1.0f), ra, glm::vec3(0.1f, 0.0f, 0.0f));
		mat4 CRy2 = glm::rotate(glm::mat4(1.0f), -1.0f, glm::vec3(0.0f, 0.1f, 0.0f));

		mat4 Mcylinder2 = Mcylinder * CTo2 * CRy2 * CRx2 * CTpx2;
		M = RotateY * Mcylinder2 * CS;

		glUniformMatrix4fv(cylinderProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, 1440);

		// Left upper-arm
		mat4 CTo3 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.1f, 0.0f));
		mat4 CTpz3 = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.4f, 0));
		mat4 CRx3 = glm::rotate(glm::mat4(1.0f), -0.8f, glm::vec3(1.0f, 0.0f, 0.0f));
		mat4 CRz3 = glm::rotate(glm::mat4(1.0f), -0.5f, glm::vec3(0.0f, 0.0f, 1.0f));

		mat4 Mcylinder3 = CTo3 * CRx3 * CRz3 * CTpz3;
		M = RotateY * Mcylinder3 * CS;

		glUniformMatrix4fv(cylinderProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, 1440);

		// Left arm
		mat4 CTo4 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.4f, 0.0f));
		mat4 CTpx4 = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.4f, 0));
		mat4 CRx4 = glm::rotate(glm::mat4(1.0f), -2.0f, glm::vec3(0.1f, 0.0f, 0.0f));
		mat4 CRy4 = glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(0.0f, 0.1f, 0.0f));

		mat4 Mcylinder4 = Mcylinder3 * CTo4 * CRy4 * CRx4 * CTpx4;
		M = RotateY * Mcylinder4 * CS;
		glUniformMatrix4fv(cylinderProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, 1440);

		if (armAnimation == 1 || armAnimation == 2) {
			// Tophat Base
			mat4 CS5 = glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.1f, 0.3f));
			mat4 CTo5 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.7f, 0.0f));
			mat4 CTpz5 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0, 0.0f, 0));
			mat4 CRx5 = glm::rotate(glm::mat4(1.0f), hatx, glm::vec3(1.0f, 0.0f, 0.0f));
			mat4 CRz5 = glm::rotate(glm::mat4(1.0f), hatz, glm::vec3(0.0f, 0.0f, 1.0f));

			mat4 Mcylinder5 = CTo5 * CRx5 * CRz5 * CTpz5;
			M = RotateY * Mcylinder5 * CS5;
			glUniformMatrix4fv(cylinderProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glBindVertexArray(VertexArrayID);
			glDrawArrays(GL_TRIANGLES, 0, 1440);

			// Tophat Top
			mat4 CS6 = glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 0.5f, 0.15f));
			mat4 CTo6 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 0.0f));

			mat4 Mcylinder6 = Mcylinder5 * CTo6;
			M = RotateY * Mcylinder6 * CS6;
			glUniformMatrix4fv(cylinderProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glBindVertexArray(VertexArrayID);
			glDrawArrays(GL_TRIANGLES, 0, 1440);
		}
		else {
			// Tophat Base
			mat4 CS5 = glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.1f, 0.3f));
			mat4 CTo5 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, .7f, 0.0f));

			mat4 Mcylinder5 = CTo5 * CS5;
			M = RotateY * Mcylinder5;
			glUniformMatrix4fv(cylinderProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glBindVertexArray(VertexArrayID);
			glDrawArrays(GL_TRIANGLES, 0, 1440);

			// Tophat Top
			mat4 CS6 = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 5.0f, 0.5f));
			mat4 CTo6 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f));

			mat4 Mcylinder6 = Mcylinder5 * CTo6;
			M = RotateY * Mcylinder6 * CS6;
			glUniformMatrix4fv(cylinderProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glBindVertexArray(VertexArrayID);
			glDrawArrays(GL_TRIANGLES, 0, 1440);
		}

		// Right Eye
		mat4 CS7 = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
		mat4 CTo7 = glm::translate(glm::mat4(1.0f), glm::vec3(0.15f, 0.5f, 0.319f));
		mat4 CRx7 = glm::rotate(glm::mat4(1.0f), -2.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		mat4 Mcylinder7 = CTo7 * CRx7;
		M = RotateY * Mcylinder7 * CS7;
		glUniformMatrix4fv(cylinderProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, 1440);

		// Left Eye
		mat4 CTo8 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f, 0.0f, 0.0f));

		mat4 Mcylinder8 = Mcylinder7 * CTo8;
		M = RotateY * Mcylinder8 * CS7;
		glUniformMatrix4fv(cylinderProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, 1440);

		glBindVertexArray(0);
		cylinderProg->unbind();
		
		// Draw Snow
		snowProg->bind();
		float snowflakes[20] = {
			-0.34234634262857,
			-0.21326234623148,
			0.0654163461613128,
			0.087746586458542,
			-0.856867842355401,
			0.425765376356353,
			0.92473563743568,
			-0.83456435646437,
			0.646456547563432,
			0.146534745624353,
			-0.72154375636335,
			0.83574567822546,
			0.132575685811437,
			0.243654826456474,
			0.615834097539815,
			0.720043765907428,
			-0.32367563476422,
			0.3256245742632604,
			-0.176425634624192,
			-0.7631252415541512
		};

		float depth[20] = {
			0.41324532542364,
			1.30363426454559,
			-0.60732623530393,
			0.418643534253559,
			-2.3245236564984,
			-0.88457345774667,
			0.374357437443525,
			2.534574574353135,
			1.745853452346438,
			-2.3188324751639,
			2.55234234534401,
			-2.4541352345048,
			-1.112345324537,
			1.7223452345238,
			1.9502345345094,
			-2.254462623466,
			-0.2346234623469,
			-1.6857658568902,
			-1.9947193367382,
			1.74362347568856
		};

		for (int i = 0; i < 20; i++) {
			mat4 snowScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
			mat4 snowTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(1.6 *snowflakes[i], snowFall, depth[i]));
			M = snowTranslate * snowScale;
			glUniformMatrix4fv(snowProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(snowProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(snowProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			shape.draw(snowProg);

		}

		snowProg->unbind();
		
		// Draw Globe
		globeProg->bind();
		glUniformMatrix4fv(globeProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(globeProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(globeProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
	
		mat4 GS = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
		mat4 GT = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1, 0));
		M = GS;
		
		glUniformMatrix4fv(globeProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(globeProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(globeProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(globeProg);
		
		mat4 GS2 = glm::scale(glm::mat4(1.0f), glm::vec3(1.1f, 1.1f, 1.1f));
		M = M * GS2;
		glUniformMatrix4fv(globeProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(globeProg);
		globeProg->unbind();
	}
};
//******************************************************************************************
int main(int argc, char** argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application* application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager* windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
		// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(windowManager->getHandle()))
	{

		snowFall += incSnow(&snowFall);
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
