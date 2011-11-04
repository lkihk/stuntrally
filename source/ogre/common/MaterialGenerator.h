#ifndef MATERIALGENERATOR_H
#define MATERIALGENERATOR_H

class MaterialFactory;  class MaterialDefinition;  struct ShaderProperties;
#include "MaterialProperties.h" // textureMap typedef

#include <OgreHighLevelGpuProgram.h>
#include <OgreGpuProgramParams.h>

class MaterialGenerator
{
public:
	MaterialDefinition* mDef;
	ShaderProperties* mShader;
	MaterialFactory* mParent;
	
	// name of material generator - used for custom (water, glass...)
	// for standard material generator (i.e. this one) name is empty
	std::string mName;
	
	// shader cache
	bool mShaderCached;
	Ogre::HighLevelGpuProgramPtr mVertexProgram;
	Ogre::HighLevelGpuProgramPtr mFragmentProgram;
	
	virtual void generate(bool fixedFunction=false); // craft material
	
protected:
	// tex unit indices
	unsigned int mDiffuseTexUnit;
	unsigned int mLightTexUnit;
	unsigned int mBlendTexUnit;
	unsigned int mAlphaTexUnit;
	unsigned int mNormalTexUnit;
	unsigned int mEnvTexUnit;
	unsigned int mTerrainLightTexUnit; // global terrain lightmap
	unsigned int mShadowTexUnit_start; // start offset for shadow tex units
	
	unsigned int mTexUnit_i; // counter
	
	/// utility methods
	// get pointer to material if it exists and delete all techniques, if not, create new
	Ogre::MaterialPtr prepareMaterial(const std::string& matName);
	
	// vertex program
	Ogre::HighLevelGpuProgramPtr createVertexProgram();
	virtual void generateVertexProgramSource(Ogre::StringUtil::StrStreamType& outStream);
	virtual void vertexProgramParams(Ogre::HighLevelGpuProgramPtr program);
	virtual void individualVertexProgramParams(Ogre::GpuProgramParametersSharedPtr params);
	
	virtual void fpRealtimeShadowHelperSource(Ogre::StringUtil::StrStreamType& outStream);
	
	Ogre::HighLevelGpuProgramPtr createAmbientVertexProgram(); // ambient pass vertex program
	
	// fragment program
	Ogre::HighLevelGpuProgramPtr 	createFragmentProgram();
	virtual void generateFragmentProgramSource(Ogre::StringUtil::StrStreamType& outStream);
	virtual void fragmentProgramParams(Ogre::HighLevelGpuProgramPtr program);
	virtual void individualFragmentProgramParams(Ogre::GpuProgramParametersSharedPtr params);
	
	Ogre::HighLevelGpuProgramPtr createAmbientFragmentProgram(); // ambient pass fragment program
	
	bool needShaders();
	bool needShadows();
	
	// textures
	bool needNormalMap(); bool needEnvMap();
	bool needAlphaMap(); bool needBlendMap();
	bool needDiffuseMap(); bool needLightMap();
	bool needTerrainLightMap();
	
	// vertex shader input
	bool vpNeedTangent();
	bool vpNeedWMat();
	bool vpNeedWITMat();
	
	// passtrough (vertex to fragment)
	bool fpNeedTangentToCube();
	bool fpNeedWsNormal();
	bool fpNeedEyeVector();
	 
	// lighting
	bool fpNeedLighting(); // fragment lighting
	//bool vpNeedLighting(); // vertex lighting
	
	bool needFresnel();
	bool needLightingAlpha();
	
	
	std::string getChannel(unsigned int n);
	
	// pick best texture size (not higher than user tex size)
	std::string pickTexture(textureMap* textures);

	Ogre::CullingMode chooseCullingMode();
	Ogre::CullingMode chooseCullingModeAmbient();

};

#endif
