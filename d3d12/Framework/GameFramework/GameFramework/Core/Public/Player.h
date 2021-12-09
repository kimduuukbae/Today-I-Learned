#pragma once

class Player : public Object
{
public:
	Player();
	virtual ~Player() = default;

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;
	virtual void Update(const GameTimer& gt) override;

	void LeftKey();
	void RightKey();
	void UpKey();
	void DownKey();
	void CtrlKey();

	void MouseLeft(float f);
private:
	class LagCameraComponent* cameraComponent;
	class InputComponent* inputComponent;
};

