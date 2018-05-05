#pragma once
// DEFUNCT

// DO NOT USE (REFERENCE STATIC CLASS InputHandler)


class Input
{
public:
	Input(){ }
	virtual ~Input(){}

	virtual void HandleKey(int key, int action, int mods){}
};