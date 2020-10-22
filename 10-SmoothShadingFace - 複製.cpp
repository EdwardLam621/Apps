// Assignment 6 (Exercise 15.5) - Texture-mapped mesh 
// Eric Nunn & Yvonne Rogell
// CPSC 5700, Seattle University, Fall Quarter 2019

// Include statements
#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "VecMat.h"
#include "Widgets.h"
#include <float.h>
#include <Misc.h>

using namespace std;

// GPU identifiers
GLuint vBuffer = 0;
GLuint program = 0;

int screenWidth = 900, screenHeight = 900;
Camera camera(screenWidth, screenHeight, vec3(0, 0, 0), vec3(0, 0, -20), 30);

// interaction
vec3 light(1.1f, 1.7f, -1.2f);
Mover lightMover;
void* picked = NULL;

float fieldOfView = 30, cubeSize = 0.05f, cubeStretch = cubeSize;

// Identify points on face
vec3 points[] = {
	vec3(761, -268, 1225),  vec3(392, -429, 1167),  vec3(486, -629, 1281),  vec3(292, -726, 1084),  vec3(761, -669, 1344),
	vec3(761, -735, 1359),  vec3(630, -710, 1331),  vec3(397, -727, 1233),  vec3(324, -812, 1142),  vec3(303, -846, 1060),
	vec3(305, -934, 1070),  vec3(347, -918, 1124),  vec3(503, -823, 1254),  vec3(761, -885, 1312),  vec3(761, -951, 1335),
	vec3(601, -1058, 1250), vec3(394, -1044, 1210), vec3(761, -1197, 1433), vec3(242, -1002, 918),  vec3(315, -1179, 1100),
	vec3(761, -1364, 1316), vec3(400, -1190, 1185), vec3(534, -1296, 1255), vec3(761, -1434, 1318), vec3(602, -1421, 1256),
	vec3(360, -1356, 1137), vec3(329, -1374, 1023), vec3(335, -1447, 826),  vec3(360, -1479, 1042), vec3(453, -1513, 1157),
	vec3(761, -1552, 1287), vec3(761, -1587, 1267), vec3(761, -1804, 1279), vec3(529, -1458, 1206), vec3(622, -1563, 1235),
	vec3(516, -1693, 1131), vec3(410, -1594, 994),  vec3(494, -1710, 710),  vec3(492, -1816, 643),  vec3(761, -1979, 725),
	vec3(761, -1819, 878),  vec3(545, -271, 1218),  vec3(402, -925, 1190),  vec3(506, -865, 1223),  vec3(554, -852, 1223),
	vec3(583, -925, 1223),  vec3(641, -900, 1223),  vec3(450, -972, 1223),  vec3(535, -981, 1223),  vec3(599, -969, 1223),
	vec3(629, -937, 1223),  vec3(670, -1001, 1263), vec3(651, -1477, 1246), vec3(761, -1488, 1276), vec3(692, -1069, 1284),
	vec3(761, -1070, 1376), vec3(580, -1133, 1280), vec3(606, -1206, 1273), vec3(761, -1271, 1446), vec3(761, -1329, 1316)
};

// Triangles made up of points (counter clockwise)
int triangles[][3] = {
	{0, 1, 2},    {1, 3, 2},    {0, 2, 4},    {3, 7, 2},    {7, 12, 6},   {2, 7, 6},    {2, 6, 4},    {4, 6, 5},
	{5, 6, 13},   {6, 12, 13},  {3, 8, 7},    {7, 8, 12},   {3, 18, 9},   {9, 18, 10},  {3, 9, 8},    {9, 10, 8},
	{10, 11, 8},  {8, 11, 12},  {18, 16, 10}, {10, 16, 11}, {18, 19, 16}, {19, 21, 16}, {21, 22, 16}, {19, 26, 25},
	{19, 25, 21}, {25, 22, 21}, {26, 27, 28}, {26, 28, 25}, {28, 29, 25}, {25, 29, 22}, {22, 29, 33}, {33, 24, 22},
	{24, 23, 22}, {22, 23, 20}, {28, 36, 29}, {36, 35, 29}, {29, 35, 34}, {29, 34, 33}, {33, 34, 30}, {34, 31, 30},
	{36, 37, 35}, {37, 38, 39}, {35, 32, 34}, {34, 32, 31}, {37, 40, 35}, {37, 39, 40}, {35, 40, 32}, {41, 1, 0},
	{11, 42, 12}, {11, 16, 42}, {42, 16, 47}, {42, 47, 43}, {42, 43, 12}, {43, 44, 12}, {47, 48, 43}, {43, 48, 45},
	{48, 49, 45}, {49, 50, 45}, {45, 50, 46}, {44, 45, 46}, {46, 50, 14}, {49, 51, 50}, {50, 51, 14}, {47, 16, 48},
	{16, 15, 48}, {48, 15, 49}, {49, 15, 51}, {33, 52, 24}, {33, 30, 52}, {52, 30, 53}, {52, 53, 23}, {24, 52, 23},
	{16, 56, 15}, {15, 56, 54}, {15, 54, 51}, {51, 54, 14}, {54, 55, 14}, {54, 17, 55}, {16, 22, 56}, {22, 57, 56},
	{56, 57, 54}, {22, 59, 57}, {57, 59, 58}, {57, 17, 54}, {57, 58, 17}, {22, 20, 59}, {12, 44, 13}, {44, 46, 13},
	{46, 14, 13}, {44, 43, 45}, {18, 26, 19}, {18, 27, 26}, {27, 36, 28}, {27, 37, 36}
};

