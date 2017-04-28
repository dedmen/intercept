#include "loader.hpp"
#include "controller.hpp"
#include <Psapi.h>
#pragma comment (lib, "Psapi.lib")//GetModuleInformation


namespace intercept {
    unary_function loader::_initial_trampoline;

    loader::loader() : _attached(false), _patched(false) {
    }

    loader::~loader() {

    }

    int __cdecl loader::_initial_patch(char * a_, int b_, int c_)
    {
        loader::get().do_function_walk(b_);
        return _initial_trampoline(a_, b_, c_);
    }
    
    bool loader::get_function(std::string function_name_, unary_function & function_, std::string arg_signature_)
    {
        auto it = _unary_operators.find(function_name_);
        if (it != _unary_operators.end()) {
            for (auto op : it->second) {
                if (op.op->arg_type.type().count(arg_signature_)) {
                    function_ = (unary_function)op.op->procedure_addr;
                    return true;
                }
            }
            return false;
        }
        return false;
    }

    bool loader::get_function(std::string function_name_, unary_function & function_)
    {
        auto it = _unary_operators.find(function_name_);
        if (it != _unary_operators.end()) {
            function_ = (unary_function)it->second[0].op->procedure_addr;
            return true;
        }
        return false;
    }

    bool loader::get_function(std::string function_name_, binary_function & function_)
    {
        auto it = _binary_operators.find(function_name_);
        if (it != _binary_operators.end()) {
            function_ = (binary_function)it->second[0].op->procedure_addr;
            return true;
        }
        return false;
    }

    bool loader::get_function(std::string function_name_, binary_function & function_, std::string arg1_signature_, std::string arg2_signature_)
    {
        auto it = _binary_operators.find(function_name_);
        if (it != _binary_operators.end()) {
            for (auto op : it->second) {
                if (op.op->arg1_type.type().count(arg1_signature_) && op.op->arg2_type.type().count(arg2_signature_)) {
                    function_ = (binary_function)op.op->procedure_addr;
                    return true;
                }
            }
            return false;
        }
        return false;
    }

    bool loader::get_function(std::string function_name_, nular_function & function_)
    {
        auto it = _nular_operators.find(function_name_);
        if (it != _nular_operators.end()) {
            function_ = (nular_function)it->second[0].op->procedure_addr;
            return true;
        }
        return false;
    }

    bool loader::hook_function(std::string function_name_, void * hook_, unary_function & trampoline_)
    {
        LOG(DEBUG) << "Attempting to hook unary function " << function_name_;
        auto op = _unary_operators.find(function_name_);
        if (op != _unary_operators.end()) {
            uintptr_t op_ptr = op->second[0].procedure_ptr_addr;
            trampoline_ = *(unary_function *)op_ptr;
            memcpy((void *)op_ptr, &hook_, sizeof(uintptr_t));
            return true;
        }
        return false;
    }

    bool loader::hook_function(std::string function_name_, void * hook_, binary_function & trampoline_)
    {
        LOG(DEBUG) << "Attempting to hook binary function " << function_name_;
        auto op = _binary_operators.find(function_name_);
        if (op != _binary_operators.end()) {
            uintptr_t op_ptr = op->second[0].procedure_ptr_addr;
            trampoline_ = *(binary_function *)op_ptr;
            memcpy((void *)op_ptr, &hook_, sizeof(uintptr_t));
            return true;
        }
        return false;
    }

    bool loader::hook_function(std::string function_name_, void * hook_, nular_function & trampoline_)
    {
        LOG(DEBUG) << "Attempting to hook nular function " << function_name_;
        auto op = _nular_operators.find(function_name_);
        if (op != _nular_operators.end()) {
            uintptr_t op_ptr = op->second[0].procedure_ptr_addr;
            trampoline_ = *(nular_function *)op_ptr;
            memcpy((void *)op_ptr, &hook_, sizeof(uintptr_t));
            return true;
        }
        return false;
    }

    bool loader::unhook_function(std::string function_name_, void * hook_, unary_function & trampoline_)
    {
        LOG(DEBUG) << "Attempting to unhook unary function " << function_name_;
        if (&trampoline_ == 0x0)
            return false;
        auto op = _unary_operators.find(function_name_);
        if (op != _unary_operators.end()) {
            uintptr_t op_ptr = op->second[0].procedure_ptr_addr;
            memcpy((void *)op_ptr, &trampoline_, sizeof(uintptr_t));
            return true;
        }
        return false;
    }

    bool loader::unhook_function(std::string function_name_, void * hook_, binary_function & trampoline_)
    {
        LOG(DEBUG) << "Attempting to unhook binary function " << function_name_;
        if (&trampoline_ == 0x0)
            return false;
        auto op = _binary_operators.find(function_name_);
        if (op != _binary_operators.end()) {
            uintptr_t op_ptr = op->second[0].procedure_ptr_addr;
            memcpy((void *)op_ptr, &trampoline_, sizeof(uintptr_t));
            return true;
        }
        return false;
    }

