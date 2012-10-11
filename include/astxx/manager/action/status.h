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
 * Include this file to use the status action.  It will be included 
 * automatically by including astxx/manager.h.
 */

#ifndef ASTXX_MANAGER_ACTION_STATUS_H
#define ASTXX_MANAGER_ACTION_STATUS_H

#include <astxx/manager/basic_action.h>
#include <astxx/manager/message.h>

namespace astxx {
   namespace manager {
      namespace action {
         /** Get channel status.
          * @todo Maybe a handle_event method should throw a channel not found 
          * error when a channel is not found.  If the channel is not found we 
          * will get two responses which is a bit odd.  We should account for 
          * this somehow or make Asterisk only return one response.
          */
         class status : public basic_action {
            public:
               /** Get channel status.
                * @param channel the channel to get a status for (leave blank 
                * for all active channels)
                * @param action_id the action id that will be returned with 
                * responses and events triggered by this action
                */
               status(const std::string& channel = "", const std::string& action_id = "") : channel(channel), action_id(action_id) {
               }
         
               message::action action() const {
                  message::action action("Status");
                  if (not action_id.empty()) {
                     action["ActionID"] = action_id;
                  }
                  if (not channel.empty()) {
                     action["Channel"] = channel;
                  }
                  return action;
               }

            private:
               std::string channel;
               std::string action_id;
         };
      }
   }
}

#endif
