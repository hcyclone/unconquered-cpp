#pragma once
#include <mysql.h>
#include <string>
#include <stdexcept>
#include <ios>
#include <vector>

namespace MyStd
{
    namespace MySQL
    {
        class MySQLException : public std::ios_base::failure
        {
        public:
            MySQLException(const std::string& message);
        };

        class MySQLConnectException : public MySQLException
        {
        public:
            MySQLConnectException(const std::string& message);
        };
        
        class MySQLExecuteException : public MySQLException
        {
        public:
            MySQLExecuteException(const std::string& message);
        }
    } // namespace MySQL
    
}