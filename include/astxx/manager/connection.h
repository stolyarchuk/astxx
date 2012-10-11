/* vim: set et sw=3 tw=0 fo=croqlaw cino=t0:
 *
 * Astxx, the Asterisk C++ API and Utility Library.
 * Copyright (C) 2005-2007  Matthew A. Nicholson
 * Copyright (C) 2005-2007  Digium, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** @file
 *
 * Don't include this file directly, include astxx/manager.h instead.
 * 
 * This file contains the astxx::manager::connection class which provides an 
 * interface to the Asterisk Manager API.
 */

#ifndef ASTXX_MANAGER_CONNECTION_H
#define ASTXX_MANAGER_CONNECTION_H

#include <astxx/manager/message.h>
#include <astxx/manager/basic_action.h>

#include <queue>
#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

namespace astxx {
   namespace manager {
      /** An Asterisk Manager connection.
       *
       * This class is the main point of access to the manager API.  Once 
       * connected you can send commands to and read events from Asterisk.
       *
       * Commands can be sent using the connection::send_action() or 
       * connection::send_action_async() function.  The 
       * connection::send_action() function blocks and waits for asterisk to 
       * send a response before returning.  The '()' operator is overloaded to 
       * behave as both of these functions depending on which arguments are 
       * passed to it.
       *
       * @code
       * action::logoff logoff;
       * message::response r = logoff(connection); // action::logoff will throw
       *                                           // exceptions for 'Error'
       *                                           // responses from Asterisk
       * @endcode
       *
       * @code
       * message::response r = connection(action::logoff()); // no processing
       *                                                     // of the response
       *                                                     // will be done
       * @endcode
       *
       * @code
       * async_response_handler handler();
       * action::logoff logoff;
       * connection(logoff, handler);
       * @endcode
       *
       * To recieve events you must register an event handler (via 
       * connection::register_event()) then trigger the handler by calling 
       * connection::process_events() (optionally after 
       * connection::wait_event() which will wait for an event and put it in 
       * the event queue if the queue is empty).  The handler can be a 
       * function, functor, or member function that takes message::event object 
       * as a parameter.  To unregister an event use the 
       * boost::signals::connection::disconnect() method from the 
       * boost::signals::connection object returned by 
       * connection::register_event().
       *
       * Your main application loop could look something like this:
       * @code
       * connection.wait_event();
       * connection.pump_messages();
       * connection.process_events();
       *
       * connection.process_responses(); // if you are sending actions asychronously.
       * @endcode
       *
       * @warning This library is not thread safe.  Only one thread of 
       * execution should call functions in the library at a time.
       */
      class connection {
         private:
            std::pair<std::string, std::string> parse_header(const std::string& header);
            std::string read_line();
            void read_message();
            message::response read_response();
            message::event read_event();

         public:
            typedef boost::function<void (message::response)> response_handler_t;
            typedef boost::function<void (message::event)> event_handler_t;
            typedef std::queue<message::event> events_t;
            typedef std::queue<message::response> responses_t;
            typedef std::queue<response_handler_t> response_handlers_t;
            typedef std::map<std::string, boost::shared_ptr<boost::signal<void (message::event)> > > event_handlers_t;

            connection(const std::string& host, unsigned short port = 5038);

            void connect(const std::string& host = "", unsigned short port = 0);
            void disconnect();
            bool is_connected() const;

            /** Get the name of the manager we are connected to.
             */
            std::string name() const { return m_name; }
            /** Get the version string for the manager we are connected to.
             */
            std::string version() const { return m_version; }

            message::response send_action(const manager::basic_action& command);
            void send_action_async(const manager::basic_action& command, response_handler_t handler);

            message::response operator()(const manager::basic_action& command);
            void operator()(const manager::basic_action& command, response_handler_t handler);

            void process_events();
            void wait_event();

            void process_responses();
            void wait_response();

            void pump_messages();

            boost::signals::connection register_event(const std::string& e, boost::function<void (message::event)> f);

         private:
            boost::asio::io_service io_service;
            boost::asio::ip::tcp::socket socket;

            std::string m_name;
            std::string m_version;

            std::string m_host;
            std::string m_port;

            events_t events;
            responses_t responses;
            response_handlers_t response_handlers;
            event_handlers_t event_handlers;
      };
   }
}

#endif
