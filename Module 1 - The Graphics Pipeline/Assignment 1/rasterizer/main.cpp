/* Release code for program 1 CPE 471 Fall 2016 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Image.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;

int g_width, g_height;

/*
   Helper function you will want all quarter
   Given a vector of shapes which has already been read from an obj file
   resize all vertices to the range [-1, 1]
 */
void resize_obj(std::vector<tinyobj::shape_t> &shapes){
   float minX, minY, minZ;
   float maxX, maxY, maxZ;
   float scaleX, scaleY, scaleZ;
   float shiftX, shiftY, shiftZ;
   float epsilon = 0.001;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   //Go through all vertices to determine min and max of each dimension
   for (size_t i = 0; i < shapes.size(); i++) {
      for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
         if(shapes[i].mesh.positions[3*v+0] < minX) minX = shapes[i].mesh.positions[3*v+0];
         if(shapes[i].mesh.positions[3*v+0] > maxX) maxX = shapes[i].mesh.positions[3*v+0];

         if(shapes[i].mesh.positions[3*v+1] < minY) minY = shapes[i].mesh.positions[3*v+1];
         if(shapes[i].mesh.positions[3*v+1] > maxY) maxY = shapes[i].mesh.positions[3*v+1];

         if(shapes[i].mesh.positions[3*v+2] < minZ) minZ = shapes[i].mesh.positions[3*v+2];
         if(shapes[i].mesh.positions[3*v+2] > maxZ) maxZ = shapes[i].mesh.positions[3*v+2];
      }
   }

	//From min and max compute necessary scale and shift for each dimension
   float maxExtent, xExtent, yExtent, zExtent;
   xExtent = maxX-minX;
   yExtent = maxY-minY;
   zExtent = maxZ-minZ;
   if (xExtent >= yExtent && xExtent >= zExtent) {
      maxExtent = xExtent;
   }
   if (yExtent >= xExtent && yExtent >= zExtent) {
      maxExtent = yExtent;
   }
   if (zExtent >= xExtent && zExtent >= yExtent) {
      maxExtent = zExtent;
   }
   scaleX = 2.0 /maxExtent;
   shiftX = minX + (xExtent/ 2.0);
   scaleY = 2.0 / maxExtent;
   shiftY = minY + (yExtent / 2.0);
   scaleZ = 2.0/ maxExtent;
   shiftZ = minZ + (zExtent)/2.0;

   //Go through all verticies shift and scale them
   for (size_t i = 0; i < shapes.size(); i++) {
      for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
         shapes[i].mesh.positions[3*v+0] = (shapes[i].mesh.positions[3*v+0] - shiftX) * scaleX;
         assert(shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
         shapes[i].mesh.positions[3*v+1] = (shapes[i].mesh.positions[3*v+1] - shiftY) * scaleY;
         assert(shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
         shapes[i].mesh.positions[3*v+2] = (shapes[i].mesh.positions[3*v+2] - shiftZ) * scaleZ;
         assert(shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
      }
   }
}

struct Triangle {
	float Ax;
	float Ay;
	float Bx;
	float By;
	float Cx;
	float Cy;
};


// Given the vertices, calculate the area of a triangle
float area(int x1, int y1, int x2, int y2, int x3, int y3) {
	return abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
}

// Check if point is within a triangle
bool isInTriangle(float x, float y, Triangle tri) {
	float areaT = area(tri.Ax, tri.Ay, tri.Bx, tri.By, tri.Cx, tri.Cy); // Area of trangle ABC
	float areaA = area(x, y, tri.Bx, tri.By, tri.Cx, tri.Cy); // Area of triangle PBC
	float areaB = area(x, y, tri.Ax, tri.Ay, tri.Cx, tri.Cy); // Area of triangle PAC
	float areaC = area(x, y, tri.Ax, tri.Ay, tri.Bx, tri.By); // Area of triangle PAB

	// Checks if the sum of A1, A2, A3 equals to A
	return (areaT == areaA + areaB + areaC);
}




void createImage(string input, string output, int image_width, int image_height, int mode)
{
	// OBJ filename
	string meshName(input);
	string imgName(output);

	//set g_width and g_height appropriately!
	g_width = image_width;
	g_height = image_height;
	float scale = fmin(g_width, g_height);

	//create an image
	auto image = make_shared<Image>(g_width, g_height);

	// triangle buffer
	vector<unsigned int> triBuf;
	// position buffer
	vector<float> posBuf;
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::shape_t> shapes; // geometry
	vector<tinyobj::material_t> objMaterials; // material
	string errStr;

	bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
	/* error checking on read */
	if (!rc) {
		cerr << errStr << endl;
	}
	else {
		//keep this code to resize your object to be within -1 -> 1
		resize_obj(shapes);
		posBuf = shapes[0].mesh.positions;
		triBuf = shapes[0].mesh.indices;
	}
	cout << "Number of vertices: " << posBuf.size() / 3 << endl;
	cout << "Number of triangles: " << triBuf.size() / 3 << endl;

	// Create  initialize and Initialize
	vector<vector<float> > zBuf;
	for (int i = 0; i < g_width; i++) {
		vector<float> col(g_height, 0);
		zBuf.push_back(col);
	}

	//TODO add code to iterate through each triangle and rasterize it
	/* Iterate through every shape*/
	for (size_t i = 0; i < shapes.size(); i++) 
	{
		/* Iterate through every index in the index buffer for individual triangles*/
		for (size_t v = 0; v < shapes[i].mesh.indices.size() / 3; v++) 
		{
			/* Get the Positions of the three vertex of a triange*/
			float Apos = shapes[i].mesh.indices[3 * v + 0];
			float Bpos = shapes[i].mesh.indices[3 * v + 1];
			float Cpos = shapes[i].mesh.indices[3 * v + 2];

			/* Get the x, y, z values of each vertex*/
			float Ax = ((shapes[i].mesh.positions[3 * Apos + 0] + 1) * scale) / 2;
			float Ay = ((shapes[i].mesh.positions[3 * Apos + 1] + 1) * scale) / 2;
			float Az = ((shapes[i].mesh.positions[3 * Apos + 2]));
			float Bx = ((shapes[i].mesh.positions[3 * Bpos + 0] + 1) * scale) / 2;
			float By = ((shapes[i].mesh.positions[3 * Bpos + 1] + 1) * scale) / 2;
			float Bz = ((shapes[i].mesh.positions[3 * Bpos + 2]));
			float Cx = ((shapes[i].mesh.positions[3 * Cpos + 0] + 1) * scale) / 2;
			float Cy = ((shapes[i].mesh.positions[3 * Cpos + 1] + 1) * scale) / 2;
			float Cz = ((shapes[i].mesh.positions[3 * Cpos + 2]));

			Triangle triangle = { Ax, Ay, Bx, By, Cx, Cy };
			// Find min and max to find the boudning box of the triangle
			float maxX = fmax(Ax, fmax(Bx, Cx));
			float maxY = fmax(Ay, fmax(By, Cy));
			float minX = fmin(Ax, fmin(Bx, Cx));
			float minY = fmin(Ay, fmin(By, Cy));


			// Fill in the bounding box
			for (int x = minX; x < maxX; x++)
			{
				for (int y = minY; y < maxY; y++)
				{
					float Ta = area(Ax, Ay, Bx, By, Cx, Cy); // Calcluate area of triangle
					float Fa = (area(x, y, Bx, By, Cx, Cy) / Ta); // Ratio of red value
					float Fb = (area(x, y, Ax, Ay, Bx, By) / Ta); // Ratio of green value
					float Fc = (area(x, y, Ax, Ay, Cx, Cy) / Ta); // Ratio of blue value
					double z = Fa * Cz + Fb * Az + Fc * Bz + 1;
					
					if (isInTriangle(x, y, triangle))
					{
						if (z > zBuf[x][y])
						{
							zBuf[x][y] = z;
						}

						float r = 0;
						float g = 0;
						float b = 0;

						if (mode == 1) 
						{
							r = 255 / 2 * zBuf[x][y];
						}
						else
						{
							r = 19 * y / g_height + 128 - (128 * y / g_height);
							g = 84 * y / g_height + 208 - (208 * y / g_height);//255 * y/g_height;
							b = 122 * y / g_height + 199 - (199 * y / g_height);//255 - (255*y/g_height);
						}
						
						image->setPixel(x, y, r, g, b);
					}
				}
			}

		}
	}
	//write out the image
	image->writeToFile(imgName);
}


int main(int argc, char **argv)
{
	createImage("./resources/teapot.OBJ", "output_teapot.PNG", 512, 512, 0);
	createImage("./resources/bunny.OBJ", "output_bunny.PNG", 512, 512, 1);
	return 0;
}