    bool loader::unhook_function(std::string function_name_, void * hook_, nular_function & trampoline_)
    {
        LOG(DEBUG) << "Attempting to unhook nular function " << function_name_;
        if (&trampoline_ == 0x0)
            return false;
        auto op = _nular_operators.find(function_name_);
        if (op != _nular_operators.end()) {
            uintptr_t op_ptr = op->second[0].procedure_ptr_addr;
            memcpy((void *)op_ptr, &trampoline_, sizeof(uintptr_t));
            return true;
        }
        return false;
    }

    namespace __internal {	 //@Nou where should i store this stuff? It shall only be used internally.

        struct gsFunction {	//#TODO shouldn't everything in here be const?
            const rv_string* _name;
            uint32_t placeholder1;//0x4
            uint32_t placeholder2;//0x8 actually a pointer to empty memory
            uint32_t placeholder3;//0xC
            uint32_t placeholder4;//0x10
            uint32_t placeholder5;//0x14
            uint32_t placeholder6;//0x18
            uint32_t placeholder7;//0x1C
            uint32_t placeholder8;//0x20
            uint32_t placeholder9;//0x24
            const rv_string* _name2;//0x28 this is (tolower name)
            unary_operator * _operator;//0x2C
            uint32_t placeholder10;//0x30 RString to something
            const rv_string* _description;//0x34
            const rv_string* _example;
            const rv_string* _example2;
            const rv_string* placeholder11;
            const rv_string* placeholder12;
            const rv_string* _category; //0x48
            //const rv_string* placeholder13;
        };
        struct gsOperator {
            const  rv_string* _name;
            uint32_t placeholder1;//0x4
            uint32_t placeholder2;//0x8 actually a pointer to empty memory
            uint32_t placeholder3;//0xC
            uint32_t placeholder4;//0x10
            uint32_t placeholder5;//0x14
            uint32_t placeholder6;//0x18
            uint32_t placeholder7;//0x1C
            uint32_t placeholder8;//0x20
            uint32_t placeholder9;//0x24
            const rv_string* _name2;//0x28 this is (tolower name)
            int32_t placeholder10; //0x2C Small int 0-5
            binary_operator * _operator;//0x30
            const rv_string* _leftType;//0x34 Description of left hand side parameter
            const rv_string* _rightType;//0x38 Description of right hand side parameter
            const rv_string* _description;//0x3C
            const rv_string* _example;//0x40
            const rv_string* placeholder11;//0x44
            const rv_string* _version;//0x48 some version number
            const rv_string* placeholder12;//0x4C
            const rv_string* _category; //0x50
        };
		struct gsNular {
            const rv_string* _name;
            uint32_t placeholder1;//0x4
            uint32_t placeholder2;//0x8 actually a pointer to empty memory
            uint32_t placeholder3;//0xC
            uint32_t placeholder4;//0x10
            uint32_t placeholder5;//0x14
            uint32_t placeholder6;//0x18
            uint32_t placeholder7;//0x1C -- change
            uint32_t placeholder8;//0x20
            const rv_string* _name2;//0x24 this is (tolower name)
            nular_operator * _operator;//0x28
            const rv_string* _description;//0x2C
            const rv_string* _example;
            const rv_string* _example2;
            const rv_string* _version;//0x38 some version number
            const rv_string* placeholder10;
            const rv_string* _category; //0x40
            uint32_t placeholder11;//0x44
        };



    }


    
    void loader::do_function_walk(uintptr_t state_addr_) {
        uintptr_t unary_hash = state_addr_ + 0xC;
        uintptr_t binary_hash = state_addr_ + 0x18;
        uintptr_t nulars_hash = state_addr_ + 0x24;

        /*
        Unary Hashmap

        This is a hashmap using a key->bucket system, so an array of arrays keyed by a hash.
        We don't give a fuck about that though, we just want to iterate through all of the
        buckets and in turn each item in the bucket, because they are our operator entries.
        */
        uintptr_t bucket_start = *(uintptr_t *)unary_hash;
        uint32_t bucket_count = *(uint32_t *)(unary_hash + 4);

        for (uint32_t bucket_offset = 0; bucket_offset < bucket_count; ++bucket_offset) {

            uintptr_t bucket = (bucket_start + (12 * bucket_offset));
            uint32_t bucket_length = *(uintptr_t *)(bucket + 4);

            uintptr_t entry_start = *(uintptr_t *)bucket;

            for (uint32_t entry_offset = 0; entry_offset < bucket_length; ++entry_offset) {

                uint32_t op_count = *(uintptr_t *)(entry_start + (0x34 * entry_offset) + 4);
                __internal::gsFunction* op_start = *(__internal::gsFunction**)(entry_start + (0x34 * entry_offset));
                for (uint32_t op_offset = 0; op_offset < op_count; ++op_offset) {
                    __internal::gsFunction& entry = op_start[op_offset];
                    unary_entry new_entry;
                    new_entry.op = entry._operator;
                    new_entry.procedure_ptr_addr = (uintptr_t) &entry._operator->procedure_addr;
                    const rv_string* name_entry = entry._name;
                    new_entry.name = &(name_entry->char_string);
                    LOG(INFO) << "Found unary operator: " <<
                        new_entry.op->return_type.type_str() << " " <<
                        new_entry.name <<
                        "(" << new_entry.op->arg_type.type_str() << ")" <<
                        " @ " << new_entry.op->procedure_addr;
                    std::string name = std::string(new_entry.name);
                    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
                    _unary_operators[name].push_back(new_entry);
                }
            }
        }

        /*
        Binary Hashmap
        */
        bucket_start = *(uintptr_t *)binary_hash;
        bucket_count = *(uint32_t *)(binary_hash + 4);


        for (uint32_t bucket_offset = 0; bucket_offset < bucket_count; ++bucket_offset) {

            uintptr_t bucket = (bucket_start + (12 * bucket_offset));
            uint32_t bucket_length = *(uint32_t *)(bucket + 4);

            uintptr_t entry_start = *(uintptr_t *)bucket;

            for (uint32_t entry_offset = 0; entry_offset < bucket_length; ++entry_offset) {
                uint32_t op_count = *(uintptr_t *)(entry_start + (0x38 * entry_offset) + 4);
                __internal::gsOperator* op_start = *(__internal::gsOperator**)(entry_start + (0x38 * entry_offset));
                for (uint32_t op_offset = 0; op_offset < op_count; ++op_offset) {
                    __internal::gsOperator& entry = op_start[op_offset];
                   // uintptr_t entry = (op_start + (0x4C * op_offset));
                    binary_entry new_entry;
                    new_entry.op = entry._operator;
                    new_entry.procedure_ptr_addr = (uintptr_t) &entry._operator->procedure_addr;
                    new_entry.name = &entry._name->char_string;
                    LOG(INFO) << "Found binary operator: " <<
                        new_entry.op->return_type.type_str() << " " <<
                        "(" << new_entry.op->arg1_type.type_str() << ")" <<
                        new_entry.name <<
                        "(" << new_entry.op->arg2_type.type_str() << ")" <<
                        " @ " << new_entry.op->procedure_addr;
                    std::string name = std::string(new_entry.name);
                    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
                    _binary_operators[name].push_back(new_entry);
                }
            }
        }

        /*
        Nular Hashmap
        */
        bucket_start = *(uintptr_t *)nulars_hash;
        bucket_count = *(uint32_t *)(nulars_hash + 4);


        for (uint32_t bucket_offset = 0; bucket_offset < bucket_count; ++bucket_offset) {

            uintptr_t bucket = (bucket_start + (12 * bucket_offset));
            uint32_t bucket_length = *(uintptr_t *)(bucket + 4);

            __internal::gsNular* entry_start = *(__internal::gsNular**)bucket;

            for (uint32_t entry_offset = 0; entry_offset < bucket_length; ++entry_offset) {
                __internal::gsNular entry = entry_start[entry_offset];
                nular_entry new_entry;
                new_entry.op = entry._operator;
                new_entry.procedure_ptr_addr = (uintptr_t) &entry._operator->procedure_addr;
                new_entry.name = &entry._name->char_string;
                LOG(INFO) << "Found nular operator: " << new_entry.op->return_type.type_str() << " " 
                    << new_entry.name << " @ " << new_entry.op->procedure_addr;
                std::string name = std::string(new_entry.name);
                std::transform(name.begin(), name.end(), name.begin(), ::tolower);
                _nular_operators[name].push_back(new_entry);
            }
        }

        //Find the allocator base
        MODULEINFO modInfo = { 0 };
        HMODULE hModule = GetModuleHandleA(nullptr);
        GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));

        auto findInMemory = [&modInfo](char* pattern,size_t patternLength) ->uintptr_t {
            uintptr_t base = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
            uintptr_t size = static_cast<uintptr_t>(modInfo.SizeOfImage);
            for (DWORD i = 0; i < size - patternLength; i++) {
                bool found = true;
                for (DWORD j = 0; j < patternLength; j++) {
                    found &= pattern[j] == *reinterpret_cast<char*>(base + i + j);
                    if (!found)
                        break;
                }
                if (found)
                    return base + i;
            }
            return 0;
        };

        auto getRTTIName = [](uintptr_t vtable) -> const char* {
            uintptr_t typeBase = *((uintptr_t*) (vtable - 4));
            uintptr_t type = *((uintptr_t*) (typeBase + 0xC));
            return (char*) (type + 9);
        };



        uintptr_t stringOffset = findInMemory("tbb4malloc_bi", 13);

        uintptr_t allocatorVtablePtr = (findInMemory((char*)&stringOffset, 4) -4) ;
        const char* test = getRTTIName(*reinterpret_cast<uintptr_t*>(allocatorVtablePtr));
        assert(strcmp(test, "?AVMemTableFunctions@@") == 0);
        _allocator = allocatorVtablePtr;
    }

    const unary_map & loader::unary() const {
        return _unary_operators;
    }

    const binary_map & loader::binary() const {
        return _binary_operators;
    }

    const nular_map & loader::nular() const {
        return _nular_operators;
    }

    uintptr_t loader::get_allocator() const {
        return _allocator;
    }
}