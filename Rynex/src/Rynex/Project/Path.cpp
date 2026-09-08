#include "rypch.h"
#include "Path.h"
#include <Rynex/Project/Project.h>

// #define RY_PATH_LOG_MSG

namespace Rynex::FileSystem {

	Path::Path()
		: m_Path()
		, m_Origin(Origin::None)
	{
	}

	Path::Path(const Path& path)
		: m_Path(path.m_Path)
		, m_Origin(path.m_Origin)
	{
	}

	Path::Path(const std::string& pathStr, Origin origin)
	{
		auto [path, nextOrigin] = ConvertInternalPath(pathStr, origin);
		m_Path = path;
		m_Origin = nextOrigin;
	}

	Path::Path(const std::string_view& pathView, Origin origin)
	{
		std::string pathStr = pathView.data();
		auto [path, nextOrigin] = ConvertInternalPath(pathStr, origin);
		m_Path = path;
		m_Origin = nextOrigin;
	}


	Path::Path(const std::filesystem::path& path, Origin origin)
		: m_Path(path)
		, m_Origin(origin)
	{
		ConvertInternalPath();
	}

	Path::Path(const char* pathChar, Origin origin)
	{
		std::string pathStr = pathChar;
		auto [path, nextOrigin] = ConvertInternalPath(pathStr, origin);
		m_Path = path;
		m_Origin = nextOrigin;
	}

	Path::~Path()
	{
	}

	bool Path::IsEmpty() const
	{
		return m_Path.empty() && IsMarked();
	}

	bool Path::IsMarked() const
	{
		return IsOriginPathMarked(m_Origin);
	}

	bool Path::IsRelative() const
	{
		return m_Path.is_relative();
	}

	bool Path::IsAbsolute() const
	{
		return m_Path.is_absolute();
	}

	bool Path::IsExisting() const
	{
		return std::filesystem::exists(m_Path);
	}

	bool Path::IsDirectory() const
	{
		return std::filesystem::is_directory(m_Path);
	}

	bool Path::IsFile() const
	{
		return m_Path.has_filename();
	}

	bool Path::IsAssetExtension() const
	{
		return AssetType::None != Asset::GetAssetTypeFromFilePath(m_Path);
	}

	uint64_t Path::GetHash() const
	{
		uint64_t hash = robin_hood::hash<std::filesystem::path>{}(m_Path);
		return hash;
	}

	int16_t Path::GetState() const
	{
		std::string pathStr = m_Path.string();
		int16_t state = FindMarkedPathInlineState(pathStr, m_Origin);
		return state;
	}

	Path::Origin Path::GetOrigin() const
	{
		return m_Origin;
	}

	

	const std::filesystem::path& Path::GetPath() const
	{
		return m_Path;
	}

	std::filesystem::path Path::GetAbsolutePath() const
	{
		return m_Path;
	}

	std::filesystem::path Path::GetRelativePath() const
	{
		return GetRelativePathFromAbsolutePath();
	}

	std::filesystem::path Path::GetRelativePathFromOriginBase(Origin origin) const
	{
		std::filesystem::path dstBasePath = GetPathAbsoluteOrigin(origin);
		std::string_view viewEnum = magic_enum::enum_name(origin);
#ifdef RYPATH_LOG_MSG
		RY_CORE_WARN_IF(!dstBasePath.empty(), "Origin: {} reulted in a empty base this shoud lead to a invaild Path!", viewEnum.data());
#else
#endif
		std::filesystem::path realtiveToBase = m_Path.lexically_relative(dstBasePath);
		ConvertUniversalPath(realtiveToBase);
		return realtiveToBase;
	}

	std::filesystem::path Path::GetAbsoulteBasePath() const
	{
		std::filesystem::path basePath = GetPathAbsoluteOrigin(m_Origin);
		
		ConvertUniversalPath(basePath);
		return basePath;
	}

	std::filesystem::path Path::GetNamePath() const
	{
		return m_Path.filename();
	}

	std::filesystem::path Path::GetExtensionPath() const
	{
		return m_Path.extension();
	}

	std::string Path::GetAbsolutePathString() const
	{
		return GetAbsolutePath().string();
	}

	std::string Path::GetRelativePathString() const
	{
		return GetRelativePath().string();
	}

