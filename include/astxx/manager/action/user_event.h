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
 * Include this file to use the UserEvent action.  It will be included 
 * automatically by including astxx/manager.h.
 */

#ifndef ASTXX_MANAGER_ACTION_USER_EVENT_H
#define ASTXX_MANAGER_ACTION_USER_EVENT_H

#include <astxx/manager/basic_action.h>
#include <astxx/manager/message.h>
#include <string>

namespace astxx {
   namespace manager {
      namespace action {

         /** Generate a UserEvent.
          * Add headers to the event using the public headers member.
          */
         class user_event : public basic_action {
            public:
               /** Generate a UserEvent.
                * @param event the name of the event
                */
               user_event(const std::string& event)
                  : event(event) {
               }
               
               message::action action() const {
                  message::action action("UserEvent");
                  action["UserEvent"] = event;

                  for (std::multimap<std::string, std::string>::const_iterator i; i != headers.end(); ++i) {
                     action.insert(*i);
                  }

                  return action;
               }

            private:
               std::string event;

            public:
               std::multimap<std::string, std::string> headers;
         };
      }
   }
}

#endif
