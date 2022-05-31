#include <protobot/module.h>
#ifndef _WIN32
#include <dlfcn.h>
#else
#include <Windows.h>
#define PATH_MAX MAX_PATH
#endif
#include <utility>
#include <vector>



bot_module_manager::bot_module_manager(client* bot) :
        m_bot(bot) {
    m_bot->get_core()->log(dpp::ll_info, "Initializing module loader...");
}

bot_module_manager::~bot_module_manager() {

}

void bot_module_manager::attach(const std::vector<bot_module_event> &events, bot_module* module) {
    for (auto n = events.begin(); n != events.end(); ++n) {
        if (std::find(m_event_handlers[*n].begin(), m_event_handlers[*n].end(), module) == m_event_handlers[*n].end()) {
            m_event_handlers[*n].push_back(module);
            m_bot->log(dpp::ll_debug, fmt::format("Module \"{}\" attached event \"{}\"", module->get_description(), BOT_MODULE_EVENT_NAMES[*n]));
        } else {
            m_bot->log(dpp::ll_warning, fmt::format("Module \"{}\" is already attached to event \"{}\"",
                                                    module->get_description(), BOT_MODULE_EVENT_NAMES[*n]));
        }
    }
}

void bot_module_manager::detach(const std::vector<bot_module_event> &events, bot_module* module) {
    for (auto n = events.begin(); n != events.end(); ++n) {
        auto it = std::find(m_event_handlers[*n].begin(), m_event_handlers[*n].end(), module);
        if (it != m_event_handlers[*n].end()) {
            m_event_handlers[*n].erase(it);
            m_bot->log(dpp::ll_debug, fmt::format("Module \"{}\" detached event \"{}\"", module->get_description(), BOT_MODULE_EVENT_NAMES[*n]));
        }
    }
}

bool bot_module_manager::load(const std::string &file_name) {
    module_native m;

    m.error = nullptr;
    m.library_handle = nullptr;
    m.module_object = nullptr;
    m.entry_point = nullptr;
    m.file_name = std::string(file_name);

    m_bot->log(dpp::ll_info, fmt::format("Loading bot module: \"{}\"", file_name));

    std::lock_guard l(m_mutex);

    if (m_native_modules.find(file_name) == m_native_modules.end()) {

        char buffer[PATH_MAX + 1];
        getcwd(buffer, PATH_MAX);
        std::filesystem::path currentDir{std::string(buffer)};
        std::filesystem::path moduleFile{fmt::format("{}.protomod", file_name)};
        std::filesystem::path fullModulePath =
                currentDir / m_bot->get_config()->get_modules_path() / moduleFile;

        m_bot->log(dpp::ll_debug, fmt::format("Bot module file path: {}", fullModulePath.string()));

#ifdef _WIN32
        m.dlopenHandle = LoadLibraryW(fullModulePath.c_str());
#else
        m.library_handle = dlopen(fullModulePath.c_str(), RTLD_NOW | RTLD_LOCAL);
#endif
        if (!m.library_handle) {
#ifdef _WIN32
            m_lastError = GetLastError();
#else
            m_last_error = dlerror();
#endif
            m_bot->log(dpp::ll_error, fmt::format("Can't load module: {}", m_last_error));
            return false;
        } else {
            if (!get_symbol(m, "init_module")) {
                m_bot->log(dpp::ll_error, fmt::format("Can't load module: {}", m.error ? m.error : "General error"));
                m_last_error = (m.error ? m.error : "General error");
#ifdef _WIN32
                FreeLibrary((HMODULE)m.dlopenHandle);
#else
                dlclose(m.library_handle);
#endif
                return false;
            } else {
                m_bot->log(dpp::ll_debug, fmt::format("Module shared object {} loaded, symbol found", file_name));
                m.module_object = m.entry_point(m_bot, this);

                if (!m.module_object || (uint64_t)m.module_object == 0xffffffffffffffff) {
                    m_bot->log(dpp::ll_error, fmt::format("Invalid module pointer returned"));
                    m.error = "Not a module (symbol init_module not found)";
                    m_last_error = m.error;
#ifdef _WIN32
                    FreeLibrary((HMODULE)m.dlopenHandle);
#else
                    dlclose(m.library_handle);
#endif
                    return false;
                } else {

                    const std::vector<module_command>& moduleCommands = m.module_object->get_commands();
                    m_bot->log(dpp::ll_debug, fmt::format("Loading {} commands from module {}...", moduleCommands.size(), file_name));
                    for(const auto& entry: moduleCommands) {
                        if(m_command_name_to_module.contains(entry.name)) {
                            m_bot->log(
                                    dpp::ll_critical,
                                    fmt::format("Command {} has already been added by module {}! Failed to load.",
                                                entry.name, m_module_names[m_command_name_to_module[entry.name]]));

                            // delete all commands belonging to this module
                            for(auto it = m_command_name_to_module.begin(); it != m_command_name_to_module.end(); ) {
                                if (it->second == m.module_object) {
                                    it = m_command_name_to_module.erase(it);
                                } else {
                                    ++it;
                                }
                            }

                            // unload module
                            unload(file_name);

                            return false;
                        }

                        m_command_name_to_module[entry.name] = m.module_object;
                        m_bot->log(dpp::ll_debug, fmt::format("Added module command \"{}\" ({})", entry.name, entry.description));
                    }

                    m_module_commands[file_name] = moduleCommands;
                    m_bot->log(dpp::ll_debug, fmt::format("Module {} initialized", file_name));
                    m_native_modules[file_name] = m;
                    m_module_list[file_name] = m.module_object;
                    m_module_names[m.module_object] = file_name;
                    m_last_error = "";

                    return true;
                }
            }
        }
    } else {
        m_bot->log(dpp::ll_error, fmt::format("Module {} already loaded!", file_name));
        m_last_error = fmt::format("module {} already loaded!", file_name);
        return false;
    }
}

