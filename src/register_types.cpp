#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/classes/engine.hpp>

#include "my_node.hpp"
#include "my_singleton.hpp"

// #include <windows.h>
// #include <stdio.h>

static MySingleton *_my_singleton;


// void add_additional_dll_directories() {
// 	// Функция, расширяющая список каталогов поиска DLL
// 	#ifdef _WIN32
// 		// SetDllDirectoryA("C:\\gstreamer\\1.0\msvc_x86_64\\bin");
// 	// Устанавливаем безопасный режим поиска DLL.
// 	// Это гарантирует, что Windows будет искать библиотеки только в строго заданных каталогах.
// 	if (!SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_USER_DIRS)) {
// 		DWORD error = GetLastError();
// 		// Обработка ошибки (например, можно вывести сообщение в журнал)
// 		fprintf(stderr, "SetDefaultDllDirectories failed (error %lu)\n", error);
// 		return;
// 	}

// 	// Добавляем путь к каталогу с DLL GStreamer.
// 	// Укажите полный путь к папке BIN, где располагаются DLL-библиотеки GStreamer.
// 	DLL_DIRECTORY_COOKIE cookie = AddDllDirectory(L"C:\\gstreamer\\1.0\\msvc_x86_64\\bin");
// 	AddDllDirectory(L"C:\\Windows\\System32");
// 	if (cookie == NULL) {
// 		DWORD error = GetLastError();
// 		fprintf(stderr, "AddDllDirectory failed (error %lu)\n", error);
// 	}
// 	#endif	
// }


void gdextension_initialize(ModuleInitializationLevel p_level)
{
	// add_additional_dll_directories();
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		ClassDB::register_class<MyNode>();
		ClassDB::register_class<MySingleton>();

		_my_singleton = memnew(MySingleton);
		Engine::get_singleton()->register_singleton("MySingleton", MySingleton::get_singleton());
	}
}

void gdextension_terminate(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		Engine::get_singleton()->unregister_singleton("MySingleton");
		memdelete(_my_singleton);
	}
}

extern "C"
{
	GDExtensionBool GDE_EXPORT gdextension_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

		init_obj.register_initializer(gdextension_initialize);
		init_obj.register_terminator(gdextension_terminate);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}
