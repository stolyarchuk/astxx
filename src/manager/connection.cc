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

#include <astxx/manager/connection.h>
#include <astxx/manager/message.h>
#include <astxx/manager/error.h>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

namespace astxx {
   namespace manager {
      using boost::asio::ip::tcp;
      using boost::lexical_cast;

      /** Wait for a response.
       * This is a functior to wait for a response.
       * @code
       * response_waiter rw(connection);
       * connection(action, boost::ref(rw));
       * message::response r = rw.wait();
       * @endcode
       */
      class response_waiter {
         public:
            /** Constructor.
             * @param connection the connection to watch
             */
            response_waiter(manager::connection& connection)
               : connection(connection), response("") {
            }

            /** The response handling function.
             * @param r the response to handle
             */
            void operator()(message::response r) {
               response = r;
            }

            /** Wait for a response for this functor.
             * @warning DO NOT call this function with out first registering 
             * this functor as a response handler.  Doing so will cause an 
             * infinte loop.
             */
            message::response wait() const {
               while (response == "") {
                  connection.wait_response();
                  connection.process_responses();
               }
               return response;
            }
         private:
            manager::connection& connection;
            message::response response;
      };

      /** Initilize a connection to the given host on the given port.
       * @param host the host to connect to
       * @param port the port to connect on
       * @throw boost::system::system_error if there is a problem connecting to 
       * or resolving a host
       */
      connection::connection(const std::string& host, unsigned short port) : socket(io_service) {
         connect(host, port);
      }

      /** Connect to the given host on the given port.
       * @param host the host to connect to
       * @param port the port to connect on
       *
       * @note If host is not given, the host and port given on construction 
       * will be used.  If port is not given, the port given on construction 
       * wll be used.  In order for the given port to be used, host must be 
       * specified.
       *
       * @warn Calling this function will disconnect any existing connections.
       *
       * @throw boost::system::system_error if there is a problem connecting to 
       * or resolving a host
       */
      void connection::connect(const std::string& host, unsigned short port) {
         // update the internal host and port if they are not the default 
         // values
         if (not host.empty()) {
            m_host = host;
            if (port) {
               m_port = lexical_cast<std::string>(port);
            }
         }

         tcp::resolver resolver(io_service);
         tcp::resolver::query query(m_host, lexical_cast<std::string>(m_port));

         tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
         tcp::resolver::iterator end;

         // here we loop through all possible endpoints trying to connect to 
         // each as long as we failed to connect to the previous one
         boost::system::error_code error = boost::asio::error::host_not_found;
         while (error && endpoint_iterator != end) {
            socket.close();
            socket.connect(*endpoint_iterator++, error);
         }

         if (error)
            throw boost::system::system_error(error);

         std::string greeting_line = read_line();
         
         // split the version and the name
         std::string::size_type i = greeting_line.find_last_of('/');
         if (i != std::string::npos) {
            m_name.assign(greeting_line, 0, i);
            if (++i != std::string::npos) {
               m_version.assign(greeting_line, i, std::string::npos);
            }
         }
      }

      /** Close our connection to Asterisk.
       * @note Upon destruction the connection to the manager should be 
       * properly closed automatically.
       */
      void connection::disconnect() {
         socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
         socket.close();
      }

      /** Check if we are connected.
       * @return whether we are connected or not
       */
      bool connection::is_connected() const {
         return socket.is_open();
      }

      /** Send a command to Asterisk.
       * @param command the command to send
       *
       * This function sends a command to Asterisk and also waits for a 
       * response which is then returned.
       *
       * If error handling is desired, pass the result of this function to 
       * basic_action::handle_response() which will throw exceptions for most 
       * errors the manager returns.  The basic_action::operator()() function 
       * does this automatically.
       *
       * @code
       * action::example example;
       * message::response r = example(connection); // send this action over
       *                                            // "connection" and handle
       *                                            // errors before returning
       *                                            // the response
       * @endcode
       *
       * 
       * @note This function blocks while waiting for a response.
       * 
       * @see connection::operator()(const basic_action&, response_handler_t)
       * @see connection::operator()(const basic_action&)
       * @return the response from asterisk
       */
      message::response connection::send_action(const basic_action& command) {
         response_waiter rw(*this);
         send_action_async(command, boost::ref(rw));
         return rw.wait();
      }

