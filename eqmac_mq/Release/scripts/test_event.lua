function Event_CEverQuest__InterpretCmd()
    eq.WriteStringToChat("CEverQuest::InterpretCmd: " .. Event_CEverQuest__InterpretCmd_Text)

    if Event_CEverQuest__InterpretCmd_Text == "/sot" then
        eq.WriteStringToChat("return 1 for /sot")
        return 1
    end

    return 0
end

function Event_CEverQuest__dsp_chat()
    if string.find(string.lower(Event_CEverQuest__dsp_chat_Text), "wts") then
        local file = io.open("auctions.txt", "a")
        file:write(os.date() .. ": " .. Event_CEverQuest__dsp_chat_Text .. "\n")
        file:flush()
        file:close()
    end

    return 0
end