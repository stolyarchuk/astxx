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

#include <astxx/agi.h>
#include <iostream>
#include <sstream>
#include <csignal>
#include <cerrno>
#include <cstring>

using namespace std;
using namespace astxx;

// initialize and declare our state variables
// XXX These variables are of type bool and not sig_atomic_t.  This may cause 
// problems on certain platforms.  Hopefully the volatile keyword will help 
// here.
volatile bool agi::got_sighup = false;
volatile bool agi::got_sigpipe = false;

/** Get the instance of the agi class.
 *
 * The instance function returns the single instance of the agi class.  This
 * function is probably the \em most non thread safe part of the agi class.
 *
 * @return the instance of the agi class
 * @exception agi::error thrown from the class constructor if construction 
 * fails
 */
agi& agi::instance() {
   static agi instance;
   return instance;
}

/** This constructor reads the agi environment variables.
 *
 * Because this class adheres to the Singleton patteren, it cannot be 
 * instantiated directly and must be implicitly created through the 
 * agi::instance() function.  This constructor handles getting and storing the 
 * agi environment, and setting up the SIGHUP signal handler.
 *
 * @exception agi::error thrown if there is an error during construction
 */
agi::agi() {
   // block signals
   sigset_t new_set;
   sigset_t original_set;

   if (sigfillset(&new_set) && sigprocmask(SIG_BLOCK, &new_set, &original_set))
      throw agi::error("Error blocking signals during initilization: " + string(strerror(errno)));

   // set our signal handler
   if (SIG_ERR == signal(SIGHUP, agi::handle_sighup) || SIG_ERR == signal(SIGPIPE, agi::handle_sigpipe)) {
      sigprocmask(SIG_SETMASK, &original_set, NULL);
      throw agi::error("Error blocking signals during initilization: " + string(strerror(errno)));
   }

   // read our environment
   read_env();

   // restore the previous sigmask, unblocking signals
   if (sigprocmask(SIG_SETMASK, &original_set, NULL))
      throw agi::error("Error restoring original signal mask during initilization: " + string(strerror(errno)));
}

/// Read the agi environment variables.
void agi::read_env() {
   // read our environment
   while (cin.peek() != '\n') {
      string key;
      string value;

      getline(cin, key, ':');

      if (cin.peek() == ' ')
         cin.ignore(1);

      getline(cin, value);

      env.insert(make_pair(key, value));
   }
}

/** Get an asterisk environment variable.
 *
 * The [] operator when applied to an instance of the agi class will return
 * the environment variable that matches the string passed.
 *
 * @todo Currently this function will return an empty string if the key passed
 * is not found in the environment.  It may be better to throw an exception to
 * avoid confusion.
 *
 * @param key the variable to retrieve
 * @return the value of the environment variable
 */
string agi::operator [] (const std::string& key) {
   return env[key];
}

/** Get the beginning of the env std::map.
 *
 * @return a const iterator to the end of the std::map
 */
map<string, string>::const_iterator agi::begin() const {
   return env.begin();
}

/** Get the end of the env std::map.
 *
 * @return a const_iterator to the end of the std::map
 */
map<string, string>::const_iterator agi::end() const {
   return env.end();
}

/** Send an agi command to Asterisk.
 *
 * @param command the command to be sent
 * 
 * @note The command should be in the format Asterisk expects to recieve it in, 
 * but with out the trailing '\\n'.
 */
void agi::send_command(const std::string& command) const {
   cout << command << endl;
}

/** Send an agi command and return the result.
 *
 * @param command the command to be sent
 * 
 * @return the result of the command as an agi::result struct
 *
 * @exception throws agi::hangup if the channel is hungup during execution
 * @exception throws agi::application_error if an application returns -1 
 * (applications may return -1 on hangup)
 */
agi::result agi::execute(const std::string& command) const {
   // It is important to test for a hangup here because if we attempt to 
   // execute a command, after there was a hangup Asterisk is no longer around 
   // to service us.  If we recieve a hangup during execution of an 
   // application, it will return -1 or hangup as it's data.
   test_hangup();

   send_command(command);
   agi::result result = get_result();

   if (result.data == "hangup")
      throw agi::hangup_result();

   if (result.result == -1)
      throw agi::application_error("An error occured executing an agi command or the channel was hungup");

   if (got_sigpipe)
      throw agi::io_error("Received SIGPIPE");

   return result;
}