// Vertex shader
const char* vertexShader = "\
	#version 130													\n\
	in vec3 point;													\n\
	in vec3 normal;													\n\
	in vec2 uv;														\n\
	uniform mat4 modelview;											\n\
	uniform mat4 persp;												\n\
	uniform mat4 textureTransform = mat4(1);						\n\
	out vec3 vPoint;												\n\
	out vec3 vNormal;												\n\
	out vec2 vuv;													\n\
	void main() {													\n\
		vPoint = (modelview*vec4(point, 1)).xyz;					\n\
		vNormal = (modelview*vec4(normal, 0)).xyz;					\n\
		vuv = (textureTransform * vec4(uv, 0, 1)).xy;				\n\
		gl_Position = persp * vec4(vPoint, 1);						\n\
	}";

// Pixel shader
const char* pixelShader = "\
	#version 130												\n\
	in vec3 vPoint;												\n\
	in vec3 vNormal;											\n\
	in vec2 vuv;												\n\
	uniform float a = 0.1f;										\n\
	uniform vec3 lightPos = vec3(-1, 0, -2);					\n\
	uniform vec3 color = vec3(1, 1, 1);							\n\
	uniform sampler2D textureImage;								\n\
	out vec4 pColor;											\n\
	void main() {												\n\
		vec3 N = normalize(vNormal);							\n\
		vec3 L = normalize(lightPos-vPoint);					\n\
		vec3 R = reflect(L, N);									\n\
		vec3 E = normalize(vPoint);								\n\
		float d = abs(dot(L, N));								\n\
		float h = max(0, dot(R, E));							\n\
		float s = pow(h, 100);									\n\
		float intensity = clamp(a+d+s, 0, 1);					\n\
		vec4 texColor = texture(textureImage, vuv);				\n\
		pColor = vec4(texColor.rgb, 1);	// but make opaque		\n\
	}";

// Global constant variables indicating size of points, number of points, triangles, vertices, 
// normals, and uv coordinates
// Declares arrays of normals, pointsFullFace, trianglesFullFaces and uvs
const int npoints = sizeof(points) / sizeof(points[0]);
const int ntriangles = sizeof(triangles) / sizeof(triangles[0]);
const int midline = points[0][0];
vec3 normals[npoints * 2];
vec3 pointsFullFace[npoints * 2];
int trianglesFullFace[ntriangles * 2][3];
vec2 uvs[npoints * 2];
const int nvertices = ntriangles * 2 * 3;
const int sizePts = sizeof(pointsFullFace);
const int sizeNms = sizeof(normals);
const int sizeUVs = sizeof(uvs);

// Constant holding file name.
const char* filename = "image2.tga";
int textureUnit = 0;
GLuint textureName;

