require "scripts.system.eqmac"

eq = require("eq")

function OnInterpretCommand()
    eq.WriteStringToChat("CEverQuest::InterpretCmd: " .. OnInterpretCommand_Text)

    if OnInterpretCommand_Text == "/sot" then
        eq.WriteStringToChat("return 1 for /sot")
        return 1
    end

    return 0
end