/** Get the result of the last agi command, from Asterisk.
 *
 * @return the result as an astxx::agi::result struct
 */
agi::result agi::get_result() const {
   agi::result result;

   if (!(cin >> result.code)) {
      cin.clear();
      throw agi::io_error("There was a error reading from the Asterisk server");
   }

   if (cin.peek() == ' ')
      cin.ignore(1);

   getline(cin, result.message);

   istringstream message(result.message);

   // Handle various result codes.  Different codes have different data 
   // associated with them.  Below we handle the various codes.
   switch (result.code) {
      case 200:
         message.ignore(32, '=');

         // make sure result= actually has a value after '='
         if (message.peek() != ' ') {
            // store the result= portion in a stirng and int
            message >> result.result_string;
            istringstream ss(result.result_string);
            if (!(ss >> result.result))
               result.result = 0;
         }
         else {
            result.result_string = "";
            result.result = 0;
            message.ignore();
         }

         // extract any data between ()
         {
            string end_str;
            if (result.message.find("(") != string::npos) {
               message.ignore(32, '(');
               getline(message, result.data);

               const string::size_type token = result.data.find_last_of(')');
               end_str = result.data.substr(token);
               result.data.erase(token);

               message.str(end_str);
            }
            else {
               getline(message, end_str);
               message.str(end_str);
            }

            // extract endpos if applicable
            if (end_str.find("endpos=") != string::npos) {
               message.ignore(32, '=');
               message >> result.endpos;
            }
         }

         break;
      case 510:
         throw agi::invalid_command(result.message + ".  This could be a bug in Astxx, or your Asterisk installation may be too old.");

      case 520:
         {
            string buffer;
            while (getline(cin, buffer)) {
               istringstream message(buffer);
               int end;

               if (message >> end) {
                  if (end == 520)
                     break;
               }
               else {
                  message.clear();
                  result.message += buffer;
               }
            }

            throw agi::usage_error(result.message);
         }

      default:
         throw agi::unknown_error(result.code, result.message);
   }

   return result;
}

/// Set got_sighup to true
void agi::handle_sighup(int) {
   got_sighup = true;
}

/// Set got_sigpipe to true
void agi::handle_sigpipe(int) {
   got_sigpipe = true;
}

/** Check for a hangup.
 *
 * This function tests if we have recieved a hangup from Asterisk in the form 
 * of a SIGHUP signal.  If we have it throws an agi::hangup_signal exception.
 *
 * @exception agi::hangup_signal thrown if we have recieved a hangup
 */
void agi::test_hangup() const {
   if (got_sighup)
      throw agi::hangup_signal();
}

/** Clear any sig hup or sig pipe flags.
 */
void agi::clear() {
   got_sighup = false;
   got_sigpipe = false;
}

/** @defgroup Commands agi Commands
 * @{
 */

/** Answer the channel.
 *
 * Answers the channel if not already in answer state.  Throws an exception on 
 * error.
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::answer() {
   agi::result result = execute("ANSWER");
   return *this;
}

/** Get the status of a channel.
 *
 * Returns the status of the specified channel.  If no channel is given then the 
 * status of the current channel is used.
 *
 * Return values:
 * - 0 - Channel is down and available
 * - 1 - Channel is down but reserved
 * - 2 - Channel is off hook
 * - 3 - Digits (or equivalent) have been dialed
 * - 4 - Line is ringing
 * - 5 - Remote end is ringing
 * - 6 - Line is up
 * - 7 - Line is busy
 *
 * @param channel the channel to get the status of
 *
 * @return the status of the given channel
 *
 * @todo Possibly return status values as values from an enum
 */
int agi::channel_status(const std::string& channel) const {
   return execute("CHANNEL STATUS " + quote(channel)).result;
}

/** Play a file on the channel with the option to control playback.
 *
 * @param filename the file to stream (no extension should be specfied)
 * @param escape_digits digits to break on
 * @param skip_time the number of milliseconds to skip in a fastforward or 
 * rewind operation
 * @param fastforward the character to use for fastforwarding the stream
 * @param rewind the character to use for rewinding the stream
 * @param pause the character to use to pause the stream
 *
 * @return the digit pressed if any (0 for no digit)
 */
