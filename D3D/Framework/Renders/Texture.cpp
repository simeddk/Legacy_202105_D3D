#include "framework.h"
#include "Texture.h"

using namespace DirectX;

vector<TextureDesc> Textures::descs;

Texture::Texture(wstring file, D3DX11_IMAGE_LOAD_INFO* loadInfo)
	: file(file)
{
	bool b = Path::IsRelativePath(file);
	if (b == true)
		this->file = L"../../_Textures/" + file;

	Textures::Load(this, loadInfo);
	String::Replace(&this->file, L"../../_Textures", L"");
}

Texture::~Texture()
{
	
}

void Texture::SaveFile(wstring file)
{
	ID3D11Texture2D* srcTexture;
	view->GetResource((ID3D11Resource **)&srcTexture);

	SaveFile(file, srcTexture);
}

ID3D11Texture2D * Texture::GetTexture()
{
	ID3D11Texture2D* texture;
	view->GetResource((ID3D11Resource **)&texture);

	return texture;
}

void Texture::SaveFile(wstring file, ID3D11Texture2D * src)
{
	D3D11_TEXTURE2D_DESC srcDesc;
	src->GetDesc(&srcDesc);

	ID3D11Texture2D* dest;
	D3D11_TEXTURE2D_DESC destDesc;
	ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
	destDesc.Width = srcDesc.Width;
	destDesc.Height = srcDesc.Height;
	destDesc.MipLevels = 1;
	destDesc.ArraySize = 1;
	destDesc.Format = srcDesc.Format;
	destDesc.SampleDesc = srcDesc.SampleDesc;
	destDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	destDesc.Usage = D3D11_USAGE_STAGING;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateTexture2D(&destDesc, NULL, &dest);
	Check(hr);

	hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), src, NULL, dest);
	Check(hr);


	wstring ext = Path::GetExtension(file);
	transform(ext.begin(), ext.end(), ext.begin(), toupper);

	if(ext == L"BMP")
		hr = D3DX11SaveTextureToFile(D3D::GetDC(), dest, D3DX11_IFF_BMP, file.c_str());
	else
		hr = D3DX11SaveTextureToFile(D3D::GetDC(), dest, D3DX11_IFF_PNG, file.c_str());


	Check(hr);

	SafeRelease(dest);
}

D3D11_TEXTURE2D_DESC Texture::ReadPixel(vector<Color>* pixels)
{
	ID3D11Texture2D* srcTexture;
	view->GetResource((ID3D11Resource **)&srcTexture);

	return ReadPixel(srcTexture, pixels);
}

D3D11_TEXTURE2D_DESC Texture::ReadPixel(ID3D11Texture2D * src, vector<Color>* pixels)
{
	D3D11_TEXTURE2D_DESC srcDesc;
	src->GetDesc(&srcDesc);


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = srcDesc.Width;
	desc.Height = srcDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = srcDesc.Format;
	desc.SampleDesc = srcDesc.SampleDesc;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;

	ID3D11Texture2D* texture;
	Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture));
	Check(D3DX11LoadTextureFromTexture(D3D::GetDC(), src, NULL, texture));

	UINT* colors = new UINT[desc.Width * desc.Height];
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(texture, 0, D3D11_MAP_READ, NULL, &subResource);
	{
		memcpy(colors, subResource.pData, sizeof(UINT) * desc.Width * desc.Height);
	}
	D3D::GetDC()->Unmap(texture, 0);


	for (UINT y = 0; y < desc.Height; y++)
	{
		for (UINT x = 0; x < desc.Width; x++)
		{
			UINT index = desc.Width * y + x;

			float f = 1.0f / 255.0f;
			float r = f * (float)((0xFF000000 & colors[index]) >> 24);
			float g = f * (float)((0x00FF0000 & colors[index]) >> 16);
			float b = f * (float)((0x0000FF00 & colors[index]) >> 8);
			float a = f * (float)((0x000000FF & colors[index]) >> 0);

			pixels->push_back(D3DXCOLOR(a, b, g, r));
		}
	}


	SafeDeleteArray(colors);
	SafeRelease(texture);

	return desc;
}

