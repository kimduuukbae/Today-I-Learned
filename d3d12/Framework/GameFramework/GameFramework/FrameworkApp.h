#pragma once

class SceneManager;

class FrameworkApp : public D3DApp {
public:
	FrameworkApp(HINSTANCE hInstance);
	FrameworkApp(const FrameworkApp& rhs) = delete;
	FrameworkApp& operator=(const FrameworkApp& rhs) = delete;
	~FrameworkApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw() override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

	POINT lastMousePos{};
	SceneManager* sceneManager{ nullptr };
};

