/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
shared_ptr<Shape> shape;;
shared_ptr<Shape> house1;
shared_ptr<Shape> house2;
shared_ptr<Shape> lab;
shared_ptr<Shape> fence;
shared_ptr<Shape> tree;
shared_ptr<Shape> land;
shared_ptr<Shape> mailbox;
shared_ptr<Shape> water;







double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
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
		pos = rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 10*ftime;
		}
		else if (s == 1)
		{
			speed = -10*ftime;
		}
		float yangle=0;
		if (a == 1)
			yangle = -3*ftime;
		else if(d==1)
			yangle = 3*ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;

static int tree_amount = 1000;
float radius, randX, randY, randZ;
class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog,psky;

	// Our shader program
	std::shared_ptr<Program> houseProg;
	std::shared_ptr<Program> labProg;
	std::shared_ptr<Program> treeProg;
	std::shared_ptr<Program> fenceProg;
	std::shared_ptr<Program> mailboxProg;
	std::shared_ptr<Program> waterProg;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexNormDBox, VertexTexBox, IndexBufferIDBox, InstanceBuffer;
	GLuint VertexArrayID2, MeshPosID, MeshTexID, IndexBufferIDBox2;
	GLuint houseVertexArrayID, houseIndexBufferID;
	GLuint treeVertexArrayID, treePosBufferID, treeNormBufferID, treeTexBufferID;
	GLuint waterVertexArrayID, waterVertexBufferID, waterNormBufferID, waterTexBufferID, waterIndexBufferID;

	//texture data
	GLuint Texture;
	GLuint Texture2;
	GLuint mailTexture;


	int tree_vbo_size, fence_vbo_size;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
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

