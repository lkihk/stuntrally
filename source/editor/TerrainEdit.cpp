#include "pch.h"
#include "Defines.h"
#include "OgreApp.h"
//#include "../vdrift/settings.h"
using namespace Ogre;


///  get edit rect
//--------------------------------------------------------------------------------------------------------------------------
bool App::getEditRect(Vector3& pos, Rect& rcBrush, Rect& rcMap, int size,  int& cx, int& cy)
{
	float tws = sc.td.fTerWorldSize;
	int t = sc.td.iTerSize;

	//  world float to map int
	int mapX = (pos.x + 0.5*tws)/tws*t, mapY = (-pos.z + 0.5*tws)/tws*t;
	mapX = std::max(0,std::min(t-1, mapX)), mapY = std::max(0,std::min(t-1, mapY));

	int brS = (int)mBrSize[curBr];
	float hBr = brS * 0.5f;
	rcMap = Rect(mapX-hBr, mapY-hBr, mapX+hBr, mapY+hBr);
	rcBrush = Rect(0,0, brS,brS);
	cx = 0;  cy = 0;

	if (rcMap.left < 0)  // trim
	{	rcBrush.left += 0 - rcMap.left;  cx += -rcMap.left;
		rcMap.left = 0;
	}
	if (rcMap.top < 0)
	{	rcBrush.top  += 0 - rcMap.top;   cy += -rcMap.top;
		rcMap.top = 0;
	}
	if (rcMap.right > size)
	{	rcBrush.right  -= rcMap.right - size;
		rcMap.right = size;
	}
	if (rcMap.bottom > size)
	{	rcBrush.bottom -= rcMap.bottom - size;
		rcMap.bottom = size;
	}

	if (rcMap.right - rcMap.left < 1 ||
		rcMap.bottom - rcMap.top < 1)
		return false;  // no area
	
	/*sprintf(sBrushTest,
		" ---br rect--- \n"
		"size: %3d %6.3f \n"
		"pos %3d %3d  c: %3d %3d \n"
		"rect:  %3d %3d  %3d %3d \n"
		"map: %3d %3d  %3d %3d \n"
		"br: %3d %3d  %3d %3d \n"
		,brS, mBrSize[curBr]
		,mapX, mapY, cx, cy
		 ,rcMap.right-rcMap.left, rcMap.bottom-rcMap.top
		 ,rcBrush.right-rcBrush.left, rcBrush.bottom-rcBrush.top
		,rcMap.left, rcMap.top, rcMap.right, rcMap.bottom
		,rcBrush.left, rcBrush.top, rcBrush.right, rcBrush.bottom);
	LogO(String(sBrushTest));/**/
	return true;
}

//  after size change
void App::updBrush()
{
	if (mBrSize[curBr] < 1)  mBrSize[curBr] = 1;
	if (mBrSize[curBr] > BrushMaxSize)  mBrSize[curBr] = BrushMaxSize;

	int size = (int)mBrSize[curBr], a = 0;
	float s = size * 0.5f;

	for (int y = 0; y < size; ++y)
	{	a = y * BrushMaxSize;
		for (int x = 0; x < size; ++x, ++a)
		{	// -1..1
			float fx = ((float)x - s)/s;
			float fy = ((float)y - s)/s;
			float d = std::max(0.f, 1.f - sqrt(fx*fx + fy*fy));
			//float c = abs(d);
			float c = sinf(d * PI_d*0.5f);
			mBrushData[a] = powf(c, mBrPow[curBr]);
	}	}
}


///  ^~ Deform
//--------------------------------------------------------------------------------------------------------------------------
void App::deform(Vector3 &pos, float dtime, float brMul)
{
	Rect rcBrush, rcMap;  int cx,cy;
	if (!getEditRect(pos, rcBrush, rcMap, sc.td.iTerSize, cx,cy))
		return;
	
	float *fHmap = terrain->getHeightData();
		
	float its = mBrIntens[curBr] * dtime * brMul;
	int mapPos, brPos, jj = cy;
	
	for (int j = rcMap.top; j < rcMap.bottom; ++j,++jj)
	{
		mapPos = j * sc.td.iTerSize + rcMap.left;
		brPos = jj * BrushMaxSize + cx;
		//brPos = std::max(0, std::min(BrushMaxSize*BrushMaxSize-1, brPos ));

		for (int i = rcMap.left; i < rcMap.right; ++i)
		{
			///  pos float -> sin data compute here ...
			fHmap[mapPos] += mBrushData[brPos] * its;
			++mapPos;  ++brPos;
		}
	}

	terrain->dirtyRect(rcMap);
	bTerUpd = true;
}

void App::smooth(Vector3 &pos, float dtime)
{
	float avg = 0.0f;
	int sample_count = 0;
	calcSmoothFactor(pos, avg, sample_count);
	
	if (sample_count)
		smoothTer(pos, avg / (float)sample_count, dtime);
}



