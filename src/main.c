#include <zephyr.h>

#include <stdio.h>

#include "app_fs.h"
#include "app_lua.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

struct emblua
{
	const char *start;
	const char *end;
	const char *name;
};

#define _EMBLUA_START_SYM(_name) \
	_binary_ ## _name ## _lua_start

#define _EMBLUA_START_END(_name) \
	_binary_ ##_name ## _lua_end

#define DECLARE_EMB_LUA_SCRIPT(_name) \
	extern uint32_t _EMBLUA_START_SYM(_name); \
	extern uint32_t _EMBLUA_START_END(_name); \
	STRUCT_SECTION_ITERABLE(emblua, _name) = { \
		.start = (const char *) &_EMBLUA_START_SYM(_name), \
		.end = (const char *) &_EMBLUA_START_END(_name), \
		.name = STRINGIFY(_name) STRINGIFY(.lua) \
	}

/* find a way to somehow automatically generate this list */
DECLARE_EMB_LUA_SCRIPT(helloworld);
DECLARE_EMB_LUA_SCRIPT(script1);
DECLARE_EMB_LUA_SCRIPT(math);

int main(void)
{
	int rc = 0;
	char fname[0x100];

	fs_init();

	/* iterate over all lua scripts and add them to the filesystem */
	STRUCT_SECTION_FOREACH(emblua, script) {
		const char * loc = script->start;
		const size_t size = script->end - script->start;
		LOG_INF("Adding file %s to FS, at 0x%x [ %u B ]",
			script->name, (uint32_t)loc, size);

		snprintf(fname, sizeof(fname), "%s/%s", mp->mnt_point, script->name);
		
		rc = fs_file_add(fname, loc, size);
		if (rc < 0) {
			LOG_ERR("Error adding file %s [%d]", fname, rc);
			return rc;
		}

		LOG_INF("Script %s written -> %d", fname, rc);
	}

	fs_stats();

	// app_lua_string_test();

	FILE *file = fopen ("/lfs/helloworld.lua", "r");
	__ASSERT(file != NULL, "LUA script not found");
	if (file != NULL) {
		fclose(file);
	}

	rc = app_lua_execute_script("/lfs/helloworld.lua");
	LOG_INF("Executed script rc: %d", rc);

	rc = app_lua_execute_script("/lfs/script1.lua");
	LOG_INF("Executed script rc: %d", rc);

	rc = app_lua_execute_script("/lfs/math.lua");
	LOG_INF("Executed script rc: %d", rc);

	rc = app_lua_execute_script("/lfs/notexistingscript.lua");
	LOG_INF("Executed script rc: %d", rc);

	LOG_INF("App done, rc = %d", rc);

	return rc;
}