	std::string Path::GetMarkedPathString() const
	{
		std::string relativePathStr = GetRelativePathString();
		int8_t index = GetOriginIndex(m_Origin);
		std::string markedPathStr = PATH_MARKER_STR[index];
		if(!relativePathStr.empty() && '/' != relativePathStr.at(0))
			markedPathStr += "/";
		markedPathStr += relativePathStr;
		return markedPathStr;
	}

	std::string Path::GetRelativePathFromOriginBaseString(Origin origin) const
	{
		return GetRelativePathFromOriginBase(origin).string();
	}

	std::string Path::GetAbsoulteBasePathString() const
	{
		return GetAbsoulteBasePath().string();
	}

	std::string Path::GetNamePathString() const
	{
		return GetNamePath().string();
	}

	std::string Path::GetExtensionPathString() const
	{
		return GetExtensionPath().string();
	}

	AssetType Path::GetAssetFileType() const
	{
		AssetType assetType = Asset::GetAssetTypeFromFilePath(m_Path);
		return AssetType();
	}


	void Path::SetMarker(Origin origne)
	{
		std::string_view viewNext = magic_enum::enum_name(origne);
		std::string_view viewCurent = magic_enum::enum_name(m_Origin);
#ifdef RY_PATH_LOG_MSG
		RY_CORE_WARN("SetMarker overrides the previous origin! {} -> {}", viewCurent, viewNext);
#else
		RY_REMBER_FUNC_CHANGE("Remove LOg State if not need!");
#endif

		if (Origin::Unknown == m_Origin)
		{
			m_Origin = origne;
			return;
		}

		std::filesystem::path baseOrigin = GetPathAbsoluteOrigin(m_Origin);
		std::filesystem::path relativePath = m_Path.lexically_relative(baseOrigin);

		ConvertRealtivePathFromAbsolutePath(relativePath, origne);

		m_Origin = origne;
		std::filesystem::path nextBaseOrigin = GetPathAbsoluteOrigin(m_Origin);
		
#ifdef RY_PATH_LOG_MSG
		RY_CORE_WARN("SetMarker overrides the previous the absolute base path! {} -> {}", baseOrigin, nextBaseOrigin);
#else
		RY_REMBER_FUNC_CHANGE("Remove Log State if not need!");
#endif
		m_Path = (nextBaseOrigin / relativePath);
		
		m_Path = m_Path.lexically_normal();
		ConvertAbsolutePath(m_Path, m_Origin);
		ConvertUniversalPath(m_Path);
	}

	

	std::filesystem::path Path::GetProjectDirectory()
	{
		return Project::GetActiveProjectDirectory();
	}

	std::filesystem::path Path::GetEngineDirectory()
	{
#ifdef RY_PATH_LOG_MSG
		RY_CORE_WARN("Engine directory currently returns GetWorkingDirectory.");
#else
		RY_REMBER_FUNC_CHANGE("Remove LOg State if not need!");
#endif
		return GetWorkingDirectory();
	}

	std::filesystem::path Path::GetWorkingDirectory()
	{
		std::filesystem::path workingDiretory = std::filesystem::current_path();
		ConvertUniversalPath(workingDiretory);
		return workingDiretory;
	}

	

