/* Release code for program 1 CPE 471 Fall 2016 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

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

// Given the vertices, calculate the area of a triangle
float area(int x1, int y1, int x2, int y2, int x3, int y3){
	return abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
}

bool isInTriangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3) {
	float A = area(x1, y1, x2, y2, x3, y3); // Area of trangle ABC
	float A1 = area(x, y, x2, y2, x3, y3); // Area of triangle PBC
	float A2 = area(x, y, x1, y1, x3, y3); // Area of triangle PAC
	float A3 = area(x, y, x1, y1, x2, y2); // Area of triangle PAB

	// Checks if the sum of A1, A2, A3 equals to A
	return (A == A1 + A2 + A3); 
}

//int main(int argc, char **argv)
int main()
{
	
	/*if(argc < 3) {
      cout << "Usage: Assignment1 meshfile imagefile" << endl;
      return 0;
   } */

	// OBJ filename
	string meshName("./resources/tri.OBJ");
	string imgName("output.PNG");

	//set g_width and g_height appropriately!
	g_width = g_height = 100;

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
	if(!rc) {
		cerr << errStr << endl;
	} else {
 		//keep this code to resize your object to be within -1 -> 1
   	resize_obj(shapes); 
		posBuf = shapes[0].mesh.positions;
		triBuf = shapes[0].mesh.indices;
	}
	
	cout << "Number of vertices: " << posBuf.size()/3 << endl;
	cout << "Number of triangles: " << triBuf.size()/3 << endl;

	//TODO add code to iterate through each triangle and rasterize it
	float Ax, Ay, Az, Bx, By, Bz, Cx, Cy, Cz; // Initialize ints to store vertices

	// Find and store the vertice information of the triangle
	for (size_t v = 0; v < shapes[0].mesh.positions.size() / 3; v++) {
		if (v == 0) {
			Ax = ((shapes[0].mesh.positions[3 * v + 0] + 1) * 100) / 2;
			Ay = ((shapes[0].mesh.positions[3 * v + 1] + 1) * 100) / 2;
			Az = ((shapes[0].mesh.positions[3 * v + 2] + 1) * 100) / 2;
		}
		else if (v == 1) {
			Bx = ((shapes[0].mesh.positions[3 * v + 0] + 1) * 100) / 2;
			By = ((shapes[0].mesh.positions[3 * v + 1] + 1) * 100) / 2;
			Bz = ((shapes[0].mesh.positions[3 * v + 2] + 1) * 100) / 2;
		}
		else {
			Cx = ((shapes[0].mesh.positions[3 * v + 0] + 1) * 100) / 2;
			Cy = ((shapes[0].mesh.positions[3 * v + 1] + 1) * 100) / 2;
			Cz = ((shapes[0].mesh.positions[3 * v + 2] + 1) * 100) / 2;
		}
		//image->setPixel(x, y, 255, 0, 0);
	}

	cout << Ax << endl;
	cout << Ay << endl;
	cout << Bx << endl;
	cout << By << endl;
	cout << Cx << endl;
	cout << Cy << endl;


	
	// Go through every pixel and make it red it it is in
	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			if (isInTriangle(i, j, Ax, Ay, Bx, By, Cx, Cy)) {
				image->setPixel(i, j, 255, 0, 0);
			}
		}
	}
	/*
	float Ta, Fa, Fb, Fc; // Init Var for Area of Triangle and the rgb value
	Ta = area(Ax, Ay, Bx, By, Cx, Cy); // Calcluate area of triangle
	
	// Go through every pixel
	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			if (isInTriangle(i, j, Ax, Ay, Bx, By, Cx, Cy)) {
				// Calucluate the area for each sub triangle
				Fa = (area(i, j, Bx, By, Cx, Cy) / Ta) * 255; // Ratio of red value
				Fb = (area(i, j, Ax, Ay, Bx, By) / Ta) * 255; // Ratio of green value
				Fc = (area(i, j, Ax, Ay, Cx, Cy) / Ta) * 255; // Ratio of blue value
				image->setPixel(i, j, Fa, Fb, Fc);
			}
		}
	}
	*/
	//write out the image
   image->writeToFile(imgName);

	return 0;
}
