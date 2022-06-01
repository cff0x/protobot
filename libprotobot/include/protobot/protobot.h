#ifndef PROTOBOT_PROTOBOT_H
#define PROTOBOT_PROTOBOT_H

#include <dpp/dpp.h>
#include <dpp/fmt/format.h>
#include <dpp/nlohmann/json.hpp>
#include <dpp/discordevents.h>
#include <dpp/message.h>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>


#include <string>
#include <vector>

#define ANGELSCRIPT_INTEGRATION

using json = nlohmann::json;

#endif //PROTOBOT_PROTOBOT_H
