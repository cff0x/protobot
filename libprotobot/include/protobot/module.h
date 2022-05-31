#ifndef PROTOBOT_MODULE_H
#define PROTOBOT_MODULE_H

#include <protobot/protobot.h>
#include <protobot/client.h>
#include <protobot/events.h>

namespace protobot {
using namespace protobot;

class bot_module;
class bot_module_manager;

/**
 * This #define allows us to call a method in all loaded modules in a readable simple way, e.g.:
 * 'FOREACH_MOD(I_OnGuildAdd,OnGuildAdd(guildinfo));'
 * NOTE: Locks mutex - two FOREACH_MOD() can't run asyncronously in two different threads. This is
 * to prevent one thread loading/unloading a module, changing the arrays/vectors while another thread
 * is running an event.
 */
#define FOREACH_MODULE(y,x) { \
	std::vector<bot_module*> list_to_call; \
	{ \
		std::lock_guard l(m_module_manager->get_mutex()); \
		list_to_call = m_module_manager->get_event_handlers()[y]; \
	} \
	for (auto _i = list_to_call.begin(); _i != list_to_call.end(); ++_i) \
	{ \
		try \
		{ \
			if (!(*_i)->x) { \
				break; \
			} \
		} \
		catch (std::exception& modexcept) \
		{ \
			get_core()->log(dpp::ll_error, fmt::format("Exception caught in module: {}", modexcept.what())); \
		} \
	} \
};

struct module_command {
    std::string name;
    std::string description;
    uint64_t permissions;
    std::vector<dpp::command_option> options;

};

// entry point for a module
typedef bot_module* (module_entry_point)(client*, bot_module_manager*);

// map containing modules with their corresponding name
typedef std::map<std::string, bot_module*> module_map;

// structure containing information regarding the native module binary object
struct module_native {
    module_entry_point* entry_point;
    std::string file_name;
    void* library_handle;
    const char* error;
    bot_module* module_object;
};

// this class is used to manage the modules (load/unload/etc.)
class bot_module_manager {
public:
    // creates a new module manager instance
    bot_module_manager(client* bot);

    virtual ~bot_module_manager();

    // attaches event handlers to a module
    void attach(const std::vector<bot_module_event> &events, bot_module* module);

    // detaches event handlers from a module
    void detach(const std::vector<bot_module_event> &events, bot_module* module);

    // loads a module
    bool load(const std::string &file_name);

    // unloads a module
    bool unload(const std::string &file_name);

    // unloads and then loads a module
    bool reload(const std::string &file_name);

    // loads all modules from the modules section inside the bot config
    void load_all();

    void register_module_commands();

    std::mutex& get_mutex();

    const std::vector<bot_module*>* get_event_handlers();

    // retrieves a list of all loaded modules
    const module_map* get_module_list();

    const std::map<std::string, std::vector<module_command>>* get_module_commands();
    bot_module* get_command_module(std::string command_name);

    // retrieves the last error message
    const std::string& get_last_error();

private:
    // points to the bot instance which this module manager belongs to
    client* m_bot;

    // maps module names to their respective native info
    std::map<std::string, module_native> m_native_modules;

    // retrieves a named symbol from a shared object file
    bool get_symbol(module_native &native, const char* symbol_name);

    // list containing all loaded modules
    module_map m_module_list;

    // maps a module object to a module name
    std::map<bot_module*, std::string> m_module_names;

    // maps a module name to a list of module commands
    std::map<std::string, std::vector<module_command>> m_module_commands;

    // maps a command name to a module object
    std::map<std::string, bot_module*> m_command_name_to_module;

    // last error message
    std::string m_last_error;

    // thread mutex
    std::mutex m_mutex;

    // vector holding info regarding the attached events
    std::vector<bot_module*> m_event_handlers[M_END];
};


// representation of a module
class bot_module {
protected:
    // points to the bot instance which this module belongs to
    client* m_bot;

    // manager for this module
    bot_module_manager* m_module_manager;
public:
    // used to create a new module instance
    bot_module(client* bot, bot_module_manager* manager);

    virtual ~bot_module();

    // functions to retrieve the module version and description values
    virtual std::string get_version();
    virtual std::string get_description();

    // returns a list of commands belonging to the module
    virtual std::vector<module_command> get_commands();

