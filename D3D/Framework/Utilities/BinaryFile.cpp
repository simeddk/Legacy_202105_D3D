#include "Framework.h"
#include "BinaryFile.h"

//////////////////////////////////////////////////////////////////////////

BinaryWriter::BinaryWriter(wstring filePath)
	: fileHandle(NULL), size(0)
{
	assert(filePath.length() > 0);
	fileHandle = CreateFile
	(
		filePath.c_str()
		, GENERIC_WRITE
		, 0
		, NULL
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, NULL
	);


	bool isChecked = fileHandle != INVALID_HANDLE_VALUE;
	assert(isChecked);
}

BinaryWriter::~BinaryWriter()
{
	if (fileHandle != NULL)
	{
		CloseHandle(fileHandle);
		fileHandle = NULL;
	}
}

void BinaryWriter::Bool(bool data)
{
	WriteFile(fileHandle, &data, sizeof(bool), &size, NULL);
}

void BinaryWriter::Word(WORD data)
{
	WriteFile(fileHandle, &data, sizeof(WORD), &size, NULL);
}

void BinaryWriter::Int(int data)
{
	WriteFile(fileHandle, &data, sizeof(int), &size, NULL);
}

void BinaryWriter::UInt(UINT data)
{
	WriteFile(fileHandle, &data, sizeof(UINT), &size, NULL);
}

void BinaryWriter::Float(float data)
{
	WriteFile(fileHandle, &data, sizeof(float), &size, NULL);
}

void BinaryWriter::Double(double data)
{
	WriteFile(fileHandle, &data, sizeof(double), &size, NULL);
}

void BinaryWriter::Vector2(const D3DXVECTOR2& data)
{
	WriteFile(fileHandle, &data, sizeof(D3DXVECTOR2), &size, NULL);
}

void BinaryWriter::Vector3(const D3DXVECTOR3& data)
{
	WriteFile(fileHandle, &data, sizeof(D3DXVECTOR3), &size, NULL);
}

void BinaryWriter::Vector4(const D3DXVECTOR4& data)
{
	WriteFile(fileHandle, &data, sizeof(D3DXVECTOR4), &size, NULL);
}

void BinaryWriter::Color3f(const D3DXCOLOR& data)
{
	WriteFile(fileHandle, &data, sizeof(D3DXCOLOR) - 4, &size, NULL);
}

void BinaryWriter::Color4f(const D3DXCOLOR& data)
{
	WriteFile(fileHandle, &data, sizeof(D3DXCOLOR), &size, NULL);
}

void BinaryWriter::Matrix(const D3DXMATRIX& data)
{
	WriteFile(fileHandle, &data, sizeof(D3DXMATRIX), &size, NULL);
}

void BinaryWriter::String(const string & data)
{
	UInt(data.size());

	const char* str = data.c_str();
	WriteFile(fileHandle, str, data.size(), &size, NULL);
}

void BinaryWriter::Byte(void * data, UINT dataSize)
{
	WriteFile(fileHandle, data, dataSize, &size, NULL);
}

//////////////////////////////////////////////////////////////////////////

BinaryReader::BinaryReader(wstring filePath)
	: fileHandle(NULL), size(0)
{
	assert(filePath.length() > 0);
	fileHandle = CreateFile
	(
		filePath.c_str()
		, GENERIC_READ
		, FILE_SHARE_READ
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL
	);

	bool isChecked = fileHandle != INVALID_HANDLE_VALUE;
	assert(isChecked);
}

BinaryReader::~BinaryReader()
{
	if (fileHandle != NULL)
	{
		CloseHandle(fileHandle);
		fileHandle = NULL;
	}
}

bool BinaryReader::Bool()
{
	bool temp = false;
	ReadFile(fileHandle, &temp, sizeof(bool), &size, NULL);

	return temp;
}

WORD BinaryReader::Word()
{
	WORD temp = 0;
	ReadFile(fileHandle, &temp, sizeof(WORD), &size, NULL);

	return temp;
}

int BinaryReader::Int()
{
	int temp = 0;
	ReadFile(fileHandle, &temp, sizeof(int), &size, NULL);

	return temp;
}

UINT BinaryReader::UInt()
{
	UINT temp = 0;
	ReadFile(fileHandle, &temp, sizeof(UINT), &size, NULL);

	return temp;
}

float BinaryReader::Float()
{
	float temp = 0.0f;
	ReadFile(fileHandle, &temp, sizeof(float), &size, NULL);

	return temp;
}

double BinaryReader::Double()
{
	double temp = 0.0f;
	ReadFile(fileHandle, &temp, sizeof(double), &size, NULL);

	return temp;
}

D3DXVECTOR2 BinaryReader::Vector2()
{
	float x = Float();
	float y = Float();

	return D3DXVECTOR2(x, y);
}

D3DXVECTOR3 BinaryReader::Vector3()
{
	float x = Float();
	float y = Float();
	float z = Float();

	return D3DXVECTOR3(x, y, z);
}

D3DXVECTOR4 BinaryReader::Vector4()
{
	float x = Float();
	float y = Float();
	float z = Float();
	float w = Float();

	return D3DXVECTOR4(x, y, z, w);
}

D3DXCOLOR BinaryReader::Color3f()
{
	float r = Float();
	float g = Float();
	float b = Float();

	return D3DXCOLOR(r, g, b, 1.0f);
}

D3DXCOLOR BinaryReader::Color4f()
{
	float r = Float();
	float g = Float();
	float b = Float();
	float a = Float();

	return D3DXCOLOR(r, g, b, a);
}

D3DXMATRIX BinaryReader::Matrix()
{
	D3DXMATRIX matrix;
	matrix._11 = Float();	matrix._12 = Float();	matrix._13 = Float();	matrix._14 = Float();
	matrix._21 = Float();	matrix._22 = Float();	matrix._23 = Float();	matrix._24 = Float();
	matrix._31 = Float();	matrix._32 = Float();	matrix._33 = Float();	matrix._34 = Float();
	matrix._41 = Float();	matrix._42 = Float();	matrix._43 = Float();	matrix._44 = Float();

	return matrix;
}

string BinaryReader::String()
{
	UINT size = Int();

	char* temp = new char[size + 1];
	ReadFile(fileHandle, temp, sizeof(char) * size, &this->size, NULL); //데이터 읽기
	temp[size] = '\0';

	return temp;
}

void BinaryReader::Byte(void ** data, UINT dataSize)
{
	ReadFile(fileHandle, *data, dataSize, &size, NULL);
}
