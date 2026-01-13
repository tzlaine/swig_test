#include "rng.hpp"

#include <string>
#include <vector>
#include <unordered_map>


struct name_generator
{
    explicit name_generator(int order);
    explicit name_generator(std::vector<std::string> training_set, int order);
    name_generator(name_generator const &) = delete;
    name_generator & operator=(name_generator const &) = delete;

    void train_on(std::vector<std::string> training_set);
    void training_complete();
    std::string generate(detail::rng_state & state);
    std::string generate(
        int max_size = -1, detail::rng_state & state = detail::g_rng_state);

private:
    struct production
    {
        std::string_view str_;
        float p_;
    };

    std::string_view pick(
        std::vector<production> const & productions,
        detail::rng_state & state) const;
    void train_on(std::string_view s);
    void insert(std::vector<production> & productions, std::string_view s);
    void normalize(std::vector<production> & productions);

    std::vector<production> prefixes_;
    std::unordered_map<std::string_view, std::vector<production>>
        all_productions_;
    std::vector<std::string> training_set_;
    int order_ = 0;
    bool training_complete_ = false;
};
