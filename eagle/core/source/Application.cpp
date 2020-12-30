//
// Created by Novak on 05/05/2019.
//


#include <eagle/core/Time.h>
#include "eagle/core/Log.h"
#include "eagle/core/Application.h"
#include "eagle/core/Window.h"
#include "eagle/core/events/InputEvents.h"
#include "eagle/core/events/WindowEvents.h"

EG_BEGIN

Application* Application::m_instance = nullptr;

Application& Application::instance() {
    return *m_instance;
}

Application::Application(const ApplicationCreateInfo& config) :
    m_window(config.windowType){
    EAGLE_SET_INFO(EAGLE_APP_NAME, config.appName);

    m_instance = this;

    Log::init(config.coreLogLevel, config.clientLogLevel);
    m_layerStack.emplace(config.layers);
}


void Application::run() {

    EG_CORE_TRACE_F("Initializing {0}", EAGLE_GET_INFO(EAGLE_APP_NAME));

    Time::init();

    m_window->init();
    m_layerStack.init(m_window->event_bus());

    while(!m_quit){

        Time::update();
        m_window->pool_events();

        for (auto& layer : m_layerStack){
            layer->handle_update();
        }
    }

    m_layerStack.deinit();

    m_window->deinit();
    EG_CORE_TRACE("Application terminated!");

}

void Application::quit() {
    m_quit = true;
}

EG_END