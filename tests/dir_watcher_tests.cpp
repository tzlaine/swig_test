#include <dir_watcher.h>
#include <json2pb.h>
#include <gtest/gtest.h>

#include <fstream>
#include <thread>

namespace fs = std::filesystem;

TEST(dir_watcher_tests, no_watch)
{
    dir_watcher w;
}

TEST(dir_watcher_tests, empty_dir_no_activity)
{
    fs::path const empty_dir("empty_dir");
    if (fs::exists(empty_dir))
        fs::remove_all(empty_dir);
    fs::create_directory(empty_dir);

    {
        dir_watcher w;
        w.watch(empty_dir);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::vector<Ffile_change> changes = w.get_pending_results();
        EXPECT_TRUE(changes.empty());
    }

    {
        dir_watcher w;
        w.watch(empty_dir);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::vector<Ffile_change> changes = w.get_pending_results();
        EXPECT_TRUE(changes.empty());
    }
}

TEST(dir_watcher_tests, adds_only)
{
    fs::path const dir("adds_dir");
    if (fs::exists(dir))
        fs::remove_all(dir);
    fs::create_directory(dir);

    fs::path const f_1 = dir / "f_1";
    fs::path const f_2 = dir / "f_2";
    fs::path const f_3 = dir / "f_3";

    dir_watcher w;
    w.watch(dir);
    {
        std::ofstream ofs_1(f_1);
        std::ofstream ofs_2(f_2);
        std::ofstream ofs_3(f_3);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::vector<Ffile_change> changes = w.get_pending_results();
    EXPECT_EQ(changes.size(), 3u);

    EXPECT_EQ(changes[0].file, L"f_1");
    EXPECT_EQ(changes[0].kind, Efile_change_kind::added);
    EXPECT_EQ(changes[1].file, L"f_2");
    EXPECT_EQ(changes[1].kind, Efile_change_kind::added);
    EXPECT_EQ(changes[2].file, L"f_3");
    EXPECT_EQ(changes[2].kind, Efile_change_kind::added);
}

TEST(dir_watcher_tests, modifies_only)
{
    fs::path const dir("modifies_dir");
    if (fs::exists(dir))
        fs::remove_all(dir);
    fs::create_directory(dir);

    fs::path const f_1 = dir / "f_1";
    fs::path const f_2 = dir / "f_2";
    fs::path const f_3 = dir / "f_3";

    std::ofstream ofs_1(f_1);
    std::ofstream ofs_2(f_2);
    std::ofstream ofs_3(f_3);

    dir_watcher w;
    w.watch(dir);
    ofs_1 << "1\n";
    ofs_2 << "2\n";
    ofs_3 << "3\n";
    ofs_1.close();
    ofs_2.close();
    ofs_3.close();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::vector<Ffile_change> changes = w.get_pending_results();
    EXPECT_EQ(changes.size(), 3u);

    EXPECT_EQ(changes[0].file, L"f_1");
    EXPECT_EQ(changes[0].kind, Efile_change_kind::modified);
    EXPECT_EQ(changes[1].file, L"f_2");
    EXPECT_EQ(changes[1].kind, Efile_change_kind::modified);
    EXPECT_EQ(changes[2].file, L"f_3");
    EXPECT_EQ(changes[2].kind, Efile_change_kind::modified);
}

TEST(dir_watcher_tests, removes_only)
{
    fs::path const dir("removes_dir");
    if (fs::exists(dir))
        fs::remove_all(dir);
    fs::create_directory(dir);

    fs::path const f_1 = dir / "f_1";
    fs::path const f_2 = dir / "f_2";
    fs::path const f_3 = dir / "f_3";

    {
        std::ofstream ofs_1(f_1);
        std::ofstream ofs_2(f_2);
        std::ofstream ofs_3(f_3);
    }

    dir_watcher w;
    w.watch(dir);
    fs::remove(f_1);
    fs::remove(f_2);
    fs::remove(f_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::vector<Ffile_change> changes = w.get_pending_results();
    EXPECT_EQ(changes.size(), 3u);

    EXPECT_EQ(changes[0].file, L"f_1");
    EXPECT_EQ(changes[0].kind, Efile_change_kind::removed);
    EXPECT_EQ(changes[1].file, L"f_2");
    EXPECT_EQ(changes[1].kind, Efile_change_kind::removed);
    EXPECT_EQ(changes[2].file, L"f_3");
    EXPECT_EQ(changes[2].kind, Efile_change_kind::removed);
}

TEST(dir_watcher_tests, adds_and_modifies)
{
    fs::path const dir("adds_and_modifies_dir");
    if (fs::exists(dir))
        fs::remove_all(dir);
    fs::create_directory(dir);

    fs::path const f_1 = dir / "f_1";
    fs::path const f_2 = dir / "f_2";
    fs::path const f_3 = dir / "f_3";

    dir_watcher w;
    w.watch(dir);
    {
        std::ofstream ofs_1(f_1);
        std::ofstream ofs_2(f_2);
        std::ofstream ofs_3(f_3);
        ofs_1 << "1\n";
        ofs_2 << "2\n";
        ofs_3 << "3\n";
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::vector<Ffile_change> changes = w.get_pending_results();
    EXPECT_EQ(changes.size(), 6u);

    EXPECT_EQ(changes[0].file, L"f_1");
    EXPECT_EQ(changes[0].kind, Efile_change_kind::added);
    EXPECT_EQ(changes[1].file, L"f_2");
    EXPECT_EQ(changes[1].kind, Efile_change_kind::added);
    EXPECT_EQ(changes[2].file, L"f_3");
    EXPECT_EQ(changes[2].kind, Efile_change_kind::added);
    EXPECT_EQ(changes[3].file, L"f_3");
    EXPECT_EQ(changes[3].kind, Efile_change_kind::modified);
    EXPECT_EQ(changes[4].file, L"f_2");
    EXPECT_EQ(changes[4].kind, Efile_change_kind::modified);
    EXPECT_EQ(changes[5].file, L"f_1");
    EXPECT_EQ(changes[5].kind, Efile_change_kind::modified);
}

TEST(dir_watcher_tests, adds_modifies_and_removes)
{
    fs::path const dir("adds_modifies_and_removes_dir");
    if (fs::exists(dir))
        fs::remove_all(dir);
    fs::create_directory(dir);

    fs::path const f_1 = dir / "f_1";
    fs::path const f_2 = dir / "f_2";
    fs::path const f_3 = dir / "f_3";

    dir_watcher w;
    w.watch(dir);
    {
        std::ofstream ofs_1(f_1);
        std::ofstream ofs_2(f_2);
        std::ofstream ofs_3(f_3);
        ofs_1 << "1\n";
        ofs_2 << "2\n";
        ofs_3 << "3\n";
    }
    fs::remove(f_1);
    fs::remove(f_2);
    fs::remove(f_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::vector<Ffile_change> changes = w.get_pending_results();
    EXPECT_EQ(changes.size(), 9u);

    EXPECT_EQ(changes[0].file, L"f_1");
    EXPECT_EQ(changes[0].kind, Efile_change_kind::added);
    EXPECT_EQ(changes[1].file, L"f_2");
    EXPECT_EQ(changes[1].kind, Efile_change_kind::added);
    EXPECT_EQ(changes[2].file, L"f_3");
    EXPECT_EQ(changes[2].kind, Efile_change_kind::added);
    EXPECT_EQ(changes[3].file, L"f_3");
    EXPECT_EQ(changes[3].kind, Efile_change_kind::modified);
    EXPECT_EQ(changes[4].file, L"f_2");
    EXPECT_EQ(changes[4].kind, Efile_change_kind::modified);
    EXPECT_EQ(changes[5].file, L"f_1");
    EXPECT_EQ(changes[5].kind, Efile_change_kind::modified);
    EXPECT_EQ(changes[6].file, L"f_1");
    EXPECT_EQ(changes[6].kind, Efile_change_kind::removed);
    EXPECT_EQ(changes[7].file, L"f_2");
    EXPECT_EQ(changes[7].kind, Efile_change_kind::removed);
    EXPECT_EQ(changes[8].file, L"f_3");
    EXPECT_EQ(changes[8].kind, Efile_change_kind::removed);
}

TEST(dir_watcher_tests, multipe_rounds_of_adds_modifies_and_removes)
{
    fs::path const dir("multipe_rounds_of_adds_modifies_and_removes_dir");
    if (fs::exists(dir))
        fs::remove_all(dir);
    fs::create_directory(dir);

    fs::path const f_1 = dir / "f_1";
    fs::path const f_2 = dir / "f_2";
    fs::path const f_3 = dir / "f_3";

    dir_watcher w;
    w.watch(dir);

    for (int i = 0; i < 3; ++i) {
        {
            std::ofstream ofs_1(f_1);
            std::ofstream ofs_2(f_2);
            std::ofstream ofs_3(f_3);
            ofs_1 << "1\n";
            ofs_2 << "2\n";
            ofs_3 << "3\n";
        }
        fs::remove(f_1);
        fs::remove(f_2);
        fs::remove(f_3);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::vector<Ffile_change> changes = w.get_pending_results();
        EXPECT_EQ(changes.size(), 9u);

        EXPECT_EQ(changes[0].file, L"f_1");
        EXPECT_EQ(changes[0].kind, Efile_change_kind::added);
        EXPECT_EQ(changes[1].file, L"f_2");
        EXPECT_EQ(changes[1].kind, Efile_change_kind::added);
        EXPECT_EQ(changes[2].file, L"f_3");
        EXPECT_EQ(changes[2].kind, Efile_change_kind::added);
        EXPECT_EQ(changes[3].file, L"f_3");
        EXPECT_EQ(changes[3].kind, Efile_change_kind::modified);
        EXPECT_EQ(changes[4].file, L"f_2");
        EXPECT_EQ(changes[4].kind, Efile_change_kind::modified);
        EXPECT_EQ(changes[5].file, L"f_1");
        EXPECT_EQ(changes[5].kind, Efile_change_kind::modified);
        EXPECT_EQ(changes[6].file, L"f_1");
        EXPECT_EQ(changes[6].kind, Efile_change_kind::removed);
        EXPECT_EQ(changes[7].file, L"f_2");
        EXPECT_EQ(changes[7].kind, Efile_change_kind::removed);
        EXPECT_EQ(changes[8].file, L"f_3");
        EXPECT_EQ(changes[8].kind, Efile_change_kind::removed);
    }
}

TEST(dir_watcher_tests, stress_test)
{
    fs::path const dir("stress_test_dir");
    if (fs::exists(dir))
        fs::remove_all(dir);
    fs::create_directory(dir);

    fs::path const f_1 = dir / "f_1";
    fs::path const f_2 = dir / "f_2";
    fs::path const f_3 = dir / "f_3";

    dir_watcher w;
    w.watch(dir);

    {
        int const iterations = 1 << 10;
        for (int i = 0; i < iterations; ++i) {
            {
                std::ofstream ofs_1(f_1);
                ofs_1 << "1\n";
            }
            fs::remove(f_1);
        }

        std::vector<Ffile_change> changes = w.get_pending_results();
        EXPECT_EQ((int)changes.size(), 3 * iterations);
    }

    {
        int const iterations = 1 << 12;
        for (int i = 0; i < iterations; ++i) {
            {
                std::ofstream ofs_1(f_1);
                ofs_1 << "1\n";
            }
            fs::remove(f_1);
        }

        std::vector<Ffile_change> changes = w.get_pending_results();
        EXPECT_EQ(changes.size(), 1u);

        EXPECT_EQ(changes[0].kind, Efile_change_kind::rescan_required);
    }
}
