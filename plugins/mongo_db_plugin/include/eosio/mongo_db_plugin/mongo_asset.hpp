/**
 *  @file
 *  @copyright defined in gxc/LICENSE
 */
#pragma once
#include <eosio/chain/asset.hpp>
#include <fc/reflect/reflect.hpp>

namespace eosio { namespace chain {

template <typename T>
inline fc::variant variant_from_stream(fc::datastream<const char*>& stream) {
   T temp;
   fc::raw::unpack( stream, temp );
   return fc::variant(temp);
}

template <typename T>
auto pack_unpack() {
   return std::make_pair<abi_serializer::unpack_function, abi_serializer::pack_function>(
      []( fc::datastream<const char*>& stream, bool is_array, bool is_optional) -> fc::variant  {
         if( is_array )
            return variant_from_stream<vector<T>>(stream);
         else if ( is_optional )
            return variant_from_stream<optional<T>>(stream);
         return variant_from_stream<T>(stream);
      },
      []( const fc::variant& var, fc::datastream<char*>& ds, bool is_array, bool is_optional ){
         if( is_array )
            fc::raw::pack( ds, var.as<vector<T>>() );
         else if ( is_optional )
            fc::raw::pack( ds, var.as<optional<T>>() );
         else
            fc::raw::pack( ds,  var.as<T>());
      }
   );
}

struct mongo_symbol : fc::reflect_init {
public:
   explicit mongo_symbol(uint64_t v = CORE_SYMBOL): m_value(v) {
      auto a = to_symbol();
   }

   mongo_symbol(uint8_t p, const char* s): m_value(string_to_symbol(p, s)) {
      auto a = to_symbol();
   }

   symbol to_symbol()const { return symbol(m_value); }

   friend struct fc::reflector<mongo_symbol>;

   void reflector_init()const {
      auto a = to_symbol(); // for validation check by eosio::symbol constructor
   }

private:
   uint64_t m_value;
};

struct mongo_asset : fc::reflect_init {
public:
   asset to_asset()const { return asset(amount, sym.to_symbol()); }

   friend struct fc::reflector<mongo_asset>;

   void reflector_init()const {
      auto a = to_asset(); // for validation check by eosio::asset constructor
   }

private:
   share_type amount;
   mongo_symbol sym;
};

struct mongo_extended_asset : fc::reflect_init {
public:
   friend struct fc::reflector<mongo_extended_asset>;

   void reflector_init()const {
      extended_asset a(quantity.to_asset(), contract); // for validation check by eosio::asset constructor
   }

private:
   mongo_asset quantity;
   name contract;
};

}} /// namespace eosio::chain

namespace fc {
   inline void to_variant(const eosio::chain::mongo_symbol& var, fc::variant& vo) {
      auto s = var.to_symbol();
      vo = fc::mutable_variant_object()("decimals", s.decimals())("code", s.to_symbol_code());
   }
   inline void from_variant(const fc::variant& var, eosio::chain::mongo_symbol& vo) {
      vo = eosio::chain::mongo_symbol{
         eosio::chain::symbol(
            var["decimals"].as_uint64(),
            var["code"].as_string().data()
         ).value()
      };
   }
}

FC_REFLECT(eosio::chain::mongo_symbol, (m_value))
FC_REFLECT(eosio::chain::mongo_asset, (amount)(sym))
FC_REFLECT(eosio::chain::mongo_extended_asset, (quantity)(contract))