// Function to display image on screen.
void Display(GLFWwindow* w) {
	// Clears the buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set camera speed
	camera.SetSpeed(0.3f, 0.01f);

	// Set window size
	int screenWidth, screenHeight;
	glfwGetWindowSize(w, &screenWidth, &screenHeight);

	// Set scale 
	mat4 scale = Scale(cubeSize, cubeSize, cubeStretch);

	// Set texture transform
	float dx = 0, dy = -0.05, s = 1;
	mat4 t = Translate(dx, dy, 0) * Scale(s);

	// Clear to gray, use app's shader
	glClearColor(0.5, 0.5, 0.5, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);

	// Load texture
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureName);

	// Set vertex attribute pointers & uniforms
	VertexAttribPointer(program, "point", 3, 0, (void*)0);
	VertexAttribPointer(program, "normal", 3, 0, (void*)sizePts);
	VertexAttribPointer(program, "uv", 2, 0, (void*)(sizePts + sizeNms));
	SetUniform(program, "modelview", camera.modelview);
	SetUniform(program, "persp", camera.persp);
	SetUniform(program, "textureImage", textureUnit);
	SetUniform(program, "textureTransform", t);

	// Draw shape
	glDrawElements(GL_TRIANGLES, nvertices, GL_UNSIGNED_INT, trianglesFullFace);

	glFlush();
}

// Function to changing the points from pixel values to lie between +/- 1.  
void Normalize() {
	// Scale and offset so that points fall within +/-1 in x, y and z
	vec3 mn(FLT_MAX), mx(-FLT_MAX);
	for (int i = 0; i < npoints * 2; i++) {
		vec3 p = pointsFullFace[i];
		for (int k = 0; k < 3; k++) {
			if (p[k] < mn[k]) mn[k] = p[k];
			if (p[k] > mx[k]) mx[k] = p[k];
		}
	}
	vec3 center = .5f * (mn + mx), range = mx - mn;
	float maxrange = std::max(range.x, std::max(range.y, range.z));
	float s = 2 / maxrange;
	for (int i = 0; i < npoints * 2; i++) {
		pointsFullFace[i] = s * (pointsFullFace[i] - center);
	}
}

// Method to reflect the points and triangles of the left side of the face, so that
// the whole face can be displayed
void Reflect() {
	// Compute new number of vertices and fill with existing vertices from left side of face
	for (int i = 0; i < npoints; ++i) {
		pointsFullFace[i] = points[i];
	}

	// Compute new number of triangles and fill with existing triangles from left side of face
	for (int i = 0; i < ntriangles; ++i) {
		int* t = triangles[i];
		trianglesFullFace[i][0] = t[0];
		trianglesFullFace[i][1] = t[1];
		trianglesFullFace[i][2] = t[2];
	}

	// Fill in second half of doubled array for pointsFullFace. New x-coordinates are calculated using the
	// value of the midline x-coordinate
	for (int i = 0; i < npoints; ++i) {
		vec3 reflectedPoint(midline + (midline - points[i].x), points[i].y, points[i].z);
		pointsFullFace[i + npoints] = reflectedPoint;
	}

	// Fill in second half of doubled triangle array
	for (int i = 0; i < ntriangles; ++i) {
		int* triangle = triangles[i];

		// Test each triangle corner for whether it is on(or close to) the mid - line
		// If on the midline, use the same vertex id as in the original triangle
		// If not on midline, use vertex id from second half of vertex array
		trianglesFullFace[i + ntriangles][0] = pointsFullFace[triangle[0]].x == midline ? triangle[0] : triangle[0] + npoints;
		trianglesFullFace[i + ntriangles][1] = pointsFullFace[triangle[1]].x == midline ? triangle[1] : triangle[1] + npoints;
		trianglesFullFace[i + ntriangles][2] = pointsFullFace[triangle[2]].x == midline ? triangle[2] : triangle[2] + npoints;

		// Reverse order of triangle to ensure they are still in counter clockwise order.
		int temp = trianglesFullFace[i + ntriangles][0];
		trianglesFullFace[i + ntriangles][0] = trianglesFullFace[i + ntriangles][1];
		trianglesFullFace[i + ntriangles][1] = temp;
	}
}

// Method to compute normals
void computeNormals() {
	// Allocate normals same size as doubled vertex array, and initialize all normals to (0,0,0)
	for (int i = 0; i < npoints * 2; ++i) {
		normals[i] = vec3(0, 0, 0);
	}

	// For each triangle, compute its surface normal, and add normal to each corresponding vertex normals array
	for (int i = 0; i < ntriangles * 2; ++i) {
		int* t = trianglesFullFace[i];
		vec3 p1(pointsFullFace[t[0]]), p2(pointsFullFace[t[1]]), p3(pointsFullFace[t[2]]);
		vec3 n = normalize(cross(p3 - p2, p2 - p1));
		for (int k = 0; k < 3; k++) {
			normals[t[k]] += n;
		}
	}

	// Set vertex normals to unit length
	for (int i = 0; i < npoints * 2; ++i) {
		normals[i] = normalize(normals[i]);
	}
}



