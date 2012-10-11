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
 * Don't include this file directly, include astxx/manager.h instead.
 * 
 * This file contains the astxx::manager::basic_message class and its 
 * associated typedefs.
 */

#ifndef ASTXX_MANAGER_MESSAGE_H
#define ASTXX_MANAGER_MESSAGE_H

#include <astxx/manager/error.h>
#include <map>
#include <string>
#include <sstream>

namespace astxx {
   namespace manager {
      namespace message {
         /** A required header (probably the main header) was missing from a 
          * message.
          */
         class header_missing : public manager::error {
            public:
               explicit header_missing(const std::string& header) throw() : manager::error("missing " + header + " header"), m_header(header) { }
               virtual ~header_missing() throw() { }

               /// Get the missing header.
               std::string header() const { return m_header; }

            private:
               std::string m_header;

         };

         /** An Asterisk Manager message.
          *
          * The main method for accessing headers in a manager::basic_message is 
          * through the [] operator.  In the case where multiple headers have the 
          * same key, the message should be accessed in the same manner as an 
          * std::multimap (i.e. using the equal_range() function).
          *
          * @note Headers and values for manager::basic_message objects are 
          * stored exactly as they are recieved from Asterisk.  As a result, 
          * the exact case Asterisk for headers must be used to look up values.
          */
         template<typename message_traits>
         class basic_message {
            public:
               // have to use a multimap here because messages can contain 
               // multiple headers with the same key
               typedef std::multimap<std::string, std::string> header_t;
               typedef std::pair<std::string, std::string> header_pair_t;

               message_traits traits;

               /// Addition data from the 'Command' action.
               std::string data;

            public:

               /** Construct a message initilizing the value of the main header.
                * @param value the initial value of the main header
                */
               basic_message(const std::string& value) {
                  this->operator[](traits.main_header()) = value;
               }

               /** Construct a message from an existing std::multimap of headers.
                * @param headers an std::multimap of headers
                */
               explicit basic_message(const header_t& headers) : headers(headers) { }

               /** Construct a message from an existing std::map of headers.
                * @param headers an std::map of headers
                */
               explicit basic_message(const std::map<std::string, std::string>& headers) : headers(headers.begin(), headers.end()) { }
               
               /** Construct a message from a set of iterators.
                * @param begin the start iterator
                * @param end the end iterator
                */
               explicit basic_message(const header_t::iterator begin, const header_t::iterator end) : headers(begin, end) { }

               /** Get the header that matches the given key.
                * @param key the key to use for lookup
                *
                * This function looks up the given key in the message's internal 
                * multimap and returns the first matching value if there are 
                * multiple matches.  It will also create an entry with the given 
                * key if no entry could be found (just like std::map).
                *
                * @return a reference to the value matching the given key
                */
               std::string& operator[](const std::string& key) {
                  std::pair<header_t::iterator, header_t::iterator> ii = headers.equal_range(key);
                  if (ii.first != ii.second)
                     return ii.first->second;

                  return headers.insert(std::make_pair(key, ""))->second;
               }

               /** Compare the main header for this message to a string.
                * @param s the string
                * @return the result of the compairson
                * @throw manager::message::header_missing if the main header is 
                * missing
                */
               bool operator==(const std::string& s) const {
                  return (main_header() == s);
               }

               /** Compare the main header for this message to a string.
                * @param s the string
                * @return the result of the compairson
                * @throw manager::message::header_missing if the main header is 
                * missing
                */
               bool operator!=(const std::string& s) const {
                  return !(*this == s);
               }

               /** Add a key value pair to the header.
                * @param pair the pair
                * @return an iterator to the inserted pair
                */
               header_t::iterator insert(const header_pair_t& pair) {
                  return headers.insert(pair);
               }
               
               /** Insert key value pairs into the message.
                * @param begin the start of the pairs
                * @param end the end of the pairs
                */
               void insert(header_t::iterator begin, header_t::iterator end) {
                  return headers.insert(begin, end);
               }

               /** Get a pair of iterators pointing to the beginning and one past 
                * the end of a group of headers matching the given key.
                * @param key the key to use for lookup
                * @return an std::pair of iterators
                */
               std::pair<header_t::iterator, header_t::iterator> equal_range(const std::string& key) {
                  return headers.equal_range(key);
               }

               /** Get an iterator to the first header.
                * @return an iterator to the first header
                */
               header_t::iterator begin() {
                  return headers.begin();
               }

               /** Get an iterator to one past the last header.
                * @return an iterator to one past the last header
                */
               header_t::iterator end() {
                  return headers.end();
               }

               /** Format this message as a string.
                * @return this message formatted as a string
                * @throw manager::message::header_missing if the main header is 
                * missing
                */
               std::string format() {
                  if (this->operator[](traits.main_header()).empty()) {
                     throw message::header_missing(traits.main_header());
                  }

                  std::stringstream ss;

                  // put the main header first
                  ss << traits.main_header() << ": " << this->operator[](traits.main_header()) << "\r\n";

                  for (header_t::iterator i = headers.begin(); i != headers.end(); ++i) {
                     // don't repeat the main header
                     if (i->first != traits.main_header()) {
                        ss << i->first << ": " << i->second << "\r\n";
                     }
                  }

                  ss << "\r\n";

                  return ss.str();
               }

               /** Get the value of the main header for this message.
                * @note this will be the value of the Action, Event, or Response 
                * header.
                *
                * @return the value of the main header
                * @throw manager::message::header_missing if the main header is 
                * missing
                */
               std::string main_header() const {
                  std::pair<header_t::const_iterator, header_t::const_iterator> ii = headers.equal_range(traits.main_header());
                  if (ii.first != ii.second)
                     return ii.first->second;

                  throw message::header_missing(traits.main_header());
               }
            
            private:
               header_t headers;

         };

         /** Compare the main header for this message to a string.
          * @param s the string
          * @return the result of the compairson
          * @throw manager::message::header_missing if the main header is 
          * missing
          */
         template<typename message_traits>
         bool operator==(const std::string& s, const basic_message<message_traits>& m) {
            return (s == m.main_header());
         }

         /** Compare the main header for this message to a string.
          * @param s the string
          * @return the result of the compairson
          * @throw manager::message::header_missing if the main header is 
          * missing
          */
         template<typename message_traits>
         bool operator!=(const std::string& s, const basic_message<message_traits>& m) {
            return !(s == m);
         }

         class message_traits {
            public:
               explicit message_traits(const std::string& key) : key(key) { }
               std::string main_header() const { return key; }

            private:
               std::string key;
         };

         class action_traits : public message_traits {
            public:
               action_traits() : message_traits("Action") { }
         };

         class response_traits : public message_traits {
            public:
               response_traits() : message_traits("Response") { }
         };

         class event_traits : public message_traits {
            public:
               event_traits() : message_traits("Event") { }
         };

         typedef basic_message<action_traits> action;     ///< an 'Action' message
         typedef basic_message<response_traits> response; ///< a 'Response' message
         typedef basic_message<event_traits> event;       ///< an 'Event' message
      }
   }
}

#endif
