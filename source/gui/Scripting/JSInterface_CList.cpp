/* Copyright (C) 2020 Wildfire Games.
 * This file is part of 0 A.D.
 *
 * 0 A.D. is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * 0 A.D. is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 0 A.D.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "precompiled.h"

#include "JSInterface_GUIProxy.h"

#include "gui/CGUI.h"
#include "gui/CGUISetting.h"
#include "gui/ObjectBases/IGUIObject.h"
#include "gui/ObjectTypes/CList.h"
#include "ps/CLogger.h"
#include "scriptinterface/ScriptExtraHeaders.h"
#include "scriptinterface/ScriptInterface.h"

#include <string>

// Include the definition of the generic templates.
#include "JSInterface_GUIProxy_impl.h"

namespace
{
struct SData
{
	JS::PersistentRootedObject m_ToString;
	JS::PersistentRootedObject m_Focus;
	JS::PersistentRootedObject m_Blur;
	JS::PersistentRootedObject m_GetComputedSize;
	JS::PersistentRootedObject m_AddItem;
};

bool CList_AddItem(JSContext* cx, uint argc, JS::Value* vp)
{
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	CList* e = static_cast<CList*>(js::GetProxyPrivate(args.thisv().toObjectOrNull()).toPrivate());
	if (!e)
		return false;

	ScriptInterface& scriptInterface = *ScriptInterface::GetScriptInterfaceAndCBData(cx)->pScriptInterface;
	ScriptRequest rq(scriptInterface);
	CGUIString text;
	if (!ScriptInterface::FromJSVal(rq, args.get(0), text))
		return false;
	e->AddItem(text, text);
	return true;
}
}

template <>
bool JSI_GUIProxy<CList>::FuncGetter(JS::HandleObject proxy, const std::string& propName, JS::MutableHandleValue vp) const
{
	const SData& data = *static_cast<const SData*>(js::GetProxyReservedSlot(proxy, 0).toPrivate());
	if (propName == "toString")
		return vp.setObjectOrNull(data.m_ToString), true;
	if (propName == "focus")
		return vp.setObjectOrNull(data.m_Focus), true;
	if (propName == "blur")
		return vp.setObjectOrNull(data.m_Blur), true;
	if (propName == "getComputedSize")
		return vp.setObjectOrNull(data.m_GetComputedSize), true;
	if (propName == "addItem")
		return vp.setObjectOrNull(data.m_AddItem), true;
	return false;
}

template <>
std::pair<const js::BaseProxyHandler*, void*> JSI_GUIProxy<CList>::CreateData(ScriptInterface& scriptInterface)
{
	SData* data = new SData();
	ScriptRequest rq(scriptInterface);

#define func(class, func) &apply_to<CList, class, &class::func>
	data->m_ToString.init(rq.cx, JS_GetFunctionObject(JS_NewFunction(rq.cx, func(IGUIObject, toString), 0, 0, "toString")));
	data->m_Focus.init(rq.cx, JS_GetFunctionObject(JS_NewFunction(rq.cx, func(IGUIObject, focus), 0, 0, "focus")));
	data->m_Blur.init(rq.cx, JS_GetFunctionObject(JS_NewFunction(rq.cx, func(IGUIObject, blur), 0, 0, "blur")));
	data->m_GetComputedSize.init(rq.cx, JS_GetFunctionObject(JS_NewFunction(rq.cx, func(IGUIObject, getComputedSize), 0, 0, "getComputedSize")));
#undef func
	data->m_AddItem.init(rq.cx, JS_GetFunctionObject(JS_NewFunction(rq.cx, CList_AddItem, 1, 0, "addItem")));

	return { &Singleton(), data };
}

template class JSI_GUIProxy<CList>;