// Mouse

bool Shift(GLFWwindow* w) {
	return glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		glfwGetKey(w, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
}

int WindowHeight(GLFWwindow* w) {
	int width, height;
	glfwGetWindowSize(w, &width, &height);
	return height;
}

void MouseButton(GLFWwindow* w, int butn, int action, int mods) {
	double x, y;
	glfwGetCursorPos(w, &x, &y);
	y = WindowHeight(w) - y;
	picked = NULL;
	if (action == GLFW_PRESS) {
		if (MouseOver(x, y, light, camera.fullview, 20)) {
			picked = &lightMover;
			lightMover.Down(&light, (int)x, (int)y, camera.modelview, camera.persp);
		}
		if (picked == NULL) {
			picked = &camera;
			camera.MouseDown((int)x, (int)y);
		}
	}
	if (action == GLFW_RELEASE)
		camera.MouseUp();
}

void MouseMove(GLFWwindow* w, double x, double y) {
	y = WindowHeight(w) - y;
	if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (picked == &lightMover)
			lightMover.Drag((int)x, (int)y, camera.modelview, camera.persp);
		else
			camera.MouseDrag(x, y, Shift(w));
	}
}

void MouseWheel(GLFWwindow* w, double ignore, double spin) {
	camera.MouseWheel(spin > 0, Shift(w));
}

// Application

const char* usage = "Usage\n\
    mouse-drag:\t\trotate x,y\n\
    with shift:\t\ttranslate x,y\n\
    mouse-wheel:\trotate/translate z\n";

void Resize(GLFWwindow* w, int width, int height) {
	camera.Resize(width, height);
	glViewport(0, 0, screenWidth = width, screenHeight = height);
}

// Initializes the vertex buffer
void InitVertexBuffer() {

	Reflect(); // Reflect points to create a whole face
	Normalize(); // Set points to be within +/- 1.
	computeNormals(); // Compute normals 

	// Scale uv coordinates so that all points fall within [0,1]
	for (int i = 0; i < npoints * 2; ++i) {
		uvs[i] = vec2(pointsFullFace[i].x * 0.5 + 0.5, pointsFullFace[i].y * 0.5 + 0.5);
	}

	// Create GPU buffer, make it the active buffer
	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	// Allocate memory for vertex positions and normals
	glBufferData(GL_ARRAY_BUFFER, sizePts + sizeNms + sizeUVs, NULL, GL_STATIC_DRAW);
	// Copy data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizePts, &pointsFullFace[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizePts, sizeNms, &normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizePts + sizeNms, sizeUVs, &uvs[0]);
}

// Display a message if GFLW throws an error
void ErrorGFLW(int id, const char* reason) {
	printf("GFLW error %i: %s\n", id, reason);
}

void Close() {
	// unbind vertex buffer and free GPU memory
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vBuffer);
}

// Main function runs when program is started.
int main() {
	glfwSetErrorCallback(ErrorGFLW);
	if (!glfwInit())
		return 1;
	int screenWidth = 1200;
	GLFWwindow* w = glfwCreateWindow(screenWidth, screenWidth, "Face", NULL, NULL);
	if (!w) {
		glfwTerminate();
		return 1;
	}
	glfwSetScrollCallback(w, MouseWheel);
	glfwSetMouseButtonCallback(w, MouseButton);
	glfwSetCursorPosCallback(w, MouseMove);
	glfwMakeContextCurrent(w);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	printf("GL version: %s\n", glGetString(GL_VERSION));
	PrintGLErrors();
	program = LinkProgramViaCode(&vertexShader, &pixelShader);
	InitVertexBuffer();
	camera.SetSpeed(.01f, .001f); // **** otherwise, a bit twitchy
	glfwSetWindowSizeCallback(w, Resize); // ***** so can view larger window
	glfwSwapInterval(1);
	// Set texture unit and initialize texture name
	textureName = LoadTexture(filename, textureUnit); // in Misc.h
	while (!glfwWindowShouldClose(w)) {
		Display(w);
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
	Close();
	glfwDestroyWindow(w);
	glfwTerminate();
}