bool bot_module_manager::unload(const std::string &file_name) {
    std::lock_guard lockGuard(m_mutex);

    m_bot->log(dpp::ll_info, fmt::format("Unloading module {}", file_name));

    auto m = m_native_modules.find(file_name);
    if (m == m_native_modules.end()) {
        m_last_error = "module is not loaded";
        return false;
    }

    module_native& mod = m->second;

    // remove attached events
    for (int j = M_BEGIN; j != M_END; ++j) {
        auto p = std::find(m_event_handlers[j].begin(), m_event_handlers[j].end(), mod.module_object);
        if (p != m_event_handlers[j].end()) {
            m_event_handlers[j].erase(p);
            m_bot->log(dpp::ll_debug, fmt::format("Module {}: removed event {}", file_name, BOT_MODULE_EVENT_NAMES[j]));
        }
    }

    // remove native module entry
    m_native_modules.erase(m);

    auto v = m_module_list.find(file_name);
    if (v != m_module_list.end()) {
        m_module_list.erase(v);
        m_bot->log(dpp::ll_debug, fmt::format("Module {} removed from module list", file_name));
    }

    if (mod.module_object) {
        m_module_names.erase(mod.module_object);
        delete mod.module_object;
        m_bot->log(dpp::ll_debug, fmt::format("Module {} object removed!", file_name));
    }

    // unload module
    if (mod.library_handle) {

#ifdef _WIN32
        FreeLibrary((HMODULE)mod.dlopenHandle);
#else
        dlclose(mod.library_handle);
#endif
        m_bot->log(dpp::ll_debug, fmt::format("Module {} unloaded!", file_name));
    }

    return true;
}

bool bot_module_manager::reload(const std::string &file_name) {
    return (unload(file_name) && load(file_name));
}

void bot_module_manager::load_all() {
    json moduleConfig = m_bot->get_config()->data()["modules"];
    for(const auto& entry: moduleConfig) {
        std::string moduleName = entry.get<std::string>();
        load(moduleName);
    }
}

bool bot_module_manager::get_symbol(module_native &native, const char* symbol_name) {
    // Find exported symbol in shared object
    if (native.library_handle) {
#ifdef _WIN32
        native.entryPoint = (ModuleEntryPoint*)GetProcAddress((HMODULE)native.dlopenHandle, symbolName);
#else
        dlerror(); // clear value
        native.entry_point = (module_entry_point*)dlsym(native.library_handle, symbol_name);
        native.error = dlerror();
#endif
        if (!native.entry_point || native.error) {
            return false;
        }
    } else {
        native.error = "bot_module_loader::get_symbol(): invalid library handle!";
        return false;
    }
    return true;
}

const module_map &bot_module_manager::get_module_list() const {
    return m_module_list;
}

const std::string& bot_module_manager::get_last_error() {
    return m_last_error;
}

std::mutex& bot_module_manager::get_mutex() {
    return m_mutex;
}

const std::vector<bot_module *> *bot_module_manager::get_event_handlers() {
    return m_event_handlers;
}

const std::map<std::string, std::vector<module_command>> &bot_module_manager::get_module_commands() const {
    return m_module_commands;
}

bot_module *bot_module_manager::get_command_module(std::string name) {
    if(m_command_name_to_module.contains(name)) {
        return m_command_name_to_module[name];
    }

    return nullptr;
}

