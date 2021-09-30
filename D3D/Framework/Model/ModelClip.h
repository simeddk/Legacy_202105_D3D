#pragma once

struct ModelKeyFrameData
{
	float Frame;

	Vector3 Scale;
	Quaternion Rotation;
	Vector3 Translation;
};

struct ModelKeyFrame
{
	wstring BoneName;
	vector<ModelKeyFrameData> Transforms;
};

class ModelClip
{
public:
	friend class Model;

private:
	ModelClip();
	~ModelClip();

public:
	float FrameRate() { return frameRate; }
	UINT FrameCount() { return frameCount; }

	ModelKeyFrame* Keyframe(wstring name);

private:
	wstring name;

	float frameRate;
	UINT frameCount;

	unordered_map<wstring, ModelKeyFrame*> keyframeMap;

};