#include "Framework.h"
#include "ModelClip.h"

ModelClip::ModelClip()
{
}

ModelClip::~ModelClip()
{
}

ModelKeyFrame * ModelClip::Keyframe(wstring name)
{
	if (keyframeMap.count(name) < 1)
		return nullptr;

	return keyframeMap[name];
}