      /** Send a command to Asterisk and recieve the response asynchronously.
       * @param command the command to send
       * @param handler the handler function/functor
       *
       * This function sends a command to Asterisk and then returns.  The given 
       * handler will be called with the response to this action when it is 
       * recieved.
       *
       * The handler function pointer/functor will be copied unless you use 
       * boost::ref to prevent it.  You can use boost::bind to pass member 
       * functions as response handlers.
       *
       * The signature of the handler should be:
       * @code
       * void response_handler(message::response r); // r is the response
       * @endcode
       *
       * The connection::process_responses() function triggers response handler 
       * execution.  See its documentation for circumstances when it will be 
       * called.
       *
       * @see connection::send_action()
       * @see connection::operator()(const basic_action&, response_handler_t)
       * @see connection::process_responses()
       * @return the response from asterisk
       */
      void connection::send_action_async(const basic_action& command, response_handler_t handler) {

         message::action action = command.action();
         if (not command.action_id().empty()) {
            action["ActionID"] = command.action_id();
         }

         boost::asio::write(socket, boost::asio::buffer(action.format()));
         response_handlers.push(handler);
      }
      
      /** Send a command to Asterisk.
       * @param command the command to send
       *
       * This function executes connection::send_action().
       */
      message::response connection::operator()(const basic_action& command) {
         return send_action(command);
      }
      
      /** Send a command to Asterisk and recieve the response asynchronously.
       * @param command the command to send
       * @param handler the handler function/functor
       *
       * This function executes connection::send_action_async().
       */
      void connection::operator()(const basic_action& command, response_handler_t handler) {
         send_action_async(command, handler);
      }
      
      /** Parse a colon (:) seperated header.
       * @param header a string representing the header
       * @return an key value pair std::pair
       * @throw manager::parse_error if there is a problem parsing the message
       * @throw manager::empty_header if we attempt to parse and 
       * empty header
       */
      std::pair<std::string, std::string> connection::parse_header(const std::string& header) {
         std::string key;
         std::string value;

         if (header.empty()) {
            throw manager::empty_header();
         }

         std::string::size_type i = header.find_first_of(':');
         if (i == std::string::npos) {
            throw manager::parse_error("missing ':' in header: " + header);
         }

         key.assign(header, 0, i++);
         
         if (i != std::string::npos) {
            if (header[i] == ' ') {
               ++i;
            }
            if (i != std::string::npos) {
               value.assign(header, i, std::string::npos);
            }
         }
         return std::make_pair(key, value);
      }

      /** Read a line from Asterisk.
       * @note This function strips the \r\n off the end of the line
       * @return the line read
       */
      std::string connection::read_line() {
         std::string line;

         // loop reading one character at a time until we find a '\n'
         for (char c = 0, cc = 0; !(c == '\n' && cc == '\r'); line += c) {
            cc = c;
            boost::asio::read(socket, boost::asio::buffer(&c, 1), boost::asio::transfer_at_least(1));
         }

         // strip the \r\n
         std::string::size_type rn = line.find("\r\n");
         if (rn != std::string::npos) {
            line.erase(rn);
         }

         return line;
      }

      /** Read a message from Asterisk.
       * This function reads a message from asterisk and places it in to the 
       * proper queue.
       * @throw manager::parse_error if there is a problem parsing a header
       * @throw manager::unknown_message if the header we recieve is 
       * anything other than 'Event' or 'Response'
       */
      void connection::read_message() {
         std::pair<std::string, std::string> pair = parse_header(read_line());
         if (pair.first == "Event") {
            message::event event(pair.second);
            for (;;) {
               std::string line = read_line();

               // stop if we get a blank line
               if (line.empty()) {
                  break;
               }
               event.insert(parse_header(line));
            }
            // put the event in the queue
            events.push(event);
         }
         else if (pair.first == "Response") {
            message::response response(pair.second);
            for (;;) {
               std::string line = read_line();

               // stop if we get a blank line
               if (line.empty()) {
                  break;
               }

               // check for '--END COMMAND--' if necessary
               if (pair.second == "Follows") {
                  std::string::size_type ec = line.find("--END COMMAND--");

                  // strip the '--END COMMAND--', if found, otherwise this is a 
                  // normal header
                  if (ec != std::string::npos) {
                     line.erase(ec);
                     response.data = line;
                  }
                  else {
                     response.insert(parse_header(line));
                  }

               } else {
                  response.insert(parse_header(line));
               }
            }
            // put the resposne in the queue
            responses.push(response);
         }
         else {
            throw manager::unknown_message(pair.first);
         }
      }
      
