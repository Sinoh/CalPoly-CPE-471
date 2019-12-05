/*
ZJ Wood CPE 471 Lab 3 base code
*/

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Application : public EventCallbacks
{

public:
	 
	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;	   // sun prgram
	std::shared_ptr<Program> backProg; // backgroud Program

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;
	GLuint BackVertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;
	GLuint BackBufferID;


	

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

			//change this to be the points converted to WORLD
			//THIS IS BROKEN< YOU GET TO FIX IT - yay!
			newPt[0] = 0;
			newPt[1] = 0;

			std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
			//update the vertex array with the updated points
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{

		// This block of code will be the Vertex Array Object
		// for the middle part of the sun. It will contain
		// 80 triangles

		// NOTE: To be a littler more creative, I decided to make all
		// Verticies of the the sun change over time
		// One set of rays will be changing based on the cose of time
		// The other set will be changing based on the sine of time

		//generate the VAO for the sun
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		glm::vec3 sundata[3 * 80];
		float radius = 0.4;
		float alpha = 0;
		float delta_alpha = (2 * 3.1415926) / 80;
		for (int i = 0; i < 80; i++, alpha += delta_alpha)
		{
			int a = i * 3;		// Center of the Sun
			int b = i * 3 + 1;	// For Second Vertex
			int c = i * 3 + 2;  // For Third Vertex, delta_alpha
			sundata[a] = glm::vec3(0.0f, 0.0f, 0.0f);
			if (b % 2 == 0) {
				sundata[b] = glm::vec3(-sin(alpha) * radius, cos(alpha) * radius, 0.0f);
			}
			else {
				sundata[b] = glm::vec3(-sin(alpha) * radius, cos(alpha) * radius, 0.2f);
			}
			if (c % 2 == 0) {
				sundata[c] = glm::vec3(-sin(alpha + delta_alpha) * radius, cos(alpha + delta_alpha) * radius, 0.0f);
			}
			else {
				sundata[c] = glm::vec3(-sin(alpha + delta_alpha) * radius, cos(alpha + delta_alpha) * radius, 0.2f);
			}
		}

		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(sundata), sundata, GL_DYNAMIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);



		// This block of code will be the Vertex Array Object
		// for the background. It will contain 2 triangles
		// that span the entire window.

		//generate the background VAO
		glGenVertexArrays(1, &BackVertexArrayID);
		glBindVertexArray(BackVertexArrayID);
		glGenBuffers(1, &(BackBufferID));
		glBindBuffer(GL_ARRAY_BUFFER, BackBufferID);

		static const GLfloat g_back_buffer_data[] =
		{
			// Left
			-2.0f, -1.0f, 0.0f,
			-2.0f, 1.0f, 0.0f,
			2.0f, 1.0f, 0.0f,

			// Right
			2.0f, -1.0f, 0.0f,
			2.0f, 1.0f, 0.0f,
			-2.0f, -1.0f, 0.0f,

		};

		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_back_buffer_data), g_back_buffer_data, GL_DYNAMIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindVertexArray(0);
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the Background.
		backProg = std::make_shared<Program>();
		backProg->setVerbose(true);
		backProg->setShaderNames(resourceDirectory + "/back_shader_vertex.glsl", resourceDirectory + "/back_shader_fragment.glsl");
		backProg->init();
		backProg->addUniform("P");
		backProg->addUniform("V");
		backProg->addUniform("M");
		backProg->addUniform("center");		 // Pass center to shader
		backProg->addAttribute("vertPos");   // Pass vertex positions to shader

		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("center");		// Pass center to shader
		prog->addAttribute("vertPos");  // Pass vertex positions to shader
		prog->addUniform("time");		// Pass time to shader

	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, PP;
		V = glm::mat4(1);
		M = glm::mat4(1);
		// Apply orthographic projection.
		PP = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);		
		if (width < height)
		{
		PP = glm::ortho(-1.0f, 1.0f, -1.0f / aspect,  1.0f / aspect, -2.0f, 100.0f);
		}
	
		// Draw the triangle using GLSL.
		prog->bind();

		//send the matrices to the shaders
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &PP[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform2f(prog->getUniform("center"), width / 2.0, height / 2.0);
		glUniform1f(prog->getUniform("time"), glfwGetTime());

		
		glBindVertexArray(VertexArrayID);
		//actually draw from vertex 0, 3 vertices
		glDrawArrays(GL_TRIANGLES, 0, 240);
		glBindVertexArray(0);
		prog->unbind();


		glBindVertexArray(BackVertexArrayID);
		// Draw the triangle using GLSL.
		backProg->bind();

		//send the matrices to the shaders
		glUniformMatrix4fv(backProg->getUniform("P"), 1, GL_FALSE, &PP[0][0]);
		glUniformMatrix4fv(backProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(backProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform2f(backProg->getUniform("center"), width / 2.0, height / 2.0);

		glBindVertexArray(BackVertexArrayID);
		//actually draw from vertex 0, 3 vertices
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		backProg->unbind();

	}

};
//******************************************************************************************
int main()
{
	
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	/*
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}
	*/

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
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
