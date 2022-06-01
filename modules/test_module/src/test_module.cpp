#include "test_module.h"

#include <dpp/dpp.h>
#include <dpp/message.h>
#include <protobot/client.h>
#include <protobot/module.h>

#include <string>

test_module::test_module(client* bot, bot_module_manager* moduleManager) : bot_module(bot, moduleManager) {
    m_bot->log(dpp::ll_debug, "TestModule created!");
    m_module_manager->attach({M_on_ready }, this);
    m_module_manager->attach({M_on_slashcommand }, this);
    m_module_manager->attach({M_on_button_click}, this);
}

test_module::~test_module() {

}

std::string test_module::get_version() {
    return "TestModule v0.420.69";
}

std::string test_module::get_description() {
    return "Just used for testing, Awoooo! :3";
}

bool test_module::on_slashcommand(const dpp::slashcommand_t &cmd) {

    cmd.thinking(true);
    dpp::command_interaction cmd_data = cmd.command.get_command_interaction();
    if(cmd.command.get_command_name() == "bot") {

        if (cmd_data.options[0].name == "set_activity") {
            std::string activityType = std::get<std::string>(cmd_data.options[0].options[0].value);
            std::string activityMessage = std::get<std::string>(cmd_data.options[0].options[1].value);

            m_bot->get_db()->set_bot_option("activity_type", activityType);
            m_bot->get_db()->set_bot_option("activity_message", activityMessage);
            m_bot->get_core()->set_presence(dpp::presence(dpp::ps_idle, client::string_to_activity_type(activityType), activityMessage));
            cmd.edit_original_response(dpp::message("Set bot activity status!"));
        }
    } else if(cmd.command.get_command_name() == "testcmd_user") {
        m_bot->log(dpp::ll_debug, cmd.raw_event);
        cmd.edit_original_response(dpp::message(cmd.command.msg.channel_id, "Hewwoo").add_component(
            dpp::component().add_component(
                dpp::component().set_label("P.. push me! >//<").
                    set_type(dpp::cot_button).
                    set_emoji("", m_bot->find_emoji_by_name("WolfBlush")->id).
                    set_style(dpp::cos_danger).
                    set_id("awooo")
            )
        ));
    } else {
        cmd.edit_original_response(dpp::message(fmt::format("(Module 1) Command {} has not been implemented yet!", cmd.command.get_command_name())).set_flags(dpp::message_flags::m_ephemeral));
    }

    return true;
}

bool test_module::on_button_click(const dpp::button_click_t &btn) {
    auto guild_id = m_bot->get_config()->dev_guild_id();
    auto test_channel = m_bot->find_channel_by_name("testing", guild_id);
    auto emoji = m_bot->find_emoji_by_name("WolfYawn");
    dpp::message msg(test_channel->id,
                     fmt::format("{}{}{}", emoji->get_mention(), emoji->get_mention(), emoji->get_mention()));
    m_bot->get_core()->message_create_sync(msg.set_flags(dpp::message_flags::m_ephemeral));
    btn.reply(dpp::message(
        fmt::format("(Module 1 btn id: {}) Button clicked :3", btn.custom_id)
    ).set_flags(dpp::message_flags::m_ephemeral));
}

bool test_module::on_ready(const dpp::ready_t &ready) {
    m_bot->log(dpp::ll_debug, "TestModule: ready!");

    std::lock_guard lockGuard(m_bot->get_mutex());

    std::string activityType = m_bot->get_db()->get_bot_option("activity_type");
    std::string activityMessage = m_bot->get_db()->get_bot_option("activity_message");

    m_bot->get_core()->set_presence(dpp::presence(dpp::ps_idle, client::string_to_activity_type(activityType), activityMessage));
    return true;
}

std::vector<module_command> test_module::get_commands() {
    return {
        {
            "bot",
            "Bot management",
            dpp::permissions::p_administrator,
            {
                dpp::command_option(dpp::co_sub_command, "set_activity", "Set bot activity status")
                    .add_localization("de", "set_activity", "Aktivitätsstatus vom Bot setzen")
                    .add_option(dpp::command_option(dpp::co_string, "type", "The activity type", true)
                        .add_localization("de", "type", "Aktivitäts-Typ")
                        .add_choice(dpp::command_option_choice("Playing", std::string("playing"))
                                        .add_localization("de", "Spielt"))
                        .add_choice(dpp::command_option_choice("Streaming", std::string("streaming"))
                                        .add_localization("de", "Streamt"))
                        .add_choice(dpp::command_option_choice("Listening", std::string("listening"))
                                        .add_localization("de", "Hört zu"))
                        .add_choice(dpp::command_option_choice("Watching", std::string("watching"))
                                        .add_localization("de", "Schaut zu"))
                        .add_choice(dpp::command_option_choice("Competing", std::string("competing"))
                                        .add_localization("de", "Tritt an"))
                    ).add_option(dpp::command_option(dpp::co_string, "message", "The activity message", true))
            }
        },
        {
            "testcmd_user",
            "Test command (user)!",
            dpp::permissions::p_use_application_commands
        }
    };
}