	std::tuple<std::filesystem::path, Path::Origin> Path::ConvertPathNoMarker(const std::string& pathStr, Origin origin)
	{
		int16_t state = State::None;
		std::filesystem::path path;
		if (pathStr.empty())
		{
			RY_CORE_WARN("Path is empty!");
			return std::tuple<std::filesystem::path, Origin>(path, Origin::None);
		}
		Origin originPath = GetPathOriginFromMarkerPath(pathStr);
		std::string pathCopyStr = pathStr;
		

		RY_CORE_WARN_IF(IsOriginPathMarked(originPath) && IsOriginPathMarked(origin), "Multiple marked path: origin is not the default case, currently overriding the previous origin!");
		
		
		switch (originPath)
		{
			case Origin::None:
			{	
				path = pathStr;
#ifdef RY_PATH_LOG_MSG
				RY_CORE_TRACE_IF(!IsOriginPathMarked(origin), "No Marker in Path and origni is valid!");
#else
				RY_REMBER_FUNC_CHANGE("Remove LOg State if not need!");
#endif

				ConvertUniversalPath(path);
				if (path.is_absolute())
				{
					state |= State::Absolute;
					Origin originAbsolute = GetExpectedOriginFromAbsolutePath(path);
					if (IsOriginPathMarked(origin))
					{
						if(origin == originAbsolute)
							break;
#ifdef RY_PATH_LOG_MSG
						RY_CORE_WARN("Absolute path has another origne then origne state say! Absolute path wins!");
#else
						RY_REMBER_FUNC_CHANGE("Remove LOg State if not need!");
#endif
					}
					origin = originAbsolute;
				}
				if (path.is_relative())
				{
					state |= State::Relative;
					Origin originRelative = GetExpectedOriginFromRelativePath(path);
					if (IsOriginPathMarked(origin))
					{
 						RY_CORE_INFO_IF(origin != originRelative, "Relative path origne state as base path! Origne state wins!");
						originRelative = origin;
					}
					origin = originRelative;
					if (Origin::Unknown != origin)
					{
						std::filesystem::path basePath = GetPathAbsoluteOrigin(origin);
						path = (basePath / path).lexically_normal();
					}
				}

				break;
			}
			case Origin::Unknown:
			{
				state |= State::MarkedInline | State::MarkedUnknown;
				path = RemovePathMarker(pathStr, originPath);
				origin = originPath;
				break;
			}
			case Origin::Engine:
			{
				state |= State::MarkedInline | State::MarkedEngine;
				path = RemovePathMarker(pathStr, originPath);

				origin = originPath;
				break;
			}
			case Origin::Project:
			{
				state |= State::MarkedInline | State::MarkedProject;
				path = RemovePathMarker(pathStr, originPath);
				origin = originPath;
				break;
			}
			default:
			{
				std::string_view view = magic_enum::enum_name(originPath);
				RY_CORE_ERROR("Origin-path: {}, path: {}", view, pathCopyStr);
				RY_CORE_ASSERT(false, "Unexpected origin state!");
				return std::tuple<std::filesystem::path, Origin>(path, origin);
			}
		}
#ifdef RY_PATH_LOG_MSG
		LogPathState(state);
#else
		RY_REMBER_FUNC_CHANGE("Remove LOg State if not need!");
#endif
		return std::tuple<std::filesystem::path, Origin>(path, origin);
		
	}

	void Path::ConvertAbsolutePath(std::filesystem::path& path, Origin origin)
	{
		bool isAbsolute = path.is_absolute();
		bool isRelative = path.is_relative();
		RY_CORE_ASSERT(isAbsolute != isRelative, "A path must be either absolute or relative, never both!");
		if (!isAbsolute && isRelative)
		{
			path = GetResolveRelativePathToAbsoluteFromOrigin(path, origin);
			if (Origin::Unknown != origin)
				path = path.lexically_normal();
		}
		
	}

	

	void Path::ConvertInternalPath()
	{
		std::string pathStr = m_Path.string();
		auto [path, origin] = ConvertInternalPath(pathStr, m_Origin);
		m_Path = path;
		m_Origin = origin;
	}

	std::filesystem::path Path::GetResolveMarkerPath(Origin origne) const
	{
		return GetResolveRelativePathToAbsoluteFromOrigin(m_Path, origne);
	}

	std::filesystem::path Path::GetResolveAbsoluteToMarkedPath(Origin origne) const
	{
		return GetResolveAbsoluteToMarkedPath(m_Path, origne);
	}

	std::filesystem::path Path::GetRelativePathFromAbsolutePath() const
	{
		if (Origin::Unknown == m_Origin)
			return m_Path;

		std::filesystem::path baseOrigin = GetPathAbsoluteOrigin(m_Origin);
		std::filesystem::path relativePath = m_Path.lexically_relative(baseOrigin);

		ConvertRealtivePathFromAbsolutePath(relativePath, m_Origin);
		return relativePath;
	}
	

