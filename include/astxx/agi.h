/* vim: set et sw=3 tw=0 fo=croqlaw cino=t0:
 *
 * astxx, the Asterisk C++ API and Utility Library.
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
 * Include this file to use the AGI API in astxx.
 *
 * The agi class contains the AGI functionallity in astxx.  There is one global
 * instance of this class for any particular program.  The instance should be
 * accessed via the static member function astxx::agi::instance().  This class
 * is \em not currently thread safe.
 */

#ifndef ASTXX_AGI_H
#define ASTXX_AGI_H

#include <map>
#include <string>
#include <astxx/exception.h>

namespace astxx {

   /** The agi handling class.
    *
    * This class can be used to interface with asterisk through the Asterisk
    * Gateway Interface. It is designed to be instantated only once per run.
    * Because of the design of the AGI interface, this is all that is 
    * necessary.  The Singleton Patteren is used to ensure that all instances 
    * of the agi class are the same instance.
    * 
    * To use the agi class you simply call astxx::agi::instance(), which will
    * return a refrence to the agi interface, creating an instance of the class
    * if necessary.
    *
    * Do not embed new lines (\\n) in any of your AGI commands.  This will 
    * cause unexpected behavior.  One exception to this rule is the 
    * agi::verbose command which has been specially written in astxx to parse 
    * new lines out into multiple verbose commands (one per line).
    *
    * Asterisk will send SIGHUP when the controlling channel is hungup.  This 
    * signal is caught by agi::sighup_handler() and an internal flag is set.  
    * You can test for hangups using the agi::test_hangup() function which 
    * throws agi::hangup_signal (derrived from agi::hangup) if there was a 
    * hangup.
    *
    * @note All agi commands may throw agi::hangup and or 
    * agi::application_error.
    * 
    * Currently this implmentation is \em not thread safe.
    * 
    * @see Commands
    */
   class agi {
      public: // agi Exceptions

         /** Generic agi Exception.
          *
          * This class is the base for all other agi related exceptions.
          */
         class error : public astxx::exception {
            public:
               error() throw() : astxx::exception("astxx AGI Exception") { }
               /// constructor
               explicit error(const std::string& desc) throw() : astxx::exception(desc) { }
         };

         /** Asterisk has reported an error we do not understand.
         */
         class unknown_error : public error {
            public:
               /// constructor
               explicit unknown_error(int error_code, const std::string& desc) throw() : error(desc), code(error_code) { }

               /** Get the error code of the unknown error.
                * @return the unknown error code
                */
               int error_code() const { return code; }

            private:
               int code;

         };

         /** An application has reported a usage error.
         */
         class usage_error : public error {
            public:
               /// constructor
               explicit usage_error(const std::string& desc) throw() : error(desc) { }
         };

         /** An invalid or unknown command was executed.
         */
         class invalid_command : public error {
            public:
               invalid_command() throw() : error("Invalid or unknown command.  This could be a bug in astxx, or your Asterisk installation may be too old.") { }
               /// constructor
               explicit invalid_command(const std::string& desc) throw() : error(desc) { }

         };

         /** There was an error communicating with the asterisk server.
         */
         class io_error : public error {
            public:
               io_error() throw() : error("There was a error communicating with the Asterisk server") { }
               /// constructor
               explicit io_error(const std::string& desc) throw() : error(desc) { }

         };

         /** An application encountered an error during execution.
         */
         class application_error : public error {
            public:
               application_error() throw() : error("Error during application execution, possibly a hangup") { }
               /// constructor
               explicit application_error(const std::string& desc) throw() : error(desc) { }

         };

         /** A channel was hungup while executing an agi command.
          *
          * @todo Possibly in the future include the current agi::result with 
          * the exception.
          */
         class hangup : public application_error {
            public:
               hangup() throw() : application_error("Channel hungup during execution") { }
               /// constructor
               explicit hangup(const std::string& desc) throw() : application_error(desc) { }

         };

         /** A channel was hungup while executing an agi command.
          *
          * @todo Possibly in the future include the current agi::result with 
          * the exception.
          */
         class hangup_signal : public hangup {
            public:
               hangup_signal() throw() : hangup("Received SIGHUP from Asterisk, indicating a hangup") { }
         };

         /** "hangup" was recieved as the result of a command.
          *
          * @todo Possibly in the future include the current agi::result with 
          * the exception.
          */
         class hangup_result : public hangup {
            public:
               hangup_result() throw() : hangup("Received \"hangup\" as the result of an AGI command") { }
         };

         /// The current command timed out
         class timeout : public application_error {
            public:
               timeout() throw() : application_error("Operation timed out") { }
               /// constructor
               explicit timeout(const std::string& desc) throw() : application_error(desc) { }

         };

         /// There was a error with the Asterisk database.
         class database_error : public application_error {
            public:
               database_error() : application_error("An Asterisk database operation failed") { }
               /// constructor
               explicit database_error(const std::string& desc) : application_error(desc) { }

         };