    // the following code between the two BOT_MODULE_EVENT_DEF tags is automatically generated
    // ! PLEASE DO NOT EDIT !
    //
    // <BOT_MODULE_EVENT_DEF>
    //
    // GENERATED AT: 06/01/2022 01:14:31
    //
    virtual bool on_voice_state_update(const dpp::voice_state_update_t &event);
    virtual bool on_voice_client_disconnect(const dpp::voice_client_disconnect_t &event);
    virtual bool on_voice_client_speaking(const dpp::voice_client_speaking_t &event);
    virtual bool on_guild_join_request_delete(const dpp::guild_join_request_delete_t &event);
    virtual bool on_interaction_create(const dpp::interaction_create_t &event);
    virtual bool on_slashcommand(const dpp::slashcommand_t &event);
    virtual bool on_button_click(const dpp::button_click_t &event);
    virtual bool on_autocomplete(const dpp::autocomplete_t &event);
    virtual bool on_select_click(const dpp::select_click_t &event);
    virtual bool on_message_context_menu(const dpp::message_context_menu_t &event);
    virtual bool on_user_context_menu(const dpp::user_context_menu_t &event);
    virtual bool on_form_submit(const dpp::form_submit_t &event);
    virtual bool on_guild_delete(const dpp::guild_delete_t &event);
    virtual bool on_channel_delete(const dpp::channel_delete_t &event);
    virtual bool on_channel_update(const dpp::channel_update_t &event);
    virtual bool on_ready(const dpp::ready_t &event);
    virtual bool on_message_delete(const dpp::message_delete_t &event);
    virtual bool on_guild_member_remove(const dpp::guild_member_remove_t &event);
    virtual bool on_resumed(const dpp::resumed_t &event);
    virtual bool on_guild_role_create(const dpp::guild_role_create_t &event);
    virtual bool on_typing_start(const dpp::typing_start_t &event);
    virtual bool on_message_reaction_add(const dpp::message_reaction_add_t &event);
    virtual bool on_guild_members_chunk(const dpp::guild_members_chunk_t &event);
    virtual bool on_message_reaction_remove(const dpp::message_reaction_remove_t &event);
    virtual bool on_guild_create(const dpp::guild_create_t &event);
    virtual bool on_channel_create(const dpp::channel_create_t &event);
    virtual bool on_message_reaction_remove_emoji(const dpp::message_reaction_remove_emoji_t &event);
    virtual bool on_message_delete_bulk(const dpp::message_delete_bulk_t &event);
    virtual bool on_guild_role_update(const dpp::guild_role_update_t &event);
    virtual bool on_guild_role_delete(const dpp::guild_role_delete_t &event);
    virtual bool on_channel_pins_update(const dpp::channel_pins_update_t &event);
    virtual bool on_message_reaction_remove_all(const dpp::message_reaction_remove_all_t &event);
    virtual bool on_voice_server_update(const dpp::voice_server_update_t &event);
    virtual bool on_guild_emojis_update(const dpp::guild_emojis_update_t &event);
    virtual bool on_guild_stickers_update(const dpp::guild_stickers_update_t &event);
    virtual bool on_presence_update(const dpp::presence_update_t &event);
    virtual bool on_webhooks_update(const dpp::webhooks_update_t &event);
    virtual bool on_guild_member_add(const dpp::guild_member_add_t &event);
    virtual bool on_invite_delete(const dpp::invite_delete_t &event);
    virtual bool on_guild_update(const dpp::guild_update_t &event);
    virtual bool on_guild_integrations_update(const dpp::guild_integrations_update_t &event);
    virtual bool on_guild_member_update(const dpp::guild_member_update_t &event);
    virtual bool on_invite_create(const dpp::invite_create_t &event);
    virtual bool on_message_update(const dpp::message_update_t &event);
    virtual bool on_user_update(const dpp::user_update_t &event);
    virtual bool on_message_create(const dpp::message_create_t &event);
    virtual bool on_guild_ban_add(const dpp::guild_ban_add_t &event);
    virtual bool on_guild_ban_remove(const dpp::guild_ban_remove_t &event);
    virtual bool on_integration_create(const dpp::integration_create_t &event);
    virtual bool on_integration_update(const dpp::integration_update_t &event);
    virtual bool on_integration_delete(const dpp::integration_delete_t &event);
    virtual bool on_thread_create(const dpp::thread_create_t &event);
    virtual bool on_thread_update(const dpp::thread_update_t &event);
    virtual bool on_thread_delete(const dpp::thread_delete_t &event);
    virtual bool on_thread_list_sync(const dpp::thread_list_sync_t &event);
    virtual bool on_thread_member_update(const dpp::thread_member_update_t &event);
    virtual bool on_thread_members_update(const dpp::thread_members_update_t &event);
    virtual bool on_guild_scheduled_event_create(const dpp::guild_scheduled_event_create_t &event);
    virtual bool on_guild_scheduled_event_update(const dpp::guild_scheduled_event_update_t &event);
    virtual bool on_guild_scheduled_event_delete(const dpp::guild_scheduled_event_delete_t &event);
    virtual bool on_guild_scheduled_event_user_add(const dpp::guild_scheduled_event_user_add_t &event);
    virtual bool on_guild_scheduled_event_user_remove(const dpp::guild_scheduled_event_user_remove_t &event);
    virtual bool on_voice_buffer_send(const dpp::voice_buffer_send_t &event);
    virtual bool on_voice_user_talking(const dpp::voice_user_talking_t &event);
    virtual bool on_voice_ready(const dpp::voice_ready_t &event);
    virtual bool on_voice_receive(const dpp::voice_receive_t &event);
    virtual bool on_voice_receive_combined(const dpp::voice_receive_t &event);
    virtual bool on_voice_track_marker(const dpp::voice_track_marker_t &event);
    virtual bool on_stage_instance_create(const dpp::stage_instance_create_t &event);
    virtual bool on_stage_instance_update(const dpp::stage_instance_update_t &event);
    virtual bool on_stage_instance_delete(const dpp::stage_instance_delete_t &event);
    // </BOT_MODULE_EVENT_DEF>
};

}

// used to define a module's entry point
#define MODULE_ENTRY(module_class_name) extern "C" protobot::bot_module* init_module(protobot::client* bot, protobot::bot_module_manager* manager) {          \
    return new module_class_name(bot, manager);                                                                    \
}

#endif //PROTOBOT_MODULE_H
