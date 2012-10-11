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
 * Include this file to use the originate action.  It will be included 
 * automatically by including astxx/manager.h.
 */

#ifndef ASTXX_MANAGER_ACTION_ORIGINATE_H
#define ASTXX_MANAGER_ACTION_ORIGINATE_H

#include <astxx/manager/basic_action.h>
#include <astxx/manager/message.h>
#include <string>
#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace astxx {
   namespace manager {
      namespace action {
         using boost::lexical_cast;
         using boost::posix_time::time_duration;
         using boost::posix_time::pos_infin;

         /// Originate a call.
         class originate : public basic_action {
            public:
               /** Originate a call.
                * @param channel the channel to originate
                * @param exten the extension to begin the originate on
                * @param context the context to begin the originate in
                * @param priority the priority to begin the originate at
                */
               originate(const std::string& channel, const std::string& context, const std::string& exten, const std::string& priority)
                  : channel(channel), context(context), exten(exten), priority(priority), m_async(false) {
               }
         
               /** Originate a call.
                * @param channel the channel to originate
                * @param exten the extension to begin the originate on
                * @param context the context to begin the originate in
                * @param priority the priority to begin the originate at
                */
               originate(const std::string& channel, const std::string& context, unsigned int exten, const std::string& priority)
                  : channel(channel), context(context), exten(lexical_cast<std::string>(exten)), priority(priority), m_async(false) {
               }
         
               /** Originate a call.
                * @param channel the channel to originate
                * @param exten the extension to begin the originate on
                * @param context the context to begin the originate in
                * @param priority the priority to begin the originate at
                */
               originate(const std::string& channel, const std::string& context, const std::string& exten, unsigned int priority)
                  : channel(channel), context(context), exten(exten), priority(lexical_cast<std::string>(priority)), m_async(false) {
               }
         
               /** Originate a call.
                * @param channel the channel to originate
                * @param exten the extension to begin the originate on
                * @param context the context to begin the originate in
                * @param priority the priority to begin the originate at
                */
               originate(const std::string& channel, const std::string& context, unsigned int exten, unsigned int priority)
                  : channel(channel), context(context), exten(lexical_cast<std::string>(exten)), priority(lexical_cast<std::string>(priority)), m_async(false) {
               }
         
               /** Originate a call.
                * @param channel the channel to originate
                * @param application the application to execute on the 
                * originated channel
                * @param data the data to pass to that application
                */
               originate(const std::string& channel, const std::string& application, const std::string& data)
                  : channel(channel), application(application), data(data), m_async(false) {
               }

               /** Set the timeout for this call.
                * @param to the timeout to set (use posix_time::pos_infin for 
                * no timeout) Asterisk defaults this to 30 seconds
                * @return a reference to this action::originate object
                */
               action::originate& timeout(time_duration to) {
                  m_timeout = to;
                  return *this;
               }

               /** Set the CallerID for this call.
                * @param cid the caller id (in the format '"Caller Name" 
                * <number>' or 'Name' or 'number' or any other format Asterisk 
                * accepts)
                * @return a reference to this action::originate object
                */
               action::originate& caller_id(const std::string& cid) {
                  m_caller_id = cid;
                  return *this;
               }

               /** Set the account for this call.
                * @param account the account code to use
                * @return a reference to this action::originate object
                */
               action::originate& account(const std::string& account) {
                  m_account = account;
                  return * this;
               }

               /** Set or unset the asynchronyous flag for this originate.
                * @param state set to true for asynchronous behavior
                * @return a reference to this action::originate object
                */
               action::originate& async(bool state) {
                  m_async = state;
                  return *this;
               }

               /** Set the variables for this call.
                * @param vars the variables to assign to the channel created by 
                * this originate action
                * @return a reference to this action::originate object
                */
               action::originate& variables(const std::map<std::string, std::string>& vars) {
                  m_variables = vars;
                  return *this;
               }

               message::action action() const {
                  message::action action("Originate");
                  action["Channel"] = channel;

                  if (not context.empty()) {
                     action["Context"] = context;
                     action["Exten"] = exten;
                     action["Priority"] = priority;
                  }
                  else {
                     action["Application"] = application;
                     action["Data"] = data;
                  }

                  if (not m_timeout.is_special() and m_timeout != time_duration()) {
                     action["Timeout"] = lexical_cast<std::string>(m_timeout.total_milliseconds());
                  }
                  else if (m_timeout == pos_infin) {
                     action["Timeout"] = "-1"; // Asterisk uses -1 for infinite
                  }

                  if (not m_caller_id.empty())
                     action["CallerID"] = m_caller_id;

                  if (not m_account.empty())
                     action["Account"] = m_account;

                  if (m_async)
                     action["Async"] = "true";

                  // format "Variable:" headers
                  for (std::map<std::string, std::string>::const_iterator i = m_variables.begin(); i != m_variables.end(); ++i) {
                     action.insert(std::make_pair("Variable", i->first + "=" + i->second));
                  }

                  return action;
               }

            private:
               std::string channel;

               std::string context;
               std::string exten;
               std::string priority;

               std::string application;
               std::string data;

               time_duration m_timeout;
               std::string m_caller_id;
               std::string m_account;
               bool m_async;
               std::map<std::string, std::string> m_variables;
         };
      }
   }
}

#endif
