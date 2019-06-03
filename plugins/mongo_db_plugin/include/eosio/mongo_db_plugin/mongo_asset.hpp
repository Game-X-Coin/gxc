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


struct mongo_asset : fc::reflect_init {
public:
   asset to_asset()const { return asset(amount, sym); }

   friend struct fc::reflector<mongo_asset>;

   void reflector_init()const {
      auto a = to_asset(); // for validation check by eosio::asset constructor
   }

private:
   share_type amount;
   symbol     sym;
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

FC_REFLECT(eosio::chain::mongo_asset, (amount)(sym))
FC_REFLECT(eosio::chain::mongo_extended_asset, (quantity)(contract))
