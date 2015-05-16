eq.WriteStringToChat("test")

eq.WriteStringToChatWithColor("test color", EQ_TEXT_COLOR_YELLOW)

local charInfo = eq.GetCharInfo()

if charInfo ~= nil then
    eq.WriteStringToChat("CharInfo Platinum: " .. charInfo.Platinum)
    eq.WriteStringToChat("CharInfo Gold: " .. charInfo.Gold)
    eq.WriteStringToChat("CharInfo Silver: " .. charInfo.Silver)
    eq.WriteStringToChat("CharInfo Copper: " .. charInfo.Copper)
end

local playerSpawnInfo = eq.GetPlayerSpawnInfo()

if playerSpawnInfo ~= nil then
    eq.WriteStringToChat("PlayerSpawnInfo Name: " .. playerSpawnInfo.Name)
    eq.WriteStringToChat("PlayerSpawnInfo Level: " .. playerSpawnInfo.Level)
end

local targetSpawnInfo = eq.GetTargetSpawnInfo()

if targetSpawnInfo ~= nil then
    eq.WriteStringToChat("TargetSpawnInfo Name: " .. targetSpawnInfo.Name)
    eq.WriteStringToChat("TargetSpawnInfo Level: " .. targetSpawnInfo.Level)
end
