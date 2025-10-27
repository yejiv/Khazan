#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CEvent_Manager final : public CBase
{
public:
	struct Handler {
		_uint iId;
		function<void(const void*)> fn_erased;
	};

	typedef struct IChannel {
		virtual ~IChannel() = default;
		virtual type_index PayloadType() const = 0;
		virtual void Emitptr(const void* pPayload) = 0;
		virtual _bool Unsubscribe(_uint iListenerId) = 0;
		virtual void Clear() = 0;
	};

public:
	template<typename T>
	struct Channel final : IChannel {
		vector<Handler> handlers;

		type_index PayloadType() const override { return typeid(T); }

		void EmitPtr(const void* pPayload) override {
			const T& data = *static_cast<const T*>(pPayload);
			for (auto& Handler : handlers) if (Handler.fn_erased) {
				try { Handler.fn_erased(&data); }
				catch (...) {}
			}
		}
		bool Unsubscribe(_uint iListenerId) override {
			for (auto it = handlers.begin(); it != handlers.end(); ++it)
				if (it->id == iListenerId) { handlers.erase(it); return true; }
			return false;
		}
		void Clear() override { handlers.clear(); }
	};

private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

public:
	template<typename T>
	_uint Subscribe(_uint iEventType, function<void(const T&)> fn) {
		Channel<T>* channel = GetOrCreateChannel<T>(iEventType);
		const _uint id = ++m_iNextListenerID;

		Handler handler;
		handler.iId = id;
		handler.fn_erased = [function = move(fn)](const void* p) {
			function(*static_cast<const T*>(p));
			};
		channel->handlers.push_back(move(handler));

		return id;
	}

	template<typename T>
	HRESULT Emit(_uint iEventType, const T& payload) {
		IChannel* Channel = FindChannel(iEventType);
		if (!Channel) return E_FAIL;
		if (Channel->PayloadType() != type_index(typeid(T))) {
			return E_FAIL;
		}
		Channel->Emitptr(&payload);
		return S_OK;
	}
	
	_bool Unsubscribe(_uint iEventType, _uint iListenerId);
	void UnsubscribeAll(_uint iEventType);
	void ClearAll();

private:
	IChannel* FindChannel(_uint eventType) {
		auto it = m_Channels.find(eventType);
		return (it == m_Channels.end()) ? nullptr : it->second;
	}

	template<typename T>
	Channel<T>* GetOrCreateChannel(_uint eventType) {
		auto it = m_Channels.find(eventType);
		if (it == m_Channels.end()) {
			auto* ch = new Channel<T>();
			m_Channels.emplace(eventType, ch);
			return ch;
		}
		if (it->second->PayloadType() != type_index(typeid(T))) {

		}
		return static_cast<Channel<T>*>(it->second);
	}

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	unordered_map<_uint, IChannel*> m_Channels;
	_uint m_iNextListenerID = 0;

public:
	static CEvent_Manager* Create();
	virtual void Free() override;
};

NS_END