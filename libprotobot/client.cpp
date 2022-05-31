#include <protobot/client.h>
#include <protobot/module.h>
#include <protobot/string_utils.h>

namespace protobot {

client::client(dpp::cluster *core, config *cfg, database *db, bool dev_mode) :
    m_core(core),
    m_cfg(cfg),
    m_database(db),
    m_module_manager(new bot_module_manager(this)),
    m_dev_mode(dev_mode) {


}

client::~client() = default;


bool client::is_dev_mode() {
    return m_dev_mode;
}

uint64_t client::get_bot_user_id() {
    return m_core->me.id;
}

database *client::get_db() {
    return m_database;
}

config *client::get_config() {
    return m_cfg;
}

dpp::cluster *client::get_core() {
    return m_core;
}

std::mutex &client::get_mutex() {
    return m_mutex;
}

bot_module_manager *client::get_module_manager() {
    return m_module_manager;
}

void client::log(dpp::loglevel severity, const std::string &msg) const {
    m_core->log(severity, msg);
}

void client::load_modules() {
    size_t moduleCount = m_cfg->data()["modules"].size();
    log(dpp::ll_info, fmt::format("Loading {} bot module{}...", moduleCount, (moduleCount > 1 ? "s" : "")));
    m_module_manager->load_all();
}

dpp::emoji* client::find_emoji_by_name(const std::string& name, dpp::snowflake guild_id) {
    if(guild_id != 0) {
        auto guild = dpp::find_guild(guild_id);
        auto it = std::find_if(guild->emojis.begin(), guild->emojis.end(),
                               [name](auto emoji_id) { return dpp::find_emoji(emoji_id)->name == name; });

        return dpp::find_emoji(*it);
    } else {
        auto emoji_map = dpp::get_emoji_cache()->get_container();
        auto it = std::find_if(emoji_map.begin(), emoji_map.end(),
                               [name](auto entry) { return entry.second->name == name; });

        return it->second;
    }
}

dpp::role* client::find_role_by_name(const std::string &name, dpp::snowflake guild_id) {
    auto role_map = dpp::get_role_cache()->get_container();
    auto it = std::find_if(role_map.begin(), role_map.end(),
                           [name, guild_id](auto entry)
                           {
                                return entry.second->guild_id == guild_id && entry.second->name == name;
                           });
    return it->second;
}

dpp::channel* client::find_channel_by_name(const std::string &name, dpp::snowflake guild_id) {
    auto channel_map = dpp::get_channel_cache()->get_container();
    auto it = std::find_if(channel_map.begin(), channel_map.end(),
                           [name, guild_id](auto entry)
                           {
                               return entry.second->guild_id == guild_id && entry.second->name == name;
                           });
    return it->second;
}

dpp::activity_type client::string_to_activity_type(std::string activityType) {
    std::transform(activityType.begin(), activityType.end(), activityType.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    if (activityType == "playing" || activityType == "0") {
        return dpp::activity_type::at_game;
    } else if (activityType == "streaming" || activityType == "1") {
        return dpp::activity_type::at_streaming;
    } else if (activityType == "listening" || activityType == "2") {
        return dpp::activity_type::at_listening;
    } else if (activityType == "watching" || activityType == "3") {
        return dpp::activity_type::at_watching;
    } else if (activityType == "custom" || activityType == "4") {
        return dpp::activity_type::at_custom;
    } else if (activityType == "competing" || activityType == "5") {
        return dpp::activity_type::at_competing;
    }

    throw std::runtime_error("string_to_activity_type: Invalid activity type");
}

void client::bind_events() {
    // the following code between the two CLIENT_EVENT_BINDS tags is automatically generated
    // ! PLEASE DO NOT EDIT !
    //
    // <CLIENT_EVENT_BINDS>
    //
    // GENERATED AT: 06/01/2022 01:14:31
    //
    m_core->on_voice_state_update([this](auto && PH1) { on_voice_state_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_voice_client_disconnect([this](auto && PH1) { on_voice_client_disconnect(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_voice_client_speaking([this](auto && PH1) { on_voice_client_speaking(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_join_request_delete([this](auto && PH1) { on_guild_join_request_delete(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_interaction_create([this](auto && PH1) { on_interaction_create(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_slashcommand([this](auto && PH1) { on_slashcommand(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_button_click([this](auto && PH1) { on_button_click(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_autocomplete([this](auto && PH1) { on_autocomplete(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_select_click([this](auto && PH1) { on_select_click(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_message_context_menu([this](auto && PH1) { on_message_context_menu(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_user_context_menu([this](auto && PH1) { on_user_context_menu(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_form_submit([this](auto && PH1) { on_form_submit(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_delete([this](auto && PH1) { on_guild_delete(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_channel_delete([this](auto && PH1) { on_channel_delete(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_channel_update([this](auto && PH1) { on_channel_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_ready([this](auto && PH1) { on_ready(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_message_delete([this](auto && PH1) { on_message_delete(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_member_remove([this](auto && PH1) { on_guild_member_remove(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_resumed([this](auto && PH1) { on_resumed(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_role_create([this](auto && PH1) { on_guild_role_create(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_typing_start([this](auto && PH1) { on_typing_start(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_message_reaction_add([this](auto && PH1) { on_message_reaction_add(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_members_chunk([this](auto && PH1) { on_guild_members_chunk(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_message_reaction_remove([this](auto && PH1) { on_message_reaction_remove(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_create([this](auto && PH1) { on_guild_create(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_channel_create([this](auto && PH1) { on_channel_create(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_message_reaction_remove_emoji([this](auto && PH1) { on_message_reaction_remove_emoji(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_message_delete_bulk([this](auto && PH1) { on_message_delete_bulk(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_role_update([this](auto && PH1) { on_guild_role_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_role_delete([this](auto && PH1) { on_guild_role_delete(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_channel_pins_update([this](auto && PH1) { on_channel_pins_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_message_reaction_remove_all([this](auto && PH1) { on_message_reaction_remove_all(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_voice_server_update([this](auto && PH1) { on_voice_server_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_emojis_update([this](auto && PH1) { on_guild_emojis_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_stickers_update([this](auto && PH1) { on_guild_stickers_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_presence_update([this](auto && PH1) { on_presence_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_webhooks_update([this](auto && PH1) { on_webhooks_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_member_add([this](auto && PH1) { on_guild_member_add(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_invite_delete([this](auto && PH1) { on_invite_delete(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_update([this](auto && PH1) { on_guild_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_integrations_update([this](auto && PH1) { on_guild_integrations_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_member_update([this](auto && PH1) { on_guild_member_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_invite_create([this](auto && PH1) { on_invite_create(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_message_update([this](auto && PH1) { on_message_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_user_update([this](auto && PH1) { on_user_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_message_create([this](auto && PH1) { on_message_create(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_ban_add([this](auto && PH1) { on_guild_ban_add(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_ban_remove([this](auto && PH1) { on_guild_ban_remove(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_integration_create([this](auto && PH1) { on_integration_create(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_integration_update([this](auto && PH1) { on_integration_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_integration_delete([this](auto && PH1) { on_integration_delete(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_thread_create([this](auto && PH1) { on_thread_create(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_thread_update([this](auto && PH1) { on_thread_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_thread_delete([this](auto && PH1) { on_thread_delete(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_thread_list_sync([this](auto && PH1) { on_thread_list_sync(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_thread_member_update([this](auto && PH1) { on_thread_member_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_thread_members_update([this](auto && PH1) { on_thread_members_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_scheduled_event_create([this](auto && PH1) { on_guild_scheduled_event_create(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_scheduled_event_update([this](auto && PH1) { on_guild_scheduled_event_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_scheduled_event_delete([this](auto && PH1) { on_guild_scheduled_event_delete(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_scheduled_event_user_add([this](auto && PH1) { on_guild_scheduled_event_user_add(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_guild_scheduled_event_user_remove([this](auto && PH1) { on_guild_scheduled_event_user_remove(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_voice_buffer_send([this](auto && PH1) { on_voice_buffer_send(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_voice_user_talking([this](auto && PH1) { on_voice_user_talking(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_voice_ready([this](auto && PH1) { on_voice_ready(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_voice_receive([this](auto && PH1) { on_voice_receive(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_voice_receive_combined([this](auto && PH1) { on_voice_receive_combined(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_voice_track_marker([this](auto && PH1) { on_voice_track_marker(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_stage_instance_create([this](auto && PH1) { on_stage_instance_create(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_stage_instance_update([this](auto && PH1) { on_stage_instance_update(std::forward<decltype(PH1)>(PH1)); });
    m_core->on_stage_instance_delete([this](auto && PH1) { on_stage_instance_delete(std::forward<decltype(PH1)>(PH1)); });
    // </CLIENT_EVENT_BINDS>
}

// the following code between the two CLIENT_EVENT_BIND_IMPL tags is automatically generated
// ! PLEASE DO NOT EDIT !
//
// <CLIENT_EVENT_BIND_IMPL>
//
// GENERATED AT: 06/01/2022 01:14:31
//
void client::on_voice_state_update(const dpp::voice_state_update_t &event) {
    FOREACH_MODULE(M_on_voice_state_update, on_voice_state_update(event));
}

void client::on_voice_client_disconnect(const dpp::voice_client_disconnect_t &event) {
    FOREACH_MODULE(M_on_voice_client_disconnect, on_voice_client_disconnect(event));
}

void client::on_voice_client_speaking(const dpp::voice_client_speaking_t &event) {
    FOREACH_MODULE(M_on_voice_client_speaking, on_voice_client_speaking(event));
}

void client::on_guild_join_request_delete(const dpp::guild_join_request_delete_t &event) {
    FOREACH_MODULE(M_on_guild_join_request_delete, on_guild_join_request_delete(event));
}

void client::on_interaction_create(const dpp::interaction_create_t &event) {
    FOREACH_MODULE(M_on_interaction_create, on_interaction_create(event));
}

void client::on_slashcommand(const dpp::slashcommand_t &event) {
    if(m_module_manager->get_command_module(event.command.get_command_name()) != nullptr) {
        m_module_manager->get_command_module(event.command.get_command_name())->on_slashcommand(event);
    } else {
        event.reply(dpp::message("ERROR: The module to which this command belongs has not been loaded!").set_flags(dpp::message_flags::m_ephemeral));
    }
}

void client::on_button_click(const dpp::button_click_t &event) {
    std::string commandName = event.command.msg.interaction.name;
    if(m_module_manager->get_command_module(commandName) != nullptr) {
        m_module_manager->get_command_module(commandName)->on_button_click(event);
    } else {
        event.reply(dpp::message("ERROR: The module to which this button belongs has not been loaded!").set_flags(dpp::message_flags::m_ephemeral));
    }
}

void client::on_autocomplete(const dpp::autocomplete_t &event) {
    FOREACH_MODULE(M_on_autocomplete, on_autocomplete(event));
}

void client::on_select_click(const dpp::select_click_t &event) {
    std::string commandName = event.command.msg.interaction.name;
    if(m_module_manager->get_command_module(commandName) != nullptr) {
        m_module_manager->get_command_module(commandName)->on_select_click(event);
    } else {
        event.reply(dpp::message("ERROR: The module to which this select menu belongs has not been loaded!").set_flags(dpp::message_flags::m_ephemeral));
    }
}

void client::on_message_context_menu(const dpp::message_context_menu_t &event) {
    FOREACH_MODULE(M_on_message_context_menu, on_message_context_menu(event));
}

void client::on_user_context_menu(const dpp::user_context_menu_t &event) {
    FOREACH_MODULE(M_on_user_context_menu, on_user_context_menu(event));
}

void client::on_form_submit(const dpp::form_submit_t &event) {
    FOREACH_MODULE(M_on_form_submit, on_form_submit(event));
}

void client::on_guild_delete(const dpp::guild_delete_t &event) {
    FOREACH_MODULE(M_on_guild_delete, on_guild_delete(event));
}

void client::on_channel_delete(const dpp::channel_delete_t &event) {
    FOREACH_MODULE(M_on_channel_delete, on_channel_delete(event));
}

void client::on_channel_update(const dpp::channel_update_t &event) {
    FOREACH_MODULE(M_on_channel_update, on_channel_update(event));
}

void client::on_ready(const dpp::ready_t &event) {
    m_module_manager->register_module_commands();
    FOREACH_MODULE(M_on_ready, on_ready(event));
}

void client::on_message_delete(const dpp::message_delete_t &event) {
    FOREACH_MODULE(M_on_message_delete, on_message_delete(event));
}

void client::on_guild_member_remove(const dpp::guild_member_remove_t &event) {
    FOREACH_MODULE(M_on_guild_member_remove, on_guild_member_remove(event));
}

void client::on_resumed(const dpp::resumed_t &event) {
    FOREACH_MODULE(M_on_resumed, on_resumed(event));
}

void client::on_guild_role_create(const dpp::guild_role_create_t &event) {
    FOREACH_MODULE(M_on_guild_role_create, on_guild_role_create(event));
}

void client::on_typing_start(const dpp::typing_start_t &event) {
    FOREACH_MODULE(M_on_typing_start, on_typing_start(event));
}

void client::on_message_reaction_add(const dpp::message_reaction_add_t &event) {
    FOREACH_MODULE(M_on_message_reaction_add, on_message_reaction_add(event));
}

void client::on_guild_members_chunk(const dpp::guild_members_chunk_t &event) {
    FOREACH_MODULE(M_on_guild_members_chunk, on_guild_members_chunk(event));
}

void client::on_message_reaction_remove(const dpp::message_reaction_remove_t &event) {
    FOREACH_MODULE(M_on_message_reaction_remove, on_message_reaction_remove(event));
}

void client::on_guild_create(const dpp::guild_create_t &event) {
    FOREACH_MODULE(M_on_guild_create, on_guild_create(event));
}

void client::on_channel_create(const dpp::channel_create_t &event) {
    FOREACH_MODULE(M_on_channel_create, on_channel_create(event));
}

void client::on_message_reaction_remove_emoji(const dpp::message_reaction_remove_emoji_t &event) {
    FOREACH_MODULE(M_on_message_reaction_remove_emoji, on_message_reaction_remove_emoji(event));
}

void client::on_message_delete_bulk(const dpp::message_delete_bulk_t &event) {
    FOREACH_MODULE(M_on_message_delete_bulk, on_message_delete_bulk(event));
}

void client::on_guild_role_update(const dpp::guild_role_update_t &event) {
    FOREACH_MODULE(M_on_guild_role_update, on_guild_role_update(event));
}

void client::on_guild_role_delete(const dpp::guild_role_delete_t &event) {
    FOREACH_MODULE(M_on_guild_role_delete, on_guild_role_delete(event));
}

void client::on_channel_pins_update(const dpp::channel_pins_update_t &event) {
    FOREACH_MODULE(M_on_channel_pins_update, on_channel_pins_update(event));
}

void client::on_message_reaction_remove_all(const dpp::message_reaction_remove_all_t &event) {
    FOREACH_MODULE(M_on_message_reaction_remove_all, on_message_reaction_remove_all(event));
}

void client::on_voice_server_update(const dpp::voice_server_update_t &event) {
    FOREACH_MODULE(M_on_voice_server_update, on_voice_server_update(event));
}

void client::on_guild_emojis_update(const dpp::guild_emojis_update_t &event) {
    FOREACH_MODULE(M_on_guild_emojis_update, on_guild_emojis_update(event));
}

void client::on_guild_stickers_update(const dpp::guild_stickers_update_t &event) {
    FOREACH_MODULE(M_on_guild_stickers_update, on_guild_stickers_update(event));
}

void client::on_presence_update(const dpp::presence_update_t &event) {
    FOREACH_MODULE(M_on_presence_update, on_presence_update(event));
}

void client::on_webhooks_update(const dpp::webhooks_update_t &event) {
    FOREACH_MODULE(M_on_webhooks_update, on_webhooks_update(event));
}

void client::on_guild_member_add(const dpp::guild_member_add_t &event) {
    FOREACH_MODULE(M_on_guild_member_add, on_guild_member_add(event));
}

void client::on_invite_delete(const dpp::invite_delete_t &event) {
    FOREACH_MODULE(M_on_invite_delete, on_invite_delete(event));
}

void client::on_guild_update(const dpp::guild_update_t &event) {
    FOREACH_MODULE(M_on_guild_update, on_guild_update(event));
}

void client::on_guild_integrations_update(const dpp::guild_integrations_update_t &event) {
    FOREACH_MODULE(M_on_guild_integrations_update, on_guild_integrations_update(event));
}

void client::on_guild_member_update(const dpp::guild_member_update_t &event) {
    FOREACH_MODULE(M_on_guild_member_update, on_guild_member_update(event));
}

void client::on_invite_create(const dpp::invite_create_t &event) {
    FOREACH_MODULE(M_on_invite_create, on_invite_create(event));
}

void client::on_message_update(const dpp::message_update_t &event) {
    FOREACH_MODULE(M_on_message_update, on_message_update(event));
}

void client::on_user_update(const dpp::user_update_t &event) {
    FOREACH_MODULE(M_on_user_update, on_user_update(event));
}

void client::on_message_create(const dpp::message_create_t &event) {
    FOREACH_MODULE(M_on_message_create, on_message_create(event));
}

void client::on_guild_ban_add(const dpp::guild_ban_add_t &event) {
    FOREACH_MODULE(M_on_guild_ban_add, on_guild_ban_add(event));
}

void client::on_guild_ban_remove(const dpp::guild_ban_remove_t &event) {
    FOREACH_MODULE(M_on_guild_ban_remove, on_guild_ban_remove(event));
}

void client::on_integration_create(const dpp::integration_create_t &event) {
    FOREACH_MODULE(M_on_integration_create, on_integration_create(event));
}

void client::on_integration_update(const dpp::integration_update_t &event) {
    FOREACH_MODULE(M_on_integration_update, on_integration_update(event));
}

void client::on_integration_delete(const dpp::integration_delete_t &event) {
    FOREACH_MODULE(M_on_integration_delete, on_integration_delete(event));
}

void client::on_thread_create(const dpp::thread_create_t &event) {
    FOREACH_MODULE(M_on_thread_create, on_thread_create(event));
}

void client::on_thread_update(const dpp::thread_update_t &event) {
    FOREACH_MODULE(M_on_thread_update, on_thread_update(event));
}

void client::on_thread_delete(const dpp::thread_delete_t &event) {
    FOREACH_MODULE(M_on_thread_delete, on_thread_delete(event));
}

void client::on_thread_list_sync(const dpp::thread_list_sync_t &event) {
    FOREACH_MODULE(M_on_thread_list_sync, on_thread_list_sync(event));
}

void client::on_thread_member_update(const dpp::thread_member_update_t &event) {
    FOREACH_MODULE(M_on_thread_member_update, on_thread_member_update(event));
}

void client::on_thread_members_update(const dpp::thread_members_update_t &event) {
    FOREACH_MODULE(M_on_thread_members_update, on_thread_members_update(event));
}

void client::on_guild_scheduled_event_create(const dpp::guild_scheduled_event_create_t &event) {
    FOREACH_MODULE(M_on_guild_scheduled_event_create, on_guild_scheduled_event_create(event));
}

void client::on_guild_scheduled_event_update(const dpp::guild_scheduled_event_update_t &event) {
    FOREACH_MODULE(M_on_guild_scheduled_event_update, on_guild_scheduled_event_update(event));
}

void client::on_guild_scheduled_event_delete(const dpp::guild_scheduled_event_delete_t &event) {
    FOREACH_MODULE(M_on_guild_scheduled_event_delete, on_guild_scheduled_event_delete(event));
}

void client::on_guild_scheduled_event_user_add(const dpp::guild_scheduled_event_user_add_t &event) {
    FOREACH_MODULE(M_on_guild_scheduled_event_user_add, on_guild_scheduled_event_user_add(event));
}

void client::on_guild_scheduled_event_user_remove(const dpp::guild_scheduled_event_user_remove_t &event) {
    FOREACH_MODULE(M_on_guild_scheduled_event_user_remove, on_guild_scheduled_event_user_remove(event));
}

void client::on_voice_buffer_send(const dpp::voice_buffer_send_t &event) {
    FOREACH_MODULE(M_on_voice_buffer_send, on_voice_buffer_send(event));
}

void client::on_voice_user_talking(const dpp::voice_user_talking_t &event) {
    FOREACH_MODULE(M_on_voice_user_talking, on_voice_user_talking(event));
}

void client::on_voice_ready(const dpp::voice_ready_t &event) {
    FOREACH_MODULE(M_on_voice_ready, on_voice_ready(event));
}

void client::on_voice_receive(const dpp::voice_receive_t &event) {
    FOREACH_MODULE(M_on_voice_receive, on_voice_receive(event));
}

void client::on_voice_receive_combined(const dpp::voice_receive_t &event) {
    FOREACH_MODULE(M_on_voice_receive_combined, on_voice_receive_combined(event));
}

void client::on_voice_track_marker(const dpp::voice_track_marker_t &event) {
    FOREACH_MODULE(M_on_voice_track_marker, on_voice_track_marker(event));
}

void client::on_stage_instance_create(const dpp::stage_instance_create_t &event) {
    FOREACH_MODULE(M_on_stage_instance_create, on_stage_instance_create(event));
}

void client::on_stage_instance_update(const dpp::stage_instance_update_t &event) {
    FOREACH_MODULE(M_on_stage_instance_update, on_stage_instance_update(event));
}

void client::on_stage_instance_delete(const dpp::stage_instance_delete_t &event) {
    FOREACH_MODULE(M_on_stage_instance_delete, on_stage_instance_delete(event));
}

// </CLIENT_EVENT_BIND_IMPL>



}