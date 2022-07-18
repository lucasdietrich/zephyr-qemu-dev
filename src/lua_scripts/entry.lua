require("math")
require("io")

-- pseudo random number generator
for i=1,10 do
    io.write(math.random( 0, 100 ) .. " ")
end

print("ok")

-- calling another lua script
dofile("/lfs/helloworld.lua")
dofile("/lfs/math.lua")