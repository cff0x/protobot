#include <protobot/client.h>
#include <protobot/module.h>
#include <protobot/string_utils.h>



client::client(dpp::cluster* core, config* cfg, database* db, bool dev_mode) :
        m_core(core),
        m_cfg(cfg),
        m_database(db),
        m_module_manager(new bot_module_manager(this)),
#ifdef PROTOBOT_COMMAND_SUPPORT
        m_moduleCommandManager(new ModuleCommandManager(this)),
#endif
        m_dev_mode(dev_mode) {


}

client::~client() = default;

bool client::is_dev_mode() {
    return m_dev_mode;
}

void client::load_modules() {
    size_t moduleCount = m_cfg->data()["modules"].size();
    log(dpp::ll_info, fmt::format("Loading {} bot module{}...", moduleCount, (moduleCount > 1 ? "s" : "")));
    m_module_manager->load_all();
}

dpp::activity_type client::string_to_activity_type(std::string activityType) {
    std::transform(activityType.begin(), activityType.end(), activityType.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    if(activityType == "playing" || activityType == "0") {
        return dpp::activity_type::at_game;
    } else if(activityType == "streaming" || activityType == "1") {
        return dpp::activity_type::at_streaming;
    } else if(activityType == "listening" || activityType == "2") {
        return dpp::activity_type::at_listening;
    } else if(activityType == "watching" || activityType == "3") {
        return dpp::activity_type::at_watching;
    } else if(activityType == "custom" || activityType == "4") {
        return dpp::activity_type::at_custom;
    } else if(activityType == "competing" || activityType == "5") {
        return dpp::activity_type::at_competing;
    }

    throw std::runtime_error("string_to_activity_type: Invalid activity type");
}

uint64_t client::get_id() {
    return m_core->me.id;
}

dpp::cluster* client::get_core() {
    return m_core;
}

config* client::get_config() {
    return m_cfg;
}

// the following code between the two CLIENT_EVENT_BIND_IMPL tags is automatically generated
// ! PLEASE DO NOT EDIT !
//
// <CLIENT_EVENT_BIND_IMPL>
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

bot_module_manager *client::getModuleManager() {
    return m_module_manager;
}

void client::log(dpp::loglevel severity, const std::string &msg) const {
    m_core->log(severity, msg);
}

database *client::get_db() {
    return m_database;
}

std::mutex& client::get_mutex() {
    return m_mutex;
}

void client::recvSignal(int signal) {
    m_core->log(dpp::ll_warning, fmt::format("Received signal {}! Exiting...", signal));
    exit(signal);
}

void client::bind_events() {
    // the following code between the two CLIENT_EVENT_BINDS tags is automatically generated
    // ! PLEASE DO NOT EDIT !
    //
    // <CLIENT_EVENT_BINDS>
    m_core->on_voice_state_update(std::bind(&client::on_voice_state_update, this, std::placeholders::_1));
    m_core->on_voice_client_disconnect(std::bind(&client::on_voice_client_disconnect, this, std::placeholders::_1));
    m_core->on_voice_client_speaking(std::bind(&client::on_voice_client_speaking, this, std::placeholders::_1));
    m_core->on_guild_join_request_delete(std::bind(&client::on_guild_join_request_delete, this, std::placeholders::_1));
    m_core->on_interaction_create(std::bind(&client::on_interaction_create, this, std::placeholders::_1));
    m_core->on_slashcommand(std::bind(&client::on_slashcommand, this, std::placeholders::_1));
    m_core->on_button_click(std::bind(&client::on_button_click, this, std::placeholders::_1));
    m_core->on_autocomplete(std::bind(&client::on_autocomplete, this, std::placeholders::_1));
    m_core->on_select_click(std::bind(&client::on_select_click, this, std::placeholders::_1));
    m_core->on_message_context_menu(std::bind(&client::on_message_context_menu, this, std::placeholders::_1));
    m_core->on_user_context_menu(std::bind(&client::on_user_context_menu, this, std::placeholders::_1));
    m_core->on_form_submit(std::bind(&client::on_form_submit, this, std::placeholders::_1));
    m_core->on_guild_delete(std::bind(&client::on_guild_delete, this, std::placeholders::_1));
    m_core->on_channel_delete(std::bind(&client::on_channel_delete, this, std::placeholders::_1));
    m_core->on_channel_update(std::bind(&client::on_channel_update, this, std::placeholders::_1));
    m_core->on_ready(std::bind(&client::on_ready, this, std::placeholders::_1));
    m_core->on_message_delete(std::bind(&client::on_message_delete, this, std::placeholders::_1));
    m_core->on_guild_member_remove(std::bind(&client::on_guild_member_remove, this, std::placeholders::_1));
    m_core->on_resumed(std::bind(&client::on_resumed, this, std::placeholders::_1));
    m_core->on_guild_role_create(std::bind(&client::on_guild_role_create, this, std::placeholders::_1));
    m_core->on_typing_start(std::bind(&client::on_typing_start, this, std::placeholders::_1));
    m_core->on_message_reaction_add(std::bind(&client::on_message_reaction_add, this, std::placeholders::_1));
    m_core->on_guild_members_chunk(std::bind(&client::on_guild_members_chunk, this, std::placeholders::_1));
    m_core->on_message_reaction_remove(std::bind(&client::on_message_reaction_remove, this, std::placeholders::_1));
    m_core->on_guild_create(std::bind(&client::on_guild_create, this, std::placeholders::_1));
    m_core->on_channel_create(std::bind(&client::on_channel_create, this, std::placeholders::_1));
    m_core->on_message_reaction_remove_emoji(std::bind(&client::on_message_reaction_remove_emoji, this, std::placeholders::_1));
    m_core->on_message_delete_bulk(std::bind(&client::on_message_delete_bulk, this, std::placeholders::_1));
    m_core->on_guild_role_update(std::bind(&client::on_guild_role_update, this, std::placeholders::_1));
    m_core->on_guild_role_delete(std::bind(&client::on_guild_role_delete, this, std::placeholders::_1));
    m_core->on_channel_pins_update(std::bind(&client::on_channel_pins_update, this, std::placeholders::_1));
    m_core->on_message_reaction_remove_all(std::bind(&client::on_message_reaction_remove_all, this, std::placeholders::_1));
    m_core->on_voice_server_update(std::bind(&client::on_voice_server_update, this, std::placeholders::_1));
    m_core->on_guild_emojis_update(std::bind(&client::on_guild_emojis_update, this, std::placeholders::_1));
    m_core->on_guild_stickers_update(std::bind(&client::on_guild_stickers_update, this, std::placeholders::_1));
    m_core->on_presence_update(std::bind(&client::on_presence_update, this, std::placeholders::_1));
    m_core->on_webhooks_update(std::bind(&client::on_webhooks_update, this, std::placeholders::_1));
    m_core->on_guild_member_add(std::bind(&client::on_guild_member_add, this, std::placeholders::_1));
    m_core->on_invite_delete(std::bind(&client::on_invite_delete, this, std::placeholders::_1));
    m_core->on_guild_update(std::bind(&client::on_guild_update, this, std::placeholders::_1));
    m_core->on_guild_integrations_update(std::bind(&client::on_guild_integrations_update, this, std::placeholders::_1));
    m_core->on_guild_member_update(std::bind(&client::on_guild_member_update, this, std::placeholders::_1));
    m_core->on_invite_create(std::bind(&client::on_invite_create, this, std::placeholders::_1));
    m_core->on_message_update(std::bind(&client::on_message_update, this, std::placeholders::_1));
    m_core->on_user_update(std::bind(&client::on_user_update, this, std::placeholders::_1));
    m_core->on_message_create(std::bind(&client::on_message_create, this, std::placeholders::_1));
    m_core->on_guild_ban_add(std::bind(&client::on_guild_ban_add, this, std::placeholders::_1));
    m_core->on_guild_ban_remove(std::bind(&client::on_guild_ban_remove, this, std::placeholders::_1));
    m_core->on_integration_create(std::bind(&client::on_integration_create, this, std::placeholders::_1));
    m_core->on_integration_update(std::bind(&client::on_integration_update, this, std::placeholders::_1));
    m_core->on_integration_delete(std::bind(&client::on_integration_delete, this, std::placeholders::_1));
    m_core->on_thread_create(std::bind(&client::on_thread_create, this, std::placeholders::_1));
    m_core->on_thread_update(std::bind(&client::on_thread_update, this, std::placeholders::_1));
    m_core->on_thread_delete(std::bind(&client::on_thread_delete, this, std::placeholders::_1));
    m_core->on_thread_list_sync(std::bind(&client::on_thread_list_sync, this, std::placeholders::_1));
    m_core->on_thread_member_update(std::bind(&client::on_thread_member_update, this, std::placeholders::_1));
    m_core->on_thread_members_update(std::bind(&client::on_thread_members_update, this, std::placeholders::_1));
    m_core->on_guild_scheduled_event_create(std::bind(&client::on_guild_scheduled_event_create, this, std::placeholders::_1));
    m_core->on_guild_scheduled_event_update(std::bind(&client::on_guild_scheduled_event_update, this, std::placeholders::_1));
    m_core->on_guild_scheduled_event_delete(std::bind(&client::on_guild_scheduled_event_delete, this, std::placeholders::_1));
    m_core->on_guild_scheduled_event_user_add(std::bind(&client::on_guild_scheduled_event_user_add, this, std::placeholders::_1));
    m_core->on_guild_scheduled_event_user_remove(std::bind(&client::on_guild_scheduled_event_user_remove, this, std::placeholders::_1));
    m_core->on_voice_buffer_send(std::bind(&client::on_voice_buffer_send, this, std::placeholders::_1));
    m_core->on_voice_user_talking(std::bind(&client::on_voice_user_talking, this, std::placeholders::_1));
    m_core->on_voice_ready(std::bind(&client::on_voice_ready, this, std::placeholders::_1));
    m_core->on_voice_receive(std::bind(&client::on_voice_receive, this, std::placeholders::_1));
    m_core->on_voice_receive_combined(std::bind(&client::on_voice_receive_combined, this, std::placeholders::_1));
    m_core->on_voice_track_marker(std::bind(&client::on_voice_track_marker, this, std::placeholders::_1));
    m_core->on_stage_instance_create(std::bind(&client::on_stage_instance_create, this, std::placeholders::_1));
    m_core->on_stage_instance_update(std::bind(&client::on_stage_instance_update, this, std::placeholders::_1));
    m_core->on_stage_instance_delete(std::bind(&client::on_stage_instance_delete, this, std::placeholders::_1));
    // </CLIENT_EVENT_BINDS>
}