	void Path::LogPathState(int16_t state)
	{
		constexpr const char* betweenStateChar[2] = { " ", "| " };
		constexpr const char* pathNamespaceChar = "Path::";
		constexpr const char* rynexNamespaceChar = "Rynex::";
		
		if (0 == state)
		{
			RY_CORE_WARN("Path State: {}{}{}", pathNamespaceChar, rynexNamespaceChar, "State::None");
			return;
		}
		
		constexpr size_t stateCount = 12;
		
		constexpr size_t pathNamespace = stateCount * 7;
		constexpr size_t rynexNamespace = stateCount * 8;
		constexpr size_t charsBetween = (stateCount - 1) * 3;
		constexpr size_t maxChars = rynexNamespace + pathNamespace + charsBetween;
		

		std::string msg;
		msg.reserve(maxChars);
		
		for (int i = 0; i < stateCount; i++)
		{
			const int value = BIT(i);
			const int bitCheck = value & state;
			if (0 != bitCheck)
			{
				State enumValue = static_cast<State>(value);
				std::string_view view = magic_enum::enum_name(enumValue);
				if (!msg.empty())
				{
					msg += betweenStateChar[1];
				}
				
				msg += view;
				msg += betweenStateChar[0];
			}
		}
		RY_CORE_INFO("Path State: {}", msg);

	}

	int16_t Path::FindMarkedPathInlineState(const std::string& pathStr, Origin origne)
	{
		
		int16_t state = State::None;
		if (pathStr.empty())
		{
			RY_CORE_ERROR("Empty Path!");
			return state;
		}

		Origin originPath = GetPathOriginFromMarkerPath(pathStr);
		std::filesystem::path noInlineMarkedPath;
		switch (originPath)
		{
			case Origin::None:
			{
				noInlineMarkedPath = pathStr;
				break;
			}
			case Origin::Unknown:
			{
				state |= State::MarkedInline | State::MarkedUnknown;
				noInlineMarkedPath = RemovePathMarker(pathStr, originPath);
				origne = originPath;
				break;
			}
			case Origin::Engine:
			{
				state |= State::MarkedInline | State::MarkedEngine;
				noInlineMarkedPath = RemovePathMarker(pathStr, originPath);
				origne = originPath;
				break;
			}
			case Origin::Project:
			{
				state |= State::MarkedInline | State::MarkedProject;
				noInlineMarkedPath = RemovePathMarker(pathStr, originPath);
				origne = originPath;
				break;
			}
			default:
			{
				std::string_view view = magic_enum::enum_name(originPath);
				RY_CORE_ERROR("Origin-path: {}, path: {}", view, pathStr);
				RY_CORE_ASSERT(false, "Unexpected origin state!");
				return State::None;
			}
		}
		
		state |= FindFilesystemState(noInlineMarkedPath, origne);


		return state;
	}

	int16_t Path::FindFilesystemState(const std::filesystem::path& noInlineMarkedPath, Origin origne)
	{
		int16_t state = State::None;
		std::filesystem::path absolutePath;
		if (noInlineMarkedPath.is_absolute()) 
		{
			absolutePath = noInlineMarkedPath;
			state |= State::Absolute;
		}
		if (noInlineMarkedPath.is_relative()) 
		{
			std::string noInlineMarkedPathStr = noInlineMarkedPath.string();
			absolutePath = GetResolveRelativePathToAbsoluteFromOrigin(noInlineMarkedPathStr, origne);
			state |= State::Relative;
		}
		if (std::filesystem::exists(absolutePath)) 
		{
			state |= State::Existing;
		}
		if (std::filesystem::is_directory(absolutePath))
		{
			state |= State::Directory;
		}
		else 
		{
			state |= State::File;
			if (absolutePath.has_extension()) 
			{
				state |= State::Extension;
				AssetType assetType = Asset::GetAssetTypeFromFilePath(absolutePath);
				state |= assetType != AssetType::None ? State::AssetFileExtension : State::None;
			}
		}

		return state;
	}


	std::string Path::GetResolveRelativePathToAbsoluteFromOrigin(const std::filesystem::path& path, Origin origne)
	{
		std::string absoluteMarkerPath = GetPathAbsoluteOrigin(origne).string();
		std::string pathStr = path.string();
		std::string resolvePath = absoluteMarkerPath + "/" + pathStr;
		
		return resolvePath;
	}

