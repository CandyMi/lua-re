#define LUA_LIB

#define META_NAME "lua_Regex"

#include <core.h>

#if defined(USE_PCRE)
  #include <pcreposix.h>
#elif defined(USE_PCRE2)
  #include <pcre2posix.h>
#else
  #include <regex.h>
#endif

int lre_find(lua_State *L) {
  regex_t *re = luaL_checkudata(L, 1, META_NAME);
  size_t tsize;
  const char* text =  luaL_checklstring(L, 2, &tsize);
  if (!text || tsize < 1)
    return luaL_error(L, "[regex error]: The empty text content.");

  ssize_t pos = luaL_optinteger(L, 3, 0);
  if (pos < 0)
    pos = 0;
  text += pos; tsize -= pos;

  regmatch_t mlist[2];

  if (regexec(re, text, 1, mlist, 0))
    return 0;
  
  if(mlist[0].rm_so < 0 || mlist[0].rm_eo < 0)
    return 0;

  lua_pushinteger(L, mlist[0].rm_so + 1);
  lua_pushinteger(L, mlist[0].rm_eo);
  return 2;
}


int lre_split(lua_State *L) {
  regex_t *re = luaL_checkudata(L, 1, META_NAME);
  size_t tsize;
  const char* text =  luaL_checklstring(L, 2, &tsize);
  if (!text || tsize < 1)
    return luaL_error(L, "[regex error]: The empty text content.");

  ssize_t pos = luaL_optinteger(L, 3, 0);
  if (pos < 0)
    pos = 0;
  text += pos; tsize -= pos;

  lua_newtable(L); lua_Integer i = 1;
  regmatch_t mlist[2];
  while (!regexec(re, text, 1, mlist, 0))
  {
    // printf("s = %d, e = %d\n", mlist[0].rm_so, mlist[0].rm_eo);
    if(mlist[0].rm_so < 0 || mlist[0].rm_eo < 0)
      break;
    if (mlist[0].rm_so == mlist[0].rm_eo && mlist[0].rm_eo == 0) {
      if (!tsize)
        break;
      text++; tsize--; continue;
    }
    lua_pushlstring(L, text + mlist[0].rm_so, mlist[0].rm_eo - mlist[0].rm_so);
    lua_rawseti(L, -2, i++);
    text += mlist[0].rm_eo; tsize -= mlist[0].rm_eo;
  }
  return 1;
}

int lre_match(lua_State *L) {
  regex_t *re = luaL_checkudata(L, 1, META_NAME);
  size_t tsize;
  const char* text =  luaL_checklstring(L, 2, &tsize);
  if (!text || tsize < 1)
    return luaL_error(L, "[regex error]: The empty text content.");

  ssize_t pos = luaL_optinteger(L, 3, 0);
  if (pos < 0)
    pos = 0;
  text += pos; tsize -= pos;

  regmatch_t mlist[33];
  if (regexec(re, text, 33, mlist, 0))
    return 0;

  int i; int ret = 0;
  for (i = 1; i <= 32; i++) {
    if(mlist[i].rm_so < 0 || mlist[i].rm_eo < 0)
      break;
    lua_pushlstring(L, text + mlist[i].rm_so, mlist[i].rm_eo - mlist[i].rm_so);
    ret = ret + 1;
  }
  /* 检查是否要扩充栈大小 */
  if (ret > LUA_MINSTACK)
    luaL_checkstack (L, ret, "[regex error]: Too many match fields.");
  return ret;
}

int lre_compile(lua_State *L) {
  size_t psize;
  const char* pattern =  luaL_checklstring(L, 1, &psize);
  if (!pattern || psize < 1) {
    lua_pushboolean(L, 0);
    lua_pushfstring(L, "[regex error]: Invalid `pattern`.");
    return 2;
  }

  int cflags = REG_EXTENDED;
#if defined(REG_UTF8)
  cflags |= REG_UTF8 | REG_UCP;
#elif defined(REG_UTF)
  cflags |= REG_UTF | REG_UCP;
#endif

  regex_t *re = lua_newuserdata(L, sizeof(regex_t));
  int code = regcomp(re, pattern, cflags);
  if (code) {
    char buffer[] = { [0 ... 128] = 0 } ;
    regerror(code, re, buffer, 128);
    lua_pushboolean(L, 0);
    lua_pushfstring(L, "[regex error]: %s", buffer);
    return 2;
  }

  luaL_setmetatable(L, META_NAME);
  return 1;
}

int lre_free(lua_State *L) {
  regex_t *re = luaL_checkudata(L, 1, META_NAME);
  if (re)
    regfree(re);
  return 1;
}

LUAMOD_API int luaopen_lre(lua_State *L) {
  luaL_checkversion(L);
  /* metatable */
  luaL_newmetatable(L, META_NAME);
  lua_pushcfunction(L, lre_free);
  lua_setfield(L, -2, "__gc");
  /* methods */
  luaL_Reg re_libs[] = {
    {"find",      lre_find},
    {"match",     lre_match},
    {"split",     lre_split},
    {"compile",   lre_compile},
    {NULL, NULL}
  };
  luaL_newlib(L, re_libs);
  return 1;
}
