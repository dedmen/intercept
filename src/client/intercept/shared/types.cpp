#include "types.hpp"

namespace intercept {
    namespace rv_types {
        uintptr_t game_data_string::type_def;
        uintptr_t game_data_string::data_type_def;

        uintptr_t game_data_object::type_def;
        uintptr_t game_data_object::data_type_def;

        uintptr_t game_data_number::type_def;
        uintptr_t game_data_number::data_type_def;

        uintptr_t game_data_array::type_def;
        uintptr_t game_data_array::data_type_def;

        uintptr_t game_data_array_stack::type_def;
        uintptr_t game_data_array_stack::data_type_def;

        uintptr_t game_data_bool::type_def;
        uintptr_t game_data_bool::data_type_def;



        uintptr_t game_value::__vptr_def;
        std::string rv_string::string() {
            return std::string((char *)&char_string);
        }

        value_type op_value_entry::type() {
            if (single_type != NULL) {
                return{ single_type->type_name->string() };
            }
            else {
                return{
                    compound_type->types->first->type_name->string(),
                    compound_type->types->second->type_name->string()
                };
            }
        }

        std::string op_value_entry::type_str() {
            if (single_type != NULL) {
                return single_type->type_name->string();
            }
            else {
                return
                    compound_type->types->first->type_name->string() + "_" +
                    compound_type->types->second->type_name->string();
            }
        }

        std::string game_data_string::get_string() {
            return std::string((char *)&raw_string->char_string);
        }

        bool game_data_string::set_string(std::string val_) {
            if (ref_count_internal == 0x0000dede) {
                if (val_.length() + 1 < 2048) {
                    memcpy(&raw_string->char_string, val_.c_str(), val_.length() + 1);
                    raw_string->length = val_.length() + 1;
                    return true;
                }
            }
            return false;
        }

        game_value::game_value() : __vptr(NULL), data(NULL) {
            __vptr = __vptr_def;
        };

        void game_data_array::allocate(size_t size_) {
            data = new game_value[size_];
        }
    }
}

