/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once
#include "math/matrix.h"
#include "entity/entity.h"
#include "core/keyboard.h"

class Viewport;

class Plank	: public Entity {
public:
	Plank (void);
	~Plank (void);

private:
	void Update (float frame_time);
	void Draw (Viewport& viewport);
	void CreateResources (void);
	void DestroyResources (void);
};