      /** Pull a response out of the response queue or from the network.
       * This function will return the resposne at the front of the queue.  If 
       * the queue is empty it will read messages from the network until it 
       * finds a response.
       * @return a response from the queue
       */
      message::response connection::read_response() {
         wait_response();
         message::response response = responses.front();
         responses.pop();
         return response;
      }
      
      /** Pull an event out of the response queue or from the network.
       * This function will return the event at the front of the queue.  If the 
       * queue is empty it will read messages from the network until it finds 
       * an event.
       * @return an event from the queue
       */
      message::event connection::read_event() {
         wait_event();
         message::event event = events.front();
         events.pop();
         return event;
      }

      /** Process all the events in the queue.
       * This function executes all the registered event handlers that match 
       * events in the queue.
       */
      void connection::process_events() {
         /* Here we pop events off of the queue one by one because it is 
          * possible for our handlers to add events to the queue (by executing 
          * an action which adds events while waiting for a response).  By 
          * popping events off one by one, we avoid worring about our iterators 
          * becomming invalid.
          */
         while (not events.empty()) {
            message::event e = events.front();
            events.pop();

            event_handlers_t::iterator i = event_handlers.find(e.main_header());
            if (i != event_handlers.end()) {
               (*i->second)(e);
            }

            // execute the catch all
            i = event_handlers.find("");
            if (i != event_handlers.end()) {
               (*i->second)(e);
            }
         }
      }

      /** Wait for an event and put it in the queue.
       * This function waits for an event.
       * @note If there is alreay an event waiting, this function will not 
       * block.
       */
      void connection::wait_event() {
         while (events.empty()) {
            read_message();
         }
      }

      /** Process asynchronous responses.
       * This function executes handlers for responses waiting.
       *
       * This function is called by connection::send_action() while waiting for 
       * a response.
       */
      void connection::process_responses() {
         /* Here we pop responses off of the queue one by one because it is 
          * possible for our handlers to add responses to the queue (by 
          * executing an action synchronously).  By popping events off one by 
          * one, we avoid worring about our iterators becomming invalid.
          */
         while (not response_handlers.empty() and not responses.empty()) {
            response_handler_t f = response_handlers.front();
            response_handlers.pop();

            f(read_response());
         }
      }

      /** Wait for a response and puts it in the queue.
       * This function waits for a response.
       * @note If there is alreay a response waiting, this function will not 
       * block.
       */
      void connection::wait_response() {
         while (responses.empty()) {
            read_message();
         }
      }
      
      /** Read messages from the network until there is no more data waiting.
       * This function does not block.  It reads messages from the network 
       * until there are no more waiting and places them in the proper queues.  
       * If there are no messages waiting it will simply return.
       */
      void connection::pump_messages() {
         while (socket.available()) {
            read_message();
         }
      }

      /** Register an event handler.
       * @param e the name of the event (case sensitive, use a blank string to 
       * match all events)
       * @param f the callback function pointer or functor
       *
       * This function is used to register an event handler.  The name of the 
       * event passed here is case sensitive and must match exactly what 
       * Asterisk will return.  Pass a blank string to match all events.
       *
       * The event handler can be unregistered using the 
       * boost::signals::connection object that is returned.
       *
       * The signature of the handler should be:
       * @code
       * void event_handler(message::event e); // e is the event
       * @endcode
       *
       * @see connection::process_events()
       * @see connection::wait_event()
       * @see connection::pump_messages()
       *
       * @return a boost::signals::connection object that can be used to 
       * manager this event handler
       */
      boost::signals::connection connection::register_event(const std::string& e, boost::function<void (message::event)> f) {
         std::pair<event_handlers_t::iterator, bool> ii = event_handlers.insert(std::make_pair(e, boost::shared_ptr<boost::signal<void (message::event)> >(new boost::signal<void (message::event)>())));
         return ii.first->second->connect(f);
      }

   }
}

