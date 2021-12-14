#include <cstdint>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <string>


using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;

class Messanger
{
    mongocxx::collection collection;
public:
    Messanger(mongocxx::collection& collection)
    {
        try
        {
            this->collection = collection;
        }
        catch (std::exception e)
        {
            std::cout << "Error create connection with DB\t" << e.what() << "\n";
        } 
    }

    mongocxx::cursor GetMessages(int limit = 10)
    {
        try
        {
            mongocxx::options::find find;
            find.limit(limit);
            auto builder = bsoncxx::builder::stream::document{};
            bsoncxx::document::value doc_value = builder
                    << "_id" << -1
                << finalize;
            bsoncxx::v_noabi::document::view_or_value ordering(doc_value);
            find.sort(ordering);
            return collection.find({}, find);
        }
        catch (std::exception e)
        {
            std::cout << "Error get message\t" << e.what() << "\n";   
        }
        return collection.find({});
    }

    void SendMessage(std::string message)
    {
        try
        {
            auto builder = bsoncxx::builder::stream::document{};
            bsoncxx::document::value doc_value = builder
                << "message" << message
                << finalize;
            bsoncxx::document::view view = doc_value.view();
            collection.insert_one(view);
        }
        catch (std::exception e)
        {
            std::cout << "Error send message\t" << e.what() << "\n";
        }
    }

    void DisplayMessages(mongocxx::cursor& cursor)
    {
        std::cout << "---start---" << "\n";
        for (auto doc : cursor)
        {
            try
            {
                std::string str = bsoncxx::to_json(doc);
                auto value = bsoncxx::from_json(str);
                auto element = value.view()["message"];
                if (element.type() != bsoncxx::type::k_utf8)
                    std::cout << "Encoding error\t utf8 encoding does not work correctly for sending and displaying in Windows" << "\n";
                else
                    std::cout << element.get_utf8().value.to_string() << "\n";
            }
            catch (std::exception e)
            {
                std::cout << "Encoding error\t utf8 encoding does not work correctly for sending and displaying in Windows: " << e.what() << "\n";
            }    
        }
        std::cout << "---end---" << "\n";
    }
};


int main()
{
    try
    {
        
        mongocxx::instance instance{};
        mongocxx::uri uri("YOUR_URI");
        mongocxx::client client(uri);
        mongocxx::database db = client["YOUR_DB"];
        mongocxx::collection collection = db["YOUR_COLLECTION"];
        
        Messanger messenger(collection);

        std::cout << "INPUT CODE" << "\n"
            << "-g - to receive 10 messages" << "\n"
            << "-gt - to receive 100 messages " << "\n"
            << "-b - to leave" << "\n"
            << "other - to send message" << "\n";
        while (true)
        {
            std::string input;
            std::getline(std::cin, input);

            if (input == "-b")
                break;

            if (input == "-g" || input == "-gl")
            {
                mongocxx::cursor cursor = messenger.GetMessages(input == "-gl" ? 100 : 10);
                messenger.DisplayMessages(cursor);
            }
            else if (input == "")
            {
                std::cout << "Empty message" << "\n";
            }
            else
            {  
                messenger.SendMessage(input);
               std::cout << "Message sent: " << input << "\n";
            }   
        } 
    }
    catch (std::exception e)
    {
        std::cout << "Unexpected error \t" << e.what() << "\n";
    }
    std::string exit;
    std::cout << "Enter any string to exit " << "\n";
    std::cin >> exit;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
