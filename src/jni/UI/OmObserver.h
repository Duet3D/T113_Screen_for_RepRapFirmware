/*
 * UserInterface.h
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc & Andy Everitt
 */

#ifndef JNI_UI_OMOBSERVER_HPP_
#define JNI_UI_OMOBSERVER_HPP_

#include "Comm/Communication.h"

#include "Comm/JsonDecoder.h"
#include "ObjectModel/Utils.h"
#include <Duet3D/General/String.h>
#include <Duet3D/General/StringFunctions.h>
#include <map>
#include <vector>

#include "Debug.h"

#define OBSERVER_CHAR_ARGS Comm::JsonDecoder *decoder, const char *val, const size_t indices[]
#define OBSERVER_FLOAT_ARGS Comm::JsonDecoder *decoder, const float &val, const size_t indices[]
#define OBSERVER_INT_ARGS Comm::JsonDecoder *decoder, const int32_t &val, const size_t indices[]
#define OBSERVER_UINT_ARGS Comm::JsonDecoder *decoder, const uint32_t &val, const size_t indices[]
#define OBSERVER_BOOL_ARGS Comm::JsonDecoder *decoder, const bool &val, const size_t indices[]
#define OBSERVER_ARRAY_END_ARGS Comm::JsonDecoder *decoder, const size_t indices[]

#define OBSERVER_ARRAY_END(key, callback)                                                                              \
	UI::Observer<UI::ui_array_end_update_cb>                                                                           \
	{                                                                                                                  \
		key, [](Comm::JsonDecoder* decoder, const size_t indices[]) { callback(decoder, indices); },                   \
			UI::g_omArrayEndObserverHead,                                                                              \
	}

#define OBSERVER_TEMPLATE(key, callback, type, convertor)                                                              \
	UI::Observer<UI::ui_field_update_cb>                                                                               \
	{                                                                                                                  \
		key,                                                                                                           \
			[](Comm::JsonDecoder* decoder, const char* data, const size_t indices[]) {                                 \
				type val;                                                                                              \
				if (convertor(data, val))                                                                              \
				{                                                                                                      \
					callback(decoder, val, indices);                                                                   \
				}                                                                                                      \
			},                                                                                                         \
			UI::g_omFieldObserverHead,                                                                                 \
	}

#define OBSERVER_CHAR(key, callback)                                                                                   \
	UI::Observer<UI::ui_field_update_cb>                                                                               \
	{                                                                                                                  \
		key, callback, UI::g_omFieldObserverHead,                                                                      \
	}
#define OBSERVER_FLOAT(key, callback) OBSERVER_TEMPLATE(key, callback, float, Comm::GetFloat)
#define OBSERVER_INT(key, callback) OBSERVER_TEMPLATE(key, callback, int32_t, Comm::GetInteger)
#define OBSERVER_UINT(key, callback) OBSERVER_TEMPLATE(key, callback, uint32_t, Comm::GetUnsignedInteger)
#define OBSERVER_BOOL(key, callback) OBSERVER_TEMPLATE(key, callback, bool, Comm::GetBool)

#define OBSERVER_IF_CHANGED_TEMPLATE(key, callback, type, convertor)                                                   \
	OBSERVER_TEMPLATE(key,                                                                                             \
					  ([](Comm::JsonDecoder* decoder, const type val, const size_t indices[]) {                        \
						  int32_t packedIndex = 0;                                                                     \
						  packedIndex |= (static_cast<int32_t>(indices[0]) & 0xFF) << 24;                              \
						  packedIndex |= (static_cast<int32_t>(indices[1]) & 0xFF) << 16;                              \
						  packedIndex |= (static_cast<int32_t>(indices[2]) & 0xFF) << 8;                               \
						  packedIndex |= (static_cast<int32_t>(indices[3]) & 0xFF);                                    \
						  static std::map<int32_t, type> prevValMap;                                                   \
						  auto& prevVal = prevValMap[packedIndex];                                                     \
						  if (val != prevVal)                                                                          \
						  {                                                                                            \
							  prevVal = val;                                                                           \
							  callback(decoder, val, indices);                                                         \
						  }                                                                                            \
						  else                                                                                         \
						  {                                                                                            \
							  verbose("key %s[%d|%d|%d|%d] hasn't changed, skipping update",                           \
									  key,                                                                             \
									  indices[0],                                                                      \
									  indices[1],                                                                      \
									  indices[2],                                                                      \
									  indices[3]);                                                                     \
						  }                                                                                            \
					  }),                                                                                              \
					  type,                                                                                            \
					  convertor)

#define OBSERVER_FLOAT_IF_CHANGED(key, callback) OBSERVER_IF_CHANGED_TEMPLATE(key, callback, float, Comm::GetFloat)
#define OBSERVER_INT_IF_CHANGED(key, callback) OBSERVER_IF_CHANGED_TEMPLATE(key, callback, int32_t, Comm::GetInteger)
#define OBSERVER_UINT_IF_CHANGED(key, callback)                                                                        \
	OBSERVER_IF_CHANGED_TEMPLATE(key, callback, uint32_t, Comm::GetUnsignedInteger)
#define OBSERVER_BOOL_IF_CHANGED(key, callback) OBSERVER_IF_CHANGED_TEMPLATE(key, callback, bool, Comm::GetBool)

namespace UI
{
	typedef void (*ui_field_update_cb)(Comm::JsonDecoder* decoder, const char* data, const size_t arrayIndices[]);
	typedef void (*ui_array_end_update_cb)(Comm::JsonDecoder* decoder, const size_t arrayIndices[]);

	template <typename cbType>
	class ObserverMap;

	template <typename cbType>
	class Observer
	{
	  public:
		Observer(const char* key, cbType cb, Observer*& head) : m_key(key), m_cb(cb)
		{
			next = head;
			head = this;
		}
		void Init(ObserverMap<cbType>& observerMap) { observerMap.RegisterObserver(m_key, *this); }
		void Update(Comm::JsonDecoder* decoder, const size_t arrayIndices[])
		{
			if (m_cb != nullptr)
			{
				m_cb(decoder, arrayIndices);
			}
		}
		void Update(Comm::JsonDecoder* decoder, const char data[], const size_t arrayIndices[])
		{
			if (m_cb != nullptr)
			{
				m_cb(decoder, data, arrayIndices);
			}
		}
		const char* GetKey() { return m_key; }

		Observer* next;
		static Observer* s_head;

	  private:
		const char* m_key;
		cbType m_cb;
	};

	template <typename cbType>
	class ObserverMap
	{
	  public:
		void RegisterObserver(const char* key, const Observer<cbType>& observer)
		{
			auto& observerList = m_observersMap[key];
			observerList.push_back(observer);

			dbg("%d observers registered against key \"%s\"", observerList.size(), key);
		}
		const std::vector<Observer<cbType>>& GetObservers(const char* key) const
		{
			static const std::vector<Observer<cbType>> emptyVector; // Return an empty vector if key not found
			auto it = m_observersMap.find(key);
			return (it != m_observersMap.end()) ? it->second : emptyVector;
		}

	  private:
		std::map<const char*, std::vector<Observer<cbType>>, ConstCharComparator> m_observersMap;
	};

	extern Observer<ui_field_update_cb>* g_omFieldObserverHead;
	extern Observer<ui_array_end_update_cb>* g_omArrayEndObserverHead;

	extern ObserverMap<ui_field_update_cb> g_observerMap;
	extern ObserverMap<ui_array_end_update_cb> g_observerMapArrayEnd;
} // namespace UI

#endif /* JNI_UI_OMOBSERVER_HPP_ */
