/*
* TLS Record Handling
* (C) 2004-2010 Jack Lloyd
*
* Released under the terms of the Botan license
*/

#ifndef BOTAN_TLS_RECORDS_H__
#define BOTAN_TLS_RECORDS_H__

#include <botan/tls_suites.h>
#include <botan/pipe.h>
#include <botan/mac.h>
#include <botan/secqueue.h>
#include <vector>

#if defined(BOTAN_USE_STD_TR1)

#if defined(BOTAN_BUILD_COMPILER_IS_MSVC)
    #include <functional>
#else
    #include <tr1/functional>
#endif

#elif defined(BOTAN_USE_BOOST_TR1)
  #include <boost/tr1/functional.hpp>
#else
  #error "No TR1 library defined for use"
#endif

namespace Botan {

class SessionKeys;

/**
* TLS Record Writer
*/
class BOTAN_DLL Record_Writer
   {
   public:
      void send(byte type, const byte input[], size_t length);
      void send(byte type, byte val) { send(type, &val, 1); }

      void alert(Alert_Level level, Alert_Type type);

      void activate(const TLS_Cipher_Suite& suite,
                    const SessionKeys& keys,
                    Connection_Side side);

      void set_version(Version_Code version);

      Version_Code get_version() const;

      void reset();

      void set_maximum_fragment_size(size_t max_fragment);

      Record_Writer(std::tr1::function<void (const byte[], size_t)> output_fn);

      ~Record_Writer() { delete m_mac; }
   private:
      void send_record(byte type, const byte input[], size_t length);

      std::tr1::function<void (const byte[], size_t)> m_output_fn;

      Pipe m_cipher;
      MessageAuthenticationCode* m_mac;

      size_t m_block_size, m_mac_size, m_iv_size, m_max_fragment;

      u64bit m_seq_no;
      byte m_major, m_minor;
   };

/**
* TLS Record Reader
*/
class BOTAN_DLL Record_Reader
   {
   public:
      void add_input(const byte input[], size_t input_size);

      /**
      * @param msg_type (output variable)
      * @param buffer (output variable)
      * @return Number of bytes still needed (minimum), or 0 if success
      */
      size_t get_record(byte& msg_type,
                        MemoryRegion<byte>& buffer);

      SecureVector<byte> get_record(byte& msg_type);

      void activate(const TLS_Cipher_Suite& suite,
                    const SessionKeys& keys,
                    Connection_Side side);

      void set_version(Version_Code version);

      Version_Code get_version() const;

      void reset();

      bool currently_empty() const { return input_queue.size() == 0; }

      Record_Reader() { mac = 0; reset(); }

      ~Record_Reader() { delete mac; }
   private:
      SecureQueue input_queue;

      Pipe cipher;
      MessageAuthenticationCode* mac;
      size_t block_size, mac_size, iv_size;
      u64bit seq_no;
      byte major, minor;
   };

}

#endif
