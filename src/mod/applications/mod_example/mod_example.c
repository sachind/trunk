#include <switch.h>

/* Prototypes */
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_example_shutdown);
SWITCH_MODULE_RUNTIME_FUNCTION(mod_example_runtime);
SWITCH_MODULE_LOAD_FUNCTION(mod_example_load);

SWITCH_MODULE_DEFINITION(mod_example, mod_example_load, mod_example_shutdown, NULL);

static struct {
	int debug;
	int32_t threads;
	int32_t running;
	switch_mutex_t *mutex;
	switch_memory_pool_t *pool;
} globals;

/*static void *SWITCH_THREAD_FUNC outbound_agent_thread_run(switch_thread_t *thread, void *obj)
{
	switch_mutex_lock(globals.mutex);
	globals.threads++;
	switch_mutex_unlock(globals.mutex);

	while(globals.running) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Outbound Agent Thread Running !");
		switch_yield(100000);
	}

	switch_core_destroy_memory_pool(&globals.pool);

	switch_mutex_lock(globals.mutex);
	globals.threads--;
	switch_mutex_unlock(globals.mutex);

	return NULL;
}*/

static int AGENT_DISPATCH_THREAD_RUNNING = 0;
static int AGENT_DISPATCH_THREAD_STARTED = 0;

void *SWITCH_THREAD_FUNC cc_agent_dispatch_thread_run(switch_thread_t *thread, void *obj)
{
	int done = 0;

	switch_mutex_lock(globals.mutex);
	if (!AGENT_DISPATCH_THREAD_RUNNING) {
		AGENT_DISPATCH_THREAD_RUNNING++;
		globals.threads++;
	} else {
		done = 1;
	}
	switch_mutex_unlock(globals.mutex);

	if (done) {
		return NULL;
	}

	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "AGENT_DISPATCH_THREAD_RUN");
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CONSOLE, "Agent Dispatch Thread Started\n");

	while (globals.running == 1) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "AGENT_DISPATCH_THREAD_LOOP_RUNNING");
		switch_yield(100000);
	}

	switch_mutex_lock(globals.mutex);
	globals.threads--;
	AGENT_DISPATCH_THREAD_RUNNING = AGENT_DISPATCH_THREAD_STARTED = 0;
	switch_mutex_unlock(globals.mutex);

	return NULL;
}

void cc_agent_dispatch_thread_start(void)
{
	switch_thread_t *thread;
	switch_threadattr_t *thd_attr = NULL;
	int done = 0;

	switch_mutex_lock(globals.mutex);

	if (!AGENT_DISPATCH_THREAD_STARTED) {
		AGENT_DISPATCH_THREAD_STARTED++;
	} else {
		done = 1;
	}
	switch_mutex_unlock(globals.mutex);

	if (done) {
		return;
	}

	switch_threadattr_create(&thd_attr, globals.pool);
	switch_threadattr_detach_set(thd_attr, 1);
	switch_threadattr_stacksize_set(thd_attr, SWITCH_THREAD_STACKSIZE);
	switch_threadattr_priority_set(thd_attr, SWITCH_PRI_REALTIME);
	switch_thread_create(&thread, thd_attr, cc_agent_dispatch_thread_run, NULL, globals.pool);
}

void *SWITCH_THREAD_FUNC cc_member_thread_run(switch_thread_t *thread, void *obj)
{
	switch_mutex_lock(globals.mutex);
	globals.threads++;
	switch_mutex_unlock(globals.mutex);

	while(globals.running) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ALERT, "Member Thread Running...!");
		switch_yield(500000);
	}

	switch_core_destroy_memory_pool(&globals.pool);

	switch_mutex_lock(globals.mutex);
	globals.threads--;
	switch_mutex_unlock(globals.mutex);

	return NULL;
}

SWITCH_STANDARD_APP(example_app)
{
	switch_thread_t *thread;
	switch_threadattr_t *thd_attr = NULL;
	switch_memory_pool_t *pool;

	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "CALLCENTER_APP");

	switch_core_new_memory_pool(&pool);
	switch_threadattr_create(&thd_attr, pool);
	switch_threadattr_detach_set(thd_attr, 1);
	switch_threadattr_stacksize_set(thd_attr, SWITCH_THREAD_STACKSIZE);
	switch_thread_create(&thread, thd_attr, cc_member_thread_run, NULL, pool);
	return;
}

/* Macro expands to: switch_status_t mod_example_load(switch_loadable_module_interface_t **module_interface, switch_memory_pool_t *pool) */
SWITCH_MODULE_LOAD_FUNCTION(mod_example_load)
{
	switch_application_interface_t *app_interface;
//	switch_api_interface_t *api_interface;

	/** Sachin Comments */
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "MODULE_LOADED_SUCCESSFULLY" );

	memset(&globals, 0, sizeof(globals));
	globals.pool = pool;

	switch_mutex_init(&globals.mutex, SWITCH_MUTEX_NESTED, globals.pool);
	switch_mutex_lock(globals.mutex);
	globals.running = 1;
	switch_mutex_unlock(globals.mutex);

	/* connect my internal structure to the blank pointer passed to me */
	*module_interface = switch_loadable_module_create_module_interface(pool, modname);

	if (!AGENT_DISPATCH_THREAD_STARTED) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "AGENT_DISPATCH_THREAD_STARTED");
		cc_agent_dispatch_thread_start();
	}

	SWITCH_ADD_APP(app_interface, "example", "Notification Module.", NULL, example_app, NULL, SAF_NONE);

	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "LOAD_ALL_APPLICATION_AND_API_CALLS.");

	/* indicate that the module should continue to be loaded */
	return SWITCH_STATUS_SUCCESS;
}

/*
   Called when the system shuts down
   Macro expands to: switch_status_t mod_example_shutdown() */
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_example_shutdown)
{
	int sanity = 0;

	switch_mutex_lock(globals.mutex);
	if (globals.running == 1) {
		globals.running = 0;
	}
	switch_mutex_unlock(globals.mutex);

	while (globals.threads) {
		switch_cond_next();
		if (++sanity >= 60000) {
			break;
		}
	}

	return SWITCH_STATUS_SUCCESS;
}