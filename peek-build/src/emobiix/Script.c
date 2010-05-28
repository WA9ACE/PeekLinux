#include "Script.h"
#include "DataObject.h"
#include "ConnectionContext.h"
#include "URL.h"
#include "Debug.h"
#include "Style.h"
#include "Widget.h"
#include "lgui.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static DataObject *contextObject;
extern ConnectionContext *connectionContext;

#define lua_boxpointer(L,u) \
        (*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))

#define lua_unboxpointer(L,i)   (*(void **)(lua_touserdata(L, i)))

static DataObject *checkDataObject (lua_State *L, int index)
{
  luaL_checktype(L, index, LUA_TUSERDATA);
  lua_getmetatable(L, index);
  if( ! lua_equal(L, lua_upvalueindex(1), -1) )
    luaL_typerror(L, index, "DataObject");  /* die */
  lua_pop(L, 1);
  return (DataObject *)lua_unboxpointer(L, index);
}

static DataObject *pushDataObject(lua_State *L, DataObject *dobj)
{
  lua_boxpointer(L, dobj);
  lua_pushvalue(L, lua_upvalueindex(1));
  lua_setmetatable(L, -2);
  return dobj;
}

static int __dataobject_getValue(lua_State *L)
{
	DataObjectField *field;
	DataObject *dobj;

	dobj = checkDataObject(L, 1);
	field = dataobject_getValue(dobj, "data");
        if (field == NULL) {
                lua_pushstring(L, "");
        } else {
               lua_pushstring(L, field->field.string);
        }

	return 1;
}

extern Style *currentStyle;
extern DataObject *currentScreen;

static int __dataobject_setValue(lua_State *L)
{
	DataObject *dobj, *parent;
	DataObjectField *dstr;
	Rectangle rectb4, rectAfter, *rect;

	dobj = checkDataObject(L, 1);
	dstr = dataobjectfield_string(luaL_checkstring(L, 2));

    dataobject_setValue(dobj, "data", dstr);

	parent = dataobject_superparent(dobj);
	if (parent == currentScreen) {
		widget_getClipRectangle(dobj, &rectb4);
		widget_resolveLayout(currentScreen, currentStyle);
		emo_printf("drawing from script setData\n");
		widget_markDirty(dobj);
		lgui_clip_identity();
		widget_getClipRectangle(dobj, &rectAfter);
		if (rectAfter.width > rectb4.width)
			rect = &rectAfter;
		else
			rect = &rectb4;
		lgui_clip_set(rect);
		lgui_push_region();
		style_renderWidgetTree(currentStyle, parent);
#if 0
		lgui_box(rect->x, rect->y, rect->width, rect->height, 1, 0xFF, 0, 0);
#endif
	}

    return 1;
}

static int __dataobject_locate(lua_State *L)
{
	const char *durl;
	DataObject *dobj;
	URL *purl;

	durl = luaL_checkstring(L, 1);
	purl = url_parse(durl, URL_ALL);
	if (purl == NULL)
		return 0;
	dobj = dataobject_locate(purl);
	if (dobj == NULL) {
		connectionContext_syncRequest(connectionContext, purl);
		dobj = dataobject_locate(purl);
	}
	/*url_delete(purl);*/
	pushDataObject(L, dobj);

	return 1;
}

static int __dataobject_find(lua_State *L)
{
	DataObject *dobj;
	DataObject *root;

	root = dataobject_superparent(contextObject);
	if(root == NULL) 
		return 0;
	dobj = dataobject_findByName(root, luaL_checkstring(L, 1));
	pushDataObject(L, dobj);
	
	return 1;
}



extern DataObject *currentScreen;

static int __toScreen(lua_State *L)
{
	DataObject *dobj = checkDataObject(L, 1);
	currentScreen = dobj;
	//lua_pushnumber(L, gdImageColorAllocate(im, r, g, b));
	
	return 0;
}

