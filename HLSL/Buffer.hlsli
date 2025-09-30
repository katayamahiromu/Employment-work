#include"../Source/Graphics/BufferSlots.h"
#define CBUFFER(name,slot)cbuffer name : register(b ## slot)