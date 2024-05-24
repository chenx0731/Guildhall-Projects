#pragma once
#include "Engine/Core/NamedStrings.hpp"
#include <vector>
#include <string>
#include <map>
#include <mutex>

struct EventSystemConfig
{

};

typedef NamedStrings EventArgs;
typedef bool (*EventCallbackFunction)(EventArgs& args);

class EventRecipient
{
public:
	~EventRecipient();
};

class EventSubscriptionBase
{
	friend class EventSystem;
protected:
	virtual ~EventSubscriptionBase() = default;
	EventSubscriptionBase() = default;
	virtual bool Execute(EventArgs& args) = 0;
protected:
};

template <typename T>
class EventSubscription_ObjectMethod : public EventSubscriptionBase
{
	friend EventSystem;
	typedef bool (T::* EventCallbackObjectMethod)(EventArgs& args);
protected:
	virtual ~EventSubscription_ObjectMethod() = default;
	EventSubscription_ObjectMethod(T& objectInstance, EventCallbackObjectMethod method) 
		: m_object(objectInstance)
		, m_method(method)
	{}
	virtual bool Execute(EventArgs& args) override
	{
		return (m_object.*m_method)( args );
	}
protected:
	T&							m_object;
	EventCallbackObjectMethod	m_method = nullptr;
};

class EventSubscription_Function : public EventSubscriptionBase
{
	friend class EventSystem;
protected:
	virtual ~EventSubscription_Function() = default;
	EventSubscription_Function(EventCallbackFunction funcPtr) : m_funcPtr(funcPtr) 
	{ }
	virtual bool Execute(EventArgs& args) override
	{
		return m_funcPtr(args);
	}
	//virtual bool Execute(EventArgs& args) override;
protected:
	EventCallbackFunction m_funcPtr = nullptr;
};


typedef std::vector<EventSubscriptionBase*> SubscriptionList;


class EventSystem
{
public:
	EventSystem(EventSystemConfig const& config);
	~EventSystem();
	
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	template<typename T>
	void SubscribeEventCallbackObjectMethod(std::string const& eventName, T& object, bool (T::* method) (EventArgs& args));
	template<typename T>
	void UnsubscribeEventCallbackObjectMethod(std::string const& eventName, T& object, bool (T::* method) (EventArgs& args));
	void UnsubscribeObjectFromAllEvents(void* object);
	void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
	void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
	void FireEvent(std::string const& eventName, EventArgs& args);
	void FireEvent(std::string const& eventName);

	bool CheckIfAEvent(std::string const& eventName);
	Strings GetAllEvents();
	void UnsubscribeAllEvents();
protected:
	EventSystemConfig							m_config;
	std::mutex									m_subListLock;
	std::map<HCIString, SubscriptionList>		m_subscriptionListsByEventName;
};

template<typename T>
void EventSystem::SubscribeEventCallbackObjectMethod(std::string const& eventName, T& object, bool (T::* method) (EventArgs& args))
{
	SubscriptionList& subscribers = m_subscriptionListsByEventName[eventName];
	EventSubscription_ObjectMethod<T>* newSubscriber = new EventSubscription_ObjectMethod<T>(object, method);
	subscribers.push_back(newSubscriber);
}

template<typename T>
void EventSystem::UnsubscribeEventCallbackObjectMethod(std::string const& eventName, T& object, bool (T::* method) (EventArgs& args))
{
	SubscriptionList& subscribers = m_subscriptionListsByEventName[eventName];
	for (int i = 0; i < (int)subscribers.size(); i++)
	{
		//EventSubscription_ObjectMethod<T>* subscriber = subscribers[i];
		EventSubscription_ObjectMethod<T>* subscriber = dynamic_cast<EventSubscription_ObjectMethod<T>*>(subscribers[i]);
		if (subscriber != nullptr && &subscriber->m_object == &object && subscriber->m_method == method)
		{
			delete subscriber;
			subscribers[i] == nullptr;
		}
	}
}

extern EventSystem* g_theEventSystem;

void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
template<typename T>
void SubscribeEventCallbackObjectMethod(std::string const& eventName, T& object, bool (T::* method) (EventArgs& args));

template<typename T>
void SubscribeEventCallbackObjectMethod(std::string const& eventName, T& object, bool (T::* method) (EventArgs& args))
{
	g_theEventSystem->SubscribeEventCallbackObjectMethod(eventName, object, method);
}

template<typename T>
void UnsubscribeEventCallbackObjectMethod(std::string const& eventName, T& object, bool (T::* method) (EventArgs& args));

template<typename T>
void UnsubscribeEventCallbackObjectMethod(std::string const& eventName, T& object, bool (T::* method) (EventArgs& args))
{
	g_theEventSystem->UnsubscribeEventCallbackObjectMethod(eventName, object, method);
}

void UnsubscribeObjectFromAllEvents(void* object);

void FireEvent(std::string const& eventName, EventArgs& args);
void FireEvent(std::string const& eventName);
bool CheckIfAEvent(std::string const& eventName);
Strings GetAllEvents();
void UnsubscribeAllEvents();
