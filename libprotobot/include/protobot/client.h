#ifndef PROTOBOT_CLIENT_H
#define PROTOBOT_CLIENT_H
#include <protobot/protobot.h>
#include <protobot/config.h>
#include <protobot/database.h>

#include <map>

class bot_module;
#ifdef PROTOBOT_COMMAND_SUPPORT
class ModuleCommandManager;
#endif
class bot_module_manager;

class client {
public:
    client(dpp::cluster* core, config* cfg, database* db, bool dev_mode = false);
    ~client();

    void recvSignal(int signal);
    void load_modules();

    bool is_dev_mode();

    static dpp::activity_type string_to_activity_type(std::string activityType);

    void bind_events();
    std::mutex& get_mutex();
    uint64_t get_id();
    database* get_db();
    dpp::cluster* get_core();
    config* get_config();
    void log(dpp::loglevel severity, const std::string &msg) const;

    bot_module_manager* getModuleManager();

    // the following code between the two CLIENT_EVENT_BIND_DEF tags is automatically generated
    // ! PLEASE DO NOT EDIT !
    //
    // <CLIENT_EVENT_BIND_DEF>
    void on_voice_state_update (const dpp::voice_state_update_t &event);
    void on_voice_client_disconnect (const dpp::voice_client_disconnect_t &event);
    void on_voice_client_speaking (const dpp::voice_client_speaking_t &event);
    void on_guild_join_request_delete (const dpp::guild_join_request_delete_t &event);
    void on_interaction_create (const dpp::interaction_create_t &event);
    void on_slashcommand (const dpp::slashcommand_t &event);
    void on_button_click (const dpp::button_click_t &event);
    void on_autocomplete (const dpp::autocomplete_t &event);
    void on_select_click (const dpp::select_click_t &event);
    void on_message_context_menu (const dpp::message_context_menu_t &event);
    void on_user_context_menu (const dpp::user_context_menu_t &event);
    void on_form_submit (const dpp::form_submit_t &event);
    void on_guild_delete (const dpp::guild_delete_t &event);
    void on_channel_delete (const dpp::channel_delete_t &event);
    void on_channel_update (const dpp::channel_update_t &event);
    void on_ready (const dpp::ready_t &event);
    void on_message_delete (const dpp::message_delete_t &event);
    void on_guild_member_remove (const dpp::guild_member_remove_t &event);
    void on_resumed (const dpp::resumed_t &event);
    void on_guild_role_create (const dpp::guild_role_create_t &event);
    void on_typing_start (const dpp::typing_start_t &event);
    void on_message_reaction_add (const dpp::message_reaction_add_t &event);
    void on_guild_members_chunk (const dpp::guild_members_chunk_t &event);
    void on_message_reaction_remove (const dpp::message_reaction_remove_t &event);
    void on_guild_create (const dpp::guild_create_t &event);
    void on_channel_create (const dpp::channel_create_t &event);
    void on_message_reaction_remove_emoji (const dpp::message_reaction_remove_emoji_t &event);
    void on_message_delete_bulk (const dpp::message_delete_bulk_t &event);
    void on_guild_role_update (const dpp::guild_role_update_t &event);
    void on_guild_role_delete (const dpp::guild_role_delete_t &event);
    void on_channel_pins_update (const dpp::channel_pins_update_t &event);
    void on_message_reaction_remove_all (const dpp::message_reaction_remove_all_t &event);
    void on_voice_server_update (const dpp::voice_server_update_t &event);
    void on_guild_emojis_update (const dpp::guild_emojis_update_t &event);
    void on_guild_stickers_update (const dpp::guild_stickers_update_t &event);
    void on_presence_update (const dpp::presence_update_t &event);
    void on_webhooks_update (const dpp::webhooks_update_t &event);
    void on_guild_member_add (const dpp::guild_member_add_t &event);
    void on_invite_delete (const dpp::invite_delete_t &event);
    void on_guild_update (const dpp::guild_update_t &event);
    void on_guild_integrations_update (const dpp::guild_integrations_update_t &event);
    void on_guild_member_update (const dpp::guild_member_update_t &event);
    void on_invite_create (const dpp::invite_create_t &event);
    void on_message_update (const dpp::message_update_t &event);
    void on_user_update (const dpp::user_update_t &event);
    void on_message_create (const dpp::message_create_t &event);
    void on_guild_ban_add (const dpp::guild_ban_add_t &event);
    void on_guild_ban_remove (const dpp::guild_ban_remove_t &event);
    void on_integration_create (const dpp::integration_create_t &event);
    void on_integration_update (const dpp::integration_update_t &event);
    void on_integration_delete (const dpp::integration_delete_t &event);
    void on_thread_create (const dpp::thread_create_t &event);
    void on_thread_update (const dpp::thread_update_t &event);
    void on_thread_delete (const dpp::thread_delete_t &event);
    void on_thread_list_sync (const dpp::thread_list_sync_t &event);
    void on_thread_member_update (const dpp::thread_member_update_t &event);
    void on_thread_members_update (const dpp::thread_members_update_t &event);
    void on_guild_scheduled_event_create (const dpp::guild_scheduled_event_create_t &event);
    void on_guild_scheduled_event_update (const dpp::guild_scheduled_event_update_t &event);
    void on_guild_scheduled_event_delete (const dpp::guild_scheduled_event_delete_t &event);
    void on_guild_scheduled_event_user_add (const dpp::guild_scheduled_event_user_add_t &event);
    void on_guild_scheduled_event_user_remove (const dpp::guild_scheduled_event_user_remove_t &event);
    void on_voice_buffer_send (const dpp::voice_buffer_send_t &event);
    void on_voice_user_talking (const dpp::voice_user_talking_t &event);
    void on_voice_ready (const dpp::voice_ready_t &event);
    void on_voice_receive (const dpp::voice_receive_t &event);
    void on_voice_receive_combined (const dpp::voice_receive_t &event);
    void on_voice_track_marker (const dpp::voice_track_marker_t &event);
    void on_stage_instance_create (const dpp::stage_instance_create_t &event);
    void on_stage_instance_update (const dpp::stage_instance_update_t &event);
    void on_stage_instance_delete (const dpp::stage_instance_delete_t &event);
    // </CLIENT_EVENT_BIND_DEF>

