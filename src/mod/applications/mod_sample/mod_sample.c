#include <switch.h>

SWITCH_MODULE_LOAD_FUNCTION(mod_sample_load);
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_sample_shutdown);
SWITCH_MODULE_RUNTIME_FUNCTION(mod_sample_runtime);

static struct{
	int debug;
	int32_t threads;
	int32_t running;
	switch_mutex_t *mutex;
	switch_memory_pool_t *pool;
} globals;

SWITCH_MODULE_DEFINITION(mod_sample, mod_sample_load, mod_sample_shutdown, NULL);

SWITCH_STANDARD_APP(sample_app)
{
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ALERT, "Sample application has been loaded.");
	return;
}

SWITCH_STANDARD_API(sample_api)
{
	return SWITCH_STATUS_SUCCESS;
}
SWITCH_MODULE_LOAD_FUNCTION(mod_sample_load)
{
	switch_application_interface_t *app_interface;
	switch_api_interface_t *api_interface;
	
	memset(&globals, 0, sizeof(globals));
	globals.pool = pool;

	globals.running = 1;

	*module_interface = switch_loadable_module_create_module_interface(pool, modname);

	SWITCH_ADD_APP(app_interface, "sample", "Sample module loaded.", NULL, sample_app, NULL, SAF_NONE);
	SWITCH_ADD_API(api_interface, "sample", "Sample of module", sample_api, SAF_NONE);
	
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Sample module loaded successfully!!");
	
	return SWITCH_STATUS_SUCCESS;
}

SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_sample_shutdown)
{
	return SWITCH_STATUS_SUCCESS;
}

SWITCH_MODULE_RUNTIME_FUNCTION(mod_sample_runtime)
{
	return SWITCH_STATUS_SUCCESS;
}