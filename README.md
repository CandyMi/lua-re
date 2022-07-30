# lua-re

  Lua regex library
  
 # Install
 
  1. Clone to the `3rd` directory.
  
  2. Enter the `make` command to view the compilation method.
  
 # Example
 
```lua
require "utils"

local re = require "re"

local text = "我数123,321木头人!😃"

-- -- 精确匹配UTF-8
local pattern = re.charpattern
-- local pattern = '(.)'

-- 根据`pattern`查找匹配的对象位置
print(re.find(text, pattern))

-- 根据`pattern`匹配指定的对象
print(re.match(text, pattern))

-- 根据`pattern`分割对象到`数组`.
var_dump(re.split(text, pattern))
```