char agi::control_stream_file(const std::string& filename, const std::string& escape_digits, int skip_time, char fastforward, char rewind, char pause) const {
   ostringstream ss;
   ss << "STREAM FILE " << quote(filename) << " " << quote(escape_digits);

   /* The extra if checks below are just incase a user decides to pass one of 
    * our default values (0 in this case).  The checks make sure the extra 
    * values are passed in the correct positions for Asterisk to properly 
    * identify them.
    */
   if (skip_time != control_stream_file_skip_time || fastforward || rewind || pause)
      ss << " " << skip_time;
   if (fastforward || rewind || pause)
      ss << " " << quote(fastforward);
   if (rewind || pause)
      ss << " " << quote(rewind);
   if (pause)
      ss << " " << quote(pause);

   return execute(ss.str()).result;
}

/** Delete a database key.
 *
 * @param family the family to delete from
 * @param key the key to delete
 *
 * @exception agi::database_error thown when there is a error deleting a 
 * key/value
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::database_del(const std::string& family, const std::string& key) {
   agi::result result = execute("DATABASE DEL " + quote(family) + " " + quote(key));

   // posibly pass key and value with the execption
   if (result.result == 0)
      throw agi::database_error("Error deleting database value");
   return *this;
}

/** Delete a database family or keytree.
 *
 * @param family the family to delete or delete from
 * @param keytree the key tree to delete
 *
 * @exception agi::Database thown when there is a error deleting a family/value
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::database_deltree(const std::string& family, const std::string& keytree) {
   agi::result result = execute("DATABASE DELTREE " + quote(family) + " " + quote(keytree));

   // posibly pass key and value with the execption
   if (result.result == 0)
      throw agi::database_error("Error deleting database family/keytree");
   return *this;
}

/** Get a value out of the Asterisk database.
 *
 * This function may throw an exception in the future.  Currently it returns an 
 * empty string if the value does not exist in the database.
 *
 * @param family the family the key is in
 * @param key the key to look up
 */
string agi::database_get(const std::string& family, const std::string& key) const {
   return execute("DATABASE GET " + quote(family) + " " + quote(key)).data;
}

/** Put a value in the database.
 *
 * @param family the database family to use
 * @param key the database key to use
 * @param value the value to put in the database
 *
 * @exception agi::database_error thrown if there are any problems inserting 
 * the value into the database
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::database_put(const std::string& family, const std::string& key, const std::string& value) {
   if (execute("DATABASE PUT " + quote(family) + " " + quote(key) + quote(value)).result == 0)
      throw agi::database_error("Error putting value in Asterisk database");
   return *this;
}

/** Execute a given application.
 *
 * @todo This function may throw an exception if the application is not found in 
 * the future.
 * 
 * @param app the application to execute
 * @param options options for the application (these should be seperated with 
 * pipes (|))
 * 
 * @return the return value from the application or -2 if the application was 
 * not found
 */
int agi::exec(const std::string& app, const std::string& options) const {
   return execute("EXEC " + quote(app) + " " + quote(options)).result;
}

/** Stream a file and receive DTMF data.
 *
 * @param file the file to be streamed
 * @param timeout a timeout to wait for digits in seconds
 * @param max_digits the maximum number of digits to receive
 *
 * @return the DTMF digits recieved, or '' an empty string on timeout
 */
string agi::get_data(const std::string& file, int timeout, int max_digits) const {
   ostringstream ss;
   ss << "GET DATA " << file;

   if (timeout != 0 || max_digits != get_data_max_digits)
      ss << " " << timeout;

   if (max_digits != get_data_max_digits)
      ss << " " << max_digits;

   return execute(ss.str()).result_string;
}

/** More robust version of astxx::agi::get_variable.
 *
 * This function returns the specfied variable name.  It differs from 
 * astxx::agi::get_variable in that it handles complex variable names and 
 * builtin variables.
 *
 * Variable names should take the same format they do in the dialplan (ie.  
 * ${VARIABLE_NAME}).  This is different from agi::get_variable in that 
 * get_variable accepts a plain variable name and will not parse complex names.
 *
 * @param variable the variable to retrieve
 * @param channel the channel to look in
 *
 * @todo Possibly throw an exception if the variable is not found.
 *
 * @return the value of the specfied variable
 */
