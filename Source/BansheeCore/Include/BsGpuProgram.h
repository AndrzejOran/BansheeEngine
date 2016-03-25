//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsCoreObject.h"
#include "BsIReflectable.h"

namespace BansheeEngine 
{
	/** @addtogroup RenderAPI
	 *  @{
	 */

	/** Types of programs that may run on GPU. */
	enum GpuProgramType
	{
		GPT_VERTEX_PROGRAM,
		GPT_FRAGMENT_PROGRAM,
		GPT_GEOMETRY_PROGRAM,
		GPT_DOMAIN_PROGRAM,
		GPT_HULL_PROGRAM,
		GPT_COMPUTE_PROGRAM
	};

	/**	GPU program profiles representing supported feature sets. */
	enum GpuProgramProfile
	{
		GPP_NONE,
		GPP_FS_1_1,
		GPP_FS_1_2,
		GPP_FS_1_3,
		GPP_FS_1_4,
		GPP_FS_2_0,
		GPP_FS_2_x,
		GPP_FS_2_a,
		GPP_FS_2_b,
		GPP_FS_3_0,
		GPP_FS_3_x,
		GPP_FS_4_0,
		GPP_FS_4_1,
		GPP_FS_5_0,
		GPP_VS_1_1,
		GPP_VS_2_0,
		GPP_VS_2_x,
		GPP_VS_2_a,
		GPP_VS_3_0,
		GPP_VS_4_0,
		GPP_VS_4_1,
		GPP_VS_5_0,
		GPP_GS_4_0,
		GPP_GS_4_1,
		GPP_GS_5_0,
		GPP_HS_5_0,
		GPP_DS_5_0,
		GPP_CS_5_0
	};

	/** Data describing a GpuProgram. */
	class BS_CORE_EXPORT GpuProgramProperties
	{
	public:
		GpuProgramProperties(const String& source, const String& entryPoint,
			GpuProgramType gptype, GpuProgramProfile profile);

		virtual ~GpuProgramProperties() { }

        /** Source used for creating this program. */
        const String& getSource() const { return mSource; }
        
		/**	Type of GPU program (for example fragment, vertex). */
        GpuProgramType getType() const { return mType; }

		/**	Profile of the GPU program (for example VS_4_0, VS_5_0). */
		GpuProgramProfile getProfile() const { return mProfile; }

		/**	Name of the program entry method (for example "main"). */
		const String& getEntryPoint() const { return mEntryPoint; }

	protected:
		friend class GpuProgramRTTI;

		GpuProgramType mType;
		String mEntryPoint;
		GpuProgramProfile mProfile;
		String mSource;
	};

	/**
	 * Contains a GPU program such as vertex or fragment program which gets compiled from the provided source code.
	 *
	 * @note	Sim thread only.
	 */
	class BS_CORE_EXPORT GpuProgram : public IReflectable, public CoreObject
	{
	public:
		virtual ~GpuProgram() { }

		/**
		 * Returns true if shader was successfully compiled. 
		 *
		 * @note	Only valid after core thread has initialized the program.
		 */
		bool isCompiled() const;

		/**
		 * Returns an error message returned by the compiler, if the compilation failed.
		 *
		 * @note	Only valid after core thread has initialized the program.
		 */
		String getCompileErrorMessage() const;

		/**
		 * Creates a new parameters object compatible with this program definition. You may populate the returned object 
		 * with actual parameter values and bind it to the pipeline to render an object using those values and this program.
		 *
		 * @note	Only valid after core thread has initialized the program.
		 */
		GpuParamsPtr createParameters();

		/**
		 * Returns description of all parameters in this GPU program.
		 *
		 * @note	Only valid after core thread has initialized the program.
		 */
		GpuParamDescPtr getParamDesc() const;

		/** Retrieves a core implementation of a gpu program usable only from the core thread. */
		SPtr<GpuProgramCore> getCore() const;

		/** Returns properties that contain information about the GPU program. */
		const GpuProgramProperties& getProperties() const { return mProperties; }

		/**
		 * Creates a new GPU program using the provided source code. If compilation fails or program is not supported
		 * isCompiled() with return false, and you will be able to retrieve the error message via getCompileErrorMessage().
		 *
		 * @param[in]	source				Source code to compile the shader from.
		 * @param[in]	entryPoint			Name of the entry point function, for example "main".
		 * @param[in]	language			Language the source is written in, for example "hlsl" or "glsl".
		 * @param[in]	gptype				Type of the program, for example vertex or fragment.
		 * @param[in]	profile				Program profile specifying supported feature-set. Must match the type.
		 * @param[in]	requiresAdjacency	If true then adjacency information will be provided when rendering using this 
		 *									program.
		 */
		static GpuProgramPtr create(const String& source, const String& entryPoint, const String& language, GpuProgramType gptype,
			GpuProgramProfile profile, bool requiresAdjacency = false);

	protected:
		friend class GpuProgramManager;

		GpuProgram(const String& source, const String& entryPoint, const String& language,
			GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired = false);

		/** @copydoc CoreObject::createCore */
		SPtr<CoreObjectCore> createCore() const;

	protected:
		bool mNeedsAdjacencyInfo;
		String mLanguage;
		GpuProgramProperties mProperties;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class GpuProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** @} */

	/** @addtogroup RenderAPI-Internal
	 *  @{
	 */

	/**
	 * Core thread version of a GpuProgram.
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT GpuProgramCore : public CoreObjectCore
	{
	public:
		virtual ~GpuProgramCore() { }

		/** Returns whether this program can be supported on the current renderer and hardware. */
        virtual bool isSupported() const;

		/** Returns true if shader was successfully compiled. */
		virtual bool isCompiled() const { return mIsCompiled; }

		/**	Returns an error message returned by the compiler, if the compilation failed. */
		virtual String getCompileErrorMessage() const { return mCompileError; }

		/**
		 * Sets whether this geometry program requires adjacency information from the input primitives.
		 *
		 * @note	Only relevant for geometry programs.
		 */
		virtual void setAdjacencyInfoRequired(bool required) { mNeedsAdjacencyInfo = required; }

		/**
		 * Returns whether this geometry program requires adjacency information from the input primitives.
		 *
		 * @note	Only relevant for geometry programs.
		 */
		virtual bool isAdjacencyInfoRequired() const { return mNeedsAdjacencyInfo; }

		/** @copydoc GpuProgram::createParameters */
		virtual SPtr<GpuParamsCore> createParameters();

		/** @copydoc GpuProgram::getParamDesc */
		GpuParamDescPtr getParamDesc() const { return mParametersDesc; }

		/**	Returns GPU program input declaration. Only relevant for vertex programs. */
		SPtr<VertexDeclarationCore> getInputDeclaration() const { return mInputDeclaration; }

		/**	Returns properties that contain information about the GPU program. */
		const GpuProgramProperties& getProperties() const { return mProperties; }

		/** @copydoc GpuProgram::create */
		static SPtr<GpuProgramCore> create(const String& source, const String& entryPoint, const String& language, GpuProgramType gptype,
			GpuProgramProfile profile, bool requiresAdjacency = false);

	protected:
		GpuProgramCore(const String& source, const String& entryPoint,
			GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired = false);

		/** Returns whether required capabilities for this program is supported. */
		bool isRequiredCapabilitiesSupported() const;

		bool mNeedsAdjacencyInfo;

		bool mIsCompiled;
		String mCompileError;

		GpuParamDescPtr mParametersDesc;
		SPtr<VertexDeclarationCore> mInputDeclaration;
		GpuProgramProperties mProperties;
	};

	/** @} */
}