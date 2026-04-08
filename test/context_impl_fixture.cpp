#include <gtest/gtest.h>

#include <paradice9/context_impl.hpp>

#include <SQLiteCpp/SQLiteCpp.h>
#include <boost/asio/io_context.hpp>
#include <boost/filesystem.hpp>

namespace {

struct a_context_impl_fixture : ::testing::Test
{
    a_context_impl_fixture()
      : database_path(
            boost::filesystem::temp_directory_path() /
            boost::filesystem::unique_path("paradice-%%%%-%%%%-%%%%.db3")),
        context(io_context, database_path, [&] { ++shutdown_calls; })
    {
    }

    ~a_context_impl_fixture() override
    {
        boost::filesystem::remove(database_path);
    }

    boost::asio::io_context io_context;
    boost::filesystem::path database_path;
    std::size_t shutdown_calls{0};
    paradice9::context_impl context;
};

}  // namespace

TEST_F(
    a_context_impl_fixture,
    reports_that_an_account_has_a_named_permission_when_granted_in_the_database)
{
    auto account = context.new_account("account", "password");

    SQLite::Database database(database_path.string(), SQLite::OPEN_READWRITE);
    SQLite::Statement insert_permission(
        database,
        "INSERT INTO account_permissions "
        "    VALUES ((SELECT id FROM accounts WHERE name = ?), ?);");
    insert_permission.bind(1, "account");
    insert_permission.bind(2, "admin_access");
    insert_permission.exec();

    ASSERT_TRUE(context.has_permission(account, "admin_access"));
}