void bot_module_manager::register_module_commands() {
    std::vector<dpp::slashcommand> commandsToCreate;
    for(const auto& module: m_module_commands) {
        for(const auto& cmd: module.second) {
            m_bot->log(dpp::ll_debug, fmt::format("cmd name: {}", cmd.name));
            dpp::slashcommand newCmd(cmd.name, cmd.description, m_bot->get_id());
            newCmd.set_default_permissions(cmd.permissions);
            for(const auto& option: cmd.options) {
                newCmd.add_option(option);
            }
            commandsToCreate.push_back(newCmd);
        }
    }

    // create commands for guild if running in dev mode
    if(m_bot->is_dev_mode()) {
        m_bot->get_core()->guild_bulk_command_create(commandsToCreate, m_bot->get_config()->guild_id());
    } else {
        //m_client->getCore()->global_bulk_command_create(commandsToCreate);
    }

}

bot_module::bot_module(client* bot, bot_module_manager* manager) :
        m_bot(bot),
        m_module_manager(manager) {

}

bot_module::~bot_module()
{

}

std::string bot_module::get_version()
{
    return "<not implemented>";
}

std::string bot_module::get_description()
{
    return "<not implemented>";
}

std::vector<module_command> bot_module::get_commands()
{
    return {};
}

#ifdef PROTOBOT_COMMAND_SUPPORT

void Module::onCommandCreated(const std::string& name, const ModuleCommand& command) {

}

void Module::onBulkCommandCreated(const std::map<std::string, ModuleCommand>& commands) {

}

#endif

// the following code between the two BOT_MODULE_EVENT_IMPL tags is automatically generated
// ! PLEASE DO NOT EDIT !
//
// <BOT_MODULE_EVENT_IMPL>
bool bot_module::on_voice_state_update(const dpp::voice_state_update_t &event) {
    return true;
}

bool bot_module::on_voice_client_disconnect(const dpp::voice_client_disconnect_t &event) {
    return true;
}

bool bot_module::on_voice_client_speaking(const dpp::voice_client_speaking_t &event) {
    return true;
}

bool bot_module::on_guild_join_request_delete(const dpp::guild_join_request_delete_t &event) {
    return true;
}

bool bot_module::on_interaction_create(const dpp::interaction_create_t &event) {
    return true;
}

bool bot_module::on_slashcommand(const dpp::slashcommand_t &event) {
    return true;
}

bool bot_module::on_button_click(const dpp::button_click_t &event) {
    return true;
}

bool bot_module::on_autocomplete(const dpp::autocomplete_t &event) {
    return true;
}

bool bot_module::on_select_click(const dpp::select_click_t &event) {
    return true;
}

bool bot_module::on_message_context_menu(const dpp::message_context_menu_t &event) {
    return true;
}

bool bot_module::on_user_context_menu(const dpp::user_context_menu_t &event) {
    return true;
}

bool bot_module::on_form_submit(const dpp::form_submit_t &event) {
    return true;
}

bool bot_module::on_guild_delete(const dpp::guild_delete_t &event) {
    return true;
}

bool bot_module::on_channel_delete(const dpp::channel_delete_t &event) {
    return true;
}

bool bot_module::on_channel_update(const dpp::channel_update_t &event) {
    return true;
}

bool bot_module::on_ready(const dpp::ready_t &event) {
    return true;
}

bool bot_module::on_message_delete(const dpp::message_delete_t &event) {
    return true;
}

bool bot_module::on_guild_member_remove(const dpp::guild_member_remove_t &event) {
    return true;
}

bool bot_module::on_resumed(const dpp::resumed_t &event) {
    return true;
}

bool bot_module::on_guild_role_create(const dpp::guild_role_create_t &event) {
    return true;
}

bool bot_module::on_typing_start(const dpp::typing_start_t &event) {
    return true;
}

bool bot_module::on_message_reaction_add(const dpp::message_reaction_add_t &event) {
    return true;
}

bool bot_module::on_guild_members_chunk(const dpp::guild_members_chunk_t &event) {
    return true;
}

bool bot_module::on_message_reaction_remove(const dpp::message_reaction_remove_t &event) {
    return true;
}

bool bot_module::on_guild_create(const dpp::guild_create_t &event) {
    return true;
}

bool bot_module::on_channel_create(const dpp::channel_create_t &event) {
    return true;
}

bool bot_module::on_message_reaction_remove_emoji(const dpp::message_reaction_remove_emoji_t &event) {
    return true;
}

bool bot_module::on_message_delete_bulk(const dpp::message_delete_bulk_t &event) {
    return true;
}