///  -- Smooth
//-----------------------------------------------------------------------------------------
void App::calcSmoothFactor(Vector3 &pos, float& avg, int& sample_count)
{
	Rect rcBrush, rcMap;  int cx,cy;
	if (!getEditRect(pos, rcBrush, rcMap, sc.td.iTerSize, cx,cy))
		return;
	
	float *fHmap = terrain->getHeightData();
	int mapPos;

	avg = 0.0f;  sample_count = 0;
	
	for (int j = rcMap.top;j < rcMap.bottom; ++j)
	{
		mapPos = j * sc.td.iTerSize + rcMap.left;

		for (int i = rcMap.left;i < rcMap.right; ++i)
		{
			avg += fHmap[mapPos];  ++mapPos;
		}
	}
	
	sample_count = (rcMap.right - rcMap.left) * (rcMap.bottom - rcMap.top);
}

//-----------------------------------------------------------------------------------------
void App::smoothTer(Vector3 &pos, float avg, float dtime)
{
	Rect rcBrush, rcMap;  int cx,cy;
	if (!getEditRect(pos, rcBrush, rcMap, sc.td.iTerSize, cx,cy))
		return;
	
	float *fHmap = terrain->getHeightData();

	float mRatio = 1.f;
	float brushPos;
	int mapPos;
	
	float mFactor = mBrIntens[curBr] * dtime * 0.1f;

	for(int j = rcMap.top;j < rcMap.bottom;j++)
	{
		brushPos = (rcBrush.top + (int)((j - rcMap.top) * mRatio)) * BrushMaxSize;
		brushPos += rcBrush.left;
		//**/brushPos += cy * BrushMaxSize + cx;
		mapPos = j * sc.td.iTerSize + rcMap.left;

		for(int i = rcMap.left;i < rcMap.right;i++)
		{
			float val = avg - fHmap[mapPos];
			val = val * std::min(mBrushData[(int)brushPos] * mFactor, 1.0f);
			fHmap[mapPos] += val;
			++mapPos;
			brushPos += mRatio;
		}
	}

	terrain->dirtyRect(rcMap);
	bTerUpd = true;
}



//-----------------------------------------------------------------------------------------
/*void OgreApp::_splat(Vector3 &pos, float dtime)
{
	Rect rcBrush, rcMap;

	//Terrain *terrain = static_cast<Terrain*>(handle->getHandle());

	int mBlendMapSize = terrain->getLayerBlendMapSize();
	pos.x *= (float)mBlendMapSize;
	pos.y = (1.0f - pos.y) * (float)mBlendMapSize;
	if(!_getEditRect(pos, rcBrush, rcMap, mBlendMapSize))
		return;

	int mLayer = 0;
	
	mLayer = handle->_getLayerID(mTextureDiffuse, mTextureNormal, mEditDirection);  //--
	
	if(mLayer < 1)
		return;

	int mLayerMax = terrain->getLayerCount();
	TerrainLayerBlendMap *mBlendMaps[128];
	float *mBlendDatas[128];

	TerrainLayerBlendMap *mCurrentBlendMap = terrain->getLayerBlendMap(mLayer);
	float *mCurrentBlendData = mCurrentBlendMap->getBlendPointer();

	for(int l = mLayer;l < mLayerMax;l++)
	{
		mBlendMaps[l] = terrain->getLayerBlendMap(l);
		mBlendDatas[l] = mBlendMaps[l]->getBlendPointer();
	}

	float mRatio = (float)BRUSH_DATA_SIZE / (float)mBrushSize;
	float brushPos;
	int mapPos;

	int right = rcBrush.right;
	rcBrush.right = mBlendMapSize - rcBrush.left;
	rcBrush.left = mBlendMapSize - right;

	float factor = mBrushIntensity * dtime * 0.2f;
	if(!mEditDirection)
	{
		int u;
		float sum;

		for(int j = rcMap.top;j < rcMap.bottom;j++)
		{
			brushPos = (rcBrush.top + (int)((j - rcMap.top) * mRatio)) * BRUSH_DATA_SIZE;
			brushPos += rcBrush.right;

			mapPos = (j * mBlendMapSize) + rcMap.left;

			for(int i = rcMap.left;i < rcMap.right;i++)
			{
				brushPos -= mRatio;

				assert(mapPos < (mBlendMapSize * mBlendMapSize) && mapPos >= 0);
				assert((int)brushPos < (BRUSH_DATA_SIZE * BRUSH_DATA_SIZE) && (int)brushPos >= 0);

				sum = 0.0f;
				
				for(u = mLayer + 1;u < mLayerMax;u++)
					sum += mBlendDatas[u][mapPos];
				
				float val = mCurrentBlendData[mapPos] + (mBrushData[(int)brushPos] * factor);
				sum += val;

				if(sum > 1.0f)
				{
					float normfactor = 1.0f / (float)sum;
					mCurrentBlendData[mapPos] = val * normfactor;
					for(u = mLayer + 1;u < mLayerMax;u++)
						mBlendDatas[u][mapPos] *= normfactor;
				}
				else
					mCurrentBlendData[mapPos] = val;
				
				++mapPos;
			}
		}
		for(u = mLayer;u < mLayerMax;u++)
		{
			mBlendMaps[u]->dirtyRect(rcMap);
			mBlendMaps[u]->update();
		}
	}
	else
	{
		for(int j = rcMap.top;j < rcMap.bottom;j++)
		{
			brushPos = (rcBrush.top + (int)((j - rcMap.top) * mRatio)) * BRUSH_DATA_SIZE;
			brushPos += rcBrush.right;

			mapPos = (j * mBlendMapSize) + rcMap.left;

			for(int i = rcMap.left;i < rcMap.right;i++)
			{
				brushPos -= mRatio;

				assert(mapPos < (mBlendMapSize * mBlendMapSize) && mapPos >= 0);
				assert((int)brushPos < (BRUSH_DATA_SIZE * BRUSH_DATA_SIZE) && (int)brushPos >= 0);

				float val = mCurrentBlendData[mapPos] - (mBrushData[(int)brushPos] * factor);

				if(val < 0.0f)
					val = 0.0f;

				mCurrentBlendData[mapPos] = val;
				++mapPos;
			}
		}
		mCurrentBlendMap->dirtyRect(rcMap);
		mCurrentBlendMap->update();
	}
}/**/