#define MESHSIZE 100
	void init_mesh()
	{
		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &MeshPosID);
		glBindBuffer(GL_ARRAY_BUFFER, MeshPosID);
		vec3 vertices[MESHSIZE * MESHSIZE * 4];
		for (int x = 0; x < MESHSIZE; x++)
			for (int z = 0; z < MESHSIZE; z++)
			{
				vertices[x * 4 + z * MESHSIZE * 4 + 0] = vec3(0.0, 0.0, 0.0) + vec3(x, 0, z);
				vertices[x * 4 + z * MESHSIZE * 4 + 1] = vec3(1.0, 0.0, 0.0) + vec3(x, 0, z);
				vertices[x * 4 + z * MESHSIZE * 4 + 2] = vec3(1.0, 0.0, 1.0) + vec3(x, 0, z);
				vertices[x * 4 + z * MESHSIZE * 4 + 3] = vec3(0.0, 0.0, 1.0) + vec3(x, 0, z);
			}
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * MESHSIZE * MESHSIZE * 4, vertices, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//tex coords
		float t = 1. / 100;
		vec2 tex[MESHSIZE * MESHSIZE * 4];
		for (int x = 0; x < MESHSIZE; x++)
			for (int y = 0; y < MESHSIZE; y++)
			{
				tex[x * 4 + y * MESHSIZE * 4 + 0] = vec2(0.0, 0.0) + vec2(x, y) * t;
				tex[x * 4 + y * MESHSIZE * 4 + 1] = vec2(t, 0.0) + vec2(x, y) * t;
				tex[x * 4 + y * MESHSIZE * 4 + 2] = vec2(t, t) + vec2(x, y) * t;
				tex[x * 4 + y * MESHSIZE * 4 + 3] = vec2(0.0, t) + vec2(x, y) * t;
			}
		glGenBuffers(1, &MeshTexID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, MeshTexID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * MESHSIZE * MESHSIZE * 4, tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &IndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		GLushort elements[MESHSIZE * MESHSIZE * 6];
		int ind = 0;
		for (int i = 0; i < MESHSIZE * MESHSIZE * 6; i += 6, ind += 4)
		{
			elements[i + 0] = ind + 0;
			elements[i + 1] = ind + 1;
			elements[i + 2] = ind + 2;
			elements[i + 3] = ind + 0;
			elements[i + 4] = ind + 2;
			elements[i + 5] = ind + 3;
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * MESHSIZE * MESHSIZE * 6, elements, GL_STATIC_DRAW);
		glBindVertexArray(0);
	}
	

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{

		init_mesh();

		string resourceDirectory = "../resources";
		// Initialize mesh.
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/sphere.obj");
		shape->resize();
		shape->init();

		land = make_shared<Shape>();
		land->loadMesh(resourceDirectory + "/sphere.obj");
		land->resize();
		land->init();

		house1 = make_shared<Shape>();
		house1->loadMesh(resourceDirectory + "/house1/houseobj.obj");
		house1->resize();
		house1->init();

		lab = make_shared<Shape>();
		lab->loadMesh(resourceDirectory + "/lab/lab.obj");
		lab->resize();
		lab->init();

		tree = make_shared<Shape>();
		tree->loadMesh(resourceDirectory + "/pine_realistic.obj");
		tree->resize();
		tree->init();

		mailbox = make_shared<Shape>();
		mailbox->loadMesh(resourceDirectory + "/mailbox/mailbox.obj");
		mailbox->resize();
		mailbox->init();

		fence = make_shared<Shape>();
		fence->loadMesh(resourceDirectory + "/fence/fence.obj");
		fence->resize();
		fence->init();


		//generate the VAO
		glGenVertexArrays(1, &waterVertexArrayID);
		glBindVertexArray(waterVertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &waterVertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, waterVertexBufferID);

		GLfloat cube_vertices[] = {
			// front
			-1.0, -1.0,  1.0,//LD
			1.0, -1.0,  1.0,//RD
			1.0,  1.0,  1.0,//RU
			-1.0,  1.0,  1.0,//LU
		};
		//make it a bit smaller
		for (int i = 0; i < 12; i++)
			cube_vertices[i] *= 0.5;
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//color
		GLfloat cube_norm[] = {
			// front colors
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,

		};
		glGenBuffers(1, &waterNormBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, waterNormBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_norm), cube_norm, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//color
		glm::vec2 cube_tex[] = {
			// front colors
			glm::vec2(0.0, 1.0),
			glm::vec2(1.0, 1.0),
			glm::vec2(1.0, 0.0),
			glm::vec2(0.0, 0.0),

		};
		glGenBuffers(1, &waterTexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, waterTexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_tex), cube_tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &waterIndexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterIndexBufferID);
		GLushort cube_elements[] = {

			// front
			0, 1, 2,
			2, 3, 0,
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);


		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Instancing for Trees

		tree_vbo_size = tree->eleBuf[0].size();
		//Generate VAO for tree OBJ
		////////////////////////////////
		glGenVertexArrays(1, &treeVertexArrayID);
		glBindVertexArray(treeVertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &treePosBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, treePosBufferID);

		vec3* positionBuff = new vec3[tree_vbo_size];
		for (int t = 0; t < tree->eleBuf[0].size(); t += 3) {
			int index_A = tree->eleBuf[0][t];
			int index_B = tree->eleBuf[0][t + 1];
			int index_C = tree->eleBuf[0][t + 2];
			float ax = tree->posBuf[0][index_A * 3 + 0];
			float ay = tree->posBuf[0][index_A * 3 + 1];
			float az = tree->posBuf[0][index_A * 3 + 2];
			float bx = tree->posBuf[0][index_B * 3 + 0];
			float by = tree->posBuf[0][index_B * 3 + 1];
			float bz = tree->posBuf[0][index_B * 3 + 2];
			float cx = tree->posBuf[0][index_C * 3 + 0];
			float cy = tree->posBuf[0][index_C * 3 + 1];
			float cz = tree->posBuf[0][index_C * 3 + 2];

			positionBuff[t] = vec3(ax, ay, az);
			positionBuff[t + 1] = vec3(bx, by, bz);
			positionBuff[t + 2] = vec3(cx, cy, cz);
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tree_vbo_size, positionBuff, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


		//now calculate flat normals
		//generate normal buffer to hand off to OGL
		glGenBuffers(1, &treeNormBufferID);
		//set the current state to focus on our normal buffer
		glBindBuffer(GL_ARRAY_BUFFER, treeNormBufferID);

		vec3* normalBuff = new vec3[tree_vbo_size];
		for (int i = 0; i < tree_vbo_size; i += 3) {
			vec3 a = positionBuff[i + 1] - positionBuff[i];
			vec3 b = positionBuff[i + 2] - positionBuff[i];
			vec3 normal = cross(a, b);
			normalBuff[i] = normal;
			normalBuff[i + 1] = normal;
			normalBuff[i + 2] = normal;
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tree_vbo_size, normalBuff, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &treeTexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, treeTexBufferID);

		vec2* textureBuff = new vec2[tree_vbo_size];
		for (int t = 0; t < tree->eleBuf[0].size(); t += 3) {
			int index_A = tree->eleBuf[0][t];
			int index_B = tree->eleBuf[0][t + 1];
			int index_C = tree->eleBuf[0][t + 2];
			float ax = tree->texBuf[0][index_A * 2 + 0];
			float ay = tree->texBuf[0][index_A * 2 + 1];
			float bx = tree->texBuf[0][index_B * 2 + 0];
			float by = tree->texBuf[0][index_B * 2 + 1];
			float cx = tree->texBuf[0][index_C * 2 + 0];
			float cy = tree->texBuf[0][index_C * 2 + 1];

			textureBuff[t] = vec2(ax, ay);
			textureBuff[t + 1] = vec2(bx, by);
			textureBuff[t + 1] = vec2(cx, cy);

		}
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * tree_vbo_size, textureBuff, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


		//generate vertex buffer to hand off to OGL ###########################
		glGenBuffers(1, &InstanceBuffer);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, InstanceBuffer);



		glm::vec4* positions = new glm::vec4[tree_amount];
		int MinX = 40;
		int MaxX = 65;
		int MinZ = -60;
		int MaxZ = 50;
		for (int i = 0; i < tree_amount; i += 2) {
			randZ = (rand()) % (MaxZ - MinZ + 1) + MinZ;
			if (randZ > 30 || randZ < -30){
				randX = (rand()) % (MaxX - 20 + 1) + 20;
			}
			else {
				randX = (rand()) % (MaxZ - MinX + 1) + MinX;
			}
			

			positions[i] = glm::vec4(randX, 1, randZ, 0);
			positions[i + 1] = glm::vec4(-randX, 1, randZ, 0);

		}
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, tree_amount * sizeof(glm::vec4), positions, GL_STATIC_DRAW);
		int position_loc = glGetAttribLocation(treeProg->pid, "InstancePos");
		for (int i = 0; i < tree_amount; i++)
		{
			// Set up the vertex attribute
			glVertexAttribPointer(position_loc + i,              // Location
				4, GL_FLOAT, GL_FALSE,       // vec4
				sizeof(vec4),                // Stride
				(void*)(sizeof(vec4) * i)); // Start offset
											 // Enable it
			glEnableVertexAttribArray(position_loc + i);
			// Make it instanced
			glVertexAttribDivisor(position_loc + i, 1);
		}

		glBindVertexArray(0);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Instancing for Fences

		/*
		fence_vbo_size = fence->eleBuf[0].size();
		//Generate VAO for tree OBJ

		glGenVertexArrays(1, &fenceVertexArrayID);
		glBindVertexArray(fenceVertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &fencePosBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, fencePosBufferID);

		vec3* positionBuff = new vec3[fence_vbo_size];
		for (int t = 0; t < fence->eleBuf[0].size(); t += 3) {
			int index_A = fence->eleBuf[0][t];
			int index_B = fence->eleBuf[0][t + 1];
			int index_C = fence->eleBuf[0][t + 2];
			float ax = fence->posBuf[0][index_A * 3 + 0];
			float ay = fence->posBuf[0][index_A * 3 + 1];
			float az = fence->posBuf[0][index_A * 3 + 2];
			float bx = fence->posBuf[0][index_B * 3 + 0];
			float by = fence->posBuf[0][index_B * 3 + 1];
			float bz = fence->posBuf[0][index_B * 3 + 2];
			float cx = fence->posBuf[0][index_C * 3 + 0];
			float cy = fence->posBuf[0][index_C * 3 + 1];
			float cz = fence->posBuf[0][index_C * 3 + 2];

			positionBuff[t] = vec3(ax, ay, az);
			positionBuff[t + 1] = vec3(bx, by, bz);
			positionBuff[t + 2] = vec3(cx, cy, cz);
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * fence_vbo_size, positionBuff, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


		//now calculate flat normals
		//generate normal buffer to hand off to OGL
		glGenBuffers(1, &fenceNormBufferID);
		//set the current state to focus on our normal buffer
		glBindBuffer(GL_ARRAY_BUFFER, fenceNormBufferID);

		vec3* normalBuff = new vec3[fence_vbo_size];
		for (int i = 0; i < fence_vbo_size; i += 3) {
			vec3 a = positionBuff[i + 1] - positionBuff[i];
			vec3 b = positionBuff[i + 2] - positionBuff[i];
			vec3 normal = cross(a, b);
			normalBuff[i] = normal;
			normalBuff[i + 1] = normal;
			normalBuff[i + 2] = normal;
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * fence_vbo_size, normalBuff, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &fenceTexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, fenceTexBufferID);

		vec2* textureBuff = new vec2[fence_vbo_size];
		for (int t = 0; t < fence->eleBuf[0].size(); t += 3) {
			int index_A = fence->eleBuf[0][t];
			int index_B = fence->eleBuf[0][t + 1];
			int index_C = fence->eleBuf[0][t + 2];
			float ax = fence->texBuf[0][index_A * 2 + 0];
			float ay = fence->texBuf[0][index_A * 2 + 1];
			float bx = fence->texBuf[0][index_B * 2 + 0];
			float by = fence->texBuf[0][index_B * 2 + 1];
			float cx = fence->texBuf[0][index_C * 2 + 0];
			float cy = fence->texBuf[0][index_C * 2 + 1];

			textureBuff[t] = vec2(ax, ay);
			textureBuff[t + 1] = vec2(bx, by);
			textureBuff[t + 1] = vec2(cx, cy);

		}
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * fence_vbo_size, textureBuff, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


		//generate vertex buffer to hand off to OGL ###########################
		glGenBuffers(1, &InstanceBuffer);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, InstanceBuffer);



		glm::vec4* positions = new glm::vec4[tree_amount];
		int Min = -60;
		int Max = 60;
		for (int i = 0; i < tree_amount; i += 2) {

			randX = (rand()) % (40 - 20 + 1) + 20;
			randZ = (rand()) % (Max - Min + 1) + Min;
			positions[i] = glm::vec4(randX, 0, randZ, 0);
			positions[i + 1] = glm::vec4(-randX, 0, randZ, 0);

		}
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, tree_amount * sizeof(glm::vec4), positions, GL_STATIC_DRAW);
		int position_loc = glGetAttribLocation(prog->pid, "InstancePos");
		for (int i = 0; i < tree_amount; i++)
		{
			// Set up the vertex attribute
			glVertexAttribPointer(position_loc + i,              // Location
				4, GL_FLOAT, GL_FALSE,       // vec4
				sizeof(vec4),                // Stride
				(void*)(sizeof(vec4) * i)); // Start offset
											 // Enable it
			glEnableVertexAttribArray(position_loc + i);
			// Make it instanced
			glVertexAttribDivisor(position_loc + i, 1);
		}
		*/
		

		int width, height, channels;
		char filepath[1000];

		//texture 1
		string str = resourceDirectory + "/grass.jpg";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//texture 2
		str = resourceDirectory + "/sky.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);


		//texture 3
		str = resourceDirectory + "/mailbox/simple_steel3.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &mailTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mailTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		GLuint Tex1Location = glGetUniformLocation(prog->pid, "tex");//tex, tex2... sampler in the fragment shader
		GLuint Tex2Location = glGetUniformLocation(prog->pid, "tex2");
		// Then bind the uniform samplers to texture units:
		glUseProgram(prog->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);
		
		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		GLuint Tex3Location = glGetUniformLocation(mailboxProg->pid, "tex");//tex, tex2... sampler in the fragment shader
		// Then bind the uniform samplers to texture units:
		glUseProgram(mailboxProg->pid);
		glUniform1i(Tex3Location, 0);


	}
	
	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);

		// Initialize the Sphere1 program.
		houseProg = std::make_shared<Program>();
		houseProg->setVerbose(true);
		houseProg->setShaderNames(resourceDirectory + "/house_vertex.glsl", resourceDirectory + "/house_fragment.glsl");
		houseProg->init();
		houseProg->addUniform("P");
		houseProg->addUniform("V");
		houseProg->addUniform("M");
		houseProg->addUniform("campos");
		houseProg->addAttribute("vertPos");
		houseProg->addAttribute("vertNor");

		// Initialize the Sphere2 program.
		labProg = std::make_shared<Program>();
		labProg->setVerbose(true);
		labProg->setShaderNames(resourceDirectory + "/house_vertex.glsl", resourceDirectory + "/house_fragment.glsl");
		labProg->init();
		labProg->addUniform("P");
		labProg->addUniform("V");
		labProg->addUniform("M");
		labProg->addUniform("campos");
		labProg->addAttribute("vertPos");
		labProg->addAttribute("vertNor");

		// Initialize the GLSL program.
		treeProg = std::make_shared<Program>();
		treeProg->setVerbose(true);
		treeProg->setShaderNames(resourceDirectory + "/tree_vertex.glsl", resourceDirectory + "/tree_fragment.glsl");
		if (!treeProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		treeProg->addUniform("P");
		treeProg->addUniform("V");
		treeProg->addUniform("M");
		treeProg->addUniform("campos");
		treeProg->addAttribute("vertPos");
		treeProg->addAttribute("vertNor");
		treeProg->addAttribute("vertTex");
		treeProg->addAttribute("InstancePos");

		// Initialize the Sphere2 program.
		fenceProg = std::make_shared<Program>();
		fenceProg->setVerbose(true);
		fenceProg->setShaderNames(resourceDirectory + "/fence_vertex.glsl", resourceDirectory + "/fence_fragment.glsl");
		fenceProg->init();
		fenceProg->addUniform("P");
		fenceProg->addUniform("V");
		fenceProg->addUniform("M");
		fenceProg->addUniform("campos");
		fenceProg->addAttribute("vertPos");
		fenceProg->addAttribute("vertNor");
		fenceProg->addAttribute("InstancePos");

		// Initialize the Sphere2 program.
		mailboxProg = std::make_shared<Program>();
		mailboxProg->setVerbose(true);
		mailboxProg->setShaderNames(resourceDirectory + "/mailbox_vertex.glsl", resourceDirectory + "/mailbox_fragment.glsl");
		mailboxProg->init();
		mailboxProg->addUniform("P");
		mailboxProg->addUniform("V");
		mailboxProg->addUniform("M");
		mailboxProg->addUniform("campos");
		mailboxProg->addAttribute("vertPos");
		mailboxProg->addAttribute("vertNor");

		psky = std::make_shared<Program>();
		psky->setVerbose(true);
		psky->setShaderNames(resourceDirectory + "/skyvertex.glsl", resourceDirectory + "/skyfrag.glsl");
		if (!psky->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		psky->addUniform("P");
		psky->addUniform("V");
		psky->addUniform("M");
		psky->addUniform("campos");
		psky->addAttribute("vertPos");
		psky->addAttribute("vertNor");
		psky->addAttribute("vertTex");

		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/height_vertex.glsl", resourceDirectory + "/height_fragment.glsl");
		if (!prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("campos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");

		waterProg = std::make_shared<Program>();
		waterProg->setVerbose(true);
		waterProg->setShaderNames(resourceDirectory + "/water_vertex.glsl", resourceDirectory + "/water_fragment.glsl");
		if (!waterProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		waterProg->addUniform("P");
		waterProg->addUniform("V");
		waterProg->addUniform("M");
		waterProg->addUniform("campos");
		waterProg->addAttribute("vertPos");
		waterProg->addAttribute("vertNor");
		waterProg->addAttribute("vertTex");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		double frametime = get_last_elapsed_time();

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = mycam.process(frametime);
		M = glm::mat4(1);
		// Apply orthographic projection....
		P = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);		
		if (width < height)
			{
			P = glm::ortho(-1.0f, 1.0f, -1.0f / aspect,  1.0f / aspect, -2.0f, 100.0f);
			}
		// ...but we overwrite it (optional) with a perspective projection.
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		float sangle = 3.1415926 / 2.;
		glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 camp = -mycam.pos;
		glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
		glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));

		M = TransSky * RotateXSky * SSky;

		psky->bind();

		//send the matrices to the shaders
		glUniformMatrix4fv(psky->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(psky->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(psky->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(psky->getUniform("campos"), 1, &mycam.pos[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glDisable(GL_DEPTH_TEST);
		shape->draw(psky, FALSE);
		glEnable(GL_DEPTH_TEST);
	
		psky->unbind();
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		prog->bind();

		glm::mat4 TransY = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, -1.0f, -50));
		M = TransY;
		//send the matrices to the shaders
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("campos"), 1, &mycam.pos[0]);
		glBindVertexArray(VertexArrayID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glDrawElements(GL_TRIANGLES, MESHSIZE * MESHSIZE * 6, GL_UNSIGNED_SHORT, (void*)0);
		prog->unbind();
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		glm::mat4 Translate = glm::translate(glm::mat4(1.0f), glm::vec3(-12.0f, 1.0f, -12.0f));
		glm::mat4 Scale = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));


		M = Translate;
		M = M * Scale;

		houseProg->bind();
		//send the matrices to the shaders
		glUniformMatrix4fv(houseProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(houseProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(houseProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(houseProg->getUniform("campos"), 1, &mycam.pos[0]);
		house1->draw(houseProg, FALSE);

		Translate = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, 0.0f));
		M = M * Translate;
		glUniformMatrix4fv(houseProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		house1->draw(houseProg, FALSE);
		

		houseProg->unbind();
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Translate = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 1.2f, 11.0f));
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(6.0f, 6.0f, 6.0f));
		//M = Translate;
		M = Translate * Scale;
		
		labProg->bind();
		//send the matrices to the shaders
		glUniformMatrix4fv(labProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(labProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(labProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(labProg->getUniform("campos"), 1, &mycam.pos[0]);
		lab->draw(labProg, FALSE);
		labProg->unbind();
		
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		

		static float rot2 = 1.55f;
		Translate = glm::translate(glm::mat4(1.0f), glm::vec3(-11.0f, -0.4f, -9.5f));
		glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), -rot2, glm::vec3(1.0f, 0.0f, 0.0f));
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

		M = Translate * RotateX;
		M = M * Scale;

		mailboxProg->bind();

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mailTexture);
		//send the matrices to the shaders
		glUniformMatrix4fv(mailboxProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(mailboxProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(mailboxProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(mailboxProg->getUniform("campos"), 1, &mycam.pos[0]);
		mailbox->draw(mailboxProg, FALSE);

		Translate = glm::translate(glm::mat4(1.0f), glm::vec3(37.0f, 0.0f, 0.0f));
		M = M * Translate;
		glUniformMatrix4fv(mailboxProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		mailbox->draw(mailboxProg, FALSE);

		mailboxProg->unbind();

		// Draw the box using GLSL.
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, -3));
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));

		M = TransZ * S;

		// Draw the box using GLSL.
		treeProg->bind();
		glBindVertexArray(treeVertexArrayID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);

		//send the matrices to the shaders
		glUniformMatrix4fv(treeProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(treeProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(treeProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(treeProg->getUniform("campos"), 1, &mycam.pos[0]);
		glDrawArraysInstanced(GL_TRIANGLES, 0, tree_vbo_size, tree_amount);
		glBindVertexArray(0);
		treeProg->unbind();


		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		/*
		fenceProg->bind();
		glBindVertexArray(treeVertexArrayID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);

		//send the matrices to the shaders
		glUniformMatrix4fv(fenceProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(fenceProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(fenceProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(fenceProg->getUniform("campos"), 1, &mycam.pos[0]);
		glDrawArraysInstanced(GL_TRIANGLES, 0, fence_vbo_size, fence_amount);
		glBindVertexArray(0);
		fenceProg->unbind();
		*/

		static float rot3 = 1.62f;
		RotateX = glm::rotate(glm::mat4(1.0f), rot3, glm::vec3(1.0f, 0.0f, 0.0f));
		Translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 10.0f));

		waterProg->bind();

		glBindVertexArray(waterVertexArrayID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterIndexBufferID);

		mat4 Vi = glm::transpose(V);
		Vi[0][3] = 0;
		Vi[1][3] = 0;
		Vi[2][3] = 0;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);

		M = RotateX;
	
		//send the matrices to the shaders
		glUniformMatrix4fv(waterProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(waterProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(waterProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(waterProg->getUniform("campos"), 1, &mycam.pos[0]);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
		waterProg->unbind();
	}

};

int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
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
