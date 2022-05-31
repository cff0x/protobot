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
                                set_emoji("", 977304235689508904).
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
    m_bot->log(dpp::ll_debug, btn.raw_event);
    m_bot->log(dpp::ll_debug, btn.command.msg.interaction.name);
    btn.reply(fmt::format("(Module 1 btn id: {}) Button clicked :3", btn.custom_id));
    dpp::snowflake channel_id = std::stoull(json::parse(btn.raw_event)["d"]["message"]["channel_id"].get<std::string>());
    auto emoji =  m_bot->get_core()->guild_emoji_get_sync(m_bot->get_config()->guild_id(), 977279242817658880);
    std::string test_message = fmt::format("{}{}{}", emoji.get_mention(), emoji.get_mention(), emoji.get_mention());
    m_bot->log(dpp::ll_debug, test_message);
    dpp::message msg(channel_id, test_message);
    m_bot->get_core()->message_create(msg);
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
                dpp::command_option(dpp::co_sub_command, "set_activity", "Set bot activity status").add_option(
                    dpp::command_option(dpp::co_string, "type", "The activity type", true)
                        .add_choice(dpp::command_option_choice("Playing", std::string("playing")))
                        .add_choice(dpp::command_option_choice("Streaming", std::string("streaming")))
                        .add_choice(dpp::command_option_choice("Listening", std::string("listening")))
                        .add_choice(dpp::command_option_choice("Watching", std::string("watching")))
                        .add_choice(dpp::command_option_choice("Competing", std::string("competing")))
                    ).add_option(dpp::command_option(dpp::co_string, "message", "The activity message", true))
            }
        },
        {
            "testcmd_mod",
            "Test command (mod)!",
            dpp::permissions::p_priority_speaker
        },
        {
            "testcmd_user",
            "Test command (user)!",
            dpp::permissions::p_use_application_commands
        }
    };
}


