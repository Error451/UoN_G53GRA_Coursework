#include "MyScene.h"
#include "MyShadowScene.h"
#include "MyTextureScene.h"

int main(int argc, char* argv[])
{
	Scene *scene = nullptr;
	//G53GRA coursework scene
	//scene = new MyScene(argc, argv, "MyFTLSimulator", 1280, 720);

	//shadow mapping (directional light only, for now) test
	//scene = new MyShadowScene(argc, argv, "MyShadowTest", 1280, 720);

	//advanced texture techniques test
	scene = new MyTextureScene(argc, argv, "MyTextureTest", 1280, 720);

	scene->Run();

	delete scene;
	return 0;
}