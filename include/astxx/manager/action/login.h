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
 * Include this file to use the login action.  It will be included 
 * automatically by including astxx/manager.h.
 */

#ifndef ASTXX_MANAGER_ACTION_LOGIN_H
#define ASTXX_MANAGER_ACTION_LOGIN_H

#include <astxx/manager/basic_action.h>
#include <astxx/manager/message.h>
#include <astxx/manager/error.h>

namespace astxx {
   namespace manager {
      namespace action {
         /** Login to the manager.
          * @throw login::error if there is an error logging in
          */
         class login : public basic_action {
            public:
               /// An authentication error.
               class error : public manager::error {
                  public:
                     explicit error(const std::string& desc) throw() : manager::error(desc) { }
               };

            public:
               /** Construct a login action.
                * @param username the username to use
                * @param the secret to use
                */
               login(const std::string& username, const std::string& secret) : username(username), secret(secret) {
               }
         
               /** Format this action as a message::action.
                * @return this message as a message::action
                */
               message::action action() const {
                  message::action action("Login");
                  action["Username"] = username;
                  action["Secret"] = secret;
                  return action;
               }

               /** Handle the response to the login command.
                * @param response the response from Asterisk
                * @throw login::error if the login failed
                */
               message::response handle_response(message::response response) {
                  basic_action::handle_response(response);

                  if ("Success" != response) {
                     throw login::error(response["Message"]);
                  }

                  return response;
               }

            private:
               std::string username;
               std::string secret;
         };
      }
   }
}

#endif
