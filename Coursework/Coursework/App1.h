// Application.h
#ifndef _APP1_H
#define _APP1_H

#include "DXF.h"
#include "GeometryShader.h"
#include "TessellationShader.h"
#include "TessellationPlane.h"
#include "WaterTessellation.h"
#include "ColorRemoveShader.h"
#include "VerticalBlurShader.h"
#include "HorizontalBlurShader.h"
#include "BasicShader.h"
#include "DepthShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void checkLight();
	//Depth check
	void depthPassOne();
	void depthPassTwo();
	void depthPassThree();
	void renderWithLights();

	void bloom();
	void firstPass();
	void secondPass(); //remove colour
	void thirdPass(); //Horizontal blur
	void fourthPass(); //Verticle blur
	
	void finalPass(); //blend first pass with reduced colour version
	void gui();

private:
	//normal shader
	GeometryShader* geometryShader;
	TessellationShader* tesselation;
	WaterTessellation* waterTessellation;
	
	//Bloom shaders
	VerticalBlurShader* verticalBlurShader;
	HorizontalBlurShader* horizontalBlurShader;
	ColorRemoveShader* colourRemoveShader;
	BasicShader* bloomShader;

	//Mesh
	PlaneMesh* grassplane;
	TessellationPlane* WaterPlane;
	TessellationPlane* Groundplane;

	//Bloom
	OrthoMesh* orthoMesh;
	RenderTexture* baseTexture;
	RenderTexture* decresedColourTexture;
	RenderTexture* horizontalBlurTexture;
	RenderTexture* verticalBlurTexture;

	//Water
	float amper = 0.269f;
	float length = 3.197f;
	float speedW = 1.393f;
	float distance = 0.5f;
	bool waterEnabled = true;

	//Grass
	float slide = 1;
	float angle = 0;
	float time = 0;
	float speed = 5;
	float limit = 4;
	float lHeight = 2;
	float uHeight = 12;
	bool grassEnabled = true;

	//bloom control
	bool bloomToggle = false;
	float remove = 0.2;
	float threshold = 0.5;

	//IMGUI Toggles
	bool waterControls = false;
	bool grassControls = false;
	bool bloomControls = false;

	//Shadow maps and depthMaps
	ID3D11ShaderResourceView* depthMaps[3];
	ShadowMap* shadowMap[3];

	//Depth shaders
	DepthShader* depthShader;
	DepthShader* waterDepthShader;
	DepthShader* grassDepthShader;

	//Lights
	Light* light[3];

	//Light one
	float pos[3];
	float diff[4];
	bool toggleDepthOne = false;

	//light two
	float posTwo[3];
	float diffTwo[4];
	bool toggleDepthTwo = false;

	//light three
	float posThree[3];
	float diffThree[4];
	float dir[3];
	float dDir[3];
	bool toggleDepthThree = false;
};

#endif