bool bot_module::on_guild_role_update(const dpp::guild_role_update_t &event) {
    return true;
}

bool bot_module::on_guild_role_delete(const dpp::guild_role_delete_t &event) {
    return true;
}

bool bot_module::on_channel_pins_update(const dpp::channel_pins_update_t &event) {
    return true;
}

bool bot_module::on_message_reaction_remove_all(const dpp::message_reaction_remove_all_t &event) {
    return true;
}

bool bot_module::on_voice_server_update(const dpp::voice_server_update_t &event) {
    return true;
}

bool bot_module::on_guild_emojis_update(const dpp::guild_emojis_update_t &event) {
    return true;
}

bool bot_module::on_guild_stickers_update(const dpp::guild_stickers_update_t &event) {
    return true;
}

bool bot_module::on_presence_update(const dpp::presence_update_t &event) {
    return true;
}

bool bot_module::on_webhooks_update(const dpp::webhooks_update_t &event) {
    return true;
}

bool bot_module::on_guild_member_add(const dpp::guild_member_add_t &event) {
    return true;
}

bool bot_module::on_invite_delete(const dpp::invite_delete_t &event) {
    return true;
}

bool bot_module::on_guild_update(const dpp::guild_update_t &event) {
    return true;
}

bool bot_module::on_guild_integrations_update(const dpp::guild_integrations_update_t &event) {
    return true;
}

bool bot_module::on_guild_member_update(const dpp::guild_member_update_t &event) {
    return true;
}

bool bot_module::on_invite_create(const dpp::invite_create_t &event) {
    return true;
}

bool bot_module::on_message_update(const dpp::message_update_t &event) {
    return true;
}

bool bot_module::on_user_update(const dpp::user_update_t &event) {
    return true;
}

bool bot_module::on_message_create(const dpp::message_create_t &event) {
    return true;
}

bool bot_module::on_guild_ban_add(const dpp::guild_ban_add_t &event) {
    return true;
}

bool bot_module::on_guild_ban_remove(const dpp::guild_ban_remove_t &event) {
    return true;
}

bool bot_module::on_integration_create(const dpp::integration_create_t &event) {
    return true;
}

bool bot_module::on_integration_update(const dpp::integration_update_t &event) {
    return true;
}

bool bot_module::on_integration_delete(const dpp::integration_delete_t &event) {
    return true;
}

bool bot_module::on_thread_create(const dpp::thread_create_t &event) {
    return true;
}

bool bot_module::on_thread_update(const dpp::thread_update_t &event) {
    return true;
}

bool bot_module::on_thread_delete(const dpp::thread_delete_t &event) {
    return true;
}

bool bot_module::on_thread_list_sync(const dpp::thread_list_sync_t &event) {
    return true;
}

bool bot_module::on_thread_member_update(const dpp::thread_member_update_t &event) {
    return true;
}

bool bot_module::on_thread_members_update(const dpp::thread_members_update_t &event) {
    return true;
}

bool bot_module::on_guild_scheduled_event_create(const dpp::guild_scheduled_event_create_t &event) {
    return true;
}

bool bot_module::on_guild_scheduled_event_update(const dpp::guild_scheduled_event_update_t &event) {
    return true;
}

bool bot_module::on_guild_scheduled_event_delete(const dpp::guild_scheduled_event_delete_t &event) {
    return true;
}

bool bot_module::on_guild_scheduled_event_user_add(const dpp::guild_scheduled_event_user_add_t &event) {
    return true;
}

bool bot_module::on_guild_scheduled_event_user_remove(const dpp::guild_scheduled_event_user_remove_t &event) {
    return true;
}

bool bot_module::on_voice_buffer_send(const dpp::voice_buffer_send_t &event) {
    return true;
}

bool bot_module::on_voice_user_talking(const dpp::voice_user_talking_t &event) {
    return true;
}

bool bot_module::on_voice_ready(const dpp::voice_ready_t &event) {
    return true;
}

bool bot_module::on_voice_receive(const dpp::voice_receive_t &event) {
    return true;
}

bool bot_module::on_voice_receive_combined(const dpp::voice_receive_t &event) {
    return true;
}

bool bot_module::on_voice_track_marker(const dpp::voice_track_marker_t &event) {
    return true;
}

bool bot_module::on_stage_instance_create(const dpp::stage_instance_create_t &event) {
    return true;
}

bool bot_module::on_stage_instance_update(const dpp::stage_instance_update_t &event) {
    return true;
}

bool bot_module::on_stage_instance_delete(const dpp::stage_instance_delete_t &event) {
    return true;
}
// </BOT_MODULE_EVENT_IMPL>
