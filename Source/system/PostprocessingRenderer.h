#pragma once
#include"Graphics/PostProcessing.h"
#include"Graphics/FullScreenQuad.h"
#include<vector>
#include<set>

class PostprocessingRenderer
{
public:
	PostprocessingRenderer();
	~PostprocessingRenderer();

	static PostprocessingRenderer* instance()
	{
		static PostprocessingRenderer inst;
		return &inst;
	}

	void update(float elapsedTime);

	void addPostProcess(PostProcess* postProcess) { postProcessArray.push_back(postProcess); }
	void backErase() { postProcessArray.pop_back(); }

	void debugGui();

	void render();
	
	//çÌèú
	void remove(PostProcess* pp);

	PostProcess* getPostProcess() { return scenePostProcess.get(); }
private:
	std::unique_ptr<PostProcess>scenePostProcess;
	std::vector<PostProcess*>postProcessArray;
	std::unique_ptr<FullScreenQuad>fullScreenQuad;
};