//-----------------------------------------------------------------------------------------
/*void OgreApp::_paint(Vector3 &pos, float dtime)
{
	Rect rcBrush, rcMap;
	int ColourMapSize = mColourMapTextureSize->get();
	pos.x *= (float)ColourMapSize;
	pos.y = (1.0f - pos.y) * (float)ColourMapSize;
	if(!_getEditRect(pos, rcBrush, rcMap, ColourMapSize))
		return;

	float mRatio = (float)BRUSH_DATA_SIZE / (float)mBrushSize;
	float brushPos;
	int mapPos;

	int buffersize = terrain->getGlobalColourMap()->getBuffer()->getSizeInBytes();
	int spacing = buffersize / (ColourMapSize * ColourMapSize);
	unsigned char *data = (unsigned char *)terrain->getGlobalColourMap()->getBuffer()->lock(0,  buffersize, HardwareBuffer::HBL_NORMAL);
	PixelFormat pf = terrain->getGlobalColourMap()->getBuffer()->getFormat();
	ColourValue colVal;

	int right = rcBrush.right;
	rcBrush.right = ColourMapSize - rcBrush.left;
	rcBrush.left = ColourMapSize - right;

	float factor = std::min(mBrushIntensity * dtime * 0.2f, 1.0f);
	float bfactor;
	if(!mEditDirection)
	{
		for(int j = rcMap.top;j < rcMap.bottom;j++)
		{
			brushPos = (rcBrush.top + (int)((j - rcMap.top) * mRatio)) * BRUSH_DATA_SIZE;
			brushPos += rcBrush.right;

			mapPos = (j * ColourMapSize) + rcMap.left;

			for(int i = rcMap.left;i < rcMap.right;i++)
			{
				brushPos -= mRatio;

				assert(mapPos < (ColourMapSize * ColourMapSize) && mapPos >= 0);
				assert((int)brushPos < (BRUSH_DATA_SIZE * BRUSH_DATA_SIZE) && (int)brushPos >= 0);

				bfactor = mBrushData[(int)brushPos] * factor;
				PixelUtil::unpackColour(&colVal, pf, (void*)&data[mapPos * spacing]);
				colVal.r = colVal.r + ((mColour.r - colVal.r) * bfactor);
				colVal.g = colVal.g + ((mColour.g - colVal.g) * bfactor);
				colVal.b = colVal.b + ((mColour.b - colVal.b) * bfactor);
				PixelUtil::packColour(colVal, pf, (void*)&data[mapPos * spacing]);

				++mapPos;
			}
		}
	}
	else
	{
		for(int j = rcMap.top;j < rcMap.bottom;j++)
		{
			brushPos = (rcBrush.top + (int)((j - rcMap.top) * mRatio)) * BRUSH_DATA_SIZE;
			brushPos += rcBrush.right;

			mapPos = (j * ColourMapSize) + rcMap.left;

			for(int i = rcMap.left;i < rcMap.right;i++)
			{
				brushPos -= mRatio;

				assert(mapPos < (ColourMapSize * ColourMapSize) && mapPos >= 0);
				assert((int)brushPos < (BRUSH_DATA_SIZE * BRUSH_DATA_SIZE) && (int)brushPos >= 0);

				bfactor = mBrushData[(int)brushPos] * factor;
				PixelUtil::unpackColour(&colVal, pf, (void*)&data[mapPos * spacing]);
				colVal.r = colVal.r + ((1.0f - colVal.r) * bfactor);
				colVal.g = colVal.g + ((1.0f - colVal.g) * bfactor);
				colVal.b = colVal.b + ((1.0f - colVal.b) * bfactor);
				PixelUtil::packColour(colVal, pf, (void*)&data[mapPos * spacing]);

				++mapPos;
			}
		}
	}
	terrain->getGlobalColourMap()->getBuffer()->unlock();
}/**/
