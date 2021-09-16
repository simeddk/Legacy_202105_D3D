#pragma once

class Converter
{
public:
	Converter();
	~Converter();

	void ReadFile(wstring file);

private:
	wstring file;

	Assimp::Importer* importer;
	const aiScene* scene;

};