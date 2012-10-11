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
#include <astxx/manager/basic_action.h>
#include <astxx/manager/message.h>
#include <astxx/manager/error.h>

namespace astxx {
   namespace manager {
      /** Handle a response from Asterisk.
       * @param response the response to handle
       *
       * This function is called from basic_action::operator()() and passed the 
       * response from Asterisk.  This function may opt to throw exceptions if 
       * an error occours.  The default implementation handles errors common to 
       * most manager actions.  Overriding methods should call this method 
       * before processing the response if they would like to keep this error 
       * handling.
       *
       * @return the message::response we were passed
       *
       * @throw manager::permission_denied on a permission error
       * @throw manager::authentication_required on an authentication error
       * @throw manager::action::missing_data if required data was not provided
       * @throw manager::action::bad_data if the given data was invalid 
       * (invalid priority, channel, timeout...)
       * @throw manager::action::channel_not_found if the given channel was not 
       * found
       */
      message::response basic_action::handle_response(message::response response) {
         if (response == "Error") {
            if (response["Message"] == permission_error_string) {
               throw manager::permission_denied();
            }
            else if (response["Message"] == authentication_error_string) {
               throw manager::authentication_required();
            }
            else if (response["Message"] == "No timeout specified") {
               throw action::missing_data(response["Message"], action());
            }
            else if (response["Message"] == "No channel specified") {
               throw action::missing_data(response["Message"], action());
            }
            else if (response["Message"] == "Channel not specified") {
               throw action::missing_data(response["Message"], action());
            }
            else if (response["Message"] == "Extension not specified") {
               throw action::missing_data(response["Message"], action());
            }
            else if (response["Message"] == "No variable specified") {
               throw action::missing_data(response["Message"], action());
            }
            else if (response["Message"] == "No value specified") {
               throw action::missing_data(response["Message"], action());
            }
            else if (response["Message"] == "Mailbox not specified") {
               throw action::missing_data(response["Message"], action());
            }
            else if (response["Message"] == "Invalid priority") {
               throw action::bad_data(response["Message"], action());
            }
            else if (response["Message"] == "Invalid channel") {
               throw action::bad_data(response["Message"], action());
            }
            else if (response["Message"] == "Invalid timeout") {
               throw action::bad_data(response["Message"], action());
            }
            else if (response["Message"] == "No such channel") {
               throw action::channel_not_found(response["Message"], action());
            }
         }
         return response;
      }

      /** Send this action over this connection and handle the response.
       * @param c the connection to use
       *
       * This function sends this action over the give connection to Asterisk 
       * and then passes the response through basic_action::handle_response() 
       * before returning it.  basic_action::handle_response() will throw 
       * exceptions for most common errors.
       *
       * @return the response from Asterisk
       */
      message::response basic_action::operator()(connection& c) {
         return handle_response(c(*this));
      }
   }
}

