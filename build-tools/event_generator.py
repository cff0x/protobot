import sys
import re

event_pattern = re.compile(r"\tevent_router_t<(\w+)>\s(\w+);")

if __name__ == "__main__":
    DISCORDPP_CLUSTER_H_PATH = sys.argv[1]
    print(f"cluster.h: {DISCORDPP_CLUSTER_H_PATH}")
    DISCORDPP_CLUSTER_H_CONTENT = ""
    with open(DISCORDPP_CLUSTER_H_PATH, "r") as f:
        DISCORDPP_CLUSTER_H_CONTENT = f.read()

    bot_module_event_enum_entries = ["M_BEGIN", "M_END"]

    core_binds = list()
    core_bind_defs = list()
    core_bind_impl = list()

    bot_module_event_defs = list()
    bot_module_event_impl = list()

    m = event_pattern.findall(DISCORDPP_CLUSTER_H_CONTENT)
    for event_type_name, event_name in m:
        if event_name == "on_log":
            continue

        bot_module_event = f"M_{event_name}"
        bot_module_event_enum_entries.insert(len(bot_module_event_enum_entries) - 1, bot_module_event)

        core_binds.append(f"m_core->{event_name}(std::bind(&client::{event_name}, this, std::placeholders::_1));")
        core_bind_defs.append(f"void {event_name} (const dpp::{event_type_name} &event);")

        core_bind_impl_str = f"void client::{event_name}(const dpp::{event_type_name} &event)" + " {\n"
        if event_name == "on_slashcommand":
            core_bind_impl_str += "    if(m_module_manager->get_command_module(event.command.get_command_name()) != nullptr) {\n" + \
                                  "        m_module_manager->get_command_module(event.command.get_command_name())->on_slashcommand(event);\n" + \
                                  "    } else {\n" + \
                                  "        event.reply(dpp::message(\"ERROR: The module to which this command belongs has not been loaded!\").set_flags(dpp::message_flags::m_ephemeral));\n" + \
                                  "    }\n"
        elif event_name == "on_button_click":
            core_bind_impl_str += "    std::string commandName = event.command.msg.interaction.name;\n" + \
                                  "    if(m_module_manager->get_command_module(commandName) != nullptr) {\n" + \
                                  "        m_module_manager->get_command_module(commandName)->on_button_click(event);\n" + \
                                  "    } else {\n" + \
                                  "        event.reply(dpp::message(\"ERROR: The module to which this button belongs has not been loaded!\").set_flags(dpp::message_flags::m_ephemeral));\n" + \
                                  "    }\n"
        elif event_name == "on_select_click":
            core_bind_impl_str += "    std::string commandName = event.command.msg.interaction.name;\n" + \
                                  "    if(m_module_manager->get_command_module(commandName) != nullptr) {\n" + \
                                  "        m_module_manager->get_command_module(commandName)->on_select_click(event);\n" + \
                                  "    } else {\n" + \
                                  "        event.reply(dpp::message(\"ERROR: The module to which this select menu belongs has not been loaded!\").set_flags(dpp::message_flags::m_ephemeral));\n" + \
                                  "    }\n"
        else:
            if event_name == "on_ready":
                core_bind_impl_str += "    m_module_manager->register_module_commands();\n"
            core_bind_impl_str += f"    FOREACH_MODULE({bot_module_event}, {event_name}(event));\n"
        core_bind_impl_str += "}"
        core_bind_impl.append(core_bind_impl_str)

        bot_module_event_defs.append(f"virtual bool {event_name}(const dpp::{event_type_name} &event);")
        bot_module_event_impl.append(f"bool bot_module::{event_name}(const dpp::{event_type_name} &event)" + " {\n" +
                                     "    return true;\n" +
                                     "}")

    events_header_enum_entries = ",\n    ".join(bot_module_event_enum_entries)
    events_header_content = "enum bot_module_event {\n    " + events_header_enum_entries + "\n};"

    bot_module_event_enum_names = list()
    for entry in bot_module_event_enum_entries:
        bot_module_event_enum_names.append(f"\"{entry}\"")

    events_header_enum_name_entries = ",\n    ".join(bot_module_event_enum_names)
    events_header_content += "\nstatic const char* BOT_MODULE_EVENT_NAMES[" + \
                             bot_module_event_enum_entries[len(bot_module_event_enum_entries) - 1] + \
                             " + 1] = {\n    " + events_header_enum_name_entries + "\n};"

    event_header_content = "// THIS CODE IS AUTOMATICALLY GENERATED FROM THE DPP LIBRARY CODE\n" \
                           "// PLEASE DO NOT EDIT\n" \
                           f"{events_header_content}"

    with open("libprotobot/include/protobot/events.h", "w") as f:
        f.write(event_header_content)

    CLIENT_CPP_CONTENT = ""
    with open("libprotobot/client.cpp") as f:
        CLIENT_CPP_CONTENT = f.read()

    NEW_CLIENT_CPP_CONTENT = ""

    client_event_binds_pattern = re.compile(r"\s+//\s<CLIENT_EVENT_BINDS>\n((.|\n)*)\n\s+//\s</CLIENT_EVENT_BINDS>")
    for entry in client_event_binds_pattern.finditer(CLIENT_CPP_CONTENT):
        replace_start = entry.span(1)[0]
        replace_end = entry.span(1)[1]
        NEW_CLIENT_CPP_CONTENT += CLIENT_CPP_CONTENT[:replace_start] + "    "
        NEW_CLIENT_CPP_CONTENT += "\n    ".join(core_binds)
        NEW_CLIENT_CPP_CONTENT += CLIENT_CPP_CONTENT[replace_end:]

    CLIENT_CPP_CONTENT = NEW_CLIENT_CPP_CONTENT
    NEW_CLIENT_CPP_CONTENT = ""

    client_event_bind_impl_pattern = re.compile(r"//\s<CLIENT_EVENT_BIND_IMPL>\n((.|\n)*)\n//\s</CLIENT_EVENT_BIND_IMPL>")
    for entry in client_event_bind_impl_pattern.finditer(CLIENT_CPP_CONTENT):
        replace_start = entry.span(1)[0]
        replace_end = entry.span(1)[1]
        NEW_CLIENT_CPP_CONTENT += CLIENT_CPP_CONTENT[:replace_start]
        NEW_CLIENT_CPP_CONTENT += "\n\n".join(core_bind_impl)
        NEW_CLIENT_CPP_CONTENT += CLIENT_CPP_CONTENT[replace_end:]

    CLIENT_CPP_CONTENT = NEW_CLIENT_CPP_CONTENT

    if len(CLIENT_CPP_CONTENT) > 1:
        with open("libprotobot/client.cpp", "w") as f:
            f.write(CLIENT_CPP_CONTENT)

    CLIENT_H_CONTENT = ""
    with open("libprotobot/include/protobot/client.h") as f:
        CLIENT_H_CONTENT = f.read()

    NEW_CLIENT_H_CONTENT = ""

    client_event_bind_def_pattern = re.compile(r"\s+//\s<CLIENT_EVENT_BIND_DEF>\n((.|\n)*)\n\s+//\s</CLIENT_EVENT_BIND_DEF>")
    for entry in client_event_bind_def_pattern.finditer(CLIENT_H_CONTENT):
        replace_start = entry.span(1)[0]
        replace_end = entry.span(1)[1]
        NEW_CLIENT_H_CONTENT += CLIENT_H_CONTENT[:replace_start] + "    "
        NEW_CLIENT_H_CONTENT += "\n    ".join(core_bind_defs)
        NEW_CLIENT_H_CONTENT += CLIENT_H_CONTENT[replace_end:]

    CLIENT_H_CONTENT = NEW_CLIENT_H_CONTENT
    if len(CLIENT_H_CONTENT) > 1:
        with open("libprotobot/include/protobot/client.h", "w") as f:
            f.write(CLIENT_H_CONTENT)

    MODULE_H_CONTENT = ""
    with open("libprotobot/include/protobot/module.h") as f:
        MODULE_H_CONTENT = f.read()

    NEW_MODULE_H_CONTENT = ""

    client_event_bind_def_pattern = re.compile(r"\s+//\s<BOT_MODULE_EVENT_DEF>\n((.|\n)*)\n\s+//\s</BOT_MODULE_EVENT_DEF>")
    for entry in client_event_bind_def_pattern.finditer(MODULE_H_CONTENT):
        replace_start = entry.span(1)[0]
        replace_end = entry.span(1)[1]
        NEW_MODULE_H_CONTENT += MODULE_H_CONTENT[:replace_start] + "    "
        NEW_MODULE_H_CONTENT += "\n    ".join(bot_module_event_defs)
        NEW_MODULE_H_CONTENT += MODULE_H_CONTENT[replace_end:]

    MODULE_H_CONTENT = NEW_MODULE_H_CONTENT
    if len(MODULE_H_CONTENT) > 1:
        with open("libprotobot/include/protobot/module.h", "w") as f:
            f.write(MODULE_H_CONTENT)

    MODULE_CPP_CONTENT = ""
    with open("libprotobot/module.cpp") as f:
        MODULE_CPP_CONTENT = f.read()

    NEW_MODULE_CPP_CONTENT = ""

    client_event_bind_def_pattern = re.compile(r"//\s<BOT_MODULE_EVENT_IMPL>\n((.|\n)*)\n//\s</BOT_MODULE_EVENT_IMPL>")
    for entry in client_event_bind_def_pattern.finditer(MODULE_CPP_CONTENT):
        replace_start = entry.span(1)[0]
        replace_end = entry.span(1)[1]
        NEW_MODULE_CPP_CONTENT += MODULE_CPP_CONTENT[:replace_start]
        NEW_MODULE_CPP_CONTENT += "\n\n".join(bot_module_event_impl)
        NEW_MODULE_CPP_CONTENT += MODULE_CPP_CONTENT[replace_end:]

    MODULE_CPP_CONTENT = NEW_MODULE_CPP_CONTENT
    if len(MODULE_CPP_CONTENT) > 1:
        with open("libprotobot/module.cpp", "w") as f:
            f.write(MODULE_CPP_CONTENT)