string agi::get_full_variable(const std::string& variable, const std::string& channel) const {
   agi::result result;
   try {
      ostringstream ss;
      ss << "GET FULL VARIABLE " << quote(variable);

      if (!channel.empty())
         ss << " " << quote(channel);

      result = execute(ss.str());
   }
   catch (agi::hangup_result& e) {
      result.data = "hangup"; 
   }

   if (result.result == 0); // possibly throw an exception here

   return result.data;
}

/** Stream a file and read a digit.
 *
 * @param file the file to stream
 * @param escape_digits digits to return on
 * @param timeout a timeout in seconds
 *
 * @return the digit pressed if any (0 for no digit)
 */      
char agi::get_option(const std::string& file, const std::string& escape_digits, int timeout) const {
   ostringstream ss;
   ss << "GET OPTION " << quote(file) << " " << quote(escape_digits);

   if (timeout != 0)
      ss << " " << timeout;

   agi::result result = execute(ss.str());

   return result.result;
}

/** Get the value of a channel variable.
 *
 * @param variable the variable to retrieve
 *
 * @return the value of the variable
 *
 * @todo Possibly throw exception if the variable does not exist
 */
string agi::get_variable(const std::string& variable) const {
   agi::result result;
   try {
      result = execute("GET VARIABLE " + quote(variable));
   }
   catch (agi::hangup_result& e) {
      result.data = "hangup"; 
   }

   if (result.result == 0); // possibly throw an exception here

   return result.data;
}

/** Hangup a channel.
 *
 * @param channel hangup the channel to hang up, if omitted, use the current 
 * channel.
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::hangup(const std::string& channel) {
   execute("HANGUP " + quote(channel));
   return *this;
}

/** Does nothing.
 * @return a reference to this astxx::agi instance
 */
agi& agi::noop() {
   execute("NOOP");
   return *this;
}

/** Receive a character from channels that support it.
 *
 * @param timeout the timeout to wait in milliseconds
 *
 * @return the character recieved
 */
char agi::receive_char(int timeout) const {
   std::stringstream ss;
   ss << "RECEIVE CHAR " << timeout;
   return execute(ss.str()).result;
}

/** Receive text from channels that support it.
 * @param timeout the timeout to wait in milliseconds
 * @todo Maybe we should catch the application_error and just return an empty 
 * string.  We could also repackage the exception as an agi::timeout exception.
 * @exception agi::application_error when a timeout occurs
 * @return the text recieved
 */
std::string agi::receive_text(int timeout) const {
   std::stringstream ss;
   ss << "RECEIVE TEXT " << timeout;
   return execute(ss.str()).data;
}

/** Record audio and store it in the given location.
 *
 * Record an audio stream to a file until a given dtmf digit is pressed or a 
 * timeout is reached.
 *
 * @param filename the name of the file to record to
 * @param format the format of the file
 * @param escape_digits a string of digits to return on
 * @param timeout a timeout in milliseconds (use -1 for no timeout)
 * @param silence the maximum amount of silence to accept at the end of the 
 * (use 0 for no silence timeout)
 * recording in seconds
 * @param beep whether to beep or not before begining the recording
 * @param offset the offset to begin writing at
 *
 * @return any digits pressed or 0 on timeout
 */
char agi::record_file(const std::string& filename, const std::string& format, const std::string& escape_digits, int timeout, int silence, bool beep, long offset) const {
   ostringstream ss;
   ss << "RECORD FILE " << quote(filename) << " " << quote(format) << " " << quote(escape_digits) << " " << timeout;

   if (offset)
      ss << " " << offset;

   if (beep)
      ss << " BEEP";

   if (silence)
      ss << " s=" << silence;

   return execute(ss.str()).result;
}

/** Say a given character string.
 *
 * @param alphanum the string to say
 * @param escape_digits digits to break on
 *
 * @return the digit pressed if any (0 for no digit)
 */
char agi::say_alpha(const std::string& alphanum, const std::string& escape_digits) const {
   return execute("SAY ALPHA " + quote(alphanum) + " " + quote(escape_digits)).result;
}

/** Say a given character string.
 *
 * @param number the number to say
 * @param escape_digits digits to break on
 *
 * @return the digit pressed if any (0 for no digit)
 */
