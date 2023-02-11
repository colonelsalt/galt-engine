#pragma once

struct ControllerInput
{
	bool IsConnected;
	bool IsAnalogue;
	
	float MovementAxisX;
	float MovementAxisY;

	float CameraAxisX;
	float CameraAxisY;

	float DeltaTime;
	ControllerInput* LastInput;

	union
	{
		bool Buttons[12];
		struct
		{
			bool Up;
			bool Down;
			bool Left;
			bool Right;

			bool A;
			bool B;
			bool X;
			bool Y;

			bool LeftShoulder;
			bool RightShoulder;

			bool Start;
			bool Back;
		};
	};
};