static const luaL_reg meta_methods[] = {
{0,0}
};

static const luaL_reg script_methods[] = {
{"locate",           __dataobject_locate},
{"toScreen", __toScreen},
{"find", 	     __dataobject_find},
{"getValue",	__dataobject_getValue},
{"setValue",    __dataobject_setValue},
{0,0}
};

static const luaL_reg baseFunctions[] = {
	{0, 0}
};

#define newtable(L) (lua_newtable(L), lua_gettop(L))

int script_register (lua_State *L)
{
  int metatable, methods;

  luaL_openlib(L, 0, baseFunctions, 0);
  lua_pushliteral(L, "DataObject");         /* name of Image table */
  methods   = newtable(L);           /* Image methods table */
  metatable = newtable(L);           /* Image metatable */
  lua_pushliteral(L, "__index");     /* add index event to metatable */
  lua_pushvalue(L, methods);
  lua_settable(L, metatable);        /* metatable.__index = methods */
  lua_pushliteral(L, "__metatable"); /* hide metatable */
  lua_pushvalue(L, methods);
  lua_settable(L, metatable);        /* metatable.__metatable = methods */
  luaL_openlib(L, 0, meta_methods,  0); /* fill metatable */
  luaL_openlib(L, 0, script_methods, 1); /* fill Image methods table */
  lua_settable(L, LUA_GLOBALSINDEX); /* add Image to globals */
  return 0;
}

/* for this pointer */
#if 0
lua_getglobal(L,"screenWidth");
       if(lua_isnumber(L,-1))
          width = lua_tonumber(L,-1);
       else
       {  
            printf("Error screenWidth is NAN\n");
            width = 600;
       }
       lua_getglobal(L,"screenHeight");
       if(lua_isnumber(L,-1))
          height = lua_tonumber(L,-1);
       else
       {  
            printf("Error screenHeight is NAN\n");
            height = 480;
       }
  }
#endif

lua_State *script_getContext(DataObject *dobj, int *isTemporary)
{
    lua_State *output;
    DataObject *sobj;
    DataObjectField *script;
	const char *errStr;

    *isTemporary = 0;
    output = dataobject_findScriptContext(dobj);
    if (output == NULL) {
        sobj = dataobject_findFieldParent(dobj, "script");

        output = lua_open();
        luaopen_base(output);
        luaopen_table(output);
        luaopen_string(output);
        luaopen_math(output);
        luaopen_debug(output);

        script_register(output);
        if (sobj != NULL) {
            script = dataobject_getValue(sobj, "script");
            /*luaL_loadstring(output, script->field.string);*/
			luaL_dostring(output, script->field.string);
			
			if (lua_isstring(output, lua_gettop(output))) {
				errStr = lua_tostring(output, lua_gettop(output));
				lua_pop(output, 1);
				emo_printf("script error: %s\n", errStr);
			}

            dataobject_setScriptContext(sobj, output);
			*isTemporary = 0;
        } else
            *isTemporary = 1;
    }
    return output;
}

int script_event(DataObject *context, const char *eventname)
{
	lua_State *L;
	DataObjectField *field;
    int isTemporary;
	const char *errStr;

	field = dataobject_getValue(context, eventname);
	if (field == NULL || field->type != DOF_STRING)
		return 0;

	contextObject = context;

    isTemporary = 1;
    L = script_getContext(context, &isTemporary);

	if (lua_isstring(L, lua_gettop(L))) {
	    errStr = lua_tostring(L, lua_gettop(L));
		lua_pop(L, 1);
		emo_printf("script error: %s\n", errStr);
	}

	luaL_dostring(L, field->field.string);
  
	if (lua_isstring(L, lua_gettop(L))) {
	    errStr = lua_tostring(L, lua_gettop(L));
		lua_pop(L, 1);
		emo_printf("script error: %s\n", errStr);
	}

    if (isTemporary)
        lua_close(L);

	return 1;
}
