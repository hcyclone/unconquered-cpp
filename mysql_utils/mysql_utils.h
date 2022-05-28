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
        };

        class MySQLFieldNotFoundException : public MySQLException
        {
        public:
            MySQLFieldNotFoundException(const std::string& message);
        };

        class QueryFields
        {
            friend class QueryRow;
            friend class QueryResult;
        private:
            QueryFields(MYSQL_RES& result);
        public:
            QueryFields(const QueryFields& fieds) = delete;
            QueryFields(QueryFields&& fieds) = delete;
            const std::vector<std::string>& GetFields()const noexcept;
        private:
            std::vector<std::string>fields;
            size_t row_count;
        };

        class QueryRow
        {
        public:
            QueryRow(QueryFields* fields, MYSQL_ROW row);
            QueryRow(const QueryRow& row);
            QueryRow(QUeryRow&& row);
            const std::vector<std::string>& GetData()const noexcept;
            std::string operator[](std::string index)const;
            ~QueryRow();
        private:
            std::vector<std::string> data;
            QueryFields* fs;
        };

        class QueryResult
        {
            friend class QueryRow;
        public:
            QueryResult(MYSQL_RES& result);
            const std::vector<QueryRow>& GetRows()const noexcept;
            const QueryFields& GetFields()const noexcept;
        private:
            std::vector<QueryRow> rows;
            QueryFields* fields;
        };

        class DataBase
        {
        public:
            DataBase(const std::string& user_name, const std::string& password, const std::string& database, const std::string& host = "locolhost", const unsigned int port = 3306, const char* unix_socket = nullptr, const unsighed long client_flag = 0);
            void Execute(const std::string& str);
            QueryResult Query(const std::string& str);
            std::string GetDataBaseName()const;
            ~DataBase();
        private:
            MYSQL db;
        };
    } // namespace MySQL
    
}