char agi::say_alpha(int number, const std::string& escape_digits) const {
   ostringstream ss;
   ss << "SAY ALPHA " << number << " " << quote(escape_digits);
   return execute(ss.str()).result;
}

/** Say a given date.
 *
 * @param date the date in seconds since the epoch
 * @param escape_digits digits to break on
 *
 * @return the digit pressed if any (0 for no digit)
 */
char agi::say_date(time_t date, const std::string& escape_digits) const {
   ostringstream ss;
   ss << "SAY DATE " << date << " " << quote(escape_digits);
   return execute(ss.str()).result;
}

/** Say a given date in the format specfied.
 *
 * Say a given date and/or time in the format specfied.  Return early if any of 
 * the given DTMF digits are pressed.  See voicemail.conf for formatting 
 * options.  The format defaults to "ABdY 'digits/at' IMp".
 *
 * @param date the date/time to say in seconds since the epoch
 * @param escape_digits digits to break on
 * @param format the format to use (see voicemail.conf)
 * @param timezone the timezone the time is in (acceptable values can be found 
 * in /usr/share/zoneinfo)
 *
 * @return the digit pressed if any (0 for no digit)
 */
char agi::say_datetime(time_t date, const std::string& escape_digits, const std::string& format, const std::string& timezone) const {
   ostringstream ss;
   ss << "SAY DATETIME " << date << " " << quote(escape_digits);

   if (not format.empty() or not timezone.empty())
      ss << " " << quote(format);

   if (not timezone.empty())
      ss << " " << quote(timezone);

   return execute(ss.str()).result;
}

/** Say a given digit string as indiviual digits.
 *
 * @param digits the digits to say
 * @param escape_digits digits to break on
 *
 * @return the digit pressed if any (0 for no digit)
 */
char agi::say_digits(int digits, const std::string& escape_digits) const {
   ostringstream ss;
   ss << "SAY DIGITS " << digits << " " << quote(escape_digits);
   return execute(ss.str()).result;
}

/** Say a given number.
 *
 * @param number the number to say
 * @param escape_digits digits to break on
 *
 * @return the digit pressed if any (0 for no digit)
 */
char agi::say_number(int number, const std::string& escape_digits) const {
   ostringstream ss;
   ss << "SAY NUMBER " << number << " " << quote(escape_digits);
   return execute(ss.str()).result;
}

/** Say a given character string with phonetics (alpha, bravo, charile...).
 *
 * @param characters the string to say
 * @param escape_digits digits to break on
 *
 * @return the digit pressed if any (0 for no digit)
 */
char agi::say_phonetic(const std::string& characters, const std::string& escape_digits) const {
   return execute("SAY PHONETIC " + quote(characters) + " " + quote(escape_digits)).result;
}

/** Says a given time.
 *
 * @param time the time to say in seconds since the epoch
 * @param escape_digits digits to break on
 *
 * @return the digit pressed if any (0 for no digit)
 */
char agi::say_time(time_t time, const std::string& escape_digits) const {
   ostringstream ss;
   ss << "SAY TIME " << time << " " << quote(escape_digits);
   return execute(ss.str()).result;
}

