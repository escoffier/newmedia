#pragma once
#include <string>
#ifdef WINDOWS
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

class SDKPlugins
{
private:
	void* plxhandle;
public:
	SDKPlugins()
	{
		plxhandle = nullptr;
	}
	bool Load(const char* plxpath, const char* libname)
	{
		plxhandle = 0;
		// First we need to add ./ and end with .so.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
		size_t len = strlen(libname);
		len += 6; // Add room for ./, .so, and null.
		char path[260] = { 0 };
		snprintf(path, 259, "%s%s", plxpath, libname);
#ifdef WINDOWS
		strncat(path, ".dll", 5);
		plxhandle = LoadLibraryA(path);
#else
		strcat(path, ".so");
		plxhandle = dlopen(path, RTLD_LAZY);
#endif
		return plxhandle == nullptr ? false : true;
	}
	bool UnLoad()
	{
		if (plxhandle)
		{
#ifdef WINDOWS
			return FreeLibrary((HINSTANCE)plxhandle);
#else
			return dlclose(plxhandle);
#endif
		}
		return true;
	}
	~SDKPlugins()
	{
		if (plxhandle)
		{
#ifdef WINDOWS
			FreeLibrary((HINSTANCE)plxhandle);
#else
			dlclose(plxhandle);
#endif
		}
	}

	void* GetFuncAddr( const char* funcName)
	{
		void* func = 0;
		if (plxhandle) {
#ifdef WINDOWS
			func = GetProcAddress((HINSTANCE)plxhandle, funcName);
#else
			func = dlsym(plxhandle, funcName);
#endif
		}
		return func;

	}
};