void Textures::Create()
{

}

void Textures::Delete()
{
	for (TextureDesc desc : descs)
		SafeRelease(desc.view);
}

void Textures::Load(Texture * texture, D3DX11_IMAGE_LOAD_INFO * loadInfo)
{
	HRESULT hr;

	TexMetadata metaData;
	wstring ext = Path::GetExtension(texture->file);
	if (ext == L"tga")
	{
		hr = GetMetadataFromTGAFile(texture->file.c_str(), metaData);
		Check(hr);
	}
	else if (ext == L"dds")
	{
		hr = GetMetadataFromDDSFile(texture->file.c_str(), DDS_FLAGS_NONE, metaData);
		Check(hr);
	}
	else if (ext == L"hdr")
	{
		assert(false);
	}
	else
	{
		hr = GetMetadataFromWICFile(texture->file.c_str(), WIC_FLAGS_NONE, metaData);
		Check(hr);
	}

	UINT width = metaData.width;
	UINT height = metaData.height;

	if (loadInfo != NULL)
	{
		width = loadInfo->Width;
		height = loadInfo->Height;

		metaData.width = loadInfo->Width;
		metaData.height = loadInfo->Height;
	}


	TextureDesc desc;
	desc.file = texture->file;
	desc.width = width;
	desc.height = height;

	TextureDesc exist;
	bool bExist = false;
	for (TextureDesc temp : descs)
	{
		if (desc == temp)
		{
			bExist = true;
			exist = temp;

			break;
		}
	}

	if (bExist == true)
	{
		texture->metaData = exist.metaData;
		texture->view = exist.view;
	}
	else
	{
		ScratchImage image;
		if (ext == L"tga")
		{
			hr = LoadFromTGAFile(texture->file.c_str(), &metaData, image);
			Check(hr);
		}
		else if (ext == L"dds")
		{
			hr = LoadFromDDSFile(texture->file.c_str(), DDS_FLAGS_NONE, &metaData, image);
			Check(hr);
		}
		else if (ext == L"hdr")
		{
			assert(false);
		}
		else
		{
			hr = LoadFromWICFile(texture->file.c_str(), WIC_FLAGS_NONE, &metaData, image);
			Check(hr);
		}

		ID3D11ShaderResourceView* view;

		hr = DirectX::CreateShaderResourceView(D3D::GetDevice(), image.GetImages(), image.GetImageCount(), metaData, &view);
		Check(hr);

		desc.file = texture->file;
		desc.width = metaData.width;
		desc.height = metaData.height;
		desc.view = view;
		desc.metaData = metaData;

		texture->view = view;
		texture->metaData = metaData;

		descs.push_back(desc);
	}
}

//-----------------------------------------------------------------------------
// TextureArray
//-----------------------------------------------------------------------------
TextureArray::TextureArray(vector<wstring> & names, UINT width, UINT height, UINT mipLevels)
{
	for (UINT i = 0; i < names.size(); i++)
		names[i] = L"../../_Textures/" + names[i];

	vector<ID3D11Texture2D *> textures;
	textures = CreateTextures(names, width, height, mipLevels);


	D3D11_TEXTURE2D_DESC textureDesc;
	textures[0]->GetDesc(&textureDesc);

	ID3D11Texture2D* textureArray;
	//Texture2DArray
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = textureDesc.Width;
		desc.Height = textureDesc.Height;
		desc.MipLevels = textureDesc.MipLevels;
		desc.ArraySize = names.size();
		desc.Format = textureDesc.Format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &textureArray);
		Check(hr);
	}


	for (UINT i = 0; i < textures.size(); i++)
	{
		for (UINT level = 0; level < textureDesc.MipLevels; level++)
		{
			D3D11_MAPPED_SUBRESOURCE subResource;
			D3D::GetDC()->Map(textures[i], level, D3D11_MAP_READ, 0, &subResource);
			{
				D3D::GetDC()->UpdateSubresource(textureArray, D3D11CalcSubresource(level, i, textureDesc.MipLevels), NULL, subResource.pData, subResource.RowPitch, subResource.DepthPitch);
			}
			D3D::GetDC()->Unmap(textures[i], level);
		}
	}

	//Create File textures[0] -> test.png (for Test)
	//D3DX11SaveTextureToFile(D3D::GetDC(), textureArray, D3DX11_IFF_PNG, L"test.png");

	//SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		desc.Format = textureDesc.Format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MostDetailedMip = 0;
		desc.Texture2DArray.MipLevels = textureDesc.MipLevels;
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.ArraySize = names.size();

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(textureArray, &desc, &srv);
		Check(hr);
	}

	for (ID3D11Texture2D* texture : textures)
		SafeRelease(texture);

	SafeRelease(textureArray);
}