/** Send an image to channels that support it.
 *
 * Most channels do not support the transmission of images.
 *
 * @param image the name of the image, should not include an extension
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::send_image(const std::string& image) {
   execute("SEND IMAGE " + quote(image));
   return *this;
}

/** Send text to channels that support it.
 *
 * Most channels do not support the transmission of text.
 *
 * @param text the text to send
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::send_text(const std::string& text) {
   execute("SEND TEXT " + quote(text));
   return *this;
}

/** Hangup a channel after the specified time has passed.
 *
 * @param delay the number of seconds to wait before hanging up, 0 will disable 
 * any pending auto hangups
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::set_autohangup(int delay) {
   ostringstream ss;
   ss << "SET AUTOHANGUP " << delay;
   execute(ss.str());
   return *this;
}

/** Set the callerid for the current channel.
 *
 * @param number the callerid to set
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::set_callerid(int number) {
   ostringstream ss;
   ss << "SET CALLERID " << number;
   execute(ss.str());
   return *this;
}

/** Set the callerid for the current channel.
 *
 * @param cid the callerid to set, should be a number or in the form "name" 
 * \<number\> or name \<number\>
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::set_callerid(const std::string& cid) {
   execute("SET CALLERID " + quote(cid));
   return *this;
}

/** Set the channel context.
 *
 * Sets the context for continuation upon exiting the application.
 *
 * @param context the context to use
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::set_context(const std::string& context) {
   execute("SET CONTEXT " + quote(context));
   return *this;
}

/** Set the channel extension.
 *
 * Sets the extension for continuation upon exiting the application.
 *
 * @param extension the extension to use
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::set_extension(int extension) {
   ostringstream ss;
   ss << "SET EXTENSION " << extension;
   execute(ss.str());
   return *this;
}

/** Set the channel extension.
 *
 * Sets the extension for continuation upon exiting the application.
 *
 * @param extension the extension to use
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::set_extension(const std::string& extension) {
   execute("SET EXTENSION " + quote(extension));
   return *this;
}

/** Enable/Disable music on hold generator.
 *
 * @param enable enable or disable music on hold.
 * @param music_class the music on hold class to use
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::set_music(bool enable, const std::string& music_class) {
   ostringstream ss;
   ss << "SET MUSIC ";

   if (enable)
      ss << "ON";
   else
      ss << "OFF";

   if (not music_class.empty())
      ss << " " << quote(music_class);

   execute(ss.str());
   return *this;
}

/** Set the priority of the channel.
 *
 * Sets the priority for continuation upon exiting the application.
 *
 * @param priority the priority to use
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::set_priority(int priority) {
   ostringstream ss;
   ss << "SET PRIORITY " << priority;
   execute(ss.str());
   return *this;
}

/** Set the priority of the channel.
 *
 * Sets the priority for continuation upon exiting the application.
 *
 * @param priority the priority to use
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::set_priority(const std::string& priority) {
   execute("SET PRIORITY " + quote(priority));
   return *this;
}

/** Set a given variable.
 *
 * @param variable the variable to set
 * @param value the value to use
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::set_variable(const std::string& variable, const std::string& value) {
   execute("SET VARIABLE " + quote(variable) + " " + quote(value));
   return *this;
}

/** Plays a given file on the audio channel.
 *
 * @param filename the file to stream (no extension should be specfied)
 * @param escape_digits digits to break on
 * @param offset the offset to begin playback at
 *
 * @return the digit pressed if any (0 for no digit)
 */
char agi::stream_file(const std::string& filename, const std::string& escape_digits, long offset) const {
   ostringstream ss;
   ss << "STREAM FILE " << quote(filename) << " " << quote(escape_digits);

   if (offset)
      ss << " " << offset;

   return execute(ss.str()).result;
}

/** Toggles TDD mode on or off (for the deaf).
 *
 * @param enable wheather to enable or disable TDD on the channel
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::tdd_mode(bool enable) {
   if (enable)
      execute("TDD MODE ON");
   else
      execute("TDD MODE OFF");
   return *this;
}

/** Change the TDD mode of the channel.
 *
 * @param mode the mode to set (on, off, tdd, mate)
 *
 * @todo maybe use enum for modes
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::tdd_mode(const std::string& mode) {
   execute("TDD MODE " + quote(mode));
   return *this;
}

/** Log a message to the Asterisk verbose log.
 *
 * Feel free to pass newlines (\\n) in your text string as this function 
 * handles parsing them out into multiple verbose commands.
 * 
 * @param text the message to log
 * @param level the level to log at
 *
 * @return a reference to this astxx::agi instance
 */
agi& agi::verbose(const std::string& text, int level) {
   string::size_type end_pos = 0;
   string::size_type start_pos = 0;
   while (end_pos != string::npos) {
      end_pos = text.find('\n', start_pos);

      ostringstream ss;
      ss << "VERBOSE "
         << quote(text.substr(start_pos, end_pos == string::npos ? string::npos : end_pos - start_pos))
         << " " << level;
      execute(ss.str());

      start_pos = end_pos + 1;
   }
   
   return *this;
}

/** Wait for a digit to be pressed.
 *
 * @param timeout timeout to wait in miliseconds, use -1 (the default) for no 
 * timeout
 *
 * @return the digit pressed if any (0 for no digit)
 */
char agi::wait_for_digit(long timeout) const {
   ostringstream ss;
   ss << "WAIT FOR DIGIT " << timeout;
   return execute(ss.str()).result;
}

/* @} end of agi Commands group */