    void onReady (const dpp::ready_t &obj);
    void onChannelCreate(const dpp::channel_create_t &obj);
    void onChannelDelete(const dpp::channel_delete_t &obj);
    void onGuildCreate(const dpp::guild_create_t &obj);
    void onGuildDelete(const dpp::guild_delete_t &obj);
    void onGuildMemberAdd(const dpp::guild_member_add_t &obj);
    void onTypingStart (const dpp::typing_start_t &obj);
    void onMessageCreate (const dpp::message_create_t &obj);
    void onMessageUpdate (const dpp::message_update_t &obj);
    void onMessageDelete (const dpp::message_delete_t &obj);
    void onMessageDeleteBulk (const dpp::message_delete_bulk_t &obj);
    void onGuildUpdate (const dpp::guild_update_t &obj);
    void onMessageReactionAdd (const dpp::message_reaction_add_t &obj);
    void onMessageReactionRemove (const dpp::message_reaction_remove_t &obj);
    void onMessageReactionRemoveAll (const dpp::message_reaction_remove_all_t &obj);
    void onUserUpdate (const dpp::user_update_t &obj);
    void onResumed (const dpp::resumed_t &obj);
    void onChannelUpdate (const dpp::channel_update_t &obj);
    void onChannelPinsUpdate (const dpp::channel_pins_update_t &obj);
    void onGuildBanAdd (const dpp::guild_ban_add_t &obj);
    void onGuildBanRemove (const dpp::guild_ban_remove_t &obj);
    void onGuildEmojisUpdate (const dpp::guild_emojis_update_t &obj);
    void onGuildIntegrationsUpdate (const dpp::guild_integrations_update_t &obj);
    void onGuildMemberRemove (const dpp::guild_member_remove_t &obj);
    void onGuildMemberUpdate (const dpp::guild_member_update_t &obj);
    void onGuildMembersChunk (const dpp::guild_members_chunk_t &obj);
    void onGuildRoleCreate (const dpp::guild_role_create_t &obj);
    void onGuildRoleUpdate (const dpp::guild_role_update_t &obj);
    void onGuildRoleDelete (const dpp::guild_role_delete_t &obj);
    void onPresenceUpdate (const dpp::presence_update_t &obj);
    void onVoiceStateUpdate (const dpp::voice_state_update_t &obj);
    void onVoiceServerUpdate (const dpp::voice_server_update_t &obj);
    void onWebhooksUpdate (const dpp::webhooks_update_t &obj);
    void onSlashCommand(const dpp::slashcommand_t &obj);


private:
    bool m_dev_mode;

    dpp::cluster* m_core;
    config* m_cfg;
    database* m_database;
    bot_module_manager* m_module_manager;

    std::mutex m_mutex;

#ifdef PROTOBOT_COMMAND_SUPPORT
    ModuleCommandManager* m_moduleCommandManager;
#endif
};

#endif //PROTOBOT_CLIENT_H
