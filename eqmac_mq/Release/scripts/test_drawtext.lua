require "scripts.system.eqmac"

eq = require("eq")

function OnDrawNetStatus()
    eq.DrawText("Test EQ_DrawText", 512, 512, EQ_TEXT_COLOR_LIGHT_GREEN)
end