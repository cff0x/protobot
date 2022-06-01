import sys
import re
from datetime import datetime

# pattern to recognize the event types in the DPP library
EVENT_PATTERN = re.compile(r"\tevent_router_t<(\w+)>\s(\w+);")

# timestamp for the codegen output
TIMESTAMP = datetime.now().strftime("%m/%d/%Y %H:%M:%S")

# cluster.h (contains the events defined by DPP)
DISCORDPP_CLUSTER_H_PATH = sys.argv[1]
print(f"cluster.h: {DISCORDPP_CLUSTER_H_PATH}")
with open(DISCORDPP_CLUSTER_H_PATH, "r") as f:
    DISCORDPP_CLUSTER_H_CONTENT = f.read()

# array with enum entries for all events
# (initialized with start values)
bot_module_event_enum_entries = ["M_BEGIN", "M_END"]

# event binding stuff for ProtoBot client.cpp/client.h
core_binds = list()
core_bind_defs = list()
core_bind_impl = list()

# event callback stuff for ProtoBot module.cpp/module.h
bot_module_event_defs = list()
bot_module_event_impl = list()


# find all events in cluster.h
m = EVENT_PATTERN.findall(DISCORDPP_CLUSTER_H_CONTENT)
for event_type_name, event_name in m:
    if event_name == "on_log":
        # do not handle on_log, this is only used internally
        continue

    # construct event enum value
    bot_module_event = f"M_{event_name}"
    bot_module_event_enum_entries.insert(len(bot_module_event_enum_entries) - 1, bot_module_event)

    # add client function binding via lambda
    core_binds.append(
        f"m_core->{event_name}([this](auto && PH1) {{ {event_name}(std::forward<decltype(PH1)>(PH1)); }});"
    )

    # add client event callback definition
    core_bind_defs.append(f"void {event_name} (const dpp::{event_type_name} &event);")

    # add client event callback implementation
    core_bind_impl_str = f"void client::{event_name}(const dpp::{event_type_name} &event)" + " {\n"
    if event_name == "on_slashcommand":
        # we need to get the command name and the corresponding module if the event is a slash command interaction
        core_bind_impl_str += "    if(m_module_manager->get_command_module(event.command.get_command_name()) != " \
                              "nullptr) {\n" + \
                              "        m_module_manager->get_command_module(event.command.get_command_name())->" \
                              "on_slashcommand(event);\n" + \
                              "    } else {\n" + \
                              "        event.reply(dpp::message(\"ERROR: The module to which this command belongs " \
                              "has not been loaded!\").set_flags(dpp::message_flags::m_ephemeral));\n" + \
                              "    }\n"
    elif event_name == "on_button_click":
        # for a button click, we will also get the module via the interaction's command name
        core_bind_impl_str += "    std::string commandName = event.command.msg.interaction.name;\n" + \
                              "    if(m_module_manager->get_command_module(commandName) != nullptr) {\n" + \
                              "        m_module_manager->get_command_module(commandName)->on_button_click(event);\n" + \
                              "    } else {\n" + \
                              "        event.reply(dpp::message(\"ERROR: The module to which this button belongs has " \
                              "not been loaded!\").set_flags(dpp::message_flags::m_ephemeral));\n" + \
                              "    }\n"
    elif event_name == "on_select_click":
        # same goes for select menus
        core_bind_impl_str += "    std::string commandName = event.command.msg.interaction.name;\n" + \
                              "    if(m_module_manager->get_command_module(commandName) != nullptr) {\n" + \
                              "        m_module_manager->get_command_module(commandName)->on_select_click(event);\n" + \
                              "    } else {\n" + \
                              "        event.reply(dpp::message(\"ERROR: The module to which this select menu " \
                              "" \
                              "belongs has not been loaded!\").set_flags(dpp::message_flags::m_ephemeral));\n" + \
                              "    }\n"
    else:
        if event_name == "on_ready":
            # on_ready is special:
            # here, all the module commands will be registered before calling the on_ready event for our modules
            core_bind_impl_str += "    m_module_manager->register_module_commands();\n"
        # calls the event for all modules which are attached to it
        core_bind_impl_str += f"    FOREACH_MODULE({bot_module_event}, {event_name}(event));\n"
    core_bind_impl_str += "}"
    core_bind_impl.append(core_bind_impl_str)

    # virtual callback func definition for the module class
    bot_module_event_defs.append(f"virtual bool {event_name}(const dpp::{event_type_name} &event);")

    # default implementation of the callback func for the module class
    bot_module_event_impl.append(f"bool bot_module::{event_name}(const dpp::{event_type_name} &event)" + " {\n" +
                                 "    return true;\n" +
                                 "}")


# construct events.h file (contains event enum + corresponding string values)
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
                       "//\n" \
                       f"// GENERATED AT: {TIMESTAMP}\n" \
                       "//\n" \
                       f"{events_header_content}"

