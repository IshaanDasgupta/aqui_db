#include "core/database.hpp"
#include "client/parser.hpp"
#include "utils/helper.hpp"
#include "client/exception.hpp"
#include "client/command.hpp"
#include "utils/to_string.hpp"
#include <tl/expected.hpp>
#include <iostream>

namespace client{

void Parser::paserAndExecute(const std::string& command, core::Database& db){
    std::vector<std::string> segments = utils::split(command, ' ');

    if (segments.size() == 0){
        std::cout << "Invalid Command\n";
        return;
    }

    auto res = client::to_command_enum(segments[0]);
    if (!res) {
        std::cout << res.error().what() << '\n';
        return;
    }

    CommandType type = *res;
    switch (type)
    {

    case CommandType::create: {
        
        if (segments.size() < 4){
            std::cout << "Invalid Command\n";
            break;
        }

        std::string table_name = segments[1];
        int pk_index;

        try{
            pk_index = stoi(segments[2]);
        }
        catch (const std::invalid_argument & e) {
            std::cout << e.what() << "\n";
            break;
        }
        catch (const std::out_of_range & e) {
            std::cout << e.what() << "\n";
            break;
        }

        std::vector<core::ColumnSchema> columns;
        for (int i=3 ; i<segments.size(); i++){
            std::vector<std::string> vals = utils::split(segments[i], ',');
            for (auto i:vals){
                std::cout << i << ' ';
            }
            std::cout << '\n';
            if (vals.size() != 2){
                std::cout << "Invalid Command\n";
                return;
            }

            std::string col_name = vals[0];
            tl::expected<core::FieldSchema, core::DatabaseException> res = core::to_field_schema(vals[1]);
            if (!res){
                std::cout << res.error().what() << '\n';
                return;
            }

            core::ColumnSchema col(col_name, *res);
            columns.push_back(col);
        };

        tl::expected<void, core::DatabaseException> res = db.createTable(table_name, columns, pk_index);
        if (!res){
            std::cout << res.error().what() << '\n';
            break;
        }

        break;
    }
    
    case CommandType::get: {
        if (segments.size() != 3){
            std::cout << "Invalid Command\n";
            break;
        }
        std::string table_name = segments[1];
        core::Field key_val = core::to_field(segments[2]);

        auto res = db.get(table_name, key_val);

        if (!res){
            std::cout << res.error().what() << "\n";
            break;
        }

        core::Record data = *res;
        for (auto i:data){
            std::cout << utils::to_string(i) << ' ';
        }
        std::cout << '\n';
        break;
    }

    case CommandType::get_all: {
        if (segments.size() != 2){
            std::cout << "Invalid Command\n";
            break;
        }
        std::string table_name = segments[1];

        auto res = db.getAll(table_name);

        if (!res){
            std::cout << res.error().what() << "\n";
            break;
        }

        auto data = *res;
        for (auto record:data){
            for (auto i:record){
                std::cout << utils::to_string(i) << ' ';
            }
            std::cout << '\n';

        }
        break;
    }

    case CommandType::insert: {
        if (segments.size() < 3){
            std::cout << "Invalid Command\n";
            break;
        }
        std::string table_name = segments[1];
        std::vector<core::Field> vals;
        for(int i=2 ; i<segments.size() ; i++){
            core::Field res = core::to_field(segments[i]);
            vals.push_back(res);
        }

        auto res = db.insert(table_name, vals);
        if (!res){
            std::cout << res.error().what() << '\n';
            break;
        }
        break;
    }

    }
};

}