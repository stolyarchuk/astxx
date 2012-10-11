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
 * Include this file to use the extensionstate action.  It will be included 
 * automatically by including astxx/manager.h.
 */

#ifndef ASTXX_MANAGER_ACTION_EXTENSION_STATE_H
#define ASTXX_MANAGER_ACTION_EXTENSION_STATE_H

#include <astxx/manager/basic_action.h>
#include <astxx/manager/message.h>
#include <string>
#include <boost/lexical_cast.hpp>

namespace astxx {
   namespace manager {
      namespace action {
         using boost::lexical_cast;

         /** Check the state of an extension.
          * This action is used to check the state of an extension which is 
          * useful for checking the state of devices with hints.
          */
         class extension_state : public basic_action {
            public:
               /** Check the state of an extension.
                * @param context the context the extension is in.
                * @param exten the extension to check
                * @param priority the priority to begin the originate at
                */
               extension_state(const std::string& context, const std::string& exten)
                  : context(context), exten(exten) {
               }
         
               /** Check the state of an extension.
                * @param context the context the extension is in.
                * @param exten the extension to check
                * @param priority the priority to begin the originate at
                */
               extension_state(const std::string& context, unsigned int exten)
                  : context(context), exten(lexical_cast<std::string>(exten)) {
               }
               
               message::action action() const {
                  message::action action("ExtensionState");
                  action["Context"] = context;
                  action["Exten"] = exten;

                  return action;
               }

            private:
               std::string context;
               std::string exten;
         };
      }
   }
}

#endif
