#include <switch.h>

SWITCH_MODULE_LOAD_FUNCTION(mod_sample_load);
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_sample_shutdown);
SWITCH_MODULE_RUNTIME_FUNCTION(mod_sample_runtime);

SWITCH_MODULE_DEFINITION(mod_sample, mod_sample_load, mod_sample_shutdown, mod_sample_runtime);

SWITCH_MODULE_LOAD_FUNCTION(mod_sample_load)
{
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