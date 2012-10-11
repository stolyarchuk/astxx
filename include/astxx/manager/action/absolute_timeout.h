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
 * Include this file to use the AbsoluteTimeout action.  It will be included 
 * automatically by including astxx/manager.h.
 */

#ifndef ASTXX_MANAGER_ACTION_ABSOLUTE_TIMEOUT_H
#define ASTXX_MANAGER_ACTION_ABSOLUTE_TIMEOUT_H

#include <astxx/manager/basic_action.h>
#include <astxx/manager/message.h>
#include <string>
#include <ctime>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace astxx {
   namespace manager {
      namespace action {
         using boost::lexical_cast;
         using boost::posix_time::time_duration;
         using boost::posix_time::seconds;

         /// Set a timeout for a channel.
         class absolute_timeout : public basic_action {
            public:
               /** Set a timeout for the specified channel.
                * @param channel the channel to set the timeout on
                * @param timeout the number of seconds to wait before hanging 
                * up the call (use 0 to cancel the timeout)
                */
               absolute_timeout(const std::string& channel, std::time_t timeout)
                  : channel(channel), timeout(seconds(timeout)) {
               }
         
               /** Set a timeout for the specified channel.
                * @param channel the channel to set the timeout on
                * @param timeout the time to wait before hanging up the call 
                * (use 0 or a special time_duration value to cancel the 
                * timeout)
                */
               absolute_timeout(const std::string& channel, time_duration timeout)
                  : channel(channel), timeout(timeout) {
               }
         
               message::action action() const {
                  message::action action("AbsoluteTimeout");
                  action["Channel"] = channel;
                  if (timeout.is_special()) {
                     action["Timeout"] = "0";
                  }
                  else {
                     action["Timeout"] = lexical_cast<std::string>(timeout.total_seconds());
                  }

                  return action;
               }

            private:
               std::string channel;
               time_duration timeout;
         };
      }
   }
}

#endif
