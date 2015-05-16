function Event_ProcessKeyUp()
    eq.WriteStringToChat("ProcessKeyUp: " .. Event_ProcessKeyUp_Key)

    if eq.IsNotTypingInChat() == true then
        if Event_ProcessKeyUp_Key == EQ_KEY_M then
            eq.InterpretCommand("/map")
            return 1
        end
    end

    return 0
end