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
 * Include this file to use the setvar action.  It will be included 
 * automatically by including astxx/manager.h.
 */

#ifndef ASTXX_MANAGER_ACTION_SETVAR_H
#define ASTXX_MANAGER_ACTION_SETVAR_H

#include <astxx/manager/basic_action.h>
#include <astxx/manager/message.h>
#include <astxx/manager/action/error.h>

namespace astxx {
   namespace manager {
      namespace action {
         /** Set a channel variable.
          * @todo Maybe a handle_event method should throw an exception when 
          * the channel is not found or when values are not specified.
          */
         class setvar : public basic_action {
            public:
               /** Set a channel variable.
                * @param channel the channel to set the variable on
                * @param variable the variable to set
                * @param value the value to set
                */
               setvar(const std::string& channel, const std::string& variable, const std::string& value) : channel(channel), variable(variable), value(value) {
               }
         
               message::action action() const {
                  message::action action("Setvar");
                  action["Channel"] = channel;
                  action["Variable"] = variable;
                  action["Value"] = value;
                  return action;
               }

            private:
               std::string channel;
               std::string variable;
               std::string value;
         };
      }
   }
}

#endif