with open("libprotobot/include/protobot/events.h", "w") as f:
    f.write(event_header_content)


# insert event bind definitions to client.h
with open("libprotobot/include/protobot/client.h") as f:
    client_h_content = f.read()
new_client_h_content = ""

client_event_bind_def_pattern = re.compile(
    r"\s+//\s<CLIENT_EVENT_BIND_DEF>\n((.|\n)*)\n\s+//\s</CLIENT_EVENT_BIND_DEF>"
)
for entry in client_event_bind_def_pattern.finditer(client_h_content):
    replace_start = entry.span(1)[0]
    replace_end = entry.span(1)[1]
    new_client_h_content += client_h_content[:replace_start] + "    //\n" + \
                            f"    // GENERATED AT: {TIMESTAMP}\n" + \
                            "    //\n" + "    "
    new_client_h_content += "\n    ".join(core_bind_defs)
    new_client_h_content += client_h_content[replace_end:]

client_h_content = new_client_h_content
if len(client_h_content) > 1:
    with open("libprotobot/include/protobot/client.h", "w") as f:
        f.write(client_h_content)


# insert event binding info to ProtoBot client.cpp
with open("libprotobot/client.cpp") as f:
    client_cpp_content = f.read()
new_client_cpp_content = ""

client_event_binds_pattern = re.compile(r"\s+//\s<CLIENT_EVENT_BINDS>\n((.|\n)*)\n\s+//\s</CLIENT_EVENT_BINDS>")
for entry in client_event_binds_pattern.finditer(client_cpp_content):
    replace_start = entry.span(1)[0]
    replace_end = entry.span(1)[1]
    new_client_cpp_content += client_cpp_content[:replace_start] + "    //\n" + \
                              f"    // GENERATED AT: {TIMESTAMP}\n" + \
                              "    //\n" + "    "
    new_client_cpp_content += "\n    ".join(core_binds)
    new_client_cpp_content += client_cpp_content[replace_end:]

client_cpp_content = new_client_cpp_content
new_client_cpp_content = ""

client_event_bind_impl_pattern = re.compile(r"//\s<CLIENT_EVENT_BIND_IMPL>\n((.|\n)*)\n//\s</CLIENT_EVENT_BIND_IMPL>")
for entry in client_event_bind_impl_pattern.finditer(client_cpp_content):
    replace_start = entry.span(1)[0]
    replace_end = entry.span(1)[1]
    new_client_cpp_content += client_cpp_content[:replace_start] + "//\n" + \
                              f"// GENERATED AT: {TIMESTAMP}\n" + \
                              "//\n"
    new_client_cpp_content += "\n\n".join(core_bind_impl)
    new_client_cpp_content += client_cpp_content[replace_end:]

client_cpp_content = new_client_cpp_content

if len(client_cpp_content) > 1:
    with open("libprotobot/client.cpp", "w") as f:
        f.write(client_cpp_content)


# add event callback defs to module.h
with open("libprotobot/include/protobot/module.h") as f:
    module_h_content = f.read()
new_module_h_content = ""

client_event_bind_def_pattern = re.compile(r"\s+//\s<BOT_MODULE_EVENT_DEF>\n((.|\n)*)\n\s+//\s</BOT_MODULE_EVENT_DEF>")
for entry in client_event_bind_def_pattern.finditer(module_h_content):
    replace_start = entry.span(1)[0]
    replace_end = entry.span(1)[1]
    new_module_h_content += module_h_content[:replace_start] + "    //\n" + \
                            f"    // GENERATED AT: {TIMESTAMP}\n" + \
                            "    //\n" + "    "
    new_module_h_content += "\n    ".join(bot_module_event_defs)
    new_module_h_content += module_h_content[replace_end:]

module_h_content = new_module_h_content
if len(module_h_content) > 1:
    with open("libprotobot/include/protobot/module.h", "w") as f:
        f.write(module_h_content)


# add event callback implementations to module.cpp
with open("libprotobot/module.cpp") as f:
    module_cpp_content = f.read()
new_module_cpp_content = ""

client_event_bind_def_pattern = re.compile(r"//\s<BOT_MODULE_EVENT_IMPL>\n((.|\n)*)\n//\s</BOT_MODULE_EVENT_IMPL>")
for entry in client_event_bind_def_pattern.finditer(module_cpp_content):
    replace_start = entry.span(1)[0]
    replace_end = entry.span(1)[1]
    new_module_cpp_content += module_cpp_content[:replace_start] + "//\n" + \
                              f"// GENERATED AT: {TIMESTAMP}\n" + \
                              "//\n"
    new_module_cpp_content += "\n\n".join(bot_module_event_impl)
    new_module_cpp_content += module_cpp_content[replace_end:]

module_cpp_content = new_module_cpp_content
if len(module_cpp_content) > 1:
    with open("libprotobot/module.cpp", "w") as f:
        f.write(module_cpp_content)
