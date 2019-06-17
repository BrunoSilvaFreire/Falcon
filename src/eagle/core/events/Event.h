//
// Created by Novak on 05/06/2019.
// Code HEAVILY based on Hazel events: https://github.com/TheCherno/Hazel
//

#ifndef EAGLE_EVENT_H
#define EAGLE_EVENT_H

#include <functional>

#include "eagle/core/Core.h"

_EAGLE_BEGIN

enum class EVENT_TYPE {
    NONE = 0,
    WINDOW_CLOSE, WINDOW_RESIZE, WINDOW_FOCUS, WINDOW_LOST_FOCUS, WINDOW_MOVED,
    KEY_PRESSED, KEY_RELEASED, KEY_TYPED,
    MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED, MOUSE_MOVE, MOUSE_SCROLLED,
    CUSTOM
};

enum EVENT_CATEGORY {
    NONE                        = 0,
    EVENT_CATEGORY_WINDOW       = BIT(0),
    EVENT_CATEGORY_INPUT        = BIT(1),
    EVENT_CATEGORY_KEYBOARD     = BIT(2),
    EVENT_CATEGORY_MOUSE        = BIT(3),
    EVENT_CATEGORY_MOUSE_BUTTON = BIT(4),
    EVENT_CATEGORY_CUSTOM       = BIT(5)
};

#define EVENT_CLASS_TYPE(type)  static Eagle::EVENT_TYPE get_static_type() { return type; }\
                                virtual Eagle::EVENT_TYPE get_event_type() const override { return get_static_type(); }

#define EVENT_CLASS_CATEGORY(category) virtual int get_category_flags() const override { return category; }

class EventDispatcher;

class Event {
public:
    Event() = default;
    virtual ~Event() = default;

    virtual EVENT_TYPE get_event_type() const = 0;

    virtual int get_category_flags() const = 0;

    bool is_in_category(EVENT_CATEGORY category) {
        return get_category_flags() & category;
    }

    bool is_handled() { return m_handled; }
private:
    friend class EventDispatcher;
    bool m_handled = false;
};

class EventDispatcher {
    template<typename T>
    using PFN_EventHandle = std::function<bool(T &)>;
public:
    explicit EventDispatcher(Event &event)
            : m_Event(event) {
    }

    template<typename T>
    bool dispatch(PFN_EventHandle<T> func) {
        if (m_Event.get_event_type() == T::get_static_type()) {
            m_Event.m_handled = func(*(T *) &m_Event);
            return true;
        }
        return false;
    }

private:
    Event &m_Event;
};

_EAGLE_END

#endif //EAGLE_EVENT_H
