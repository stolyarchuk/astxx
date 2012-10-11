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
 * Include this file to use the redirect action.  It will be included 
 * automatically by including astxx/manager.h.
 */

#ifndef ASTXX_MANAGER_ACTION_REDIRECT_H
#define ASTXX_MANAGER_ACTION_REDIRECT_H

#include <astxx/manager/basic_action.h>
#include <astxx/manager/message.h>
#include <string>
#include <boost/lexical_cast.hpp>

namespace astxx {
   namespace manager {
      namespace action {
         using boost::lexical_cast;

         /// Reirect a call.
         class redirect : public basic_action {
            public:
               /** Redirect a call.
                * @param channel the channel to redirect
                * @param exten the extension to redirect the call(s) to
                * @param context the context to redirect the call(s) to
                * @param priority the priority to redirect the call(s) to
                */
               redirect(const std::string& channel, const std::string& context, const std::string& exten, const std::string& priority)
                  : channel(channel), context(context), exten(exten), priority(priority) {
               }
         
               /** Redirect a call.
                * @param channel the channel to redirect
                * @param exten the extension to redirect the call(s) to
                * @param context the context to redirect the call(s) to
                * @param priority the priority to redirect the call(s) to
                */
               redirect(const std::string& channel, const std::string& context, unsigned int exten, const std::string& priority)
                  : channel(channel), context(context), exten(lexical_cast<std::string>(exten)), priority(priority) {
               }
         
               /** Redirect a call.
                * @param channel the channel to redirect
                * @param exten the extension to redirect the call(s) to
                * @param context the context to redirect the call(s) to
                * @param priority the priority to redirect the call(s) to
                */
               redirect(const std::string& channel, const std::string& context, const std::string& exten, unsigned int priority)
                  : channel(channel), context(context), exten(exten), priority(lexical_cast<std::string>(priority)) {
               }
         
               /** Redirect a call.
                * @param channel the channel to redirect
                * @param exten the extension to redirect the call(s) to
                * @param context the context to redirect the call(s) to
                * @param priority the priority to redirect the call(s) to
                */
               redirect(const std::string& channel, const std::string& context, unsigned int exten, unsigned int priority)
                  : channel(channel), context(context), exten(lexical_cast<std::string>(exten)), priority(lexical_cast<std::string>(priority)) {
               }

               /** Set the ExtraChannel header.
                * @param extra_channel an additional channel to redirect 
                * (generally used for bridged calls)
                * @return a reference to this action
                */
               redirect& extra_channel(const std::string& extra_channel) {
                  m_extra_channel = extra_channel;
                  return *this;
               }
         
               message::action action() const {
                  message::action action("Redirect");
                  action["Channel"] = channel;

                  if (not m_extra_channel.empty()) {
                     action["ExtraChannel"] = m_extra_channel;
                  }

                  action["Context"] = context;
                  action["Exten"] = exten;
                  action["Priority"] = priority;

                  return action;
               }

            private:
               std::string channel;
               std::string m_extra_channel;
               std::string context;
               std::string exten;
               std::string priority;
         };
      }
   }
}

#endif
