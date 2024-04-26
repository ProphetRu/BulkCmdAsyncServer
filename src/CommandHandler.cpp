#include "CommandHandler.h"
#include <chrono>

constexpr auto BLOCK_BEGIN { "{"   };
constexpr auto BLOCK_END   { "}"   };
constexpr auto EOF_TAG     { "EOF" };

void CommandHandler::Handle(const CommandBlock& block) noexcept
{
	if (!block.IsValid())
	{
		return;
	}

    auto get_current_timestamp = []() noexcept
    {
        return std::to_string(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    };

    StringList commands;
    int deep{};
    std::string timestamp;

    for (const auto& line : block.Commands)
    {
        if (line == BLOCK_BEGIN)
        {
            ++deep;
            if (deep == 1)
            {
                m_LogerAsync.Log({ timestamp, commands });
                commands.clear();
            }
        }
        else if (line == BLOCK_END)
        {
            --deep;
            if (deep == 0)
            {
                m_LogerAsync.Log({ timestamp, commands });
                commands.clear();
            }
        }
        else if (line == EOF_TAG)
        {
            break;
        }
        else
        {
            if (deep == 0)
            {
                if (commands.empty())
                {
                    timestamp = get_current_timestamp();
                }

                commands.emplace_back(line);
                if (commands.size() == block.Bulk)
                {
                    m_LogerAsync.Log({ timestamp, commands });
                    commands.clear();
                }
            }
            else
            {
                commands.emplace_back(line);
            }
        }
    }

    if (!commands.empty() && deep == 0)
    {
        m_LogerAsync.Log({ timestamp, commands });
        commands.clear();
    }
}
