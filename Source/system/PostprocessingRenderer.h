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

	void clear();

	void update(float elapsedTime);

	void addPostProcess(PostProcess* postProcess) { postProcessArray.push_back(postProcess); }

	void backErase() { postProcessArray.pop_back(); }

	void debugGui();

	//実際の描画
	void render();
	
	//ポストエフェクトの実行
	void execution();

	//削除
	void remove(PostProcess* pp);

	PostProcess* getPostProcess() { return scenePostProcess.get(); }

	ID3D11ShaderResourceView* getCacheSrv() { return cacheSrv; }
private:
	std::unique_ptr<PostProcess>scenePostProcess;
	std::vector<PostProcess*>postProcessArray;
	std::unique_ptr<FullScreenQuad>fullScreenQuad;
	ID3D11ShaderResourceView* cacheSrv;
};

class PostprocessingRendererManager
{
private:
	PostprocessingRendererManager() {};
	~PostprocessingRendererManager() { PostEffectArray.clear(); }
public:
	static PostprocessingRendererManager* instance()
	{
		static PostprocessingRendererManager inst;
		return &inst;
	}

	void add(PostprocessingRenderer* pr) { PostEffectArray.emplace_back(pr); }
	void remove(PostprocessingRenderer* pr)
	{
		auto it = std::find(PostEffectArray.begin(), PostEffectArray.end(), pr);
		if (it != PostEffectArray.end()) {
			PostEffectArray.erase(it);
		}
	}
	PostprocessingRenderer* at(int num) { return PostEffectArray.at(num); }

	void Gui();
private:
	std::vector<PostprocessingRenderer*>PostEffectArray;
};