	std::string Path::GetResolveAbsoluteToMarkedPath(const std::filesystem::path& path, Origin origne)
	{
		std::filesystem::path absolutePath = GetPathAbsoluteOrigin(origne);
		std::filesystem::path relativePath = std::filesystem::relative(path, absolutePath);
		if (Origin::Engine == origne) 
		{
			std::filesystem::path relativeExpectedPathBegin = std::filesystem::path(RY_PATH_EXPECT_ENGINE_RELATIVE_START_STR);
			relativePath = relativeExpectedPathBegin / relativePath;
		}
		int8_t index = GetOriginIndex(origne);
		std::string_view markerView(PATH_MARKER_STR[index]);
		size_t markerCharCount = markerView.size();
		relativePath = relativePath.generic_string();

		using PathValueType = std::filesystem::path::value_type;
		const PathValueType* pathValueTypePtr = relativePath.c_str();
		size_t pathCharCount = GetPathChartersCount(pathValueTypePtr);
		
		
		size_t count = markerCharCount + pathCharCount + 1;
		std::string markedRelativePathStr;
		markedRelativePathStr.reserve(count);
		markedRelativePathStr += markerView.data();
		markedRelativePathStr += "/";
		markedRelativePathStr += relativePath.string();
		
		return markedRelativePathStr;
	}

	

	
	size_t Path::GetPathChartersCount(const char* pathPtr)
	{
		std::string_view view(pathPtr);
		size_t count = view.size();
		return count;
	}

	
	size_t Path::GetPathChartersCount(const wchar_t* pathPtr)
	{
		std::wstring_view view(pathPtr);
		size_t count = view.size();
		return count;
	}


	uint32_t Path::GetMarkerCharacterCount(Origin origne)
	{
		int8_t originIndex = GetOriginIndex(origne);
		uint32_t count = GetMarkerLength(PATH_MARKER_STR[originIndex]);
		return count;
	}

	bool Path::IsPathMarked(const std::string& markedPathStr)
	{
		Origin origne = GetMarkedPathOrigin(markedPathStr);
		return IsOriginPathMarked(origne);
	}

	bool Path::IsOriginPathMarked(Origin origne)
	{
		return Origin::None != origne;
	}

	std::filesystem::path Path::RemovePathMarker(const std::string& markedPathStr)
	{
		Origin origne = GetMarkedPathOrigin(markedPathStr);
		return RemovePathMarker(markedPathStr, origne);
	}

	std::filesystem::path Path::RemovePathMarker(const std::string& markedPathStr, Origin origne)
	{
		uint32_t markerCount = GetMarkerCharacterCount(origne);
		markerCount += 1u;
		
		std::string pathStr = markedPathStr.substr(markerCount);
		std::filesystem::path path = pathStr;
		return path;
	}

	std::filesystem::path Path::GetRelativePath(const std::string& markedPathStr)
	{
		Origin origne = GetMarkedPathOrigin(markedPathStr);
		RY_CORE_ASSERT(IsOriginPathMarked(origne), "File path is not marked!");
		std::filesystem::path relativePath = RemovePathMarker(markedPathStr, origne);
		return relativePath;
	}

	std::filesystem::path Path::GetRelativePath(const std::string& pathStr, const std::filesystem::path& pathBase)
	{
		if (IsPathMarked(pathStr))
		{
			return GetRelativePath(pathStr);
		}
		std::filesystem::path relativePath = pathStr;
		if (relativePath.is_relative())
		{
			return relativePath;
		}
		relativePath = std::filesystem::relative(relativePath, pathBase);
		return relativePath;
	}

	std::filesystem::path Path::GetPathAbsoluteOrigin(Origin origin)
	{
		switch (origin)
		{
			case Path::Origin::None:	return std::filesystem::path("");
			case Path::Origin::Unknown:	return std::filesystem::path("");
			case Path::Origin::Engine:	
			{
				std::filesystem::path enginePath = GetEngineDirectory();
				ConvertUniversalPath(enginePath);
				return enginePath;
			}
			case Path::Origin::Project:
			{
				std::filesystem::path projectPath = GetProjectDirectory();
				ConvertUniversalPath(projectPath);
				return projectPath;
			}		
			default:
				RY_CORE_ASSERT(false, "Undefined origin!");
		}
		return std::filesystem::path("");
	}

	Path::Origin Path::GetMarkedPathOrigin(const std::string& markedPath)
	{
		Origin origne = GetPathOriginFromMarkerPath(markedPath);
		if (IsOriginPathMarked(origne))
		{
			return origne;
		}
		RY_CORE_ASSERT(false, "Not implemented: marker not set!!");
		return Origin::None;
	}

