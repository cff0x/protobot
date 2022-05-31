#include "test_module_2.h"

#include <dpp/dpp.h>
#include <dpp/message.h>
#include <protobot/client.h>
#include <protobot/module.h>


test_module_2::test_module_2(client* bot, bot_module_manager* moduleManager) : bot_module(bot, moduleManager) {
    m_bot->log(dpp::ll_debug, "TestModule2 created!");
    m_module_manager->attach({M_on_ready }, this);
    m_module_manager->attach({M_on_slashcommand }, this);
    m_module_manager->attach({M_on_button_click}, this);
}

test_module_2::~test_module_2() {

}

std::string test_module_2::get_version() {
    return "TestModule2 v0.420.69";
}

std::string test_module_2::get_description() {
    return "Just used for testing, Awoooo! :3";
}

bool test_module_2::on_slashcommand(const dpp::slashcommand_t &cmd) {

    cmd.thinking(true);
    dpp::command_interaction cmd_data = cmd.command.get_command_interaction();
    if(cmd.command.get_command_name() == "testcmd_module_2") {
        m_bot->log(dpp::ll_debug, cmd.raw_event);
        cmd.edit_original_response(dpp::message(cmd.command.msg.channel_id, "Hewwoo from module 2!").add_component(
                dpp::component().add_component(
                        dpp::component().set_label("P.. push me! >//<").
                                set_type(dpp::cot_button).
                                set_emoji("", 977278114407280650).
                                set_style(dpp::cos_danger).
                                set_id("awooo")
                )
        ));
    } else {
        cmd.edit_original_response(dpp::message(fmt::format("(Module 2) Command {} has not been implemented yet!", cmd.command.get_command_name())).set_flags(dpp::message_flags::m_ephemeral));
    }

    return true;
}

bool test_module_2::on_button_click(const dpp::button_click_t &btn) {
    m_bot->log(dpp::ll_debug, btn.raw_event);
    btn.reply(fmt::format("(Module 2 btn id: {}) Button clicked :3", btn.custom_id));
    dpp::snowflake channel_id = std::stoull(json::parse(btn.raw_event)["d"]["message"]["channel_id"].get<std::string>());
    auto emoji =  m_bot->get_core()->guild_emoji_get_sync(m_bot->get_config()->guild_id(), 977279242817658880);
    std::string test_message = fmt::format("{}{}{}", emoji.get_mention(), emoji.get_mention(), emoji.get_mention());
    m_bot->log(dpp::ll_debug, test_message);
    dpp::message msg(channel_id, test_message);
    m_bot->get_core()->message_create(msg);
}

bool test_module_2::on_ready(const dpp::ready_t &ready) {
    m_bot->log(dpp::ll_debug, "TestModule2: ready!");

    return true;
}

std::vector<module_command> test_module_2::get_commands() {
    return {
        {
            "testcmd_module_2",
            "Test command module 2 (user)!",
            dpp::permissions::p_use_application_commands
        }
    };
}


