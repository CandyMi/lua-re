local lre = require "lre"
local lre_find    = lre.find
local lre_split   = lre.split
local lre_match   = lre.match
local lre_compile = lre.compile

local type = type
local strgsub = string.gsub
local toint = math.tointeger
local tconcat = table.concat

local cache = {}

---comment 自动重用所有已编过的`regex`对象.
local function load_cache(pattern)
  local p = cache[pattern]
  if not p then
    local errinfo
    p, errinfo = lre_compile(pattern)
    if not p then
      error(errinfo, 3)
    end
    cache[pattern] = p
  end
  return p
end

local re = {}

-- 用于匹配到一个`UTF-8`字节序列, `POSIX`实现必须使用它才能正确匹配`Unicode`.
-- 如果使用`PCRE`或`PCRE2`并且支持`Unicode`, 则可以用`(.)`来替代此表达式.
re.charpattern = "([^\x80-\xFF]|[\xC2-\xFD][\x80-\xBF]+)"

---comment 根据`pattern`表达式, 查找`text`并且匹配; 返回`pattern`在`text`内的起始位置
---@param text     string     @待匹配的文本内容
---@param pattern  string     @合法的`POSIX`正则表达式或`PCRE`表达式
---@param position integer?   @指定开始查找或匹配`text`的下标位置, 注意: 默认开始下标是`1`
function re.find(text, pattern, position)
  if type(text) ~= 'string' or type(pattern) ~= 'string' then
    error("[regex error]: Wrong parameter type.", 2)
  end
  return lre_find(load_cache(pattern), text, (toint(position) or 0) - 1)
end

---comment 根据`pattern`表达式, 查找`text`并且匹配; 内容存放在`table`(数组)里.
---@param text     string     @待匹配的文本内容
---@param pattern  string     @合法的`POSIX`正则表达式或`PCRE`表达式
---@param position integer?   @指定开始查找或匹配`text`的下标位置, 注意: 默认开始下标是`1`
function re.split(text, pattern, position)
  if type(text) ~= 'string' or type(pattern) ~= 'string' then
    error("[regex error]: Wrong parameter type.", 2)
  end
  return lre_split(load_cache(pattern), text, (toint(position) or 0) - 1)
end

---comment 根据`pattern`表达式, 查找`text`并且匹配; `pattern`捕获的内容会直接返回.(最多32个返回值)
---@param text     string     @待匹配的文本内容
---@param pattern  string     @合法的`POSIX`正则表达式或`PCRE`表达式
---@param position integer?   @指定开始查找或匹配`text`的下标位置, 注意: 默认开始下标是`1`
function re.match(text, pattern, position)
  if type(text) ~= 'string' or type(pattern) ~= 'string' then
    error("[regex error]: Wrong parameter type.", 2)
  end
  return lre_match(load_cache(pattern), text, (toint(position) or 0) - 1)
end

---comment 刷新所有缓存
function re.flush()
  cache = {}
end

return re