	Path::Origin Path::GetPathOriginFromMarkerPath(const std::string& markedPath)
	{
		if(Project::HasStringInPath(markedPath, RY_PATH_ENGINE_MARKER_STR))
			return Origin::Engine;

		if (Project::HasStringInPath(markedPath, RY_PATH_PROJECT_MARKER_STR))
			return Origin::Project;

		if (Project::HasStringInPath(markedPath, RY_PATH_NO_VALID_MARKER_STR))
			return Origin::Unknown;

		Origin origne = Origin::None;

#ifdef RY_PATH_LOG_MSG
		RY_CORE_TRACE("No valid marker found! Origin::None");
#else
		RY_REMBER_FUNC_CHANGE("Remove LOg State if not need!");
#endif
		return origne;
	}

	Path::Origin Path::GetExpectedOriginFromRelativePath(const std::filesystem::path& path)
	{
		int8_t i = 0;
		
		if (Project::HasStringInPath(path, RY_PATH_EXPECT_ENGINE_RELATIVE_START_FOlDER_STR))
			return Origin::Engine;

		if(Project::HasStringInPath(path, ".."))
		{
			RY_CORE_WARN("Engine root path not found and path contains parent dots - assuming Origin::Unknown");
			return Origin::Unknown;
		}

		
		Origin origne = Origin::Project;
		return origne;
	}

	Path::Origin Path::GetExpectedOriginFromAbsolutePath(const std::filesystem::path& path)
	{
		std::filesystem::path engineDirectory = GetEngineDirectory();
		engineDirectory = engineDirectory.parent_path();
		std::string engineDirectoryStr = engineDirectory.string();

		if (Project::HasStringInPath(path, engineDirectoryStr))
			return Origin::Engine;

		std::filesystem::path projectDirector = GetProjectDirectory();
		std::string projectDirectoryStr = projectDirector.string();

		if (Project::HasStringInPath(path, projectDirectoryStr))
			return Origin::Project;

#ifdef RY_PATH_LOG_MSG
		RY_CORE_TRACE("No valid marker found! Origin::Unknown");
#else
		RY_REMBER_FUNC_CHANGE("Remove Log State if not need!");
#endif
		Origin origne = Origin::Unknown;
		return origne;
	}

	void Path::ConvertUniversalPath(std::filesystem::path& path)
	{
		std::filesystem::path normalizedPath = path;
		std::string genericPathStr = normalizedPath.generic_string();
		path = genericPathStr;
	}

	void Path::ConvertRealtivePathFromAbsolutePath(std::filesystem::path& relativePath, Origin origin)
	{
		switch (origin)
		{
		case Origin::None:
		case Origin::Unknown:
			break;
		case Origin::Engine:
		{
			std::filesystem::path expextedEndineRealtiveStart = RY_PATH_EXPECT_ENGINE_RELATIVE_START_STR;
			std::string nameFolder = expextedEndineRealtiveStart.filename().string();
			if (!Project::HasStringInPath(relativePath, nameFolder))
				relativePath = nameFolder / relativePath;

			expextedEndineRealtiveStart = expextedEndineRealtiveStart.parent_path();
			expextedEndineRealtiveStart = expextedEndineRealtiveStart.generic_string();
			nameFolder = expextedEndineRealtiveStart.filename().string();

			if (!Project::HasStringInPath(relativePath, nameFolder))
				relativePath = expextedEndineRealtiveStart / relativePath;
		}
		case Origin::Project:
		{
			ConvertUniversalPath(relativePath);
			relativePath.lexically_normal();
			break;
		}
		default:
			RY_CORE_ASSERT(false, "Not Valid Origin state!");
			break;
		}
	}

	std::tuple<std::filesystem::path, Path::Origin> Path::ConvertInternalPath(const std::string& pathStr, Origin origin)
	{
		auto[path, nextOrigin] = ConvertPathNoMarker(pathStr, origin);
		ConvertAbsolutePath(path, nextOrigin);
		ConvertUniversalPath(path);
		return std::tuple<std::filesystem::path, Origin>(path, nextOrigin);
	}



}