TextureArray::~TextureArray()
{
	SafeRelease(srv);
}

vector<ID3D11Texture2D*> TextureArray::CreateTextures(vector<wstring>& names, UINT width, UINT height, UINT mipLevels)
{
	vector<ID3D11Texture2D *> returnTextures;
	returnTextures.resize(names.size());

	for (UINT index = 0; index < returnTextures.size(); index++)
	{
		HRESULT hr;

		TexMetadata metaData;
		wstring ext = Path::GetExtension(names[index]);
		if (ext == L"tga")
		{
			hr = GetMetadataFromTGAFile(names[index].c_str(), metaData);
			Check(hr);
		}
		else if (ext == L"dds")
		{
			hr = GetMetadataFromDDSFile(names[index].c_str(), DDS_FLAGS_NONE, metaData);
			Check(hr);
		}
		else if (ext == L"hdr")
		{
			assert(false);
		}
		else
		{
			hr = GetMetadataFromWICFile(names[index].c_str(), WIC_FLAGS_NONE, metaData);
			Check(hr);
		}

		ScratchImage image;

		if (ext == L"tga")
		{
			hr = LoadFromTGAFile(names[index].c_str(), &metaData, image);
			Check(hr);
		}
		else if (ext == L"dds")
		{
			hr = LoadFromDDSFile(names[index].c_str(), DDS_FLAGS_NONE, &metaData, image);
			Check(hr);
		}
		else if (ext == L"hdr")
		{
			assert(false);
		}
		else
		{
			hr = LoadFromWICFile(names[index].c_str(), WIC_FLAGS_NONE, &metaData, image);
			Check(hr);
		}

		ScratchImage resizedImage;
		hr = DirectX::Resize
		(
			image.GetImages(), image.GetImageCount(), image.GetMetadata(), width, height, TEX_FILTER_DEFAULT, resizedImage
		);
		Check(hr);

		if (mipLevels > 1)
		{
			ScratchImage mipmapedImage;
			hr = DirectX::GenerateMipMaps
			(
				resizedImage.GetImages(), resizedImage.GetImageCount(), resizedImage.GetMetadata(), D3DX11_FILTER_NONE, mipLevels, mipmapedImage
			);
			Check(hr);

			hr = DirectX::CreateTextureEx
			(
				D3D::GetDevice()
				, mipmapedImage.GetImages()
				, mipmapedImage.GetImageCount()
				, mipmapedImage.GetMetadata()
				, D3D11_USAGE_STAGING
				, 0
				, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
				, 0
				, false
				, (ID3D11Resource **)&returnTextures[index]
			);
			Check(hr);

			mipmapedImage.Release();
		}
		else
		{
			hr = DirectX::CreateTextureEx
			(
				D3D::GetDevice()
				, resizedImage.GetImages()
				, resizedImage.GetImageCount()
				, resizedImage.GetMetadata()
				, D3D11_USAGE_STAGING
				, 0
				, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
				, 0
				, false
				, (ID3D11Resource **)&returnTextures[index]
			);
			Check(hr);
		}

		image.Release();
		resizedImage.Release();

	}

	return returnTextures;
}