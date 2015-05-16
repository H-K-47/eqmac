local oldprint = print
print = function(...)
    eq.WriteStringToChat(...);
end