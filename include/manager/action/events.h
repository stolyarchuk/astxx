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
 * Include this file to use the events action.  It will be included 
 * automatically by including astxx/manager.h.
 */

#ifndef ASTXX_MANAGER_ACTION_EVENTS_H
#define ASTXX_MANAGER_ACTION_EVENTS_H

#include <astxx/manager/basic_action.h>
#include <astxx/manager/message.h>
#include <string>
#include <boost/lexical_cast.hpp>

namespace astxx {
   namespace manager {
      namespace action {
         using boost::lexical_cast;

         /// Set the event mask.
         class events : public basic_action {
            public:
               static const unsigned short system;  ///< System events such as module load/unload
               static const unsigned short call;    ///< Call event, such as state change, etc
               static const unsigned short log;     ///< Log events
               static const unsigned short verbose; ///< Verbose messages
               static const unsigned short command; ///< Ability to read/set commands
               static const unsigned short agent;   ///< Ability to read/set agent info
               static const unsigned short user;    ///< Ability to read/set user info
               static const unsigned short config;  ///< Ability to modify configurations
            public:
               /** Set the mask using an integer mask.
                * @param mask the integer mask
                *
                * @code
                * events(events::system | events::call | events::log);
                * @endcode
                */
               events(unsigned short mask) : int_mask(mask), bool_mask(false), string_mask("") {
               }

               /** Set the mask using a text mask.
                * @param mask the text mask
                *
                * @code
                * events("system, call, log");
                * @endcode
                */
               events(const std::string& mask) : int_mask(0), bool_mask(false), string_mask(mask) {
               }

               /** Set all events either on or off.
                * @param mask set events either on or off
                *
                * @code
                * events(true);
                * @endcode
                */
               events(bool mask) : int_mask(0), bool_mask(mask), string_mask("") {
               }

               message::action action() const {
                  message::action action("Events");
                  if (int_mask) {
                     action["EventMask"] = lexical_cast<std::string>(int_mask);
                  }
                  else if (not string_mask.empty()) {
                     action["EventMask"] = string_mask;
                  }
                  else if (bool_mask) {
                     action["EventMask"] = "on";
                  }
                  else {
                     action["EventMask"] = "off";
                  }

                  return action;
               }

            private:
               unsigned short int_mask;
               bool bool_mask;
               std::string string_mask;
         };

         // these values are taken from manager.h in the asterisk source
         static const unsigned short events::system  = 1 << 0;
         static const unsigned short events::call    = 1 << 1;
         static const unsigned short events::log     = 1 << 2;
         static const unsigned short events::verbose = 1 << 3;
         static const unsigned short events::command = 1 << 4;
         static const unsigned short events::agent   = 1 << 5;
         static const unsigned short events::user    = 1 << 6;
         static const unsigned short events::config  = 1 << 7;

      }
   }
}

#endif