         /// The requested channel could not be found.
         class no_channel : public application_error {
            public:
               no_channel() throw() : application_error("The requested channel could not be found") { }
               /// constructor
               explicit no_channel(const std::string& channel) throw() : application_error("Channel '" + channel + "' was not found"), channel(channel) { }
               virtual ~no_channel() throw () { }

            private:
               std::string channel;

         };

         /** Holds the result of an AGI command.
          *
          * This class is only used internally by the agi class.  It holds the 
          * response code from the AGI server, the message along with the code, 
          * and if applicable, the result code and any data with it.
          */
         struct result {
            /// The return code from the AGI command (200, 510, 520).
            int code;                   
            /// All the data after the return code.
            std::string message;        
            /// The result= value if it was an interger
            int result;                 
            /// The result= value as a string.
            std::string result_string;  
            /// Any data between ( and ).
            std::string data;            
            /// The value of endpos= if any.
            long endpos;
         };


      public:
         std::map<std::string, std::string> env;           ///< The agi environment
         result execute(const std::string& command) const; ///< execute the specfied command and return the agi::result
         std::string quote(const std::string item) const { return ("\"" + item + "\""); }  ///< Return a quoted string
         std::string quote(char item) const { return ("\"" + (item ? std::string(1, item) : "") + "\""); }  ///< Return a quoted string

      protected:
         agi();                         
         agi(const agi&);               ///< Hidden copy constructor 
         agi& operator = (const agi&);  ///< Hidden copy constructor
         void read_env();
         void send_command(const std::string& command) const;
         result get_result() const;

      private:
         static volatile bool got_sighup;
         static void handle_sighup(int);
         static volatile bool got_sigpipe;
         static void handle_sigpipe(int);

         static const int control_stream_file_skip_time = 3000; ///< The default skip_time for control_stream_file.
         static const int get_data_max_digits = 1024;           ///< The default max_digits for get_data.

      public:
         static agi& instance();
         std::string operator [] (const std::string& key);
         std::map<std::string, std::string>::const_iterator begin() const;
         std::map<std::string, std::string>::const_iterator end() const;

         void test_hangup() const;
         void clear();

         // agi commands
         agi& answer();
         int channel_status(const std::string& channel = "") const;
         char control_stream_file(const std::string& filename, const std::string& escape_digits = "", int skip_time = control_stream_file_skip_time, char fastforward = 0, char rewind = 0, char pause = 0) const;
         agi& database_del(const std::string& family, const std::string& key);
         agi& database_deltree(const std::string& family, const std::string& keytree = "");
         std::string database_get(const std::string& family, const std::string& key) const;
         agi& database_put(const std::string& family, const std::string& key, const std::string& value);
         int exec(const std::string& app, const std::string& options = "") const;
         std::string get_data(const std::string& file, int timeout = 0, int max_digits = get_data_max_digits) const;
         std::string get_full_variable(const std::string& variable, const std::string& channel = "") const;
         char get_option(const std::string& file, const std::string& escape_digits = "", int timeout = 0) const;
         std::string get_variable(const std::string& variable) const;
         agi& hangup(const std::string& channel = "");
         agi& noop();
         char receive_char(int timeout = 0) const;
         std::string receive_text(int timeout = 0) const;
         char record_file(const std::string& filename, const std::string& format, const std::string& escape_digits = "", int timeout = -1, int silence = 0, bool beep = true, long offset = 0) const;
         char say_alpha(const std::string& alphanum, const std::string& escape_digits = "") const;
         char say_alpha(int number, const std::string& escape_digits = "") const;
         char say_date(time_t date, const std::string& escape_digits = "") const;
         char say_datetime(time_t date, const std::string& escape_digits = "", const std::string& format = "", const std::string& timezone = "") const;
         char say_digits(int digits, const std::string& escape_digits = "") const;
         char say_number(int number, const std::string& escape_digits = "") const;
         char say_phonetic(const std::string& characters, const std::string& escape_digits = "") const;
         char say_time(time_t time, const std::string& escape_digits = "") const;
         agi& send_image(const std::string& image);
         agi& send_text(const std::string& text);
         agi& set_autohangup(int delay);
         agi& set_callerid(int number);
         agi& set_callerid(const std::string& cid);
         agi& set_context(const std::string& context);
         agi& set_extension(int extension);
         agi& set_extension(const std::string& extension);
         agi& set_music(bool enable, const std::string& music_class = "");
         agi& set_priority(int priority);
         agi& set_priority(const std::string& priority);
         agi& set_variable(const std::string& variable, const std::string& value);
         char stream_file(const std::string& filename, const std::string& escape_digits = "", long offset = 0) const;
         agi& tdd_mode(bool enable);
         agi& tdd_mode(const std::string& mode);
         agi& verbose(const std::string& text, int level = 1);
         char wait_for_digit(long timeout = -1) const;
   };

}

#endif
